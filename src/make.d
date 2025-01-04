fexport.o : fexport.c tonedata.h fmregdef.h disp.h diskio.h 
fmctrl.o : fmctrl.c fmctrl.h fmregdef.h 
disp.o : disp.c disp.h fmregdef.h keyutil.h fmctrl.h kbptn.h 
diskio.o : diskio.c diskio.h tonedata.h fmregdef.h 
main.o : main.c fmctrl.h fmregdef.h keyutil.h tonedata.h disp.h diskio.h \
  fexport.h 
tonedata.o : tonedata.c tonedata.h fmregdef.h disp.h diskio.h 
keyutil.o : keyutil.c keyutil.h 
