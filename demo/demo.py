#!/usr/bin/env python
# encoding: UTF-8

import sys
try:
    import json
except ImportError:
    import simplejson as json

import web
from web.contrib.template import render_mako

import onering

urls = (
    '/init', 'init',
    '/', 'index',
    '/create_window', 'create_window',
    '/create_window/new', 'create_window_new',
    '/systrayicon', 'systrayicon',
    '/systraymenu', 'systraymenu',
    '/font-face', 'font_face',
    '/shadow', 'shadow',
    '/localstorage', 'localstorage',
    '/hotkey', 'hotkey',
    '/browser', 'browser',
    '/browser/launcher', 'browser_launcher',
    '/static/(.*)', 'static',
)

render = render_mako(
    directories=['templates'],
    input_encoding='utf8',
    output_encoding='utf8',
)

def jsonize(func):
    def _(*a, **kw):
        ret = func(*a, **kw)
        web.header('Content-Type', 'application/json')
        return json.dumps(ret)
    return _

class init:
    @jsonize
    def GET(self):
        return dict(width=640, height=480, title="OneRing演示")

class index:
    def GET(self):
        return render.index()

class create_window:
    def GET(self):
        return render.create_window()

class create_window_new:
    def GET(self):
        return render.create_window_new()

class systrayicon:
    def GET(self):
        return render.systrayicon()

class systraymenu:
    def GET(self):
        return render.systraymenu()

class font_face:
    def GET(self):
        return render.font_face()

class shadow:
    def GET(self):
        return render.shadow()

class localstorage:
    def GET(self):
        return render.localstorage()

class hotkey:
    def GET(self):
        return render.hotkey()

class browser:
    def GET(self):
        return render.browser()

class browser_launcher:
    def POST(self):
        i = web.input()
        url = i.url
        import webbrowser
        webbrowser.open(url)

class static:
    def GET(self, filename):
        content = open('static/%s' % filename, 'rb').read()
        if filename.endswith('.js'):
            web.header('Content-Type', 'text/javascript')
        elif filename.endswith('.ico'):
            web.header('Content-Type', 'image/x-icon')
        else:
            web.header('Content-Type', 'application/octet-stream')
        web.header('Content-Length', len(content))
        return content

app = web.application(urls, globals(), autoreload=True)

if __name__ == '__main__':
    import logging
    if '-v' in sys.argv[1:]:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig()
    onering.register_wsgi_app("demo", app.wsgifunc())
    onering.loop("demo")
