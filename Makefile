.POSIX:
CC      = cc
CFLAGS  = -Wall -Wextra -fsigned-char -Os
LDFLAGS =
LDLIBS  =

obj = fileb.o keyb.o listc.o main.o opfile.o rterm.o scbuf.o tm.o uintf.o

fw: $(obj)
	$(CC) $(LDFLAGS) -o $@ $(obj) $(LDLIBS)

clean:
	rm -f fw $(obj)

fileb.o: fileb.c fileb.h scbuf.h rterm.h
keyb.o: keyb.c rterm.h keyb.h
listc.o: listc.c rterm.h scbuf.h keyb.h
main.o: main.c rterm.h scbuf.h tm.h keyb.h fileb.h opfile.h uintf.h \
  listc.h about.h
opfile.o: opfile.c rterm.h keyb.h uintf.h scbuf.h listc.h
rterm.o: rterm.c
scbuf.o: scbuf.c rterm.h scbuf.h
tm.o: tm.c tm.h
uintf.o: uintf.c scbuf.h rterm.h listc.h keyb.h 
