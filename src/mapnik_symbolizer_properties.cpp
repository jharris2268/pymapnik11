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

#include <mapnik/feature_type_style.hpp>
#include <mapnik/rule.hpp>

#include <mapnik/expression.hpp>
#include <mapnik/expression_string.hpp>
#include <mapnik/expression_evaluator.hpp>

#include <mapnik/symbolizer.hpp>
#include <mapnik/symbolizer_base.hpp>
#include <mapnik/symbolizer_hash.hpp>
#include <mapnik/symbolizer_utils.hpp>
#include <mapnik/symbolizer_keys.hpp>
#include <mapnik/symbolizer_enumerations.hpp>

#include <mapnik/text/placements/base.hpp>
#include <mapnik/text/placements/simple.hpp>
#include <mapnik/text/text_properties.hpp>
#include <mapnik/text/formatting/text.hpp>
#include <mapnik/label_collision_detector.hpp>


py::object get_symbolizer_enum(const mapnik::keys& key, const mapnik::enumeration_wrapper& e) {
    switch (key) {
        case mapnik::keys::stroke_linecap: return py::cast(mapnik::line_cap_enum(e.value));
        case mapnik::keys::stroke_linejoin: return py::cast(mapnik::line_join_enum(e.value));
        case mapnik::keys::line_rasterizer: return py::cast(mapnik::line_rasterizer_enum(e.value));
        //case mapnik::keys::??: return py::cast(mapnik::halo_rasterizer_enum(e.value));
        case mapnik::keys::point_placement_type: return py::cast(mapnik::point_placement_enum(e.value));
        case mapnik::keys::alignment: return py::cast(mapnik::pattern_alignment_enum(e.value));
        //case mapnik::??: return py::cast(mapnik::debug_symbolizer_mode_enum(e.value));
        
        case mapnik::keys::markers_placement_type: return py::cast(mapnik::marker_placement_enum(e.value));
        case mapnik::keys::markers_multipolicy: return py::cast(mapnik::marker_multi_policy_enum(e.value));
        case mapnik::keys::text_transform: return py::cast(mapnik::text_transform_enum(e.value));
        case mapnik::keys::label_placement: return py::cast(mapnik::label_placement_enum(e.value));
               
        case mapnik::keys::vertical_alignment: return py::cast(mapnik::vertical_alignment_enum(e.value));
        case mapnik::keys::horizontal_alignment: return py::cast(mapnik::horizontal_alignment_enum(e.value));
        case mapnik::keys::justify_alignment: return py::cast(mapnik::justify_alignment_enum(e.value));
        case mapnik::keys::upright: return py::cast(mapnik::text_upright_enum(e.value));
        case mapnik::keys::direction: return py::cast(mapnik::direction_enum(e.value));
        case mapnik::keys::stroke_gamma_method: return py::cast(mapnik::gamma_method_enum(e.value)); 
        default:
            std::string kn = std::get<0>(mapnik::get_meta(key));
            throw std::domain_error(kn+std::string(" not a recognised enumeration type??"));
    }
    return py::none();
}


struct symbolizer_property_values_impl {
    
    public:
        symbolizer_property_values_impl( const mapnik::keys& key_)
            : key(key_) {}

        py::object operator() ( mapnik::enumeration_wrapper const& e) const
        {
            return get_symbolizer_enum(key,e);
            
        }

        py::object operator () ( mapnik::path_expression_ptr const& expr) const
        {
            if (expr) {
                //node_.put("<xmlattr>." + std::string(std::get<0>(meta_)), path_processor::to_string(*expr));
                return py::str(mapnik::path_processor::to_string(*expr));
            } else {
                return py::none();
            }
        }

        py::object operator () (mapnik::raster_colorizer_ptr const& expr) const
        {
            if (expr) {
                //serialize_raster_colorizer(node_, expr, explicit_defaults_);
                return py::str("raster_colorizer");
            } else {
                return py::none();
            }
        }

        py::object operator () (mapnik::transform_type const& expr) const
        {
            if (expr)
            {
                //node_.put("<xmlattr>." + std::string(std::get<0>(meta_)), mapnik::transform_processor_type::to_string(*expr));
                return py::str(mapnik::transform_processor_type::to_string(*expr));
            } else {
                return py::none();
            }
        }

        
        
