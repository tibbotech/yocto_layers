
TO = $(PREFIX)/home/root/dvtests/

all:
	$(CC) -o dvgpio_fast dvgpio_fast.c
	$(CXX) -o dvgpio_simp dvgpio_simp.c dv/CPin.cpp
	$(CXX) -o dvi2c dvi2c.c dv/Ci2c.cpp dv/Ci2c_smbus.cpp
	$(CXX) -o dvspi dvspi.cpp dv/Cspi.cpp
	$(CC) -o rtctest rtctest.c
	$(CC) -o rtc01 rtc01.c
	$(CC) -o endian endian.c
	$(CC) -o beep_bell beep_bell.c
	$(CC) -o beep_tone beep_tone.c
	$(CC) -o spidev_test spidev_test.c
	$(CC) -o tpsid tpsid.c
	$(CC) -o ttyhrw ttyhrw.c
	$(CXX) -Idv/ -o tty485 tty485.cpp dv/Cuart.cpp
	$(CXX) -Idv/ -o set485 set485.cpp dv/Cuart.cpp
	$(CXX) -Idv/ -o twg twg.cpp dv/CWg.cpp
	$(CC) -o tstR tstR.c
	$(CC) -o tstW tstW.c
	$(CC) -o 485r 485r.c
	$(CC) -o 485s 485s.c
	$(CC) -o input_kbd_event input_kbd_event.c


clean:
	rm -f beep_bell beep_tone
	rm -f dvgpio_fast dvgpio_simp dvi2c
	rm -f dvspi
	rm -f spidev_test
	rm -f rtc rtc01 rtctest
	rm -f endian
	rm -f tpsid
	rm -f ttyhrw
	rm -f tty485
	rm -f tstR
	rm -f tstW
	rm -f twg
	rm -f 485r
	rm -f 485s
	rm -f input_kbd_event
	rm -f *.o

install:
	install -d $(TO)
	install dvgpio_fast $(TO)
	install koh.mp3 $(TO)
	install dvgpio_simp $(TO)
	install dvi2c $(TO)
	install rtctest $(TO)
	install rtc01 $(TO)
	install endian $(TO)
	install beep_bell $(TO)
	install beep_tone $(TO)
	install spidev_test $(TO)
	install dvspi $(TO)
	install tpsid $(TO)
	install ttyhrw $(TO)
	install tty485 $(TO)
	install tstR $(TO)
	install tstW $(TO)
	install 485r $(TO)
	install 485s $(TO)
	install twg $(TO)
	install input_kbd_event $(TO)
	install test.*.sh $(TO)
