CC	= gcc
CFLAGS	= -O4
MARIKO  = AB
GCC_OPTION=GTFAMLJKOEW+
GCC_LIB=.l
HAS=-u -z
HLK=-l floatfnc.l -l iocslib.l -l doslib.l -l gnulib.l -l clib.l
AS	= $(GCC_AS)
LK	= $(GCC_LINK)
LIBS	= 
OBJS	= fmctrl.o main.o keyutil.o disp.o tonedata.o diskio.o fexport.o makefnt.o
PROG	= fmedit.x
DEPEND  = make.d

.PHONY:	all
all:	$(PROG)

$(PROG):	$(OBJS)
	$(LK) -o $(PROG) $(OBJS) $(LIBS)

clean:
	echo dummy: > $(DEPEND)
	del *.o /Y
	del $(PROG) /Y

%.o:	%.c
	$(CC) $(CFLAGS) -o $@ -c $<

depend:
	$(CC) -MM *.c > $(DEPEND)

include $(DEPEND)

