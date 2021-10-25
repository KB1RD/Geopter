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
**             Date: May 16th, 2021                                                                                          
********************************************************************************/

//============================================================================
/// \file   file_io.cpp
/// \author Hiiragi
/// \date   September 12th, 2021
/// \brief  
//============================================================================

#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <vector>

#include "nlohmann/json.hpp"

#include "fileio/file_io.h"

#include "assembly/circular.h"
#include "material/air.h"

using namespace geopter;

using RgbAsVector = std::vector<double>;

FileIO::FileIO()
{

}

FileIO::~FileIO()
{

}

bool FileIO::save_to_json(const OpticalSystem& opt_sys, std::string json_path)
{
    nlohmann::json json_data;

    json_data["Title"] = opt_sys.title();
    json_data["Note"]  = opt_sys.note();

    /* PupilSpec */
    auto pupil = opt_sys.optical_spec()->pupil_spec();
    json_data["Spec"]["Pupil"]["Type"]  = pupil->pupil_type();
    json_data["Spec"]["Pupil"]["Value"] = pupil->value();


    /* FieldSpec */
    json_data["Spec"]["Field"]["Type"] = opt_sys.optical_spec()->field_of_view()->field_type();

    int num_flds = opt_sys.optical_spec()->field_of_view()->field_count();
    
    for(int fi = 0; fi < num_flds; fi++){
        auto fld = opt_sys.optical_spec()->field_of_view()->field(fi);
        json_data["Spec"]["Field"]["X"].push_back(fld->x());
        json_data["Spec"]["Field"]["Y"].push_back(fld->y());
        json_data["Spec"]["Field"]["Weight"].push_back(fld->weight());
        json_data["Spec"]["Field"]["VUX"].push_back(fld->vux());
        json_data["Spec"]["Field"]["VLX"].push_back(fld->vlx());
        json_data["Spec"]["Field"]["VUY"].push_back(fld->vuy());
        json_data["Spec"]["Field"]["VLY"].push_back(fld->vly());

        json_data["Spec"]["Field"]["Color"].push_back({fld->render_color().r, fld->render_color().g, fld->render_color().b, fld->render_color().a});
    }


    /* WvlSpec */
    json_data["Spec"]["Wvl"]["RefIndex"] = opt_sys.optical_spec()->spectral_region()->reference_index();

    int num_wvls = opt_sys.optical_spec()->spectral_region()->wvl_count();

    for(int wi = 0; wi < num_wvls; wi++){
        auto wvl = opt_sys.optical_spec()->spectral_region()->wvl(wi);

        json_data["Spec"]["Wvl"]["Value"].push_back(wvl->value());
        json_data["Spec"]["Wvl"]["Weight"].push_back(wvl->weight());

        json_data["Spec"]["Wvl"]["Color"].push_back({ wvl->render_color().r, wvl->render_color().g, wvl->render_color().b, wvl->render_color().a });
    }

    /* assembly data */
    int num_srfs = opt_sys.optical_assembly()->surface_count();

    int stop_surface = opt_sys.optical_assembly()->stop_index();
    json_data["Assembly"]["Stop"] = stop_surface;

    for( int ci = 0; ci < num_srfs; ci++){

        std::string cur_idx = std::to_string(ci);

        auto s = opt_sys.optical_assembly()->surface(ci);
        auto g = opt_sys.optical_assembly()->gap(ci);

        // ---> surface attributes
        std::string surface_label = s->label();
        std::string surface_type  = s->profile()->name();
        double      cv            = s->profile()->cv();
        

        json_data["Assembly"][cur_idx]["Label"]     = surface_label;
        json_data["Assembly"][cur_idx]["Type"]      = surface_type;
        json_data["Assembly"][cur_idx]["Curvature"] = cv;

        if(surface_type == "SPH"){
            // do nothing
        }else if(surface_type == "ASP"){
            EvenPolynomial *prf = dynamic_cast<EvenPolynomial*>(s->profile());
            if(prf){
                json_data["Assembly"][cur_idx]["Conic"] = prf->conic();
                for(int ci = 0; ci < prf->coef_count(); ci++){
                    json_data["Assembly"][cur_idx]["Coefs"].push_back( prf->coef(ci) );
                }
            }
        }else if(surface_type == "ODD"){
            OddPolynomial *prf = dynamic_cast<OddPolynomial*>(s->profile());
            if(prf){
                json_data["Assembly"][cur_idx]["Conic"] = prf->conic();
                for(int ci = 0; ci < prf->coef_count(); ci++){
                    json_data["Assembly"][cur_idx]["Coefs"].push_back( prf->coef(ci) );
                }
            }
        }

        if(s->clear_aperture()){
            std::string aperture_type = s->aperture_shape();
            json_data["Assembly"][cur_idx]["Aperture"]["Type"]  = aperture_type;

            if(aperture_type == "Circular"){
                double cir_ap_r = dynamic_cast<Circular*>(s->clear_aperture())->max_dimension();
                json_data["Assembly"][cur_idx]["Aperture"]["Radius"] = cir_ap_r;
            }else if(aperture_type == "Rectangular"){
                // not implemented
            }
        }
        // <--- surface attributes end 


        // ---> gap attributes
        double thi;
        std::string mat_name;

        if(g){
            thi = g->thi();
            mat_name = g->material()->name();
        }else{ // assume at image surface
            thi = 0.0;
            mat_name = "";
        }

        json_data["Assembly"][cur_idx]["Thickness"] = thi;
        json_data["Assembly"][cur_idx]["Material"]  = mat_name;
        // <--- gap attributes end

    }

    /* output to file */
    std::ofstream fout(json_path);
    //fout << std::setw(4) << json_data << std::endl;
    fout << json_data.dump(4) << std::endl;

    return true;
}


