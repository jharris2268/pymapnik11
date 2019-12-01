/*****************************************************************************
 *
 * This file is part of pymapnik11
 *
 * Copyright (C) 2018 James Harris
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/
 
#include "pymapnik11.hpp"
#include <pybind11/pytypes.h>
#include <mapnik/feature.hpp>

#include <mapnik/text/placement_finder.hpp>
#include <mapnik/text/placement_finder_impl.hpp>
#include <mapnik/text/placements/base.hpp>
#include <mapnik/label_collision_detector.hpp>

py::tuple read_glyph_position(const mapnik::glyph_positions& gp) {
    
    auto a=gp.get_base_point();
    auto b=gp.get_marker();
    auto c=gp.marker_pos();
    py::list d;
    for (const auto& g: gp) {
        d.append(py::make_tuple(g.glyph.glyph_index, g.pos, g.rot.angle()));
    }
    return py::make_tuple(a,b,c,d);
}

/*std::vector<mapnik::glyph_positions>*/ py::list find_placements_point(
    mapnik::feature_impl const& feature,
    double x, double y,
    mapnik::attributes const& attr,
    mapnik::label_collision_detector4 & detector,
    mapnik::box2d<double> const& extent,
    mapnik::text_placement_info const& placement_info,
    //const mapnik::freetype_engine::font_file_mapping_type& ffm,
    double scale_factor) {
        
    
    
    mapnik::font_library fl;
    auto& fe = mapnik::freetype_engine::instance();
    mapnik::face_manager_freetype fm(fl,fe.get_mapping(), fe.get_cache());
    


    mapnik::placement_finder finder(feature, attr, detector, extent, placement_info, fm, scale_factor);
        
        
    while (finder.next_position()) {
        
        finder.find_point_placement(mapnik::pixel_position(x,y));
    }
    
    auto& ll = finder.placements();
    //std::vector<mapnik::glyph_positions> res;
    py::list res;
    for (auto& l: ll) {
        //res.push_back(*l);
        res.append(read_glyph_position(*l));
    }
    return res;
}

struct line_adaptor {
    
    const std::vector<std::pair<double,double>>& points;
    size_t idx=0;
    line_adaptor(const std::vector<std::pair<double,double>>& points_) : points(points_), idx(0) {}
        
    void rewind(int) { idx=0; }
    
    unsigned vertex(double *x, double *y) {
        if (idx >= points.size()) { return 0; }
        
        std::tie(*x,*y) = points.at(idx);
        idx++;
        
        return (idx==1 ? 1 : 2);
    }
    
};
    

/*std::vector<mapnik::glyph_positions>*/ py::list find_placements_line(
    mapnik::feature_impl const& feature,
    const std::vector<std::pair<double,double>>& points,
    mapnik::attributes const& attr,
    mapnik::label_collision_detector4 & detector,
    mapnik::box2d<double> const& extent,
    mapnik::text_placement_info const& placement_info,
    //const mapnik::freetype_engine::font_file_mapping_type& ffm,
    double scale_factor) {
        
    
    
    mapnik::font_library fl;
    auto& fe = mapnik::freetype_engine::instance();
    mapnik::face_manager_freetype fm(fl,fe.get_mapping(), fe.get_cache());


    mapnik::placement_finder finder(feature, attr, detector, extent, placement_info, fm, scale_factor);
        
    line_adaptor la(points);
    while (finder.next_position()) {
        
        finder.find_line_placements<line_adaptor>(la, false);
    }
    
    auto& ll = finder.placements();
    py::list res;
    //std::vector<mapnik::glyph_positions> res;
    for (auto& l: ll) {
        
        
        res.append(read_glyph_position(*l));
        //res.push_back(*l);
    }
    return res;
}

struct tl_cont {
    
    
    mapnik::font_library fl;
    mapnik::face_manager_freetype fm;
    mapnik::text_layout_ptr tl;
    
    tl_cont(mapnik::feature_impl const& feature, mapnik::attributes const& attr, double scale_factor, mapnik::text_placement_info const& info_) :
        fm(fl, mapnik::freetype_engine::instance().get_mapping(), mapnik::freetype_engine::instance().get_cache()),
        tl(std::make_shared<mapnik::text_layout>(fm, feature, attr, scale_factor,
            info_.properties, info_.properties.layout_defaults, info_.properties.format_tree())) {}
            
        
        
};


