
all:
	$(CC) $(CFLAGS) `pkg-config --cflags --libs freetype2` $(LDFLAGS) test-freetype.c -o test-freetype
	$(CC) $(CFLAGS) $(LDFLAGS) fbtest.c -o fbtest
	$(CC) $(CFLAGS) `pkg-config --cflags --libs harfbuzz` `pkg-config --cflags --libs freetype2` $(LDFLAGS) testhb.c -o testhb
	$(CC) $(CFLAGS) `pkg-config --cflags --libs harfbuzz` `pkg-config --cflags --libs freetype2` $(LDFLAGS) hbfb.c -o hbfb
	$(CC) $(CFLAGS) -lm `pkg-config --cflags --libs harfbuzz` `pkg-config --cflags --libs freetype2` `pkg-config --cflags --libs cairo` $(LDFLAGS) harfbuzz-freetype.c -o harfbuzz-freetype
	$(CC) $(CFLAGS) -lm `pkg-config --cflags --libs harfbuzz` `pkg-config --cflags --libs freetype2` `pkg-config --cflags --libs cairo` $(LDFLAGS) cairo.c -o cairo

clean:
	rm -rf test-freetype
	rm -rf fbtest
	rm -rf testfb
	rm -rf hbfb
	rm -rf harfbuzz-freetype
	rm -rf cairo

install:
	install -m 0755 test-freetype ${DESTDIR}/
	install -m 0755 fbtest ${DESTDIR}/
	install -m 0755 testhb ${DESTDIR}/
	install -m 0755 hbfb ${DESTDIR}/
	install -m 0755 harfbuzz-freetype ${DESTDIR}/
	install -m 0755 cairo ${DESTDIR}/

