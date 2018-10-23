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
