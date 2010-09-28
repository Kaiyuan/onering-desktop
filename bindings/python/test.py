#!/usr/bin/env python
# encoding: UTF-8

import sys
import logging
try:
    import json
except ImportError:
    import simplejson as json

import onering

logging.basicConfig(level=logging.DEBUG)

def app(environ, start_response):
    start_response('200 OK', [])
    path = environ['PATH_INFO']
    if path == '/init':
        ret = json.dumps({'width':400, 'height':300, 'title':"Douban Radio",
                          'url': '/',
                         })
    elif path == '/':
        ret = """<html>
<head>
<title>OneRing测试</title>
<script type="text/javascript" src="onering://onering/onering.js"></script>
</head>
<body>
<img src="http://www.douban.com/pics/logosmall.gif" />
<br/>
<a href="#"
onclick="javascript:ONERING.getCurrentWindow().createWindow('/', 100, 100);return false">创建新窗口</a>
</body>
</html>
"""
    else:
        ret = ""
    return [ret]

onering.register_wsgi_app('test', app)
onering.loop("test")
