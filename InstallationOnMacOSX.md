# 从源码编译安装 #

## 安装 OneRing framework ##
  1. 安装 XCode
  1. 安装 [Qt4 for Mac OS X](http://qt.nokia.com/downloads/qt-for-open-source-cpp-development-on-mac-os-x)
  1. [hg clone 一份源码](http://code.google.com/p/onering-desktop/source/checkout)
  1. `cd onering-desktop`
  1. `qmake -spec macx-g++`
  1. `make`
  1. ``ln -s `pwd`/OneRing.framework /Library/Frameworks/OneRing.framework``

## 安装 OneRing Python binding ##
  1. `cd onering-desktop/bindings/python`
  1. /usr/bin/python setup.py install