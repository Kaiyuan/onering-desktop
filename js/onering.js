ONERING = new Object();

// Window class

ONERING.Window = function() {
};

ONERING.Window.prototype.createWindow = function(url, width, height, props) {
    return _OneRing.createWindow(url, width, height, JSON.stringify(props));
};

ONERING.Window.prototype.showInspector = function() {
    return _OneRing.showInspector();
}

ONERING.Window.prototype.hide = function() {
    return _OneRing.Window_hide();
}
ONERING.Window.prototype.maximize = function() {
    return _OneRing.Window_maximize();
}
ONERING.Window.prototype.showNormal = function() {
    return _OneRing.Window_showNormal();
}

ONERING.window = new ONERING.Window();

// Audio class

ONERING.Audio = Audio;
ONERING.Audio.prototype.bind = function(event, callback) {
    return this.addEventListener(event, callback);
};

// System Tray Icon

ONERING.SystemTrayIcon = function(url) {
    this.handler = _OneRing.SystemTrayIcon_new();
    if (url) {
	this.load(url);
    }
};
ONERING.SystemTrayIcon.prototype.load = function(url) {
    _OneRing.SystemTrayIcon_load(this.handler, url);
};
ONERING.SystemTrayIcon.prototype.bind = function(event, callback) {
    if (event == 'click') {
	var callback_name = _register_function(callback);
	_OneRing.SystemTrayIcon_bind(this.handler, event, callback_name);
    }
};
ONERING.SystemTrayIcon.prototype.getGeometry = function() {
    return _OneRing.SystemTrayIcon_getGeometry(this.handler);
};
ONERING.SystemTrayIcon.prototype.setContextMenu = function(menu) {
    _OneRing.SystemTrayIcon_setContextMenu(this.handler, menu.handler);
}

// Menu

ONERING.Menu = function(items) {
    this.handler = _OneRing.Menu_new();
    for (var i in items) {
	var item = items[i];
	if (item === ONERING.Menu.SEPARATOR) {
	    this.addSeparator();
	} else {
	    this.addItem(item[0], item[1] || null);
	}
    }
};
ONERING.Menu.SEPARATOR = Object();  // a const
ONERING.Menu.prototype.addSeparator = function() {
    _OneRing.Menu_addSeparator(this.handler);
};
ONERING.Menu.prototype.addItem = function(title, callback) {
    if (callback instanceof Function) {
	callback = _register_function(callback);
    } else {
	callback = "";
    }
    _OneRing.Menu_addItem(this.handler, title, callback);
};

// functions

ONERING.getCurrentWindow = function() {
    return ONERING.window;
};

ONERING.createWindow = function(url, width, height, props) {
    ONERING.getCurrentWindow().createWindow(url, width, height, props);
};

ONERING.log = function(o) {
    return _OneRing.log(o);
};

var hotkeys = {};
ONERING.setHotKey = function(shortcut, func) {
    var hotkey = ONERING._setHotKey(shortcut, _register_function(func));
    hotkeys[shortcut] = hotkey;
}
ONERING.clearHotKey = function(shortcut) {
    var hotkey = hotkeys[shortcut];
    ONERING._clearHotKey(hotkey);
}

ONERING.callback = function(name) {
    var f = _get_registered_function(name);
    if (f) {
	f();
    }
};

ONERING.exit = function() {
    _OneRing.exit();
}

// internal utilities

var _guid = 0;
var _get_guid = function() {
    _guid += 1;
    return '' + _guid;
}

var _registered_functions = new Object();
var _register_function = function(func) {
    var name = 'f' + _get_guid();
    _registered_functions[name] = func;
    return name;
}
var _get_registered_function = function(name) {
    return _registered_functions[name];
}


