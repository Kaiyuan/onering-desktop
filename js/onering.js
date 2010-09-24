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
	var callback_name = ONERING._register_function(callback);
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
	    this.addItem(item);
	}
    }
};
ONERING.Menu.SEPARATOR = Object();  // a const
ONERING.Menu.prototype.addSeparator = function() {
    _OneRing.Menu_addSeparator(this.handler);
};
ONERING.Menu.prototype.addItem = function(item) {
    var title = item[0];
    var callback = item[1];
    if (callback instanceof Function) {
	callback = ONERING._register_function(callback);
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

ONERING.callback = function(name) {
    var f = ONERING._get_registered_function(name);
    if (f) {
	f();
    }
};

// internal utilities

ONERING._guid = 0;
ONERING._get_guid = function() {
    ONERING._guid += 1;
    return '' + ONERING._guid;
}

ONERING._registered_functions = new Object();
ONERING._register_function = function(func) {
    var name = 'f' + ONERING._get_guid();
    ONERING._registered_functions[name] = func;
    return name;
}
ONERING._get_registered_function = function(name) {
    return ONERING._registered_functions[name];
}


