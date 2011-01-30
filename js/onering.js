ONERING = new Object();

ONERING.Base = function() {};
ONERING.Base.prototype = {
	_call: function(command, param) {
		if (!param) { param = {}; }
		param.id = this.id;
		return ONERING.call_app(this.appname, command, param);
	},
	_create: function(command, param) {
		var obj = ONERING.call_app(this.appname, command, param);
		if (!(this.validate_type(obj))) {
			throw new Error(this.type + " not created");
		}
		return obj;
	},
	_destroy: function(command) {
		if (!this.id) { return; }
		var r = this._call(command);
		this.id = null;
		return r;
	},
	validate_type: function(obj) {
		return (obj && obj.type == this.type);
	},
	bind: function(event, callback) {
		ONERING.subscribe(this.appname+"."+this.type+"."+this.id+"."+event, callback);
	},
	extend: function(d) {
		for (var k in d) {
			this[k] = d[k];
		}
		return this;
	},
};

// Application {{{

ONERING.Application = function() {
};
ONERING.Application.prototype = (new ONERING.Base()).extend({
		appname: "onering",
		type: "Application",
		setQuitOnLastWindowClosed: function(quit) {
			return this._call("Application.setQuitOnLastWindowClosed", {quit: quit}),
		}
	});

ONERING.getApplication = function() {
	return new ONERING.Application();
};

// }}}

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

};

ONERING.Window = function(obj) {
	if (!this.validate_type(obj)) {
		throw new Error("invalid type");
	}
	this.id = obj.id;
};
ONERING.Window.prototype = (new ONERING.Base()).extend({
		appname: "onering",
		type: "Window",
		createWindow: function(url, width, height, props) {
			url = ONERING.resolve(url);
			props.url = url;
			props.width = width;
			props.height = height;
			var window = this._create("Window.create", props);
			return new ONERING.Window(window);
		},
		isAlive: function() {
			return this._call("Window.isAlive");
		},
		showInspector: function() {
			return this._call("Window.showInspector");
		},
		hide: function() {
			return this._call("Window.hide");
		},
		show: function() {
			return this._call("Window.show");
		},
		maximize: function() {
			return this._call("Window.maximize");
		},
		showNormal: function() {
			return this._call("Window.showNormal");
		},
		isMinimized: function() {
			return this._call("Window.isMinimized");
		},
		activateWindow: function() {
			return this._call("Window.activateWindow");
		}

	});

ONERING.window = new ONERING.Window({type: "Window",
	   								 id: _OneRing.getCurrentWindowId()});

ONERING.getCurrentWindow = function() {
	return ONERING.window;
};

// shortcuts
ONERING.createWindow = function(url, width, height, props) {
	return ONERING.getCurrentWindow().createWindow(url, width, height, props);
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
	this.id = this._create("create").id;
	if (url) {
		this.load(url);
	}
};
ONERING.SystemTrayIcon.prototype = (new ONERING.Base()).extend({
		appname: "systray",
		type: "SystemTrayIcon",
		destroy: function() {
			this._destroy("destroy");
		},
		load: function(url) {
			return this._call("load", {url: ONERING.resolve(url)});
		},
		setContextMenu: function(menu) {
			return this._call("setContextMenu", {menuId: menu.id});
		}
	});

// }}}

// Menu {{{

ONERING.Menu = function(items) {
	this.id = this._create("Menu.create").id;
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
ONERING.Menu.prototype = new ONERING.Base();
ONERING.Menu.prototype.extend({
		appname: "menu",
		type: "Menu",
		destroy: function() {
			this._destroy("Menu.destroy");
		},
		addSeparator: function() {
			return this._call("Menu.addSeparator");
		},
		addItem: function(title, callback, props) {
			if (!(callback instanceof Function)) {
				props = callback;
				callback = null;
			}
			var item = new ONERING.MenuItem(this._call("Menu.addMenuItem", {text: title}));
			if (callback) {
				item.bind('triggered', callback);
			}
			if (props) {
				item.setProperties(props);
			}
		},
		get: function(index) {
			return new ONERING.MenuItem(this._call("Menu.getMenuItem", {index: index}));
		}
	});

ONERING.MenuItem = function(item) {
	if (!item || item.type != this.type) {
		throw new Error("invalid menu item");
	}
	this.id = item.id;
};
ONERING.MenuItem.prototype = (new ONERING.Base()).extend({
		appname: "menu",
		type: "MenuItem",
		setProperties: function(props) {
			return this._call("MenuItem.setProperties", props);
		},
		setText: function(text) {
			return this._call("MenuItem.setText", {text: text});
		},
		setEnabled: function(enabled) {
			return this.setProperties({enabled: enabled});
		},
	});

// }}}

// functions {{{


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
	var data = settings.data || "";
	if ( data instanceof Object ) {
		data = ONERING.param(data);
	}
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
					s = window.JSON.parse(s);
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

ONERING.call_app = function(appname, command, param) {
	var url = appname ? ("onering://"+appname+"/"+command) : ("/"+command);
	var data = JSON.stringify(param || {});
	var r = _OneRing.call("POST", url, data);
	r = JSON.parse(r);
	if (r && r.err) {
		throw new Error("ONERING.call_app("+appname+", "+command+") failed: " + r.err);
	}
	return r;
};

ONERING.subscribe = function(channel, callback) {
	ONERING.connect(_OneRing.getPubSubHub().published, function(ch, msg) {
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

ONERING.resolve = function(url) {
	return _OneRing.resolve(url);
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

// vim:set foldmethod=marker:
