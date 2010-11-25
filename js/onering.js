ONERING = new Object();

// Window class {{{

ONERING.Window = function(q) {
    this.q = q;
    this.boundEvents = {};
    this.q.eventOccurred.connect(this, "_eventOccurred");
    var w = this;
    window.addEventListener('unload', function(){
	    ONERING.log("unload");
	    w.q.eventOccurred.disconnect(w, "_eventOccurred");
	    w.unbind();
	}, false);
};

ONERING.Window.prototype._eventOccurred = function(event) {
    var callbacks = this.boundEvents[event.type()];
    if (callbacks) {
	callbacks.forEach(function(callback) {
		callback(event);
	    });
    }
};

ONERING.Window.prototype.bind = function(event, callback) {
    this.q.bind(event);
    if (!this.boundEvents[event]) {
	this.boundEvents[event] = [];
    }
    this.boundEvents[event].push(callback);
};

ONERING.Window.prototype.unbind = function(event, callback) {
    if (event) {
	var callbacks = this.boundEvents[event];
	if (callbacks) {
	    var new_callbacks;
	    if (callback) {
		new_callbacks = callbacks.filter(function(c) {
			return c !== callback;
		    });
	    } else {
		new_callbacks = [];
	    }
	    var unbind_count = callbacks.length - new_callbacks.length;
	    if (unbind_count) {
		this.q.unbind(event, unbind_count);
		if (new_callbacks.length) {
		    this.boundEvents[event] = new_callbacks;
		} else {
		    delete this.boundEvents[event];
		}
	    }
	}
    } else {
	for (var e in this.boundEvents) {
	    this.q.unbind(e, this.boundEvents[e].length);
	}
	this.boundEvents = {};
    }
};

ONERING.Window.prototype.createWindow = function(url, width, height, props) {
    return new ONERING.Window(_OneRing.createWindow(url, width, height, JSON.stringify(props)));
};

ONERING.Window.prototype.isAlive = function() {
    return _OneRing.checkAlive(this.q);
};

ONERING.Window.prototype.showInspector = function() {
    return _OneRing.showInspector();
};

ONERING.Window.prototype.enableContextMenu = function() {
    _OneRing.Window_enableContextMenu();
};

ONERING.Window.prototype.disableContextMenu = function() {
    _OneRing.Window_disableContextMenu();
};

ONERING.Window.prototype.hide = function() {
    this.q.hide();
};
ONERING.Window.prototype.show = function() {
    this.q.show();
};
ONERING.Window.prototype.maximize = function() {
    this.q.showMaximized();
};
ONERING.Window.prototype.showNormal = function() {
    this.q.showNormal();
};
ONERING.Window.prototype.isMinimized = function() {
    return this.q.minimized;
};
ONERING.Window.prototype.activateWindow = function() {
    return this.q.activateWindow();
};

// }}}

// Audio class {{{

ONERING.Audio = Audio;
ONERING.Audio.prototype.bind = function(event, callback) {
    return this.addEventListener(event, callback);
};

// }}}

// System Tray Icon {{{

ONERING.SystemTrayIcon = function(url) {
    this.handler = _OneRing.SystemTrayIcon_new();
    if (url) {
	this.load(url);
    }
};
ONERING.SystemTrayIcon.prototype.destroy = function() {
    if (!this.handler) return;
    _OneRing.SystemTrayIcon_delete(this.handler);
    this.handler = null;
};
ONERING.SystemTrayIcon.prototype.load = function(url) {
    if (!this.handler) return;
    _OneRing.SystemTrayIcon_load(this.handler, url);
};
ONERING.SystemTrayIcon.prototype.bind = function(event, callback) {
    if (!this.handler) return;
    if (event == 'click') {
	var callback_name = _register_function(callback);
	_OneRing.SystemTrayIcon_bind(this.handler, event, callback_name);
    }
};
ONERING.SystemTrayIcon.prototype.getGeometry = function() {
    if (!this.handler) return;
    return _OneRing.SystemTrayIcon_getGeometry(this.handler);
};
ONERING.SystemTrayIcon.prototype.setContextMenu = function(menu) {
    if (!this.handler) return;
    _OneRing.SystemTrayIcon_setContextMenu(this.handler, menu.handler);
}

// }}}

// Menu {{{

ONERING.Menu = function(items) {
    this.handler = _OneRing.Menu_new();
    for (var i in items) {
	var item = items[i];
	if (item === ONERING.Menu.SEPARATOR) {
	    this.addSeparator();
	} else {
	    this.addItem(item[0], item[1], item[2]);
	}
    }
};
ONERING.Menu.SEPARATOR = Object();  // a const
ONERING.Menu.prototype.destroy = function() {
    if (!this.handler) return;
    _OneRing.Menu_delete(this.handler);
    this.handler = null;
}
ONERING.Menu.prototype.addSeparator = function() {
    if (!this.handler) return;
    _OneRing.Menu_addSeparator(this.handler);
};
ONERING.Menu.prototype.addItem = function(title, callback, props) {
    if (!this.handler) return;
    if (callback instanceof Function) {
	callback = _register_function(callback);
    } else {
	if (callback instanceof Object) {
	    props = callback;
	}
	callback = "";
    }
    _OneRing.Menu_addItem(this.handler, title, callback, props);
};
ONERING.Menu.prototype.get = function(index) {
    return new ONERING.MenuItem(_OneRing.Menu_get(this.handler, index));
};

