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

from builtins import range

import pyproj    
from . import _mapnik
import time
from math import log, floor, ceil, pi, cosh
import urllib.request

try:
    from io import BytesIO
except:
    from cStringIO import StringIO as BytesIO


p4326 = pyproj.Proj('epsg:4326')
p3857 = pyproj.Proj('epsg:3857')
transformer_forward = pyproj.Transformer.from_proj(p4326,p3857,always_xy=True)
trans_forward = lambda x,y: transformer_forward.transform(x,y)

transformer_backward = pyproj.Transformer.from_proj(p3857,p4326,always_xy=True)
trans_backward = lambda x,y: transformer_backward.transform(x,y)

earth_width = 40075016.6856
zoom = lambda z: earth_width / 256 / (2**z)

merc_scale = lambda y: cosh(2 * pi * y / earth_width)


def transform(trans, *args):
    if len(args)==2:
        return trans(args[0], args[1])
    
    if len(args)==4:
        a,b = trans(args[0],args[1])
        c,d = trans(args[2],args[3])
        return [a,b,c,d]
    
    #if len(args)==1 and hasattr(args[0], 'tuple'):
    if len(args)==1 and isinstance(args[0], _mapnik.box2d):
        return _mapnik.box2d(*transform(trans, *list(args[0])))
    
    if len(args)==1 and len(args[0]) in (2,4):
        return transform(trans, *args[0])
            
    raise Exception("can't handle input values %s" % str(args))

forward = lambda *args: transform(trans_forward, *args)
backward = lambda *args: transform(trans_backward, *args)


def time_op(op, *args, **kwargs):
    st=time.time()
    ans=op(*args,**kwargs)
    return time.time()-st, ans
def find_zoom(sc):
    if sc<zoom(20): return 20
    if sc>zoom(0): return 0
    zl = log(earth_width/256/sc) / log(2)
    
    return zl

def find_min_zoom(sc):
    p=find_zoom(sc)
    if p<.5: return 0
    if p>=20: return 20
    return int(p+1)

def find_max_zoom(sc):
    return int(find_zoom(sc))


def make_box(x, y, zm=15, width=1024, height=None):
    if height is None:
        height=width
    
    hw = zoom(zm)*width/2
    hh = zoom(zm)*height/2
    return _mapnik.box2d(x-hw, y-hh, x+hw, y+hh)

def tile_bound(x, y, z, nx=1, ny=None):
    ts = zoom(z)*256
    if ny==None: ny=nx
    mx = x*ts - earth_width/2
    My = earth_width/2 - y*ts
    Mx = mx + ts*nx
    my = My - ts*ny
    
    return _mapnik.box2d(mx,my,Mx,My)

def coord_to_tile(*args):
    x,y,z=None,None,None
    if len(args)==1:
        x,y,z=args[0]
    elif len(args)==2:
        x,y = args[0]
        z=args[1]
    else:
        x,y,z=args
    ts = zoom(z)*256
    xt = (x + earth_width/2) / ts
    yt = (earth_width/2 - y) / ts
    return xt, yt


import PIL.Image as Pi
import PIL.ImageDraw as Pid
import PIL.ImageFont as Pif
import os

def add_label(im, msg, origin = (10,10), fill='black', font='DejaVuSans.ttf', size=14,fontpath=None, background='white'):
    ox,oy = origin
    if fontpath==None:
        fontpath='/usr/share/fonts/truetype/dejavu/'
    fn = font
    if not os.path.exists(fn):
        fn = os.path.join(fontpath,font)
    if not os.path.exists(fn):
        raise Exception("can't find font %s in current directory or %s" % (font,fontpath))
    imd = Pid.Draw(im)
    font = Pif.truetype(fn,size)
    
    w,h = imd.textsize(msg, font=font)
    if background is not None:
        imd.rectangle([ox-2,oy-2, ox+w+4,oy+h+4], fill='white')
    imd.text([ox,oy],msg, fill=fill,font=font)

def draw_rectangle(im, box, color='blue', width=2):
    imd=Pid.Draw(im)
    
    pts=[box[0],box[1], box[0],box[3], box[2], box[3], box[2], box[1], box[0], box[1]]
    imd.line(pts, fill=color, width=width)


def im_from_string(ims):
    imm=BytesIO(ims)
    return Pi.open(imm)

def im_to_string(im):
    imm=BytesIO()
    im.save(imm,'png')
    return imm.getvalue()

def get_tile(src, x, y, z):
    url = src.format(x=x,y=y,z=z)
    headers={'User-Agent': "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:98.0) Gecko/20100101 Firefox/98.0"}
    req = urllib.request.Request(url, headers=headers)
    return Pi.open(urllib.request.urlopen(req))

