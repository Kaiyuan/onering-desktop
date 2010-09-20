import os, sys
from setuptools import setup, Extension

include_dirs = [os.path.join('..', '..', 'include')]

if sys.platform == 'darwin':
    library_dirs = []
    libraries = []
    extra_link_args = ['-F../..', '-framework OneRing']
else:
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
      version="1.0",
      ext_modules = [onering_module],
      py_modules = ['onering'],
     )