        py::object operator () (mapnik::group_symbolizer_properties_ptr const& properties) const
        {
            if (properties) {
                //serialize_group_symbolizer_properties(node_, properties, explicit_defaults_);
                return py::str("group_symbolizer_properties");
            } else {
                return py::none();
            }
        }
        
        py::object operator() (const std::string& v)  const {
            return py::str(v);
        }
        

        template <class T>
        py::object operator () (T const& val) const {
            return py::cast(val);
        }
    
    
        
        
        

    private:
        mapnik::keys key;
    
};
void export_mapnik_keys(py::module& m) {
    py::enum_<mapnik::keys>(m,"keys")
        .value("gamma", mapnik::keys::gamma)
        .value("gamma_method", mapnik::keys::gamma_method)
        .value("opacity", mapnik::keys::opacity)
        .value("alignment", mapnik::keys::alignment)
        .value("offset", mapnik::keys::offset)
        .value("comp_op", mapnik::keys::comp_op)
        .value("clip", mapnik::keys::clip)
        .value("fill", mapnik::keys::fill)
        .value("fill_opacity", mapnik::keys::fill_opacity)
        .value("stroke", mapnik::keys::stroke)
        .value("stroke_width", mapnik::keys::stroke_width)
        .value("stroke_opacity", mapnik::keys::stroke_opacity)
        .value("stroke_linejoin", mapnik::keys::stroke_linejoin)
        .value("stroke_linecap", mapnik::keys::stroke_linecap)
        .value("stroke_gamma", mapnik::keys::stroke_gamma)
        .value("stroke_gamma_method", mapnik::keys::stroke_gamma_method)
        .value("stroke_dashoffset", mapnik::keys::stroke_dashoffset)
        .value("stroke_dasharray", mapnik::keys::stroke_dasharray)
        .value("stroke_miterlimit", mapnik::keys::stroke_miterlimit)
        .value("geometry_transform", mapnik::keys::geometry_transform)
        .value("line_rasterizer", mapnik::keys::line_rasterizer)
        .value("image_transform", mapnik::keys::image_transform)
        .value("spacing", mapnik::keys::spacing)
        .value("max_error", mapnik::keys::max_error)
        .value("allow_overlap", mapnik::keys::allow_overlap)
        .value("ignore_placement", mapnik::keys::ignore_placement)
        .value("width", mapnik::keys::width)
        .value("height", mapnik::keys::height)
        .value("file", mapnik::keys::file)
        .value("shield_dx", mapnik::keys::shield_dx)
        .value("shield_dy", mapnik::keys::shield_dy)
        .value("unlock_image", mapnik::keys::unlock_image)
        .value("mode", mapnik::keys::mode)
        .value("scaling", mapnik::keys::scaling)
        .value("filter_factor", mapnik::keys::filter_factor)
        .value("mesh_size", mapnik::keys::mesh_size)
        .value("premultiplied", mapnik::keys::premultiplied)
        .value("smooth", mapnik::keys::smooth)
        .value("simplify_algorithm", mapnik::keys::simplify_algorithm)
        .value("simplify_tolerance", mapnik::keys::simplify_tolerance)
        .value("halo_rasterizer", mapnik::keys::halo_rasterizer)
        .value("text_placements_", mapnik::keys::text_placements_)
        .value("label_placement", mapnik::keys::label_placement)
        .value("markers_placement_type", mapnik::keys::markers_placement_type)
        .value("markers_multipolicy", mapnik::keys::markers_multipolicy)
        .value("point_placement_type", mapnik::keys::point_placement_type)
        .value("colorizer", mapnik::keys::colorizer)
        .value("halo_transform", mapnik::keys::halo_transform)
        .value("num_columns", mapnik::keys::num_columns)
        .value("start_column", mapnik::keys::start_column)
        .value("repeat_key", mapnik::keys::repeat_key)
        .value("group_properties", mapnik::keys::group_properties)
        .value("largest_box_only", mapnik::keys::largest_box_only)
        .value("minimum_path_length", mapnik::keys::minimum_path_length)
        .value("halo_comp_op", mapnik::keys::halo_comp_op)
        .value("text_transform", mapnik::keys::text_transform)
        .value("horizontal_alignment", mapnik::keys::horizontal_alignment)
        .value("justify_alignment", mapnik::keys::justify_alignment)
        .value("vertical_alignment", mapnik::keys::vertical_alignment)
        .value("upright", mapnik::keys::upright)
        .value("direction", mapnik::keys::direction)
        .value("avoid_edges", mapnik::keys::avoid_edges)
        .value("ff_settings", mapnik::keys::ff_settings)
        .value("MAX", mapnik::keys::MAX_SYMBOLIZER_KEY)
        .export_values();
}


