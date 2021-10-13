TO = $(DESTDIR)/home/root/

all:
	$(CC) $(CFLAGS) $(LDFLAGS) -o linux-serial-test linux-serial-test.c

clean:
	rm -f linux-serial-test
	rm -f *.o

install:
	install -d $(TO)
	install linux-serial-test $(TO)
