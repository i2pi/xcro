OBJS=main.o fft.o quant.o tc64.o
LIBS=-lX11 -lm libtinyptc.a 
#CFLAGS=-Wall -mpentium -O6
CFLAGS=-Wall -mpentium -g

xcro: $(OBJS)
	gcc $(CFLAGS) $(OBJS) $(LIBS) -o xcro  -lm -L/usr/X11R6/lib -lXext -lX11

clean:
	rm -f xcro $(OBJS) core