py::object conv_symbolizer_value(const mapnik::keys& key, const mapnik::symbolizer_base::value_type& val) {
    return mapnik::util::apply_visitor(symbolizer_property_values_impl(key), val);
}


py::object get_symbolizer_value(const mapnik::symbolizer_base::cont_type& props, const mapnik::keys& key) {
    //auto meta = mapnik::get_meta(key);
    auto it = props.find(key);
    if (it==props.end()) {
        throw py::index_error();
    }
    return conv_symbolizer_value(key, it->second);
}

mapnik::expression_ptr get_text_node(const mapnik::text_symbolizer_properties& tsp) {
    auto np = tsp.format_tree();
    auto tnp = std::dynamic_pointer_cast<mapnik::formatting::text_node>(np);
    if (tnp) {
        return tnp->get_text();
    }
    return nullptr;
}




void export_symbolizer_properties(py::module& m) {
    
    
    export_mapnik_keys(m);
    
    
    
/* from mapnik/symbolizer_enumerations.hpp */    
    py::enum_<mapnik::line_cap_enum>(m,"stroke_linecap_enum")
        .value("BUTT_CAP",mapnik::BUTT_CAP)
        .value("SQUARE_CAP",mapnik::SQUARE_CAP)
        .value("ROUND_CAP",mapnik::ROUND_CAP)
        .export_values()
    ;

    py::enum_<mapnik::line_join_enum>(m,"stroke_linejoin_enum")
        .value("MITER_JOIN",mapnik::MITER_JOIN)
        .value("MITER_REVERT_JOIN",mapnik::MITER_REVERT_JOIN)
        .value("ROUND_JOIN",mapnik::ROUND_JOIN)
        .value("BEVEL_JOIN",mapnik::BEVEL_JOIN)
        .export_values()
    ;
    py::enum_<mapnik::line_rasterizer_enum>(m,"line_rasterizer_enum")
        .value("FULL",mapnik::RASTERIZER_FULL)
        .value("FAST",mapnik::RASTERIZER_FAST)
        .export_values()
    ;
    
    py::enum_<mapnik::halo_rasterizer_enum>(m,"halo_rasterizer_enum")
        .value("FULL",mapnik::HALO_RASTERIZER_FULL)
        .value("FAST",mapnik::HALO_RASTERIZER_FAST)
        .export_values()
    ;
    
    py::enum_<mapnik::point_placement_enum>(m,"point_placement_enum")
        .value("CENTROID",mapnik::CENTROID_POINT_PLACEMENT)
        .value("INTERIOR",mapnik::INTERIOR_POINT_PLACEMENT)
        .export_values()
    ;

    
    py::enum_<mapnik::pattern_alignment_enum>(m,"pattern_alignment_enum")
        .value("LOCAL", mapnik::LOCAL_ALIGNMENT)
        .value("GLOBAL", mapnik::GLOBAL_ALIGNMENT)
        .export_values()
    ;
    py::enum_<mapnik::debug_symbolizer_mode_enum>(m,"debug_symbolizer_mode_enum")
        .value("COLLISION",mapnik::DEBUG_SYM_MODE_COLLISION)
        .value("VERTEX",mapnik::DEBUG_SYM_MODE_VERTEX)
        .value("RINGS",mapnik::DEBUG_SYM_MODE_RINGS)
        .export_values()
    ;
    
    py::enum_<mapnik::marker_placement_enum>(m,"marker_placement_enum")
        .value("POINT_PLACEMENT",mapnik::MARKER_POINT_PLACEMENT)
        .value("INTERIOR_PLACEMENT",mapnik::MARKER_INTERIOR_PLACEMENT)
        .value("LINE_PLACEMENT",mapnik::MARKER_LINE_PLACEMENT)
        .value("VERTEX_FIRST_PLACEMENT",mapnik::MARKER_VERTEX_FIRST_PLACEMENT)
        .value("VERTEX_LAST_PLACEMENT",mapnik::MARKER_VERTEX_LAST_PLACEMENT)
        .export_values()
    ;
    py::enum_<mapnik::marker_multi_policy_enum>(m,"marker_multi_policy_enum")
        .value("EACH",mapnik::MARKER_EACH_MULTI)
        .value("WHOLE",mapnik::MARKER_WHOLE_MULTI)
        .value("LARGEST",mapnik::MARKER_LARGEST_MULTI)
        .export_values()
    ;
    
    py::enum_<mapnik::text_transform_enum>(m, "text_transform_enum")
        .value("NONE", mapnik::NONE)
        .value("UPPERCASE", mapnik::UPPERCASE)
        .value("LOWERCASE", mapnik::LOWERCASE)
        .value("CAPITALIZE", mapnik::CAPITALIZE)
        .value("REVERSE", mapnik::REVERSE)
        .export_values()
    ;
    
    
    py::enum_<mapnik::label_placement_enum>(m, "label_placement_enum")
        .value("POINT", mapnik::POINT_PLACEMENT)
        .value("LINE", mapnik::LINE_PLACEMENT)
        .value("VERTEX", mapnik::VERTEX_PLACEMENT)
        .value("INTERIOR", mapnik::INTERIOR_PLACEMENT)
        .export_values()
    ;

    py::enum_<mapnik::vertical_alignment_enum>(m, "vertical_alignment_enum")
        .value("V_TOP", mapnik::V_TOP)
        .value("V_MIDDLE", mapnik::V_MIDDLE)
        .value("V_BOTTOM", mapnik::V_BOTTOM)
        .value("V_AUTO", mapnik::V_AUTO)
        .export_values()
    ;

    py::enum_<mapnik::horizontal_alignment_enum>(m, "horizontal_alignment_enum")
        .value("H_LEFT", mapnik::H_LEFT)
        .value("H_MIDDLE", mapnik::H_MIDDLE)
        .value("H_RIGHT", mapnik::H_RIGHT)
        .value("H_AUTO", mapnik::H_AUTO)
        .value("H_ADJUST", mapnik::H_ADJUST)
        .export_values()
    ;
    
    py::enum_<mapnik::justify_alignment_enum>(m, "justify_alignment_enum")
        .value("J_LEFT", mapnik::J_LEFT)
        .value("J_MIDDLE", mapnik::J_MIDDLE)
        .value("J_RIGHT", mapnik::J_RIGHT)
        .value("J_AUTO", mapnik::J_AUTO)
        .export_values()
    ;
    
    py::enum_<mapnik::text_upright_enum>(m, "text_upright_enum")
        .value("UPRIGHT_AUTO", mapnik::UPRIGHT_AUTO)
        .value("UPRIGHT_AUTO_DOWN", mapnik::UPRIGHT_AUTO_DOWN)
        .value("UPRIGHT_LEFT", mapnik::UPRIGHT_LEFT)
        .value("UPRIGHT_RIGHT", mapnik::UPRIGHT_RIGHT)
        .value("UPRIGHT_LEFT_ONLY", mapnik::UPRIGHT_LEFT_ONLY)
        .value("UPRIGHT_RIGHT_ONLY", mapnik::UPRIGHT_RIGHT_ONLY)
        .export_values()
    ;

    py::enum_<mapnik::direction_enum>(m, "direction_enum")
        .value("DIRECTION_LEFT", mapnik::DIRECTION_LEFT)
        .value("DIRECTION_RIGHT", mapnik::DIRECTION_RIGHT)
        .value("DIRECTION_LEFT_ONLY", mapnik::DIRECTION_LEFT_ONLY)
        .value("DIRECTION_RIGHT_ONLY", mapnik::DIRECTION_RIGHT_ONLY)    
        .value("DIRECTION_AUTO", mapnik::DIRECTION_AUTO)
        .value("DIRECTION_AUTO_DOWN", mapnik::DIRECTION_AUTO_DOWN)
        .value("DIRECTION_UP", mapnik::DIRECTION_UP)
        .value("DIRECTION_DOWN", mapnik::DIRECTION_DOWN)
        .export_values()
    ;
    
    py::enum_<mapnik::gamma_method_enum>(m, "gamma_method_enum")
        .value("GAMMA_POWER", mapnik::GAMMA_POWER)
        .value("GAMMA_LINEAR", mapnik::GAMMA_LINEAR)
        .value("GAMMA_NONE", mapnik::GAMMA_NONE)
        .value("GAMMA_THRESHOLD", mapnik::GAMMA_THRESHOLD)
        .value("GAMMA_MULTIPLY", mapnik::GAMMA_MULTIPLY)
        .export_values()
    ;
       
    m.def("string_to_key", &mapnik::get_key);
    m.def("key_to_string", [](const mapnik::keys& k) { auto meta = mapnik::get_meta(k); return py::str(std::get<0>(meta)); });
    
    
    py::enum_<mapnik::directions_e>(m, "directions")
        .value("NORTH", mapnik::directions_e::NORTH)
        .value("EAST", mapnik::directions_e::EAST)
        .value("SOUTH", mapnik::directions_e::SOUTH)
        .value("WEST", mapnik::directions_e::WEST)
        .value("NORTHEAST", mapnik::directions_e::NORTHEAST)
        .value("SOUTHEAST", mapnik::directions_e::SOUTHEAST)
        .value("NORTHWEST", mapnik::directions_e::NORTHWEST)
        .value("SOUTHWEST", mapnik::directions_e::SOUTHWEST)
        .value("EXACT_POSITION", mapnik::directions_e::EXACT_POSITION)
    ;
    
    py::class_<mapnik::text_symbolizer_properties>(m, "text_symbolizer_properties")
        .def(py::init<>(), "default values")
        .def_property_readonly("label_placement", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::label_placement, tsp.expressions.label_placement); })
        .def_property_readonly("label_position_tolerance", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.expressions.label_position_tolerance); })
        .def_property_readonly("label_spacing", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::spacing, tsp.expressions.label_spacing); })
        .def_property_readonly("margin", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.expressions.margin); })
        .def_property_readonly("repeat_distance", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.expressions.repeat_distance); })
        .def_property_readonly("minimum_distance", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.expressions.minimum_distance); })
        .def_property_readonly("minimum_padding", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.expressions.minimum_padding); })
        .def_property_readonly("minimum_path_length", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.expressions.minimum_path_length); })
        .def_property_readonly("avoid_edges", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::avoid_edges, tsp.expressions.avoid_edges); })
        .def_property_readonly("allow_overlap", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.expressions.allow_overlap); })
        .def_property_readonly("largest_bbox_only", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::largest_box_only, tsp.expressions.largest_bbox_only); })
        .def_property_readonly("max_char_angle_delta", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.expressions.max_char_angle_delta); })
        .def_property_readonly("upright", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::upright, tsp.expressions.upright); })
        
        .def_property_readonly("dx", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.layout_defaults.dx); })
        .def_property_readonly("dy", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.layout_defaults.dy); })
        .def_property_readonly("orientation", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.layout_defaults.orientation); })
        .def_property_readonly("text_ratio", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.layout_defaults.text_ratio); })
        .def_property_readonly("wrap_width", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.layout_defaults.wrap_width); })
        .def_property_readonly("wrap_char", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.layout_defaults.wrap_char); })
        .def_property_readonly("wrap_before", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.layout_defaults.wrap_before); })
        .def_property_readonly("repeat_wrap_char", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.layout_defaults.repeat_wrap_char); })
        .def_property_readonly("rotate_displacement", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.layout_defaults.rotate_displacement); })
        .def_property_readonly("halign", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::horizontal_alignment, tsp.layout_defaults.halign); })
        .def_property_readonly("jalign", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::justify_alignment, tsp.layout_defaults.jalign); })
        .def_property_readonly("valign", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::vertical_alignment, tsp.layout_defaults.valign); })
        .def_property_readonly("dir", [](const mapnik::text_symbolizer_properties& tsp) { return tsp.layout_defaults.dir; })
   
        .def_property_readonly("face_name", [](const mapnik::text_symbolizer_properties& tsp) { return tsp.format_defaults.face_name; })
        .def_property_readonly("fontset", [](const mapnik::text_symbolizer_properties& tsp)->py::object {
            if (tsp.format_defaults.fontset) { return py::str(tsp.format_defaults.fontset->get_name()); }
            return py::none();
        })
        .def_property_readonly("text_size", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.format_defaults.text_size); })
        .def_property_readonly("character_spacing", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.format_defaults.character_spacing); })
        .def_property_readonly("line_spacing", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.format_defaults.line_spacing); })
        .def_property_readonly("text_opacity", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::opacity, tsp.format_defaults.text_opacity); })
        .def_property_readonly("halo_opacity", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::opacity, tsp.format_defaults.halo_opacity); })
        .def_property_readonly("fill", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::fill, tsp.format_defaults.fill); })
        .def_property_readonly("halo_fill", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::fill, tsp.format_defaults.halo_fill); })
        .def_property_readonly("halo_radius", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.format_defaults.halo_radius); })
        .def_property_readonly("character_spacing", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::gamma, tsp.format_defaults.character_spacing); })
        .def_property_readonly("text_transform", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::text_transform, tsp.format_defaults.text_transform); })
        .def_property_readonly("ff_settings", [](const mapnik::text_symbolizer_properties& tsp) { return conv_symbolizer_value(mapnik::keys::ff_settings, tsp.format_defaults.ff_settings); })
        
        .def_property_readonly("text", &get_text_node);
    ;
        
    py::class_<mapnik::text_placements, mapnik::text_placements_ptr>(m, "text_placements")
        .def_readonly("defaults", &mapnik::text_placements::defaults, py::return_value_policy::reference)
        .def("get_placement_info", &mapnik::text_placements::get_placement_info, py::arg("scale_factor"), py::arg("feature"), py::arg("attrs"))
    ;
    py::class_<mapnik::text_placement_info, mapnik::text_placement_info_ptr>(m,"text_placement_info")
        .def_readonly("properties",&mapnik::text_placement_info::properties, py::return_value_policy::reference)
        .def_readonly("scale_factor", &mapnik::text_placement_info::scale_factor)
        .def("next", &mapnik::text_placement_info::next)
    ;
    
    m.def("evaluate_text_properties", &mapnik::evaluate_text_properties);
    
    py::class_<mapnik::detail::evaluated_text_properties,mapnik::evaluated_text_properties_ptr>(m, "evaluated_text_properties")
        //.def_readonly("label_placement", &mapnik::detail::evaluated_text_properties::label_placement)
        .def_property_readonly("label_placement", [](const mapnik::detail::evaluated_text_properties& x) { return (mapnik::label_placement_enum) x.label_placement; })
        .def_readonly("label_spacing", &mapnik::detail::evaluated_text_properties::label_spacing)
        .def_readonly("label_position_tolerance", &mapnik::detail::evaluated_text_properties::label_position_tolerance)
        .def_readonly("avoid_edges", &mapnik::detail::evaluated_text_properties::avoid_edges)
        .def_readonly("margin", &mapnik::detail::evaluated_text_properties::margin)
        .def_readonly("repeat_distance", &mapnik::detail::evaluated_text_properties::repeat_distance)
        .def_readonly("minimum_distance", &mapnik::detail::evaluated_text_properties::minimum_distance)
        .def_readonly("minimum_padding", &mapnik::detail::evaluated_text_properties::minimum_padding)
        .def_readonly("minimum_path_length", &mapnik::detail::evaluated_text_properties::minimum_path_length)
        .def_readonly("max_char_angle_delta", &mapnik::detail::evaluated_text_properties::max_char_angle_delta)
        .def_readonly("allow_overlap", &mapnik::detail::evaluated_text_properties::allow_overlap)
        .def_readonly("largest_bbox_only", &mapnik::detail::evaluated_text_properties::largest_bbox_only)
        //.def_readonly("upright", &mapnik::detail::evaluated_text_properties::upright)
        .def_property_readonly("upright", [](const mapnik::detail::evaluated_text_properties& x) { return (mapnik::text_upright_enum) x.upright; })
        .def_readonly("grid_cell_width", &mapnik::detail::evaluated_text_properties::grid_cell_width)
        .def_readonly("grid_cell_height", &mapnik::detail::evaluated_text_properties::grid_cell_height)
    ;
    py::class_<mapnik::font_feature_settings>(m, "font_feature_settings")
        .def("to_string", &mapnik::font_feature_settings::to_string)
    ;

    /*
     * NOT EXPOSED IN libmapnik.so ??
    py::class_<mapnik::text_placements_simple, std::shared_ptr<mapnik::text_placements_simple>>(m,"text_placements_simple",py::base<mapnik::text_placements>())
        .def("get_positions", &mapnik::text_placements_simple::get_positions)
    ;
    */
        
}

