from distutils.core import setup
from glob import glob
import sys, os
from demo import __version__ as version

def gen_data_files(topdir):
    for dirpath, dirnames, filenames in os.walk(topdir):
        filenames = [x for x in filenames if not x.startswith('.')]
        yield dirpath, [os.path.join(dirpath, fn) for fn in filenames]
        dotdirs = [d for d in dirnames if d.startswith('.')]
        for d in dotdirs:
            dirnames.remove(d)

data_files = list(gen_data_files('templates')) + list(gen_data_files('static'))
data_files.append(('', ['qt.conf']))

if sys.platform != 'darwin':
    # Mac OS X should use ./deploy_plugins to change plugins install name
    data_files += list(gen_data_files('plugins'))

setup_kwargs = dict(
    data_files = data_files,
)

if sys.platform == 'win32':
    import py2exe

    py2exe_options = dict(
            compressed = True,
            includes = ['mako.cache'],
            dist_dir = 'demo-' + version,
            )
    py2exe_kwargs = dict(
            windows = [
                {'script': 'demo.py',
                 'icon_resources': [(1, 'static/onering.ico')],
                },
            ],
            zipfile = os.path.normcase('lib/site-packages.zip'),
            options = {'py2exe': py2exe_options},
            )
    setup_kwargs.update(py2exe_kwargs)

if sys.platform == 'darwin':
    import py2app

    py2app_options = dict(
        includes = ['mako.cache'],
        iconfile = 'onering.icns',
    )
    py2app_kwargs = dict(
        app = ['demo.py'],
        setup_requires = ['py2app'],
        options=dict(py2app=py2app_options),
    )
    setup_kwargs.update(py2app_kwargs)

setup(**setup_kwargs)
