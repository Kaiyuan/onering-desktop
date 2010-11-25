import logging
from cStringIO import StringIO
try:
    import json
except ImportError:
    import simplejson as json

import _onering

__all__ = ['register_wsgi_app', 'loop', 'publish']

logger = logging.getLogger('onering')

def register_wsgi_app(appname, app):
    def callback(method, path, body=None):
        response = []
        environ = {
            'REQUEST_METHOD': method,
            'SCRIPT_NAME': '',
            'PATH_INFO': path,
            'HTTP_HOST': appname,
        }
        if body is not None:
            environ['wsgi.input'] = StringIO(body)
            environ['CONTENT_LENGTH'] = len(body)
        def start_response(status, response_headers, exc_info=None):
            response.append("HTTP/1.1 %s\r\n" % status)
            response.extend("%s: %s\r\n" % h for h in response_headers)
            response.append("\r\n")
        response.extend(app(environ, start_response))
        response = ''.join(response)
        logger.debug("%s %s %s -> %d bytes", method, path, body,
                     len(response))
        return response
    return _onering.register_app(appname, callback)

def loop(appname):
    return _onering.loop(appname)

def publish(channel, data=None):
    msg = json.dumps(data)
    return _onering.publish(channel, msg)
