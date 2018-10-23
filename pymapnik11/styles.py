#-----------------------------------------------------------------------
#
# This file is part of pymapnik11
#
# Copyright (C) 2018 James Harris
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
#-----------------------------------------------------------------------

from __future__ import unicode_literals, print_function
from . import _mapnik
import utils

def iter_map_styles(stys):
    for k in stys.keys():
        yield k, stys.find(k)

_mapnik.styles_wrap.__iter__ = iter_map_styles
_mapnik.styles_wrap.__getitem__ = _mapnik.styles_wrap.find

_mapnik.Map.styles = property(lambda mp: _mapnik._map_styles(mp))
_mapnik.Map.layers = property(lambda mp: _mapnik._map_layers(mp))

class rules:
    def __init__(self, style):
        self.style=style
    
    def __len__(self):
        return int(self.style.rules_len())
    
    def __getitem__(self, i):
        return self.style.rules_getitem(i)
    
    def __iter__(self):
        for i in xrange(len(self)):
            yield self[i]
    
    def add(self):
        return self.style.rules_add()

_mapnik.style_wrap.rules = property(lambda sw: rules(sw))


def min_zoom(rule):
    return utils.find_min_zoom(rule.max_scale*0.00028) #min_zoom refers to max_scale

def max_zoom(rule):
    return utils.find_max_zoom(rule.min_scale*0.00028) #max_zoom refers to min_scale

_mapnik.rule_wrap.min_zoom = property(min_zoom)
_mapnik.rule_wrap.max_zoom = property(max_zoom)

class symbolizers:
    def __init__(self, rule):
        self.rule=rule
    
    def __len__(self):
        return int(self.rule.symbolizers_len())
    
    def __getitem__(self, i):
        if i<0:
            i += len(self)
        if i<0 or i>= len(self):
            raise IndexError()
        return self.rule.symbolizers_getitem(i)
    
    def __iter__(self):
        for i in xrange(len(self)):
            yield self[i]

_mapnik.rule_wrap.symbolizers = property(lambda rl: symbolizers(rl))

class props:
    def __init__(self, symbolizer):
        self.symbolizer=symbolizer
    
    def keys(self):
        return self.symbolizer.props_keys()
    
    def __len__(self):
        return int(self.symbolizer.props_len())
    
    def __contains__(self, i):
        return self.symbolizer.props_contains(i)
    
    def __getitem__(self, k):
        ans = self.symbolizer.props_getitem(k)
        if k == _mapnik.keys.text_placements_:
            return dict((s,t) for s,t in ans.defaults)
        return ans
    
    def __iter__(self):
        for k in self.keys():
            yield k, self[k]

_mapnik.symbolizer_wrap.props = property(lambda sy: props(sy))

tsp_dfl = _mapnik.text_symbolizer_properties()

def tsp_iter(tsp, explicit_defaults=False):
    for k in dir(tsp):
        if k.startswith('_'): continue
        v=getattr(tsp,k)
        if explicit_defaults or v!=getattr(tsp_dfl, k):
            yield k, v
        

_mapnik.text_symbolizer_properties.__iter__ = tsp_iter
