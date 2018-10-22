#ifndef PYMAPNIK11_HPP
#define PYMAPNIK11_HPP

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <mapnik/map.hpp>
#include <mapnik/value.hpp>
#include <mapnik/attribute.hpp>
namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(XX, std::shared_ptr<XX>);

namespace values {
    py::object mapnik_to_py(const mapnik::value& v);
    mapnik::value py_to_mapnik(py::object v);
    mapnik::attributes dict2attr(py::dict d);
};


typedef std::shared_ptr<mapnik::Map> Map_ptr;

void export_map(py::module& m);
void export_layer(py::module& m);
void export_style(py::module& m);
void export_datasource(py::module& m);
void export_feature(py::module& m);
void export_symbolizer_properties(py::module& m);
#endif
