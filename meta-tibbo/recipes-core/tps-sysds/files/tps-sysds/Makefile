DV_LIB = SysDs

all:
	$(CXX) -c -Wall -Werror -fPIC SysDs.cpp
	$(CXX) -shared -Wl,-soname,lib$(DV_LIB).so.0 -o lib$(DV_LIB).so.0.1 SysDs.o
	ln -sf lib$(DV_LIB).so.0.1 lib$(DV_LIB).so.0
	ln -sf lib$(DV_LIB).so.0 lib$(DV_LIB).so

node:
	swig -javascript -node -c++ $(DV_LIB).i
	${NODE_GYP_PFX}node-gyp clean configure build --arch=arm ${NODE_DIR} --verbose

test:
	$(CXX) -Wall -o sysd `pkg-config --cflags --libs libsystemd` sysd.c -l$(DV_LIB) -L.


clean:
	rm -f sysd
	rm -f lib$(DV_LIB).so
	rm -f *.o

install:
	install -d $(PREFIX)/usr/include/ltps/$(DV_LIB)/
	install -m 0644 SysDs.h $(PREFIX)/usr/include/ltps/$(DV_LIB)/
	install -d $(PREFIX)/usr/lib/
	cp -rf lib$(DV_LIB).so* $(PREFIX)/usr/lib/

install-test:
	install -d $(PREFIX)/home/root/dvtests/
	install sysd $(PREFIX)/home/root/dvtests/

install-node:
	${NODE_GYP_PFX}node-gyp install --arch=arm ${NODE_DIR}
	install -d $(PREFIX)/usr/lib/node_modules/@tibbo-tps/${DV_LIB}
	install build/Release/${DV_LIB}.node $(PREFIX)/usr/lib/node_modules/@tibbo-tps/${DV_LIB}/
	install -m 0644 package.json $(PREFIX)/usr/lib/node_modules/@tibbo-tps/${DV_LIB}/
