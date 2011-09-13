all: test plugin

test: test.c
	gcc -o test $^ -I ../../include -F.. -framework OneringQtCore

plugin: testplugin.so testplugin2.so

testplugin.so: testplugin.c
	gcc -o testplugin.so $^ -I ../../include -shared
	cp testplugin.so plugins

testplugin2.so: testplugin2.c
	gcc -o testplugin2.so $^ -I ../../include -shared
	cp testplugin2.so plugins

clean:
	rm test testplugin.so testplugin2.so
