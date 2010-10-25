APPNAME=demo
VERSION=`python -c "import ${APPNAME}; print ${APPNAME}.__version__"`

deploy:
	rm -rf dist
	python setup.py py2app
	patch -d dist/${APPNAME}.app/Contents/Resources/ < syspath.patch
	./deploy_plugins dist/${APPNAME}.app imageformats/libq{gif,ico,jpeg}.dylib phonon_backend/libphonon_qt7.dylib

dmg:
	hdiutil create dist/${APPNAME}-${VERSION}.dmg -srcfolder dist/${APPNAME}.app -format UDBZ -volname ${APPNAME}
