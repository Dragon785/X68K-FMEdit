CC	= gcc
CFLAGS	= -O4 -I\src\cmn\inc
AS	= $(GCC_AS)
LK	= $(GCC_LINK)
LIBS	= 
OBJS	= fmctrl.o main.o keyutil.o disp.o tonedata.o diskio.o fexport.o
PROG	= fmedit.x
DEPEND  = make.d

.PHONY:	all
all:	$(PROG)

$(PROG):	$(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LIBS)

clean:
	echo dummy: > $(DEPEND)
	del *.o /Y
	del $(PROG) /Y

%.o:	%.c
	$(CC) $(CFLAGS) -o $@ -c $<

depend:
	$(CC) -MM *.c > $(DEPEND)

include $(DEPEND)

