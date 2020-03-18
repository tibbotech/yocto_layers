TO = $(PREFIX)/home/root/

all:
	${CC} -O -o mii-diag mii-diag.c

clean:
	rm -f mii-diag
	rm -f *.o

install:
	install -d $(TO)
	install mii-diag $(TO)
