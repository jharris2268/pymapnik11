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
import io
from . import _mapnik
import subprocess, os.path, json

try:
    import yaml
except:
    yaml = None
    
    
def get_basepath(fn, abspath):
    if not abspath:
        return ''
    
    a,b=os.path.split(fn)
    return a
    
def call_carto(fn):
    if io.open(fn).read(6)=='scale:':

        convfn=os.path.splitext(fn)[0]+"-conv.mml"
        if not os.path.exists(convfn) or os.stat(convfn).st_mtime < os.stat(fn).st_mtime:
            if yaml is None:
                raise Exception("need pyyaml to convert yaml file to json format [call pip install --user pyyaml]")
            src = yaml.load(io.open(fn))
            json.dump(src, io.open(convfn,'w'))
        fn = convfn            


    cc=[l for l in subprocess.check_output(['carto','-q',fn]).decode("utf-8").split("\n") if not l.startswith('[millstone')]
    return cc


def prepare_map_string(cc, tabpp, scale, srs, avoidEdges):
    if scale!=None:
        for i,c in enumerate(cc):
            if 'ScaleDenominator' in c:
                sd=c.strip()[21:-22]
                nsd=str(int(sd)*scale)
                c=c.replace(sd, nsd)
                cc[i]=c

    

    if avoidEdges:
        for i,c in enumerate(cc):
            if '<ShieldSymbolizer ' in c:
                #print('add avoid-edges to',c)
                cs = c.replace("ShieldSymbolizer ", "ShieldSymbolizer avoid-edges=\"true\" ")
                cc[i]=cs

    if tabpp != None:
        cc=[l.replace("planet_osm",tabpp) for l in cc]

    return cc


def load_mapnik_carto(fn, tabpp = None, scale=None, srs=None, mp=None, avoidEdges=False, abspath=True, nt=8, force=False):
    """load carto map style from fn. Specify table prfx (replacing 'planet_osm') tabpp, scale, map srs etc."""

    if mp==None:
        mp = _mapnik.Map(256*nt,256*nt)

    #convfn = "%s-conv-%s-%s-%s-%s.xml" % (fn, tabpp,scale,srs,avoidEdges)
    convfn = "%s-conv.xml" % (fn,)
    
    cc=None
    if not force and os.path.exists(convfn) and os.stat(convfn).st_mtime > os.stat(fn).st_mtime:
        #_mapnik.load_map_string(mp,open(convfn).read(), False, get_basepath(fn,abspath))
        #return mp
        cc = [c.strip() for c in io.open(convfn,encoding='utf-8')]
    else:
        cc = call_carto(fn)
        io.open(convfn,'w',encoding='utf-8').write("\n".join(cc))#.encode('utf-8'))
        

    cc = prepare_map_string(cc, tabpp, scale, srs, avoidEdges)
    basepath = get_basepath(fn, abspath)
        
    _mapnik.load_map_string(mp,"\n".join(cc),False,basepath)

    

    if not scale is None:
        for l in mp.layers:
            if l.minimum_scale_denominator != 0:
                l.minimum_scale_denominator = int(l.minimum_scale_denominator*scale)
            if l.maximum_scale_denominator < 1e100:
                l.maximum_scale_denominator = int(l.maximum_scale_denominator*scale)


    if not srs is None:
        mp.srs=srs

    
    #open(convfn,'w').write(_mapnik.save_map_to_string(mp))

    return mp
