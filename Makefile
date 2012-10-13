CC = cc
CFLAGS = -Wall -g `pkg-config --cflags gtk+-2.0 --libs gtk+-2.0`
CFLAGSD = -Wall -g -DDEBUG `pkg-config --cflags gtk+-2.0 --libs gtk+-2.0`

wubi-dict:wubi-dict.c
	$(CC) $(CFLAGS) $@.c -o $@

debug:
	$(CC) $(CFLAGSD) wubi-dict.c -o wubi-dict
clean:
	rm -f wubi-dict

install:
	cp wubi-dict /usr/bin
	mkdir /usr/share/wubi-dict
	cp -r data /usr/share/wubi-dict
	cp data/wubi-dict.png /usr/share/pixmaps
	cp wubi-dict.desktop /usr/share/applications
uninstall:
	rm -f /usr/bin/wubi-dict
	rm -rf /usr/share/wubi-dict
	rm -f /usr/share/pixmaps/wubi-dict.png 
	rm -f /usr/share/applications/wubi-dict.desktop