def demo_tile_from_tiles(src, x, y, z, lab='osm', buffer=0.75, border=True):
    st=time.time()
    
    rt=int(ceil(buffer))
    nt=1+2*int(ceil(buffer))
    sz=int(ceil(256*(1+2*buffer)))
    off = (256*nt - sz)//2
    im=Pi.new('RGBA',(sz,sz),(0,0,0,0))
    for ii in range(nt):
        for jj in range(nt):
            tt = get_tile(src, x + ii - rt, y + jj - rt, z)
            im.paste(tt, (ii*256-off,jj*256-off))
    
    if not lab is None:
        add_label(im, "%d %d %d %s: %0.1fs" % (x,y,z,lab,time.time()-st))
    if border:
        draw_rectangle(im, [buffer*256, buffer*256, 256+buffer*256, 256+buffer*256])
    
    return im
    


def render_demo_tile(mp, x, y, z, lab='orig', buffer=.75, border=True, scale_factor=None):
    bounds = tile_bound(x-buffer, y-buffer, z, 1+2*buffer)
    im_sz = int(floor(256*(1+2*buffer))+.1)
    t,im=time_op(render_image,mp, bounds, im_sz, scale_factor=scale_factor)
    
    if not lab is None:
        add_label(im, "%d %d %d %s: %0.1fs" % (x,y,z,lab,t))
    if border:
        draw_rectangle(im, [buffer*256, buffer*256, 256+buffer*256, 256+buffer*256])
    
    return im


tile_bound_buffer = lambda x,y,z,buffer=0.75: tile_bound(x-buffer, y-buffer, z, 1+2*buffer)

def render_demo_tile_parts(mp, x, y, z, lab, parts, buffer=.75, border=True, scale_factor=None,merge=True):
    
    bounds = tile_bound_buffer(x,y,z,buffer)
    im_sz = int(floor(256*(1+2*buffer))+.1)
    t,im_parts=time_op(render_image,mp, bounds, im_sz, scale_factor=scale_factor,parts=parts)
    if merge:
        im_parts=merge_image_parts(im_parts)
       
    
    for a,b,c in im_parts:
        if not lab is None:
            add_label(c, "%d %d %d %s %s: %0.1fs" % (x,y,z,lab,a,b))
        if border:
            draw_rectangle(c, [buffer*256, buffer*256, 256+buffer*256, 256+buffer*256])
        
    return im_parts



def render_image(mp, box=None, width=None, height=None, scale=None, parts=None, scale_factor=None):
    
    if scale is not None:
        if box is None:
            raise Exception("specify box if specifing scale")
        if width is not None or height is not None:
            raise Exception("specify either scale or size")
        
            
        mp.width = int(round(box.width()/scale)+0.5)
        mp.height = int(round(box.height()/scale)+0.5)
        
        
    else:
        if width is not None:
            mp.width=width
            
        if height is not None:
            mp.height=height
        elif width is not None:
            mp.height=width
    
        
    
    if not box is None:
        mp.zoom_to_box(box)
        
    kw={}
    if scale_factor is not None:
        kw['scale_factor']=scale_factor
    if not parts is None:
        aa = _mapnik.render_image_split(mp, parts, **kw)
        if Pi is None:
            return aa
        return [(a,b,Pi.frombytes('RGBA', (mp.width,mp.height), c)) for a,b,c in aa]
        
    aa = _mapnik.render_image(mp,**kw)
    if Pi is None:
        return aa
    return Pi.frombytes('RGBA', (mp.width,mp.height), aa)
    
def merge_images(ims):
    if not ims:
        return None
    if len(ims)==1:
        return ims[0]
    
    
    bottom = ims[0]
    top = merge_images(ims[1:])
    
    if top is None:
        return top
    if bottom is None:
        bottom = Pi.new('RGBA',(top.width, top.height), 'white')
    
    return Pi.alpha_composite(bottom, top)
        
def merge_image_parts(image_parts):
    tt={}
    order=[]
    
    for a,b,c in image_parts:
        if not a in tt:
            order.append(a)
            tt[a]=[0,[]]
        tt[a][0]+=b
        tt[a][1].append(c)
    
    return [(o, tt[o][0], merge_images(tt[o][1])) for o in order]
        

default_num_tiles = dict((z,1 if z<6 else 2 if z<10 else 4 if z<13 else 8) for z in range(19))

def tile_extent_zoom(x,y,z, zl):
    sf = 2.**(zl-z)
    return x*sf, (y+1)*sf, (x+1)*sf, y*sf

