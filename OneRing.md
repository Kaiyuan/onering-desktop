# 桌面应用框架 OneRing #

## 框架目标 ##

OneRing是一个跨平台的桌面应用框架。和Adobe AIR类似，它支持用html/js/css制作用户界面，与之不同的是，它的应用为本地程序，可以直接访问操作系统的数据。

## 架构 ##

一个OneRing应用程序分为两个部分：框架部分和应用部分。

### 框架部分 ###

框架部分为通用框架，各应用都一样。其提供各操作系统上的界面展现，为本地窗口内嵌一个WebKit浏览器，并提供一致的js接口让应用和操作系统功能进行交互。

框架以运行时库的形式提供。

框架以系统原生语言编写，提供其他语言的绑定。

### 应用部分 ###

应用部分为一个web server，负责应用逻辑，用html/css描述界面，用js提供用户交互。

### 运行流程 ###

  1. 应用启动时，会加载框架的运行时库，注册应用url的访问方法（如WSGI），并调用框架的loop函数。
  1. loop函数会访问应用的 `/init` URL，得到一个json数据，描述初始应用窗口的相关参数，如位置、大小、窗口属性、初始页面url等。
  1. 框架根据该数据创建窗口，并让该窗口内的浏览器访问初始页面url，渲染用户界面，然后等待UI事件。
  1. 用户在界面上操作时，可以通过超链接改变整个窗口内容，也可以使用AJAX技术更新窗口内部分内容。
  1. 应用可以让浏览器调用`ONERING`名字空间下的[js函数](OneRingJsInterface.md)，和操作系统进行交互，该名字空间由url `onering://onering/onering.js` 加载。
  1. 应用通过 `bind(event, function)` 函数监听窗口更改大小、移动等操作系统UI事件。
  1. 可以使用pub/sub机制主动推送消息给浏览器。
  1. 调用 `ONERING.exit()` 可以退出应用；所有窗口都关闭后也会退出应用。

### Python应用示例 ###

```
#!/usr/bin/env python
import json
import web
import onering

urls = (
    '/init', 'init',
    '/', 'index',
)

class init:
    def GET(self):
        web.header('Content-Type', 'application/json')
        return json.dumps({'width': 400, 'height': 300, 'url': '/'})

class index:
    def GET(self):
        web.header('Content-Type', 'text/html')
        return """<html>
<head><script type="text/javascript" src="onering://onering/onering.js"></script></head>
<body>
<p>Hello, world!</p>
<button onclick="javascript:ONERING.exit()">Exit</button>
</body></html>"""

app = web.application(urls, globals())

if __name__ == '__main__':
    onering.register_wsgi_app("demo", app.wsgifunc())
    onering.loop("demo")

```


### 各组件之间的交互 ###

一个OneRing应用中，存在三个需要互相通讯的组件:

  1. 操作系统 OS ，控制窗口行为（如最小化窗口），及操作系统相关界面元素（如系统通知图标）
  1. 运行在WebKit内的javascript，控制界面内元素（如程序界面里的按钮）
  1. WSGI应用，控制HTML/JS的生成逻辑，以及应用相关的后台逻辑

#### js -> os ####

当js需要通知os对js所在窗口做某个行为（如最小化），通过调用 onering.js 中的get\_current\_window()返回值（为Window对象）的相应方法实现：

```
var current_window = ONERING.get_current_window();
current_window.minimize();
```

当js需要os执行整个应用层面的操作时（如结束应用），通过调用 onering.js 中的函数实现：
```
ONERING.exit();
```

当js需要os创建一个新窗口时，通过调用createWindow函数实现，并获得新建窗体的句柄实例：
```
new_window = ONERING.createWindow('/a_dialog', 400, 300);
```

当js需要向另一个窗口做某个os行为时（如隐藏另一个窗口），通过调用窗体的句柄实例的方法实现：
```
new_window.hide();
```

#### os -> js ####

js可以通过bind机制，关注操作系统发生的时间（如窗口大小改变）：
```
current_window.bind('resize', function(event) {
    if (event.width < 300) {
        $('#sidebar').hide();
    }
});
```

#### js -> app ####

js可以通过类AJAX调用，从app获取数据。
```
ONERING.getJSON('/userdata.json', function(data) {
    ...
});
```

#### app -> js ####

app希望向窗体内的js主动推送消息时，可以采用pub/sub机制。

首先js通知框架自己关注的事件:
```
current_window.subscribe("a_file_scanned", function(fileinfo) {
  ...
});
```

然后当app希望发送消息时，调用onering框架提供的publish方法：
```
for fileinfo in scan_files():
    onering.publish("a_file_scanned", fileinfo);
```
其中的fileinfo为可以转化为JSON的python对象（即 json.dumps(fileinfo) 不会抛出异常）。