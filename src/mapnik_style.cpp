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
struct style_wrap {
    style_wrap(Map_ptr mp_, const std::string& name_, mapnik::feature_type_style* style_) :
        mp(mp_), name(name_), style(style_) {}
        
    Map_ptr mp;
    std::string name;
    mapnik::feature_type_style* style;
};
typedef std::shared_ptr<style_wrap> style_wrap_ptr;
    

struct styles_wrap {
    Map_ptr mp;
    
    styles_wrap(Map_ptr mp_) : mp(mp_) {}
    
    std::vector<std::string> keys() {
        std::vector<std::string> res;
        for (const auto& kv: mp->styles()) {
            res.push_back(kv.first);
        }
        return res;
    }
    
    style_wrap_ptr find(const std::string& key) {
        //auto r = mp->find_style(key);
        auto it = mp->styles().find(key);
        if (it==mp->end_styles()) {
            return nullptr;
        }
        return std::make_shared<style_wrap>(mp, key, &(it->second));
    }
    
};

struct rule_wrap {
    rule_wrap(Map_ptr mp_, mapnik::feature_type_style* style_, int i_) :
        mp(mp_), style(style_), i(i_) {}
        
    Map_ptr mp;
    mapnik::feature_type_style* style;
    int i;
    
    const mapnik::rule& rule() const { return style->get_rules().at(i); }
    mapnik::rule& rule_nc() { return style->get_rules_nonconst().at(i); }
};



typedef std::shared_ptr<rule_wrap> rule_wrap_ptr;

struct symbolizer_props_impl
{
public:
    template <typename Symbolizer>
    const mapnik::symbolizer_base::cont_type& operator () (const Symbolizer& sy)
    {
        return sy.properties;
    }
};


struct symbolizer_wrap {
    symbolizer_wrap(Map_ptr mp_, mapnik::feature_type_style* style_, int i_, int j_) :
        mp(mp_), style(style_), i(i_), j(j_) {}
        
    Map_ptr mp;
    mapnik::feature_type_style* style;
    int i,j;
        
    const mapnik::symbolizer& get() const { return style->get_rules().at(i).get_symbolizers().at(j); }
    mapnik::symbolizer& get_nc() {
        auto& rule = style->get_rules_nonconst().at(i);
        if ((j<0) || (j>= (int) rule.get_symbolizers().size())) { throw pybind11::index_error(); }
        mapnik::rule::symbolizers::iterator it = rule.begin();
        for (int x=0; x < j; x++) { it++; }
        return *it;
    }
    
    const mapnik::symbolizer_base::cont_type& get_props() const {
        const auto& sy = get();
        return mapnik::util::apply_visitor(symbolizer_props_impl(), sy);
    }
        
};
typedef std::shared_ptr<symbolizer_wrap> symbolizer_wrap_ptr;

py::object get_symbolizer_value(const mapnik::symbolizer_base::cont_type& cont, const mapnik::keys& k);

