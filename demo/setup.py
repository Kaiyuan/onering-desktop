from distutils.core import setup
from glob import glob
import sys, os

if sys.platform == 'win32':
    QT_PLUGINS_DIR = os.path.normcase('C:/Qt/4.6.3/plugins')
elif sys.platform == 'darwin':
    QT_PLUGINS_DIR = '/Developer/Applications/Qt/plugins'

data_files = [
    ('templates', glob('templates/*')),
    ('static', glob('static/*')),
]

QT_PLUGINS = {
    'phonon_backend': ['*'],
    'imageformats': ['*jpeg*'],
}
for cata, files in QT_PLUGINS.items():
    data_files.append(('plugins/'+cata,
                       sum((glob(os.path.join(QT_PLUGINS_DIR, cata, f))
                            for f in files),
                           [])
                      ))

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
