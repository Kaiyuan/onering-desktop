# 从源码编译安装 #

## 安装OneRing framework ##

  1. 安装python开发文件 `sudo apt-get install python-dev`
  1. 安装Qt4开发版本 `sudo apt-get libqt4-dev`
  1. [hg clone 一份源码](http://code.google.com/p/onering-desktop/source/checkout)
  1. `cd onering-desktop`
  1. 看有没有makefile, 如果没有, 就执行 `qmake` 一下
  1. `make`, 如果没有错误的话, 库应该就编出来了
  1. 把库拷贝到系统目录下, 这里要注意libOneRing.so要改名, 因为在运行demo的时候, 程序寻找的是libOneRing1.so这个库
    * `sudo cp libOneRing.so /usr/lib/libOneRing1.so`
    * `sudo cp libOneRing.so.1 /usr/lib/libOneRing.so.1`
  1. 安装完成

## 安装 OneRing Python binding ##
  1. `cd onering-desktop/bindings/python`
  1. /usr/bin/python setup.py install