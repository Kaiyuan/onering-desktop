import os, sys
from setuptools import setup, Extension

if sys.platform == 'darwin':
    include_dirs = ['/Library/Frameworks/OneRing.framework/Headers/']
    library_dirs = []
    libraries = []
    extra_link_args = ['-framework', 'OneRing']
else:
    include_dirs = [os.path.join('..', '..', 'include')]
    library_dirs = [os.path.join('..', '..', 'release'),
		    os.path.join('..', '..', 'debug')]
    libraries = ['OneRing1']
    extra_link_args = []

onering_module = Extension('_onering',
                           sources=['_onering.c'],
                           include_dirs=include_dirs,
                           library_dirs=library_dirs,
                           libraries=libraries,
                           extra_link_args=extra_link_args,
                          )

install_requires = []
if sys.version_info < (2, 6):
    install_requires.append("simplejson")

setup(name = "OneRing",
      version="0.1",
      ext_modules = [onering_module],
      py_modules = ['onering'],
      zip_safe = False,
      install_requires=install_requires,
     )
