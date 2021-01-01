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
#include <mapnik/value_types.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/feature_factory.hpp>
#include <mapnik/feature_kv_iterator.hpp>

#include <mapnik/wkb.hpp>
#include <mapnik/util/geometry_to_geojson.hpp>
#include <mapnik/util/feature_to_geojson.hpp>
#include <mapnik/featureset.hpp>
#include <mapnik/datasource.hpp>
#include <mapnik/memory_datasource.hpp>


py::object get_parameters_value(const mapnik::parameters& params, const std::string& k) {
    boost::optional<std::string> s = params.get<std::string>(k);
    if (s) { return py::cast(s.get()); }
    boost::optional<mapnik::value_integer> i = params.get<mapnik::value_integer>(k);
    if (i) { return py::cast(i.get()); }
    boost::optional<mapnik::value_double> d = params.get<mapnik::value_double>(k);
    if (d) { return py::cast(d.get()); }
    boost::optional<mapnik::value_bool> b = params.get<mapnik::value_bool>(k);
    if (b) { return py::cast(b.get()); }
    return py::object();
}
py::dict get_datasource_params(const mapnik::datasource& ds) {
    py::dict result;
    for (const auto& kv: ds.params()) {
        /*auto k = py::cast(kv.first);
        kv.second.match(
            [&result,&k](const std::string& s) { result[k]=py::cast(s); },
            [&result,&k](mapnik::value_integer i) { result[k]=py::cast(i); },
            [&result,&k](mapnik::value_double d) { result[k]=py::cast(d); },
            [&result,&k](mapnik::value_bool b) { result[k]=py::cast(b); },
            [&result,&k](mapnik::value_null n) { result[k]=py::cast(nullptr); },
            [&result,&k](const mapnik::unicode_string& us) { result[k]=py::cast(us); }
        );*/
        result[py::cast(kv.first)] = get_parameters_value(ds.params(),kv.first);
    }
    return result;
}

mapnik::parameters make_parameters(py::dict pp) {
    mapnik::parameters params;
    for (const auto& kv: pp) {
        params[py::cast<std::string>(kv.first)] = py::cast<std::string>(kv.second);
    }
    return params;
}

typedef std::function<mapnik::featureset_ptr(const mapnik::query&)> features_func_type;

class python_datasource : public mapnik::datasource {
    public:
        python_datasource(mapnik::parameters const& params,
            mapnik::box2d<double> bounds_,
            mapnik::datasource_geometry_t geom_type_,
            features_func_type func_) :
            mapnik::datasource(params),
            bounds(bounds_),
            geom_type(geom_type_),
            func(func_),
            desc(python_datasource::name(), *params.get<std::string>("encoding","utf-8")) {}
    
        // destructor
        virtual ~python_datasource () {};

        // mandatory: type of the plugin, used to match at runtime
        mapnik::datasource::datasource_t type() const {
            return mapnik::datasource::Vector;
        }

        // mandatory: name of the plugin
        static const char* name() {
            return "python_datsasource";
        }
        
        // mandatory: function to query features by box2d
        // this is called when rendering, specifically in feature_style_processor.hpp
        mapnik::featureset_ptr features(mapnik::query const& q) const {
            py::gil_scoped_acquire aq;
            try {
                return func(q);
            } catch(std::exception& ex) {
                std::cout << "failed " << ex.what() << std::endl;
            }
            return mapnik::featureset_ptr();
        }

        // mandatory: function to query features by point (coord2d)
        // not used by rendering, but available to calling applications
        mapnik::featureset_ptr features_at_point(mapnik::coord2d const& pt, double tol = 0) const {
            mapnik::box2d<double> bx(pt.x-tol, pt.y-tol, pt.x+tol, pt.y+tol);
            mapnik::query q(bx);
            return features(q);
        }

        // mandatory: return the box2d of the datasource
        // called during rendering to determine if the layer should be processed
        mapnik::box2d<double> envelope() const {
            return bounds;
        }

