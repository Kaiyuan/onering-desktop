import os, sys
from setuptools import setup, Extension

if sys.platform == 'darwin':
    include_dirs = ['/Library/Frameworks/OneRing.framework/Headers/']
    library_dirs = []
    libraries = []
    extra_link_args = ['-framework', 'OneRing']
else:
    include_dirs = [os.path.join('..', '..', 'include')]
    library_dirs = [os.path.join('..', '..', 'release')]
    libraries = ['OneRing1']
    extra_link_args = []

onering_module = Extension('_onering',
                           sources=['_onering.c'],
                           include_dirs=include_dirs,
                           library_dirs=library_dirs,
                           libraries=libraries,
                           extra_link_args=extra_link_args,
                          )

setup(name = "OneRing",
      version="0.1",
      ext_modules = [onering_module],
      py_modules = ['onering'],
     )