class Tiles:
    def __init__(self, mp, num_tiles=None, scale_factor=None):
        self.mp=mp
        self.num_tiles=default_num_tiles if num_tiles is None else num_tiles
        
        self.tiles={}
        self.scale_factor=scale_factor
        
    
    def render_tile_image(self, x, y, z, nt):    
        tsz = int(256*(self.scale_factor if not self.scale_factor is None else 1))
        imsz=tsz*nt
        bx = tile_bound(x,y,z, nt)
        return render_image(self.mp, bx, imsz, scale_factor=self.scale_factor), tsz
        
        
    
    def render_tiles(self, x,y,z):
        
        nt = (self.num_tiles[z] if z in self.num_tiles else 1) if hasattr(self.num_tiles,'__getitem__') else self.num_tiles
        if nt==1:
            yield x, y, self.render_tile_image(x, y, z, 1)[0]
            return
            
        
        xx = (x//nt) * nt
        yy = (y//nt) * nt
        im, tsz = self.render_tile_image(xx,yy,z,nt)
        
        for i in range(nt):
            for j in range(nt):
                imc=im.crop([tsz*i, tsz*j, tsz*(i+1), tsz*(j+1)])
                imc.load()
                yield xx+i, yy+j, imc
    
    def add_tiles(self, x, y, z):
        
        for xi,yi,im in self.render_tiles(x,y,z):
            self.tiles[xi,yi,z] = im
        
        
        
        
    def __call__(self, x, y, z, ax=None, zoom_level=None, **kw):
        if not (x,y,z) in self.tiles:
            self.add_tiles(x,y,z)
        
        if ax is None:
            return self.tiles[x,y,z]
        else:
            if not zoom_level is None:
                a,b,c,d = tile_extent_zoom(x,y,z,zoom_level)
                ax.imshow(self.tiles[x,y,z], extent=[a,c,b,d], **kw2)
                
            else:
                
                bx = tile_bound(x, y, z)
                ax.imshow(self.tiles[x,y,z], extent=[bx.minx, bx.maxx, bx.miny, bx.maxy], **kw)
    
    
    def demo_image(self, x, y, z, tab='orig', buffer=0.75):
        
        st=time.time()
        im_sz = int(floor(256*(1+2*buffer))+.1)
        
        im = Pi.new('RGBA', (im_sz,im_sz), 'white')
        bi=int(ceil(buffer))
        cx,cy = im_sz//2-128, im_sz/2-128
        for xi in range(-bi, bi+1):
            for yi in range(-bi, bi+1):
                t=self(xi+x,yi+y,z)
                im.paste(t, [cx+xi*256, cy+yi*256])
                
        et=time.time()-st
        add_label(im, '%d %d %d %s: %0.1fs' % (x,y,z,tab,et))
        draw_rectangle(im, [buffer*256, buffer*256, 256+buffer*256, 256+buffer*256])
        return im

def demo():
    mp = load_mapnik_carto('openstreetmap-carto/project.mml')
    bx = tile_bound(16369, 10893, 15, 8)
    return mp, bx, render_image(mp,bx,8*256)


def render_demo_tile_project(mp, x, y, z,lab='', sz=640):
    bx=tile_bound(x,y,z)
    
    forward = lambda xx,yy: pyproj.transform(p3857, pyproj.Proj(mp.srs.replace("+init=epsg:","epsg:")), xx, yy)
    bxcx, bxcy = bx.center()
    ms = merc_scale(bxcy)        
    sc = zoom(z)/ms
    cx, cy = forward(bxcx, bxcy)
    
    bxff = make_box(cx, cy, z, sz/ms)
    
    t,im=time_op(render_image, mp, bxff, sz)
    
    msg='zoom %2d [%5.1f] %7.1f %7.1f%s: [%5.1fs]' % (z, sc, cx, cy,' '+lab if lab else '', t)
    add_label(im, msg)
    
    
    bxpts = [[bx.minx,bx.miny],
        [bxcx, bx.miny], [bx.maxx,bx.miny],
        [bx.maxx, bxcy], [bx.maxx,bx.maxy],
        [bxcx, bx.maxy], [bx.minx,bx.maxy],
        [bx.minx, bxcy], [bx.minx,bx.miny]]
        
    bxpts2 = [forward(x,y) for x,y in bxpts]
    bxpts3 = [(sz/2+(x-cx)/sc, sz/2-(y-cy)/sc) for x,y in bxpts2]
    
    imd=Pid.ImageDraw(im)
    imd.line(bxpts3, fill='blue',width=2)
    return im

def prep_feat(tp,props):
    ctx=_mapnik.context()
    for p in props:
        ctx.push(p)
    feat=_mapnik.feature(ctx,1)
    for p in props:
        feat[p]=props[p]
    tpi = tp.get_placement_info(1,feat,{})
    return feat, tpi
    
def get_text_layout(tp, props):
    feat,tpi = prep_feat(tp,props)
    return _mapnik.make_text_layout(feat,{},1,tpi)

def get_placement_point(tp, props, cx, cy, dd):
    feat,tpi = prep_feat(tp,props)
    return _mapnik.find_placements_point(feat, cx, cy, {}, dd, dd.extent(), tpi, 1)

def get_placement_line(tp, props, pts, dd):
    feat,tpi = prep_feat(tp,props)
    return _mapnik.find_placements_line(feat, pts, {}, dd, dd.extent(), tpi, 1)

    
    
    mk.find_placements_point(feat, cx, cy, {}, dd, mk.box2d(0,0,640,640), tpi, 1)

