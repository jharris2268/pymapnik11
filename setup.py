from distutils.core import setup, Extension
#import setuptools
import shutil, os.path, subprocess

def call_pkgconfig(args):
    try:
        ans = subprocess.getoutput(args)
        return ans.split()
    except:
        ans = subprocess.check_output(args.split(" "))
        return ans.split()

libs = call_pkgconfig('mapnik-config --libs')
cflags = call_pkgconfig('mapnik-config --cflags')

#libs.append('-lboost_thread')

srcs=['src/'+f for f in (
    "pymapnik11.cpp",
    "mapnik_map.cpp",
    "mapnik_layer.cpp",
    "mapnik_style.cpp",
    "mapnik_datasource.cpp",
    "mapnik_feature.cpp",
    "mapnik_symbolizer_properties.cpp",
)]

from distutils.command.build_ext import build_ext
from distutils.sysconfig import customize_compiler

class my_build_ext(build_ext):
    def build_extensions(self):
        customize_compiler(self.compiler)
        try:
            self.compiler.compiler_so.remove("-Wstrict-prototypes")
        except (AttributeError, ValueError):
            pass
        build_ext.build_extensions(self)

ext_modules = [
    Extension(
        'pymapnik11._mapnik',
        srcs,
        include_dirs=[
            '/usr/local/include',
            
        ],
        extra_link_args=libs,
        extra_compile_args=cflags,
        
    ),
    
]



setup(
    name='pymapnik11',
    packages=['pymapnik11'],
    version='0.0.1',
    long_description='',
    ext_modules=ext_modules,
    #zip_safe=False,
    cmdclass = {'build_ext': my_build_ext}
)
