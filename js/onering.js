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
    this.handler = _OneRing.systemTrayIcon_new();
    if (url) {
	this.load(url);
    }
};
ONERING.SystemTrayIcon.prototype.load = function(url) {
    _OneRing.systemTrayIcon_load(this.handler, url);
};
ONERING.SystemTrayIcon.prototype.bind = function(event, callback) {
    if (event == 'click') {
	var callback_name = ONERING._register_function(callback);
	_OneRing.systemTrayIcon_bind(this.handler, event, callback_name);
    }
};
ONERING.SystemTrayIcon.prototype.getGeometry = function() {
    return _OneRing.systemTrayIcon_getGeometry(this.handler);
}

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


