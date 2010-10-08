import logging
from cStringIO import StringIO

import _onering
from _onering import loop

__all__ = ['register_wsgi_app', 'loop']

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