ONERING.MenuItem = function(qaction) {
    this.q = qaction;
};
ONERING.MenuItem.prototype.setText = function(text) {
    this.q.text = text;
};
ONERING.MenuItem.prototype.setEnabled = function(enabled) {
    this.q.setEnabled(enabled);
};

// }}}

// functions {{{

ONERING.getCurrentWindow = function() {
    return ONERING.window;
};


ONERING.createWindow = function(url, width, height, props) {
    return ONERING.getCurrentWindow().createWindow(url, width, height, props);
};

ONERING.log = function(o) {
    return _OneRing.log(o);
};

var hotkey_handlers = {};
ONERING.setHotKey = function(shortcut, func) {
    var handler = _OneRing.HotKey_new(shortcut, _register_function(func));
    hotkey_handlers[shortcut] = handler;
}
ONERING.clearHotKey = function(shortcut) {
    var handler = hotkey_handlers[shortcut];
    if (handler) {
	_OneRing.HotKey_delete(handler);
	delete hotkey_handlers[shortcut];
    }
}

ONERING.callback = function(name, para) {
    var window_id = name.split('_')[1];
    if (window_id != ONERING._window_id) {
	return;
    }
    var f = _get_registered_function(name);
    if (f) {
	f(para);
    }
};

ONERING.exit = function() {
    _OneRing.exit();
};

ONERING.ajax = function(settings) {
    var url = settings.url;
    var data = ONERING.param(settings.data || {});
    var type = settings.type || "GET";
    var dataType = settings.dataType;
    var success = settings.success;

    if (data && type == "GET") {
	url += ((url.indexOf('?') < 0) ? '?' : '&') + data;
	data = "";
    }

    _OneRing.ajax(type, url, data, _register_function(function(s) {
	    if (dataType == "json") {
		s = eval('('+s+')');
	    }
	    success && success(s);
	}));
};

ONERING.get = function(url, data, callback, dataType) {
    if (data instanceof Function) {
	dataType = dataType || callback;
	callback = data;
	data = null;
    }

    return ONERING.ajax({
	    type: "GET",
	    url: url,
	    data: data,
	    success: callback,
	    dataType: dataType
	});
};

ONERING.getJSON = function(url, data, callback) {
    return ONERING.get(url, data, callback, "json");
};

ONERING.post = function(url, data, callback, dataType) {
    if (data instanceof Function) {
	dataType = dataType || callback;
	callback = data;
	data = null;
    }

    return ONERING.ajax({
	    type: "POST",
	    url: url,
	    data: data,
	    success: callback,
	    dataType: dataType
	});
};

ONERING.bind = function(event, callback) {
    var signal = _OneRing.getApplication()[event];
    if (signal) {
	signal.connect(callback);
    }
};

ONERING.subscribe = function(channel, callback) {
    ONERING.connect(_OneRing.getPubSubHub().published, function(ch, msg) {
	    ONERING.log(ch);
	    if (ch == channel) {
		var data = eval('('+msg+')');
		callback(data);
	    }
	});
};

ONERING._connections = [];

ONERING.connect = function(signal, slot) {
    ONERING._connections.push([signal, slot]);
    signal.connect(slot);
}

window.addEventListener('unload', function() {
	var signal, slot;
	ONERING._connections.forEach(function(connection){
	    signal = connection[0];
	    slot = connection[1];
	    signal.disconnect(slot);
	});
    });

ONERING.Application = function(q) {
    this.q = q;
};
ONERING.Application.prototype = {
    setQuitOnLastWindowClosed: function(quit) {
	this.q.quitOnLastWindowClosed = quit;
    }
}

ONERING.getApplication = function() {
    return new ONERING.Application(_OneRing.getApplication());
};

// }}}

// internal utilities {{{

var _guid = 0;
var _get_guid = function() {
    _guid += 1;
    return '' + _guid;
};

ONERING._window_id = Math.floor(Math.random()*1000000);

var _registered_functions = new Object();
var _register_function = function(func) {
    var name = 'f_' + ONERING._window_id + '_' + _get_guid();
    _registered_functions[name] = func;
    return name;
};
var _get_registered_function = function(name) {
    return _registered_functions[name];
};

ONERING.param = function(a) {
    var s = [];
    for (var key in a) {
	var value = a[key];
	s.push( encodeURIComponent(key) + "=" + encodeURIComponent(value) );
    }
    return s.join("&").replace(/%20/g, "+");
};

ONERING.Dict = function() {
};
ONERING.Dict.prototype.setDefault = function(key, defaultValue) {
    if (this[key] === undefined) {
	this[key] = defaultValue;
    }
    return this[key];
};

// }}}

ONERING.window = new ONERING.Window(_OneRing.getCurrentWindow());

// vim:set foldmethod=marker:
