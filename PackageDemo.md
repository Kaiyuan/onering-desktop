# Windows #

  1. 安装 [py2exe](http://www.py2exe.org/)
  1. 把 Qt 安装目录下的 plugins 目录复制到 demo 目录 （可以只拷贝需要的plugins，如 imageformats, phonon\_backend 等 ）
  1. cd demo
  1. python setup.py py2exe
  1. 可执行程序在 demo-0.1 目录下

# Mac OS X #

Mac OS X 10.6 自带的 py2app 不能正常工作，而当前py2app的发布版本 0.5.2 在 Mac OS X 10.6 下有 bug ，需要手工安装代码仓库的最新版本。其依赖的 modulegraph-0.8.1 尚未发布，因此也需要从代码仓库手工安装。
  1. hg clone http://bitbucket.org/ronaldoussoren/modulegraph
  1. cd modulegraph
  1. python setup.py install
  1. hg clone http://bitbucket.org/ronaldoussoren/py2app
  1. cd py2app
  1. sudo python setup.py install

  1. cd demo
  1. make -f Makefile.macx
  1. 可执行程序 demo.app 在 dist 目录下
  1. (optional) 制作 dmg:  make -f Makefile.macx dmg