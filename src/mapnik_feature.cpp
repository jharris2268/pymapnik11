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
#include <mapnik/query.hpp>
#include <mapnik/value/types.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/feature_factory.hpp>
#include <mapnik/feature_kv_iterator.hpp>

#include <mapnik/wkb.hpp>
#include <mapnik/util/geometry_to_wkb.hpp>
#include <mapnik/util/geometry_to_geojson.hpp>
#include <mapnik/util/feature_to_geojson.hpp>
#include <mapnik/featureset.hpp>
#include <mapnik/datasource.hpp>





    
py::bytes get_feature_wkb(const mapnik::feature_impl& feat) {
    auto geom = feat.get_geometry();
    auto buf = mapnik::util::to_wkb(geom,mapnik::wkbByteOrder::wkbNDR);
    if (!buf) {
        throw std::domain_error("extract geometry wkb failed");
    }
    return py::bytes(buf->buffer(), buf->size());
}

void set_feature_wkb(mapnik::feature_ptr feat, const std::string& wkb) {
    
    try {
        auto geom = mapnik::geometry_utils::from_wkb(wkb.c_str(), wkb.size());
        feat->set_geometry_copy(geom);
    } catch (...) {
        throw std::domain_error("reading geometry wkb failed");
    }
    
}


py::dict get_feature_properties(const mapnik::feature_impl& f, bool skipnulls) {
    py::dict result;
    for (const auto& kb: f) {
        auto v = values::mapnik_to_py(std::get<1>(kb));
        if (!skipnulls || (!v.is_none())) {
            result[py::cast(std::get<0>(kb))] = v;
        }
    }
    return result;
}

namespace intjson {
    py::list coord_to_py(const mapnik::geometry::point<double>& pt) {
        py::list result;
        result.append(py::cast(pt.x));
        result.append(py::cast(pt.y));
        return result;
    }
    py::list ring_to_py(const std::vector<mapnik::geometry::point<double>>& ring) {
        py::list result;
        for (const auto& pt: ring) {
            result.append(coord_to_py(pt));
        }
        return result;
    }
    
    py::dict make_json(const std::string& type, py::list coords) {
        py::dict result;
        result["type"]=py::str(type);
        result["coordinates"]=coords;
        return result;
    }
    
    py::list poly_to_py(const mapnik::geometry::polygon<double>& py) {
        py::list rings;
        
                
        //rings.append(ring_to_py(py.exterior_ring));
        for (const auto& rr: py) {//.interior_rings) {
            rings.append(ring_to_py(rr));
        }
        /*
        rings.append(ring_to_py(py.exterior_ring));
        if (!py.interior_rings.empty()) {
            for (const auto& rr: py.interior_rings) {
                rings.append(ring_to_py(rr));
            }
        }*/
        return rings;
    }
    py::object get_geometry(const mapnik::geometry::geometry<double>& geom);
    
    
    struct get_geometry_impl {
        public:
            py::object operator() (const mapnik::geometry::geometry_empty& em) {
                return py::none();
            }
        
            py::object operator()(const mapnik::geometry::point<double>& pt) {
                return make_json("Point", coord_to_py(pt));
            }
            
            
            py::object operator()(const mapnik::geometry::line_string<double>& ln) {
                return make_json("LineString",ring_to_py(ln));
            }
            
            
            
            
            py::object operator()(const mapnik::geometry::polygon<double>& py) {
                return make_json("Polygon", poly_to_py(py));
            }
            
            py::object operator()(const mapnik::geometry::multi_point<double>& mpt) {
                return make_json("MultiPoint", ring_to_py(mpt));
            }
            
            py::object operator()(const mapnik::geometry::multi_line_string<double>& mls) {
                py::list lines;
                for (auto ls: mls) {
                    lines.append(ring_to_py(ls));
                }
                return make_json("MultiLineString", lines);
            }
            
