diskio.o : diskio.c diskio.h tonedata.h fmregdef.h 
fexport.o : fexport.c tonedata.h fmregdef.h disp.h diskio.h 
main.o : main.c fmctrl.h fmregdef.h keyutil.h tonedata.h disp.h diskio.h \
  fexport.h makefnt.h 
fmctrl.o : fmctrl.c fmctrl.h fmregdef.h 
disp.o : disp.c disp.h fmregdef.h keyutil.h fmctrl.h kbptn.h 
makefnt.o : makefnt.c algofnt.h makefnt.h 
tonedata.o : tonedata.c tonedata.h fmregdef.h disp.h diskio.h 
keyutil.o : keyutil.c keyutil.h 
