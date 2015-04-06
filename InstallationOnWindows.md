# 从源代码安装 #

## 编译 OneRing 库 ##
  1. [下载 QT for Windows](http://qt.nokia.com/downloads) (SDK或libraries均可, 4.7.0以上版本) 并安装
  1. [hg clone 一份OneRing源码](http://code.google.com/p/onering-desktop/source/checkout)
  1. 打开 Qt 4.7.0 Command Prompt
  1. cd onering-desktop
  1. qmake CONFIG+=release
  1. 如果是minGW版本Qt:
    * mingw32-make -f Makefile.Release
  1. 如果是VS2008版本Qt:
    * nmake /f Makefile.Release

## 编译安装 python bing ##
  1. 下载安装[Python](http://www.python.org/)
  1. 如果你没有和编译Python时所使用的编译器一致的编译器（对于Python 2.6/2.7而言，是Visual C++ 2008），则需要下载安装 [MinGW](http://www.mingw.org)
  1. cd bindings/python
  1. 如果你的编译器和编译Python的编译器一致：
    * python setup.py build
  1. 如果不一致:
    * python setup.py build -c mingw32
  1. python setup.py install