        // mandatory: optionally return the overal geometry type of the datasource
        boost::optional<mapnik::datasource_geometry_t> get_geometry_type() const {
            return geom_type;
        }

        // mandatory: return the layer descriptor
        mapnik::layer_descriptor get_descriptor() const {
            return desc;
        }

    private:
        mapnik::box2d<double> bounds;
        mapnik::datasource_geometry_t geom_type;
        features_func_type func;
        mapnik::layer_descriptor desc;
};


class python_featureset : public mapnik::Featureset {
    public:
        python_featureset(const std::vector<mapnik::feature_ptr>& feats_) : feats(feats_), idx(0) {}
        
        ~python_featureset() {}
        
        mapnik::feature_ptr next() {
            if (feats.empty()) { return nullptr; }
            if (idx<feats.size()) {
                auto f = feats.at(idx);
                idx++;
                return f;
            }
            return nullptr;
        }
    private:
        std::vector<mapnik::feature_ptr> feats;
        size_t idx;
};

mapnik::featureset_ptr make_python_featureset(const std::vector<mapnik::feature_ptr>& feats) {
    return std::make_shared<python_featureset>(feats);
}

mapnik::datasource_ptr make_python_datasource(
    py::dict params_in,
    mapnik::box2d<double> bounds,
    mapnik::datasource_geometry_t geom_type,
    features_func_type func) {
    
    auto params = make_parameters(params_in);    
    return std::make_shared<python_datasource>(params, bounds, geom_type, func);
}

       

void export_datasource(py::module& m) {   

    py::class_<mapnik::Featureset, mapnik::featureset_ptr>(m, "featureset")
        .def("next", &mapnik::Featureset::next)
    ;
    

    py::class_<mapnik::query>(m, "query")
        .def(py::init<mapnik::box2d<double>,std::tuple<double,double>,double>())
        .def_property_readonly("bbox", &mapnik::query::get_bbox)
        .def_property_readonly("resolution", &mapnik::query::resolution)
        .def("add_property_name", &mapnik::query::add_property_name)
        .def_property_readonly("property_names", &mapnik::query::property_names)
    ;

    py::enum_<mapnik::datasource::datasource_t>(m, "datasource_t")
        .value("Vector", mapnik::datasource::Vector)
        .value("Raster", mapnik::datasource::Raster)
        .export_values();

    py::class_<mapnik::datasource, mapnik::datasource_ptr>(m, "datasource")
        .def_property_readonly("type", &mapnik::datasource::type)
        .def("envelope", &mapnik::datasource::envelope)
        .def("params", &get_datasource_params)
        .def("features", &mapnik::datasource::features)
    ;
    
    py::enum_<mapnik::datasource_geometry_t>(m, "datasource_geometry_t")
        .value("Unknown", mapnik::datasource_geometry_t::Unknown)
        .value("Point", mapnik::datasource_geometry_t::Point)
        .value("LineString", mapnik::datasource_geometry_t::LineString)
        .value("Polygon", mapnik::datasource_geometry_t::Polygon)
        .value("Collection", mapnik::datasource_geometry_t::Collection)
        .export_values();
        

    
    m.def("make_python_featureset", &make_python_featureset);
    m.def("make_python_datasource", &make_python_datasource);
    
    
    py::class_<mapnik::parameters>(m,"parameters");
    m.def("make_parameters", &make_parameters);
    
    py::class_<mapnik::memory_datasource, std::shared_ptr<mapnik::memory_datasource>, mapnik::datasource>(m, "memory_datasource")
        .def(py::init<mapnik::parameters>())
        .def("push", &mapnik::memory_datasource::push)
        .def("size", &mapnik::memory_datasource::size)
        .def("set_envelope", &mapnik::memory_datasource::set_envelope)
        .def("clear", &mapnik::memory_datasource::clear)
    ;
        
}