std::shared_ptr<tl_cont> make_text_layout(mapnik::feature_impl const& feature, mapnik::attributes const& attr, double scale_factor, mapnik::text_placement_info const& info_) {
    auto res =std::make_shared<tl_cont>(feature, attr, scale_factor, info_);
    
    res->tl->layout();
    return res;
}

py::list text_layout_lines(std::shared_ptr<tl_cont> tl) {
    
    py::list res;
    for (const auto& l: *(tl->tl)) {
        py::list gi;
        for (const auto& g: l) {
            gi.append(py::make_tuple(g.glyph_index, g.advance(), g.height(), g.line_height()));
        }
        res.append(py::make_tuple(gi, l.width(), l.height(), l.max_char_height(), l.line_height(), l.space_count()));
    }
    return res;
}
        
        
    

void export_text(py::module& m) {
    
    
    py::class_<mapnik::label_collision_detector4,std::shared_ptr<mapnik::label_collision_detector4>>(m, "label_collision_detector4")
        .def(py::init<mapnik::box2d<double>>())
        .def("insert", [](mapnik::label_collision_detector4& lcd, mapnik::box2d<double> box, std::string text) {
            lcd.insert(box, mapnik::value_unicode_string::fromUTF8(text));
        })
        .def("clear", &mapnik::label_collision_detector4::clear)
        .def("labels", [](mapnik::label_collision_detector4& lcd) {
            std::vector<std::pair<mapnik::box2d<double>, std::string>> res;
            for (auto it=lcd.begin(); it !=lcd.end(); it++) {
                std::string s;
                it->get().text.toUTF8String(s);
                res.push_back(std::make_pair(it->get().box, s));
                
            }
            return res;
        })
        .def("extent", &mapnik::label_collision_detector4::extent)
    ;
    py::class_<mapnik::pixel_position>(m,"pixel_position")
        .def_readonly("x", &mapnik::pixel_position::x)
        .def_readonly("y", &mapnik::pixel_position::y)
    ;
    
    
    py::class_<mapnik::marker_info, mapnik::marker_info_ptr>(m,"marker_info");
    
    py::class_<mapnik::glyph_position>(m,"glyph_position")
        .def_property_readonly("glyph_index",[](const mapnik::glyph_position& gp) { return gp.glyph.glyph_index; })
        .def_readonly("pos", &mapnik::glyph_position::pos)
        .def_property_readonly("rot", [](const mapnik::glyph_position& gp) { return gp.rot.angle(); })
    ;
    
    py::class_<mapnik::glyph_positions>(m,"glyph_positions")
        .def_property_readonly("base_point", &mapnik::glyph_positions::get_base_point)
        .def_property_readonly("marker", &mapnik::glyph_positions::get_marker)
        .def_property_readonly("marker_pos", &mapnik::glyph_positions::marker_pos)
        .def_property_readonly("glyphs", [](const mapnik::glyph_positions& gp) {
            return std::vector<mapnik::glyph_position>(gp.begin(),gp.end()); })
    ;
    
    m.def("find_placements_point", &find_placements_point, py::arg("feature"), py::arg("x"), py::arg("y"), py::arg("attr"), py::arg("detector")
        , py::arg("extent"), py::arg("placement_info"), /*py::arg("ffm"),*/ py::arg("scale_factor"));
    m.def("find_placements_line", &find_placements_line, py::arg("feature"), py::arg("points"), py::arg("attr"), py::arg("detector")
        , py::arg("extent"), py::arg("placement_info"), /*py::arg("ffm"),*/ py::arg("scale_factor"));
        
    py::class_<tl_cont, std::shared_ptr<tl_cont>>(m,"text_layout")
        .def_property_readonly("orientation", [](const tl_cont& tl) { return tl.tl->orientation().angle(); })
        .def_property_readonly("displacement", [](const tl_cont& tl) { return tl.tl->displacement(); })
        .def_property_readonly("bounds", [](const tl_cont& tl) { return tl.tl->bounds(); })
        .def_property_readonly("horizontal_alignment", [](const tl_cont& tl) { return py::cast(mapnik::horizontal_alignment_enum(tl.tl->horizontal_alignment())); })
        .def_property_readonly("alignment_offset", [](const tl_cont& tl) { return tl.tl->alignment_offset(); })
        .def_property_readonly("lines", &text_layout_lines)
    ;
    m.def("make_text_layout", &make_text_layout);
   
};

