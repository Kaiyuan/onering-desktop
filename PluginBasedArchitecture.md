# 基于插件的架构 (Draft) #


一个完整的 OneRing 应用程序分成两个部分：
  1. 一个 OneRingCore
  1. 零个或多个 OneRingApp

## OneRingCore ##

每个Core已动态链接库的形态发布，需要实现以下内容:

  * 提供 onering.h 的实现(C API)，包括
    * `onering_register_app(appname, app_func, free_response_func)` -- 注册 OneRingApp
    * `onering_loop(appname)` -- 启动某个 OneRingApp
    * `const char* onering_version()` -- 输出 OneRingCore 的版本号，如 "OneRingQtCore 1.0.0"
    * `onering_load_plugins(const char* dir)` -- 加载某个目录下的所有 OneRingPlugin
    * `onering_response_handle_t onering_call_app(const char* appname, const char* method, const char*body, int body_len, char** response, int* response_len)` -- 调用某个app
    * `void onering_free_response(const char* appname, onering_response_handle_t response_handle)` -- 释放app调用的response资源
    * `void onering_subscribe(const char* channel, const char* callback_appname, const char* callback_method)` -- 订阅某个channel，当有信息向此channel发布时，调用指定app，message作为body。调用完成后，将自动调用 onering\_free\_response() 释放 respones 资源
    * `void onering_unsubscribe(const char* channel, const char* callback_appname, const char* callback_method)` -- 解除对某个channel的订阅
    * `onering_publish(channel, msg)` -- 向某个通道发布信息
  * 实现一个OneRingApp: onering，并自动注册。该app提供 onering://onering/onering.js，其中包含以下定义
    * `ONERING.Base` -- OneRingApp 中定义的native对象的基类，提供如下方法：
      * `bind(event, callback)` -- 绑定某个事件，当事件发生时，回调callback
      * `unbind(event)` -- 解绑定某个事件
      * `extend(obj)` -- 把 obj 扩展到自身的属性列表中（在app中继承Base时使用，见 OneRingPlugin 中的示例）
      * `_call(method[, param])` -- 调用native函数
      * `_create([param])` -- 调用native的create函数，创建一个新对象，并检查对象的type属性是否为this.type
      * `_destroy()` -- 调用native的destroy函数，销毁对象
    * 系统native对象，包括
      * `ONERING.application` -- 提供下列方法:
        * `setQuitOnLastWindowClosed(quit)`
        * `exit()`
      * `ONERING.window` -- 提供下列方法：
        * `createWindow(url, width, height, props)`
        * `showInspector()`
    * pub/sub实现
      * `ONERING.subscribe(channel, callback)`
      * `ONERING.unsubscribe([channel, [callback]])`
    * native调用
      * `ONERING.call_app(appname, command, param)`
      * `ONERING.call_app_async(appname, command, param, callback)`
    * 工具函数
      * `ONERING.log(obj)`
      * `ONERING.version()`

加载 OneRingPlugin 的流程
  1. 遍历`dir`参数所指目录下的所有文件（不包括子目录）
  1. 对每一个文件，尝试以动态链接库加载
  1. 若加载成功，调用其 require\_onering\_version() 方法，与 onering\_version() 的输出进行比较，若不匹配，输出调试信息并跳过
  1. 若匹配，调用其 register\_onering\_plugin(get\_onering\_appname()) 方法完成注册。如果返回非0值，表示注册失败，输出调试信息并跳过该plugin

目前OneRingCore有下列实现：
  1. OneRingQtCore -- 基于 Qt 的 core


## OneRingPlugin ##

每个 OneRingPlugin 都是一个以C DLL的方式发布的 OneRingApp ，提供了以下入口函数：

  * `const char* require_onering_version()`
> > 返回该app依赖的 OneRingCore 最小版本。输出的 version 字符串由"|"字符分隔为若干个部分，每个部分形如"OneRingQtCore 1.0"，表示该plugin能够正常运行所需的最小core版本号。多个"|"分割的core版本，只要有一个能匹配，即认为该plugin可以运行。

  * `const char* get_onering_appname()`
> > 返回该plugin会被注册到的appname。

  * `int register_onering_plugin(const char* appname)`
> > 用appname为名注册该plugin。

每个 OneRingPlugin 应提供一个 onering://{appname}/{appname}.js 文件，在其中可以定义一个到多个native类的代理js类，该类应定义在 ONERING.{appname} 名字空间下。一般而言，在构造函数中调用`this._create`创建native对象，在自定义方法中调用`this._call`调用native对象的方法。一个典型的例子如下
```
ONERING.hotkey = new Object();
ONERING.hotkey.HotKey = function(shortcut, callback) {
    this._create({shortcut: shortcut});
    this.bind("activated", callback);
};
ONERING.hotkey.HotKey.prototype = (new ONERING.Base()).extend({
    appname: "hotkey",  // required
    type: "HotKey",  // required
    destroy: function() { return this._destroy(); },
    enable: function() { return this._call("enable", {enabled: true});},
    disable: function() { return this._call("enable", {enabled: false});},
});
```

`this._call(command, param)` 会调用 `ONERING.call_app(this.appname, command, param)` ， `param`中会增加一个`id`属性，值为this.id。 `this._create(param)` 相当于 `this._call("create", param)`，并且设置`this.id`为返回值json中的`id`属性。 `this._destroy()` 相当于 `this._call("destroy")`，并且会将`this.id`置为`null`。

`ONERING.call_app(appname, command, param)` 会调用 POST onering://{appname}/{command} ， param作为json字符串的body传给app。response为一个json对象，如果该json对象有`err`属性，则抛出异常，否则返回json对象作为`ONERING.call_app()`的返回值。

为了简化app对call\_app POST请求的处理，core应当提供一些帮助函数，包括下列函数:
  * onering\_json\_t**onering\_parse\_json(const char** body)

一个典型的C plugin代码如下:

```
#include <onering.h>
#include <string.h>

static char* process_call(const char* path, onering_json_t* param)
{
    char* response = malloc(4096);
    if (strcmp(path, "myplugin.js") == 0) {
        strcpy(*response, MYPLUGIN_JS);  // MYPLUGIN_JS 是字符串常量
    } else if (strcmp(path, "version") == 0) {
        strcpy(*response, "1.0");
    } else {
        strcpy(*response, "{\"err\": \"unknown command\"}");
    }
    return response;
}

static void* app(const char* appname, const char* method, const char* path, const char*body,
                                     char** response, int* response_len)
{
    assert(path[0] == '/');
    onering_json_t* param = onering_parse_json(body);
    *response = process_call(path+1, param);
    *response_len = strlen(*response);
    return (void*)(*response);
}

static void app_free_response(const char* appname, void* response)
{
    free(response);
}

int register_onering_plugin(const char* appname)
{
    return onering_register_app(appname, &app, &app_free_response);
}

const char* require_onering_version()
{
    return "OneRingQtCore 1.0";
}

const char* get_onering_appname()
{
    return "myplugin";
}
```