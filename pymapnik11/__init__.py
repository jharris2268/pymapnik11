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

from . import _mapnik 
from ._mapnik import *
from .styles import *
from .loadmap import load_mapnik_carto
from .utils import *

try:
    import PIL.Image as Pi
except:
    Pi=None



def box2d_repr(box):
    return "box2d(%0.1f, %0.1f, %0.1f, %0.1f)" % (box.minx, box.miny, box.maxx, box.maxy)

def box2d_area(box):
    return (box.maxx-box.minx)*(box.maxy-box.miny)

def box2d_intersection(left, right):
    ans= _mapnik.box2d(
        max(left.minx, right.minx),
        max(left.miny, right.miny),
        min(left.maxx, right.maxx),
        min(left.maxy, right.maxy))
    
    if ans.minx>ans.maxx or ans.miny>ans.maxy:
        return None
    return ans

_mapnik.box2d.__repr__ = box2d_repr
_mapnik.box2d.area = property(box2d_area)
_mapnik.box2d.intersection = box2d_intersection

_mapnik.layer_wrap.min_zoom = property(lambda l: find_min_zoom(l.maximum_scale_denominator*0.00028))
_mapnik.layer_wrap.max_zoom = property(lambda l: find_max_zoom(l.minimum_scale_denominator*0.00028))

def color_repr(col):
    return 'color(%d,%d,%d,%d)' % (col.r,col.g,col.b,col.a)
    
_mapnik.color.__repr__ = color_repr

def context(*args):
    ctx = _mapnik.context()
    for a in args:
        ctx.push(a)
    return ctx
    




def iter_featureset(fs):
    f = fs.next()
    while f is not None:
        yield f
        f=fs.next()

_mapnik.featureset.__iter__ = iter_featureset

_mapnik.feature.set_wkb = lambda f,w: set_feature_wkb(f,w)


expression = _mapnik.make_expression

register_font_path('/usr/share/fonts',True)
register_font_path()

register_datasource_path()

