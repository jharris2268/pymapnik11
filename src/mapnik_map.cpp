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

#include <mapnik/layer.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/save_map.hpp>

#include <mapnik/geometry/box2d.hpp>
#include <mapnik/image.hpp>
#include <mapnik/agg_renderer.hpp>

#include <mapnik/util/singleton.hpp>
#include <mapnik/font_engine_freetype.hpp>
#include <mapnik/datasource_cache.hpp>

#include <pybind11/operators.h>
    
py::bytes render_image(Map_ptr mp, double scale_factor, unsigned offset_x, unsigned offset_y) {
    if (!mp) { throw std::domain_error("null map"); }
    py::gil_scoped_release release;
    
    mapnik::image_rgba8 im(mp->width(), mp->height());
    size_t sz = mp->width()*mp->height()*4;
    
    mapnik::agg_renderer<mapnik::image_rgba8> ren(*mp,im,scale_factor,offset_x, offset_y);
    ren.apply();
    
    return py::bytes(reinterpret_cast<const char*>(im.bytes()), sz);
}

bool register_font(const std::string& path) {
    return mapnik::singleton<mapnik::freetype_engine,mapnik::CreateStatic>::instance().register_font(path);
}

bool register_font_path(const std::string& path) {
    return mapnik::singleton<mapnik::freetype_engine,mapnik::CreateStatic>::instance().register_fonts(path);
}

std::vector<std::string> face_names() {
    return mapnik::singleton<mapnik::freetype_engine,mapnik::CreateStatic>::instance().face_names();
}

bool register_datasource(const std::string& path) {
    return mapnik::singleton<mapnik::datasource_cache,mapnik::CreateStatic>::instance().register_datasource(path);
}
bool register_datasource_path(const std::string& path) {
    return mapnik::singleton<mapnik::datasource_cache,mapnik::CreateStatic>::instance().register_datasources(path);
}

void export_map(py::module& m) {
    
 
    
    using box2d = mapnik::box2d<double>;
    py::class_<box2d>(m, "box2d")
        .def(py::init<>())
        .def(py::init<double,double,double,double>())
        .def_property("minx", &box2d::minx, &box2d::set_minx)
        .def_property("miny", &box2d::miny, &box2d::set_miny)
        .def_property("maxx", &box2d::maxx, &box2d::set_maxx)
        .def_property("maxy", &box2d::maxy, &box2d::set_maxy)
        //.def_property_readonly("tuple", [](const box2d bx) { return py::make_tuple(bx.minx(),bx.miny(),bx.maxx(),bx.maxy()); })
        .def("__getitem__", [](const box2d& bx, int i)->double {
            if (i<0) { i+=4; }
            if (i==0) { return bx.minx(); }
            if (i==1) { return bx.miny(); }
            if (i==2) { return bx.maxx(); }
            if (i==3) { return bx.maxy(); }
            throw py::index_error();
            return 0;
        })
        .def("__len__", [](const box2d& bx) { return 4; })
        
        .def("width", [](const box2d& bx) { return bx.width();})
        .def("height", [](const box2d& bx) { return bx.height();})
        .def("center", [](const box2d& bx) {
            auto c = bx.center();
            return py::make_tuple(c.x,c.y);
        })
        .def("contains", [](const box2d& bx, double x, double y) {
            return bx.contains(x,y);
        })
        .def("contains", [](const box2d& bx, const box2d& other) {
            return bx.contains(other);
        })
        .def("intersects", [](const box2d& bx, const box2d& other) {
            return bx.intersects(other);
        })
        .def("valid", &box2d::valid)
        .def(py::self * float())
        .def(py::self / float())
        .def(float() * py::self)
        .def(py::self + py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        
    ;
        
    py::class_<mapnik::Map, Map_ptr>(m, "Map")
        .def(py::init<int,int,std::string>(), py::arg("width"), py::arg("height"), py::arg("srs")=mapnik::MAPNIK_LONGLAT_PROJ)
        .def_property("width", &mapnik::Map::width, &mapnik::Map::set_width)
        .def_property("height", &mapnik::Map::height, &mapnik::Map::set_height)
        .def_property("srs", &mapnik::Map::srs, &mapnik::Map::set_srs)
        .def_property("background", 
            [](const mapnik::Map& mp) -> py::object {
                if (mp.background()) { return py::cast(*mp.background()); }
                return py::none();},
            &mapnik::Map::set_background)
        .def("zoom_to_box", &mapnik::Map::zoom_to_box)
        .def("envelope", &mapnik::Map::get_current_extent)
        .def("scale", &mapnik::Map::scale)
        .def("scale_denominator", &mapnik::Map::scale_denominator)
        .def("zoom", &mapnik::Map::zoom)
    ;
    
    
    py::class_<mapnik::color>(m, "color")
        .def(py::init<std::uint8_t,std::uint8_t,std::uint8_t,std::uint8_t>())
        .def(py::init<std::uint32_t>())
        .def(py::init<std::string>())
        //.def_property("premultiplied", &mapnik::color::premultiplied, &mapnik::color::set_premultiplied)
        .def_property("r", &mapnik::color::red, &mapnik::color::set_red)
        .def_property("g", &mapnik::color::green, &mapnik::color::set_green)
        .def_property("b", &mapnik::color::blue, &mapnik::color::set_blue)
        .def_property("a", &mapnik::color::alpha, &mapnik::color::set_alpha)
        .def("__str__", &mapnik::color::to_string)
        .def("to_hex", &mapnik::color::to_hex_string)
        .def("rgba", &mapnik::color::rgba)
    ;
    
    m.def("load_map_string", 
        [](Map_ptr mp, std::string str, bool strict, std::string basepath) { mapnik::load_map_string(*mp,str,strict,basepath); },
        py::arg("mp"), py::arg("str"), py::arg("strict")=false, py::arg("basepath")="");
    
    m.def("save_map_to_string",
        [](Map_ptr mp, bool explicit_defaults) { return py::str(mapnik::save_map_to_string(*mp, explicit_defaults)); },
        py::arg("mp"), py::arg("explicit_defaults")=false);
    
    
    m.def("render_image", &render_image, py::arg("mp"), py::arg("scale_factor")=1.0, py::arg("offset_x")=0, py::arg("offset_y")=0);
    
    m.def("register_font", &register_font);
    m.def("register_font_path", &register_font_path, py::arg("path")="/usr/local/lib/mapnik/fonts");
    m.def("face_names", &face_names);
    m.def("register_datasource", &register_datasource);
    m.def("register_datasource_path", &register_datasource_path, py::arg("path")="/usr/local/lib/mapnik/input");
    
}

