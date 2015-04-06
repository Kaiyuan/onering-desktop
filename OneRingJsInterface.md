

# onering.js接口定义 #

在 onering://onering/onering.js 中，应提供一个名字空间ONERING，所有定义的函数和类都应该挂载在该名字空间下。

## 类 ##

### ONERING.Window ###

表征一个窗口，封装对窗口的操作。该类不应在应用js中直接实例化，而应该通过 ONERING.getCurrentWindow() 获得当前窗口的实例，或者用createWindow()的返回值得到其他窗口的实例。

#### ONERING.Window 的方法 ####

  * createWindow(url, width, height)
> > 创建一个新窗口，大小为width\*height，显示url的内容。

  * showInspector()
> > 显示该窗口的WebInspector，供调试使用。仅对debug版OneRing生效。release版OneRing会不做任何事情。

  * hide()
> > 隐藏窗口。

  * maximize()
> > 最大化窗口。

  * showNormal()
> > 还原窗口大小。

### ONERING.Audio ###

表征一个音频播放器。可以通过 new ONERING.Audio(src) 创建。

目前 ONERING.Audio 的接口与 [HTML5 Audio()](http://dev.w3.org/html5/spec/Overview.html#audio) 的接口一致。

#### ONERING.Audio 的方法 ####

  * new ONERING.Audio(src)
> > 创建一个新播放器，加载src指向的音频。src可以缺省，在以后用 .src 属性设置。

  * bind(event, callback)
> > 绑定一个播放器事件，当事件发生时执行callback函数。

  * play()
> > 开始播放。

  * load()
> > 加载 src 指定的音频

#### ONERING.Audio 的属性 ####

  * src
> > 待播放的音频url
  * buffered
  * duration
  * played

#### ONERING.Audio 的事件 ####

  * progress
  * ended
  * error
  * timeupdate

### ONERING.SystemTrayIcon ###

表征一个在系统托盘区的图标。

#### ONERING.SystemTrayIcon 的方法 ####

  * new ONERING.SystemTrayIcon([url](url.md))
> > 创建并显示一个系统托盘图标，其图形为url指定的图片。如果url没有传入，则只创建一个系统托盘图标实例，但并不显示。

  * load(url)
> > 加载url指定的图片作为图标，并显示。

  * bind(event, callback)
> > 当发生event事件时，调用callback函数。目前支持的event有:
      * "click"
      * "rightclick"
      * "doubleclick"

  * setContextMenu(menu)
> > 将一个 `ONERING.Menu` 对象 `menu` 与本图标绑定，点击图标时会显示菜单。

### ONERING.Menu ###

表征一个操作系统菜单。

#### ONERING.Menu 的方法 ####

  * new ONERING.Menu(items)
> > 创建一个菜单对象，并使用数组items中的值初始化菜单。items中的每一个元素取值可以为:
      * ONERING.Menu.SEPARATOR 常量
> > > > 菜单分割线
      * 一个数组
> > > > 其值与addItem()方法的参数列表一致。

  * addSeparator()

> > 增加一个分割线

  * addItem(title, callback)
> > 增加一个可点击的菜单项，当点击时会调用callback函数。callback为可选项。

## 函数 ##

  * getCurrentWindow()
> > 得到表征当前窗口的[Window](#ONERING.Window.md)对象

  * log(s)
> > 在标准错误输出打印一行 log。（仅debug版OneRing）

  * createWindow(url, width, height)
> > 创建一个新窗口

  * exit()
> > 退出应用程序。