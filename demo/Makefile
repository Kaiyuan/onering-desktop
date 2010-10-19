deploy:
	rm -rf dist
	python setup.py py2app
	patch -p0 < syspath.patch
	mv dist/demo.app/Contents/MacOS/demo{,.bak}
	cp -f ../OneRing.framework/OneRing dist/demo.app/Contents/MacOS/demo
	macdeployqt dist/demo.app
	mv -f dist/demo.app/Contents/MacOS/demo{.bak,}
