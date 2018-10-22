#include "pymapnik11.hpp"
#include <mapnik/util/variant.hpp>
namespace values {
//using value_base=variant<value_null, value_bool, value_integer,value_double, value_unicode_string>
py::object mapnik_to_py_old(const mapnik::value& v) {
    switch (v.which()) {
        case 0: return py::none();
        case 1: return py::cast(v.get<mapnik::value_bool>());
        case 2: return py::cast(v.get<mapnik::value_integer>());
        case 3: return py::cast(v.get<mapnik::value_double>());
        case 4: 
            auto r = v.get<mapnik::value_unicode_string>();
            if (r.isEmpty()) { return py::str(""); }
            std::string s;
            r.toUTF8String(s);
            return py::str(s);
        
    }
    throw std::domain_error("???");
    return py::object();
}

class mapnik_to_py_impl {
    public:
        template <typename T>
        py::object operator () (const T& v)
        {
            return py::cast(v);
        }
        py::object operator() (const mapnik::value_null&) {
            return py::none();
        }
        
        py::object operator() (const mapnik::value_unicode_string& r) {
            if (r.isEmpty()) { return py::str(""); }
            std::string s;
            r.toUTF8String(s);
            return py::str(s);
        }
};

py::object mapnik_to_py(const mapnik::value& v) {
    return mapnik::util::apply_visitor(mapnik_to_py_impl(), v);
}

mapnik::value py_to_mapnik(py::object v) {
    if (v.is_none()) {
        return mapnik::value_null();
    }
    
    try {
        auto b = py::cast<bool>(v);
        return mapnik::value_bool(b);
    } catch (...) {}
    
    try {
        auto b = py::cast<int64_t>(v);
        return mapnik::value_integer(b);
    } catch (...) {}
    
    try {
        auto b = py::cast<double>(v);
        return mapnik::value_double(b);
    } catch (...) {}
    
    try {
        auto s = py::cast<std::string>(v);
        mapnik::value_unicode_string::fromUTF8(s);
    } catch (...) {}
    throw py::value_error("couldn't extract mapnik value");
}

mapnik::attributes dict2attr(py::dict d) {
    mapnik::attributes attrs;
    for (const auto& kv: d) {
        const std::string k = py::cast<std::string>(kv.first);
        //mapnik::value v = py_to_mapnik(py::object(kv.second,true));
        mapnik::value v = py_to_mapnik(py::reinterpret_borrow<py::object>(kv.second));
        attrs[k]=v;
    }
    return attrs;
}

}
    
    


PYBIND11_PLUGIN(_mapnik) {
    
    py::module m("_mapnik", "alternative mapnik bindings");

    export_map(m);
    export_layer(m);
    export_style(m);
    export_datasource(m);
    export_feature(m);
    export_symbolizer_properties(m);
    return m.ptr();
};