void export_style(py::module& m) {
    
    
    py::class_<symbolizer_wrap, symbolizer_wrap_ptr>(m, "symbolizer_wrap")
        .def("type", [](const symbolizer_wrap& s) { return mapnik::symbolizer_name(s.get()); })
        .def("type_idx", [](const symbolizer_wrap& s) { return s.get().which(); })
        .def("props_len", [](const symbolizer_wrap& s) { return s.get_props().size(); })
        .def("props_keys", [](const symbolizer_wrap& s) {
            const auto& p = s.get_props();
            py::list keys;
            for (const auto& kv: p) {
                //auto meta = mapnik::get_meta(kv.first);
                //keys.append(py::str(std::get<0>(meta)));
                keys.append(py::cast(kv.first));
            }
            return keys;
        })
        .def("props_contains", [](const symbolizer_wrap& s, const mapnik::keys& k) {
            //mapnik::keys ki = mapnik::get_key(k);
            const auto& p = s.get_props();
            return p.count(k)>0;
        })  
        .def("props_getitem", [](const symbolizer_wrap& s, const mapnik::keys& k) {
            return get_symbolizer_value(s.get_props(), k);
        })
        /*.def("__getitem__", [](const symbolizer_wrap& s, const std::string& k) {
            return values::mapnik_to_py(s.get().get(k));
        })*/
    ;
    
    py::class_<mapnik::expr_node, mapnik::expression_ptr>(m, "expression")
        .def("__str__", [](const mapnik::expr_node& expr) {
            std::string s= mapnik::to_expression_string(expr);
            return py::str(s);})
        .def("__call__", [](const mapnik::expr_node& expr, const mapnik::feature_impl& feat, py::dict d) {
            return mapnik::util::apply_visitor(mapnik::evaluate<mapnik::feature_impl,mapnik::value,mapnik::attributes>(feat,values::dict2attr(d)),expr).to_bool();
        }, py::arg("feature"), py::arg("attrs")=py::dict())
        .def("evaluate", [](const mapnik::expr_node& expr, const mapnik::feature_impl& feat, py::dict d) {
            return values::mapnik_to_py(mapnik::util::apply_visitor(mapnik::evaluate<mapnik::feature_impl,mapnik::value,mapnik::attributes>(feat,values::dict2attr(d)),expr));
        }, py::arg("feature"), py::arg("attrs")=py::dict())
    ;
    m.def("make_expression", &mapnik::parse_expression);
    
    py::class_<rule_wrap, rule_wrap_ptr>(m, "rule_wrap")
        .def_property("name",
            [](const rule_wrap& r) { return r.rule().get_name(); },
            [](rule_wrap& r, const std::string& n) { return r.rule_nc().set_name(n); })
        .def_property("min_scale",
            [](const rule_wrap& r) { return r.rule().get_min_scale(); },
            [](rule_wrap& r, double sd) { return r.rule_nc().set_min_scale(sd); })
        .def_property("max_scale",
            [](const rule_wrap& r) { return r.rule().get_max_scale(); },
            [](rule_wrap& r, double sd) { return r.rule_nc().set_max_scale(sd); })
        .def("active", [](const rule_wrap& r, double sd) { return r.rule().active(sd); })
        .def_property("filter",
            [](const rule_wrap& r) { return r.rule().get_filter(); },
            [](rule_wrap& r, mapnik::expression_ptr ep) { return r.rule_nc().set_filter(ep); })
        .def_property("else_filter",
            [](const rule_wrap& r) { return r.rule().has_else_filter(); },
            [](rule_wrap& r, bool e) { return r.rule_nc().set_else(e); })
        .def_property("also_filter",
            [](const rule_wrap& r) { return r.rule().has_also_filter(); },
            [](rule_wrap& r, bool h) { return r.rule_nc().set_also(h); })
        
        .def("symbolizers_len", [](const rule_wrap& r) { return (int) r.rule().get_symbolizers().size(); })
        .def("symbolizers_getitem", [](const rule_wrap& r, int j) {
            if (j<0) { j+= r.rule().get_symbolizers().size(); }
            
            return std::make_shared<symbolizer_wrap>(r.mp, r.style, r.i, j);
        })
    ;
    
    py::enum_<mapnik::filter_mode_enum>(m, "filter_mode")
        .value("FILTER_ALL", mapnik::filter_mode_enum::FILTER_ALL)
        .value("FILTER_FIRST", mapnik::filter_mode_enum::FILTER_FIRST)
        .export_values()
    ;
       
    py::class_<style_wrap, style_wrap_ptr>(m,"style_wrap")
        .def("rules_len", [](style_wrap& sw) { return (int) sw.style->get_rules().size(); })
        .def("rules_getitem", [](style_wrap& sw, int i) {
            if (i<0) { i+=sw.style->get_rules().size(); }
            return std::make_shared<rule_wrap>(sw.mp, sw.style, i);
        })
        .def("rules_add", [](style_wrap& sw) {
            mapnik::rule r;
            int i = sw.style->get_rules().size();
            sw.style->add_rule(std::move(r));
            return std::make_shared<rule_wrap>(sw.mp, sw.style, i);
        })
        .def_property("filter_mode", 
            [](const style_wrap& sw) { return (mapnik::filter_mode_enum) sw.style->get_filter_mode(); },
            [](style_wrap& sw, mapnik::filter_mode_enum fm) { return sw.style->set_filter_mode(mapnik::filter_mode_e(fm)); }
        )
        .def_property("opacity",
            [](const style_wrap& sw) { return sw.style->get_opacity(); }, 
            [](style_wrap& sw, float o) { sw.style->set_opacity(o); }
        )
        .def_readonly("name", &style_wrap::name)        
    ;
    
    py::class_<styles_wrap, std::shared_ptr<styles_wrap>>(m,"styles_wrap")
        .def("keys", &styles_wrap::keys)
        .def("find", &styles_wrap::find)
        .def("__len__", [](const styles_wrap& s) { return s.mp->styles().size(); })
        
    ;
    m.def("_map_styles", [](std::shared_ptr<mapnik::Map> mp) { return std::make_shared<styles_wrap>(mp); });
    
}
