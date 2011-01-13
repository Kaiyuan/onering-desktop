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

ONERING.Window.prototype = {
    _eventOccurred: function(event) {
	var callbacks = this.boundEvents[event.type()];
	if (callbacks) {
	    callbacks.forEach(function(callback) {
		    callback(event);
		});
	}
    },

    bind: function(event, callback) {
	this.q.bind(event);
	if (!this.boundEvents[event]) {
	    this.boundEvents[event] = [];
	}
	this.boundEvents[event].push(callback);
    },

    unbind: function(event, callback) {
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
    },

    createWindow: function(url, width, height, props) {
	return new ONERING.Window(_OneRing.createWindow(url, width, height, props));
    },

    isAlive: function() {
	return _OneRing.checkAlive(this.q);
    },

    showInspector: function() {
	return _OneRing.showInspector();
    },

    enableContextMenu: function() {
	this.q.enableContextMenu();
    },

    disableContextMenu: function() {
	this.q.disableContextMenu();
    },

    hide: function() {
	this.q.hide();
    },
    show: function() {
	this.q.show();
    },
    maximize: function() {
	this.q.showMaximized();
    },
    showNormal: function() {
	this.q.showNormal();
    },
    isMinimized: function() {
	return this.q.minimized;
    },
    activateWindow: function() {
	return this.q.activateWindow();
    }
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
    this.q = _OneRing.SystemTrayIcon_new();
    if (url) {
	this.load(url);
    }
};
ONERING.SystemTrayIcon.prototype = {
    destroy: function() {
	this.q.deleteLater();
	this.q = null;
    },
    load: function(url) {
	this.q.load(_OneRing.resolve(url));
    },
    bind: function(event, callback) {
	ONERING.connect(this.q[event], callback);
    },
    getGeometry: function() {
	return this.q.getGeometry();
    },
    setContextMenu: function(menu) {
	this.q.setContextMenu(menu.q);
    }
};

// }}}

// Menu {{{

ONERING.Menu = function(items) {
    this.q = _OneRing.Menu_new();
    for (var i=0; i<items.length; i++) {
	var item = items[i];
	if (item === ONERING.Menu.SEPARATOR) {
	    this.addSeparator();
	} else {
	    this.addItem(item[0], item[1], item[2]);
	}
    };
};
ONERING.Menu.SEPARATOR = Object();  // a const
ONERING.Menu.prototype = {
    destroy: function() {
	this.q.deleteLater();
	this.q = null;
    },
    addSeparator: function() {
	this.q.addSeparator();
    },
    addItem: function(title, callback, props) {
	if (!(callback instanceof Function)) {
	    props = callback;
	    callback = null;
	}
	var item = new ONERING.MenuItem(this.q.addAction(title));
	if (callback) {
	    item.bind('triggered', callback);
	}
	if (props) {
	    item.setProperties(props);
	}
    },
    get: function(index) {
	return new ONERING.MenuItem(this.q.get(index));
    }
};

ONERING.MenuItem = function(qaction) {
    this.q = qaction;
};
ONERING.MenuItem.prototype = {
    bind: function(event, callback) {
	ONERING.connect(this.q[event], callback);
    },
    setProperties: function(props) {
	if (props.shortcut !== undefined) {
	    this.q.shortcut = props.shortcut;
	    this.q.shortcutContext = 2; // ApplicationShortcut
	}
	if (props.enabled !== undefined) {
	    this.q.enabled = props.enabled;
	}
	if (props.disabled !== undefined) {
	    this.q.enabled = !props.disabled;
	}
    },
    setText: function(text) {
	this.q.text = text;
    },
    setEnabled: function(enabled) {
	this.q.setEnabled(enabled);
    },
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

ONERING._hotkeys = {};
ONERING.setHotKey = function(shortcut, func) {
    var hotkey = ONERING._hotkeys[shortcut];
    if (hotkey === undefined) {
	var hotkey = _OneRing.HotKey_new(shortcut);
	ONERING._hotkeys[shortcut] = hotkey;
    }
    ONERING.connect(hotkey.activated, func);
};
ONERING.clearHotKey = function(shortcut) {
    var hotkey = ONERING._hotkeys[shortcut];
    if (hotkey !== undefined) {
	hotkey.enabled = false;
	hotkey.deleteLater();
	delete ONERING._hotkeys[shortcut];
    }
};

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
    var async = settings.async || true;

    if (data && type == "GET") {
	url += ((url.indexOf('?') < 0) ? '?' : '&') + data;
	data = "";
    }

    _OneRing.ajax(type, url, data, _register_function(function(s) {
	    if (dataType == "json") {
		s = eval('('+s+')');
	    }
	    success && success(s);
	}), async);
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

ONERING.call = function(type, url, data, callback) {
    if (data instanceof Function) {
	callback = data;
	data = null;
    }

    return ONERING.ajax({
	    type: type,
	    url: url,
	    data: data,
	    success: callback,
	    dataType: 'json',
	    async: false
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
	    try {
		signal.disconnect(slot);
	    } catch (e) {
	    }
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

// }}}

ONERING.window = new ONERING.Window(_OneRing.getCurrentWindow());

// vim:set foldmethod=marker:
