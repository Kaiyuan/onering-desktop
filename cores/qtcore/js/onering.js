ONERING = new Object();

// Base {{{
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
		ONERING.subscribe(this.appname+"."+this.type+"."+this.id+"."+event,
		   	function(e) {
				if (e instanceof Object && e.event_id) {
					e = new ONERING.Event(e.event_id);
				}
				callback(e);
			});
	},
	unbind: function(event) {
		ONERING.unsubscribe(this.appname+"."+this.type+"."+this.id+"."+event);
	},
	extend: function(d) {
		for (var k in d) {
			this[k] = d[k];
		}
		return this;
	},
};
//}}}

// Event {{{
ONERING.Event = function(id) {
	this.id = id;
};
ONERING.Event.prototype = (new ONERING.Base()).extend({
		appname: "onering",
		type: "Event",
		preventDefault: function() {
			this._call("Event.preventDefault");
		}
	});
// }}}

// Application {{{

ONERING.Application = function() {
	this.id = 0;
};
ONERING.Application.prototype = (new ONERING.Base()).extend({
		appname: "onering",
		type: "Application",
		setQuitOnLastWindowClosed: function(quit) {
			return this._call("Application.setQuitOnLastWindowClosed", {quit: quit}),
		},
		exit: function() {
			this._call("Application.exit");
		},
	});

ONERING.application = new ONERING.Application();
ONERING.getApplication = function() {
	return ONERING.application;
};

// }}}

// Window {{{

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

// subscribe/unsubscribe {{{

ONERING.subscriptions = {};
ONERING.hub = _OneRing.getPubSubHub().published;

ONERING.subscribe = function(channel, callback) {
	var ss = ONERING.subscriptions[channel] = ONERING.subscriptions[channel] || {};
	var f = ss[callback] = ss[callback] || function(ch, msg) {
		if (ch === channel) {
			callback(JSON.parse(msg));
		}
	};
	ONERING.hub.connect(f);
};

ONERING.unsubscribe = function(channel, callback) {
	if (channel === undefined) {
		for (channel in ONERING.subscriptions) {
			ONERING.unsubscribe(channel);
		}
	} else {
		var ss = ONERING.subscriptions[channel];
		if (ss) {
			if (callback === undefined) {
				for (callback in ss) {
					ONERING.hub.disconnect(ss[callback]);
				}
				delete ONERING.subscriptions[channel];
			} else {
				var f = ss[callback];
				if (f) {
					ONERING.hub.disconnect(f);
					delete ss[callback];
				}
			}
		}
	}
};

window.addEventListener('unload', function() {
		ONERING.unsubscribe();
	});

// }}}

// functions {{{

ONERING.log = function(o) {
	return _OneRing.log(o);
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
	ONERING.getApplication().exit();
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
