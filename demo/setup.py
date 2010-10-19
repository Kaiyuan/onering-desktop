from distutils.core import setup
from glob import glob
import sys, os

data_files = [
    ('templates', glob('templates/*')),
    ('static', glob('static/*')),
]

setup_kwargs = dict(
    data_files = data_files,
)

if sys.platform == 'win32':
    import py2exe

    py2exe_options = dict(
            compressed = True,
            includes = ['mako.cache'],
            dist_dir = 'radio',
            )
    py2exe_kwargs = dict(
            windows = ['demo.py'],
            zipfile = os.path.normcase('lib/site-packages.zip'),
            options = {'py2exe': py2exe_options},
            )
    setup_kwargs.update(py2exe_kwargs)

if sys.platform == 'darwin':
    import py2app

    py2app_options = dict(
        includes = ['mako.cache'],
    )
    py2app_kwargs = dict(
        app = ['demo.py'],
        setup_requires = ['py2app'],
        options=dict(py2app=py2app_options),
    )
    setup_kwargs.update(py2app_kwargs)

setup(**setup_kwargs)