bool FileIO::load_from_json(OpticalSystem& opt_sys, std::string json_path)
{
    std::ifstream ifs(json_path);
    if(!ifs){
        std::cout << "JSON Error: Failed to open json file" << json_path << std::endl;
        return false;
    }

    nlohmann::json json_data;
    ifs >> json_data;

    opt_sys.clear();

    // ---> title, note
    std::string sys_title = json_data["Title"].get< std::string >();
    opt_sys.set_title(sys_title);

    std::string sys_note = json_data["Note"].get< std::string >();
    opt_sys.set_note(sys_note);

    // ---> Spec start
    if(json_data.find("Spec") == json_data.end()){
        std::cout << "JSON Error: Wrong format, Spec not found" << json_path << std::endl;
    }

    // pupil
    try
    {
        int pupil_type = json_data["Spec"]["Pupil"]["Type"].get<int>();
        double pupil_value = json_data["Spec"]["Pupil"]["Value"].get<double>();
        opt_sys.optical_spec()->pupil_spec()->set_pupil_type(pupil_type);
        opt_sys.optical_spec()->pupil_spec()->set_value(pupil_value);
    }
    catch(...)
    {
        std::cout << "Failed to load pupil spec" << std::endl;
    }
    

    // wavelengths
    try
    {
        int ref_wvl_idx = json_data["Spec"]["Wvl"]["RefIndex"].get<int>();
        opt_sys.optical_spec()->spectral_region()->set_reference_index(ref_wvl_idx);

        std::vector<double> wvl_val  = json_data["Spec"]["Wvl"]["Value"].get< std::vector<double> >();
        std::vector<double> wvl_wt   = json_data["Spec"]["Wvl"]["Weight"].get< std::vector<double> >();

        std::vector< RgbAsVector > wvl_color = json_data["Spec"]["Wvl"]["Color"].get< std::vector<RgbAsVector> >();

        for(int wi = 0; wi < (int)wvl_val.size(); wi++){
            Rgb color(wvl_color[wi][0], wvl_color[wi][1], wvl_color[wi][2], 0.0);
            opt_sys.optical_spec()->spectral_region()->add(wvl_val[wi], wvl_wt[wi],color);
        }
    }
    catch(...)
    {
        std::cout << "Failed to load wvl spec" << std::endl;
    }

    // fields
    try
    {
        int field_type = json_data["Spec"]["Field"]["Type"].get<int>();
        opt_sys.optical_spec()->field_of_view()->set_field_type(field_type);

        std::vector<double> fld_x   = json_data["Spec"]["Field"]["X"].get< std::vector<double> >();
        std::vector<double> fld_y   = json_data["Spec"]["Field"]["Y"].get< std::vector<double> >();
        std::vector<double> fld_wt  = json_data["Spec"]["Field"]["Weight"].get< std::vector<double> >();
        std::vector<double> fld_vlx = json_data["Spec"]["Field"]["VLX"].get< std::vector<double> >();
        std::vector<double> fld_vux = json_data["Spec"]["Field"]["VUX"].get< std::vector<double> >();
        std::vector<double> fld_vly = json_data["Spec"]["Field"]["VLY"].get< std::vector<double> >();
        std::vector<double> fld_vuy = json_data["Spec"]["Field"]["VUY"].get< std::vector<double> >();

        std::vector< RgbAsVector > fld_color = json_data["Spec"]["Field"]["Color"].get< std::vector<RgbAsVector> >();
        
        for(int fi = 0; fi < (int)fld_x.size(); fi++){
            
            Rgb color(fld_color[fi][0], fld_color[fi][1], fld_color[fi][2], 0.0);

            opt_sys.optical_spec()->field_of_view()->add(fld_x[fi], fld_y[fi], 
                                                            fld_wt[fi], 
                                                            color,
                                                            fld_vuy[fi], fld_vly[fi],
                                                            fld_vux[fi], fld_vux[fi]);
        }
    }
    catch(...)
    {
        std::cout << "Failed to load field spec" << std::endl;
    }
    // <--- Spec end


    // ---> Assembly start
    if(json_data.find("Assembly") == json_data.end()){
        std::cout << "JSON Error: Wrong format, Assembly not found" << json_path << std::endl;
    }

    opt_sys.optical_assembly()->clear();

    int ci = 0;
    
    while(true){
        std::string cur_idx = std::to_string(ci);

        if( !json_data["Assembly"].contains(cur_idx) ){
            break;
        }

        opt_sys.optical_assembly()->add_surface_and_gap();

        // surface attributes
        auto srf = opt_sys.optical_assembly()->current_surface();

        std::string surf_label = json_data["Assembly"][cur_idx]["Label"].get<std::string>();
        srf->set_label(surf_label);

        std::string surf_type =  json_data["Assembly"][cur_idx]["Type"].get<std::string>();
        double cv= 0.0;
        try
        {
            cv = json_data["Assembly"][cur_idx]["Curvature"].get<double>();
        }
        catch(...)
        {
            cv = 0.0;
        }
        

        if( surf_type == "SPH" ) {
            srf->set_profile<SurfaceProfile::Type::Sphere>(cv);
        }
        else if( surf_type == "ASP" ) {
            double conic = json_data["Assembly"][cur_idx]["Conic"].get<double>();
            std::vector<double> coefs = json_data["Assembly"][cur_idx]["Coefs"].get< std::vector<double> >();
            
            srf->set_profile<SurfaceProfile::Type::EvenAsphere>(cv, conic, coefs);
        }
        else if(surf_type == "ODD") {
            double conic = json_data["Assembly"][cur_idx]["Conic"].get<double>();
            std::vector<double> coefs = json_data["Assembly"][cur_idx]["Coefs"].get< std::vector<double> >();

            srf->set_profile<SurfaceProfile::Type::OddAsphere>(cv, conic, coefs);
        }

        
        // aperture
        if( json_data["Assembly"][cur_idx].contains("Aperture") ){
            std::string aperture_type = json_data["Assembly"][cur_idx]["Aperture"]["Type"].get<std::string>();
            if(aperture_type == "Circular"){
                double cir_ap_r = json_data["Assembly"][cur_idx]["Aperture"]["Radius"].get<double>();
                srf->set_clear_aperture<Aperture::Shape::Circular>(cir_ap_r);
            }
        }

        // gap
        double thi = json_data["Assembly"][cur_idx]["Thickness"].get<double>();
        std::string mat_name = json_data["Assembly"][cur_idx]["Material"].get<std::string>();
        auto mat = opt_sys.material_lib()->find(mat_name);

        auto gap = opt_sys.optical_assembly()->current_gap();
        gap->set_thi(thi);
        gap->set_material(mat.get());

        ci++;
    }

    int stop_idx = json_data["Assembly"]["Stop"].get<int>();
    opt_sys.optical_assembly()->set_stop(stop_idx);

    // <--- Assembly end

    return true;
}

