#include "pymapnik11.hpp"

#include <mapnik/layer.hpp>
#include <mapnik/datasource.hpp>

struct layer_wrap {
    Map_ptr mp;
    mapnik::layer& lyr;
    
    layer_wrap(Map_ptr mp_, mapnik::layer& lyr_) : mp(mp_), lyr(lyr_) {}
};
    
typedef std::shared_ptr<layer_wrap> layer_wrap_ptr;

struct layers_wrap {
    Map_ptr mp;
    layers_wrap(Map_ptr mp_) : mp(mp_) {}
    
    int size() { return mp->layer_count(); }
    layer_wrap_ptr at(int i) {
        if (i<0) { i+=size(); }
        if (i>=size()) {
            throw py::index_error();
        }
        return std::make_shared<layer_wrap>(mp, mp->get_layer(i));
    }
    
    layer_wrap_ptr add(const std::string& n) {
        int sz = size();
        mapnik::layer l(n);
        mp->add_layer(std::move(l));
        return at(sz);
    }
        
};


void export_layer(py::module& m) {
    py::class_<layer_wrap, layer_wrap_ptr>(m,"layer_wrap")
        .def_property_readonly("name", [](layer_wrap& l) { return l.lyr.name(); })
        .def_property_readonly("styles", [](layer_wrap& l) { return l.lyr.styles(); })
        .def("add_style", [](layer_wrap& l, const std::string& sty) { l.lyr.add_style(sty); })
        .def_property("srs", [](layer_wrap& l) { return l.lyr.srs(); }, [](layer_wrap& l, const std::string& s) { l.lyr.set_srs(s); })
        .def_property("minimum_scale_denominator", [](layer_wrap& l) { return l.lyr.minimum_scale_denominator(); }, [](layer_wrap& l, double s) { l.lyr.set_minimum_scale_denominator(s); })
        .def_property("maximum_scale_denominator", [](layer_wrap& l) { return l.lyr.maximum_scale_denominator(); }, [](layer_wrap& l, double s) { l.lyr.set_maximum_scale_denominator(s); })
        .def_property("active", [](layer_wrap& l) { return l.lyr.active(); }, [](layer_wrap& l, bool a) { l.lyr.set_active(a); })
        .def_property("queryable", [](layer_wrap& l) { return l.lyr.queryable(); }, [](layer_wrap& l, bool q) { l.lyr.set_queryable(q); })
        .def_property("datasource", [](layer_wrap& l) { return l.lyr.datasource(); }, [](layer_wrap& l, mapnik::datasource_ptr d) { l.lyr.set_datasource(d); })
        .def("visible", [](layer_wrap& l, double sd) { return l.lyr.visible(sd); })
    ;
    
    py::class_<layers_wrap, std::shared_ptr<layers_wrap>>(m,"layers_wrap")
        .def("__len__", &layers_wrap::size)
        .def("__getitem__", &layers_wrap::at)
        .def("add", &layers_wrap::add)
    ;
    m.def("_map_layers", [](std::shared_ptr<mapnik::Map> mp) { return std::make_shared<layers_wrap>(mp); });
    
}