            py::object operator()(const mapnik::geometry::multi_polygon<double>& mpy) {
                py::list polys;
                for (auto py: mpy) {
                    polys.append(poly_to_py(py));
                }
                return make_json("MultiPolygon", polys);
            }
            
            
            py::object operator()(const mapnik::geometry::geometry_collection<double>& gc) {
                py::dict result;
                result["type"]=py::str("GeometryCollection");
                py::list geoms;
                for (const auto& g: gc) {
                    geoms.append(get_geometry(g));
                }
                result["geometries"]=geoms;
                return result;
            }
    };
    
    
    py::object get_geometry(const mapnik::geometry::geometry<double>& geom) {
        
        return mapnik::util::apply_visitor(get_geometry_impl(), geom);
        
        /*switch (geom.which()) {
            case 0: return py::dict();
            case 1: return intjson::get_point(geom.get<mapnik::geometry::point<double>>());
            case 2: return intjson::get_linestring(geom.get<mapnik::geometry::line_string<double>>());
            case 3: return intjson::get_polygon(geom.get<mapnik::geometry::polygon<double>>());
            case 4: return intjson::get_multi_point(geom.get<mapnik::geometry::multi_point<double>>());
            case 5: return intjson::get_multi_linestring(geom.get<mapnik::geometry::multi_line_string<double>>());
            case 6: return intjson::get_multi_polygon(geom.get<mapnik::geometry::multi_polygon<double>>());
            case 7: return intjson::get_geometry_collection(geom.get<mapnik::geometry::geometry_collection<double>>());
        }
        throw std::domain_error("???");
        return py::none();*/
    }
    
}


py::object get_geometry_json(const mapnik::feature_impl& f) {
    auto geom = f.get_geometry();
    return intjson::get_geometry(geom);
    
}

py::dict get_feature_json(const mapnik::feature_impl& f) {
    py::dict result;
    result["type"]=py::str("Feature");
    result["id"] = py::cast(f.id());
    result["properties"] = get_feature_properties(f, true);
    result["geometry"] = get_geometry_json(f);
    return result;
}

template <class T>
void feature_push(mapnik::feature_impl& f, const std::string& k, T val) {
    f.put(k, mapnik::value(val));
}


void feature_push_unicode(mapnik::feature_impl& f, const std::string& k, std::string val) {
    auto r = mapnik::value_unicode_string::fromUTF8(val);
    f.put(k, r);
}

void feature_remove(mapnik::feature_impl& f, const std::string& k) {
    f.put(k,mapnik::value_null());
}

void export_feature(py::module& m) {

    
    py::class_<mapnik::context_type, mapnik::context_ptr>(m, "context")
        .def(py::init<>())
        .def("push", &mapnik::context_type::push)
        .def("keys", [](const mapnik::context_type& ctx) {
            py::dict res;
            for (const auto& k: ctx) { res[py::str(k.first)]=py::cast(k.second); }
            return res;
        })
    ;
    
    py::class_<mapnik::feature_impl, mapnik::feature_ptr>(m, "feature")
        .def(py::init<mapnik::context_ptr,int>())
        .def("id", &mapnik::feature_impl::id)
        .def("properties", &get_feature_properties, py::arg("skipnulls")=false)
        .def("__getitem__", [](const mapnik::feature_impl& f, const std::string& k) {
            return values::mapnik_to_py(f.get(k));
        })
        .def("__getitem__", [](const mapnik::feature_impl& f, int i) {
            return values::mapnik_to_py(f.get(i));
        })
        .def("__len__", [](const mapnik::feature_impl& f) { return f.size(); })
        
        
        .def("wkb", &get_feature_wkb)
        
        /*
         //undefined symbol [mapnik::json::feature_generator_grammar::back_insert_iterator<std::string,feature_impl> ??]
        .def("to_geojson", [](const mapnik::feature_impl& f) {
            std::string s;
            if (!mapnik::util::to_geojson(s,f)) {
                throw std::domain_error("failed to convert feature");
            }
            return s;
        });*/
        .def("geometry_json", &get_geometry_json)
        .def("json", &get_feature_json)
        
        .def("__setitem__", &feature_push<bool>)
        .def("__setitem__", &feature_push<int64_t>)
        .def("__setitem__", &feature_push<double>)
        .def("__setitem__", &feature_push_unicode)
        .def("__delitem__", &feature_remove)
    ;
    m.def("set_feature_wkb", &set_feature_wkb);
    
}
