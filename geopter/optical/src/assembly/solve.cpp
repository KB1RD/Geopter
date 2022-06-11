/*******************************************************************************
** Geopter
** Copyright (C) 2021 Hiiragi All Rights Reserved.
**
** This file is part of Geopter.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
********************************************************************************
**           Author: Hiiragi
**          Website: https://github.com/heterophyllus/Geopter
**          Contact: heterophyllus.work@gmail.com
**             Date: May 4th, 2022
********************************************************************************/

#include "assembly/solve.h"
#include "system/optical_system.h"

using namespace geopter;


EdgeThicknessSolve::EdgeThicknessSolve(int gap_index, double height, double value) :
    Solve()
{
    solve_type_ = SolveType::EdgeThickness;
    gap_index_ = gap_index;
    height_ = height;
    value_ = value;
}

bool EdgeThicknessSolve::check(const OpticalSystem *opt_sys)
{
    if(gap_index_ < 0 || height_ < 0.0 || value_ < 0.0) return false;

    if(gap_index_ > opt_sys->optical_assembly()->gap_count()-1) return false;

    return true;
}

void EdgeThicknessSolve::apply(OpticalSystem* opt_sys)
{
    int s1 = gap_index_;
    int s2 = s1+1;
    double sag1 = opt_sys->optical_assembly()->surface(s1)->sag(0.0, height_);
    double sag2 = opt_sys->optical_assembly()->surface(s2)->sag(0.0, height_);

    double thi = value_ - ( -sag1 + sag2); //et = -sag1 + thi + sag2;

    opt_sys->optical_assembly()->gap(gap_index_)->set_thi(thi);
}

OverallLengthSolve::OverallLengthSolve(int gi, double value, int s1, int s2) : Solve()
{
    solve_type_ = SolveType::OverallLength;
    surface1_ = s1;
    surface2_ = s2;
    gap_index_ = gi;
    value_ = value;
}

bool OverallLengthSolve::check(const OpticalSystem *opt_sys)
{
    if(surface1_ >= surface2_) return false;
    if(surface2_ > opt_sys->optical_assembly()->surface_count()-1) return false;
    if(gap_index_ > opt_sys->optical_assembly()->gap_count()-1) return false;

    return true;
}

void OverallLengthSolve::apply(OpticalSystem *opt_sys)
{
    double cur_oal = opt_sys->optical_assembly()->overall_length(surface1_, surface2_);
    double cur_thi = opt_sys->optical_assembly()->gap(gap_index_)->thi();
    double new_thi = (value_ - cur_oal) + cur_thi;

    opt_sys->optical_assembly()->gap(gap_index_)->set_thi(new_thi);
}


ParaxialImageSolve::ParaxialImageSolve(int gi) :
    Solve()
{
    solve_type_ = SolveType::ParaxialImageDistance;
    gap_index_ = gi;
}

bool ParaxialImageSolve::check(const OpticalSystem *opt_sys)
{
    if(gap_index_ == opt_sys->optical_assembly()->gap_count() -1) return true;
    return false;
}

void ParaxialImageSolve::apply(OpticalSystem* opt_sys)
{
    // l-l' method
    const int num_srfs = opt_sys->optical_assembly()->surface_count();
    const double ref_wvl = opt_sys->optical_spec()->spectral_region()->reference_wvl();

    double l = - opt_sys->optical_assembly()->gap(0)->thi();
    double l_prime = 0.0;

    for(int i = 1; i < num_srfs - 1; i++) {
        double c = opt_sys->optical_assembly()->surface(i)->cv();
        double n = opt_sys->optical_assembly()->gap(i-1)->material()->rindex(ref_wvl);
        double n_prime = opt_sys->optical_assembly()->gap(i)->material()->rindex(ref_wvl);
        double phi = (n_prime - n)*c;
        double d = opt_sys->optical_assembly()->gap(i)->thi();

        l_prime = n_prime/(phi + n/l);

        l = l_prime - d;
    }

    // set value
    opt_sys->optical_assembly()->image_space_gap()->set_thi(l_prime);

}
