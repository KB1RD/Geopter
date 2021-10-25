/*******************************************************************************
** Geopter
** Copyright (C) 2021 Hiiragi
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
**             Date: May 16th, 2021                                                                                          
********************************************************************************/

//============================================================================
/// \file   decenter_data.h
/// \author Hiiragi
/// \date   October 12th, 2021
/// \brief  
//============================================================================


#ifndef POINT_SET_H
#define POINT_SET_H

#include <cassert>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include "renderer/rgb.h"

namespace geopter {


class PointSet
{
public:
    PointSet();
    PointSet(const std::vector<double>& x, const std::vector<double>& y, const Rgb& color, int line_style= 0, double line_width= 1.0);
    ~PointSet();

    inline void set_name(const std::string& name);
    inline void set_data(const std::vector<double>& x, const std::vector<double>& y);
    inline void add_data(double x, double y);
    inline void set_render_color(const Rgb& color);
    inline void set_line_width(double lw);
    inline void set_line_style(int ls);

    inline std::string name() const;
    inline std::vector<double> x_data() const;
    inline std::vector<double> y_data() const;
    inline double lower_x() const;
    inline double higher_x() const;
    inline double lower_y() const;
    inline double higher_y() const;
    inline int data_count() const;
    inline Rgb render_color() const;
    inline int line_style() const;
    inline double line_width() const;

    void print() const;
    void print(std::ostringstream& oss) const;

private:
    std::string name_;
    std::vector<double> xdata_;
    std::vector<double> ydata_;
    Rgb render_color_;
    int line_style_;
    double line_width_;
};


void PointSet::set_name(const std::string &name)
{
    name_ = name;
}

void PointSet::set_data(const std::vector<double> &x, const std::vector<double> &y)
{
    xdata_ = x;
    ydata_ = y;
}

void PointSet::add_data(double x, double y)
{
    xdata_.push_back(x);
    ydata_.push_back(y);
}

void PointSet::set_render_color(const Rgb &color)
{
    render_color_ = color;
}

void PointSet::set_line_style(int ls)
{
    line_style_ = ls;
}

void PointSet::set_line_width(double lw)
{
    line_width_ = lw;
}

std::string PointSet::name() const
{
    return name_;;
}

std::vector<double> PointSet::x_data() const
{
    return xdata_;
}

std::vector<double> PointSet::y_data() const
{
    return ydata_;
}

double PointSet::lower_x() const
{
    return *std::min_element(xdata_.begin(), xdata_.end());
}

double PointSet::higher_x() const
{
    return *std::max_element(xdata_.begin(), xdata_.end());
}

double PointSet::lower_y() const
{
    return *std::min_element(ydata_.begin(), ydata_.end());
}

double PointSet::higher_y() const
{
    return *std::max_element(ydata_.begin(), ydata_.end());
}

int PointSet::data_count() const
{
    assert(xdata_.size() == ydata_.size());
    return xdata_.size();
}

Rgb PointSet::render_color() const
{
    return render_color_;
}

int PointSet::line_style() const
{
    return line_style_;
}

double PointSet::line_width() const
{
    return line_width_;
}

} //namespace

#endif
