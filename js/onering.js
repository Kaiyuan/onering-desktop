ONERING = new Object();

// Window class {{{

ONERING.Window = function(q) {
    this.q = q;
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

ONERING.window = new ONERING.Window(_OneRing.getCurrentWindow());

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

ONERING.callback = function(name) {
    var f = _get_registered_function(name);
    if (f) {
	f();
    }
};

ONERING.exit = function() {
    _OneRing.exit();
};

ONERING.post = function(url, data, callback) {
    if (data instanceof Function) {
	callback = data;
	data = {};
    }

    data = param(data);
    _OneRing.post(url, data, _register_function(callback));
};

// }}}

// internal utilities {{{

var _guid = 0;
var _get_guid = function() {
    _guid += 1;
    return '' + _guid;
};

var _registered_functions = new Object();
var _register_function = function(func) {
    var name = 'f' + _get_guid();
    _registered_functions[name] = func;
    return name;
};
var _get_registered_function = function(name) {
    return _registered_functions[name];
};

var param = function(a) {
    var s = [];
    for (var key in a) {
	var value = a[key];
	s.push( encodeURIComponent(key) + "=" + encodeURIComponent(value) );
    }
    return s.join("&").replace(/%20/g, "+");
};

// }}}

// vim:set foldmethod=marker:
