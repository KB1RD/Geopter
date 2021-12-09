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

#ifndef GEOPTER_ELEMENT_LENS_H
#define GEOPTER_ELEMENT_LENS_H

#include "element.h"

#include "assembly/surface.h"
#include "assembly/gap.h"

namespace geopter {


class Lens : public Element
{
public:
    Lens(Surface* s1, Surface* s2, Gap* g);
    ~Lens();

    Surface* surface1() const;
    Surface* surface2() const;
    
    void set_render_color_s1(const Rgb& color);
    void set_render_color_s2(const Rgb& color);

private:
    Surface* s1_;
    Surface* s2_;
    Gap* gap_;

    double max_edge_extent_;
    double min_edge_extent_;
    Rgb render_color_s1_;
    Rgb render_color_s2_;
};

} // namespace

#endif //GEOPTER_ELEMENT_LENS_H
