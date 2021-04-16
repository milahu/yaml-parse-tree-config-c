CC=gcc
CFLAGS=-Wall -pedantic -I.
#LDLIBS=-ldl -lglib-2.0 # *.so
LDLIBS=-lglib-2.0 -lyaml
#TARGET=spotify-adblock
TARGET=yaml-parse
prefix ?= /usr/local
sysconfdir ?= /etc
bindir ?= /bin
libdir ?= /lib
CDEFINES=-DSYSCONFDIR=\"$(sysconfdir)\"
pname = spotify-adblock-linux

.PHONY: all
#all: $(TARGET).so
all: $(TARGET)

#$(TARGET).so: $(TARGET).c whitelist.h blacklist.h
#	$(CC) $(CFLAGS) -shared -fPIC -o $@ $(LDFLAGS) $(LDLIBS) $^
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(CDEFINES) -o $(TARGET) $(LDFLAGS) $(LDLIBS) $^

.PHONY: clean
clean:
#	rm -f $(TARGET).so
	rm -f $(TARGET)

.PHONY: install
install: $(TARGET)
	install -d $(DESTDIR)$(prefix)$(bindir)
	install -t $(DESTDIR)$(prefix)$(bindir) --strip -m 755 $^
	install -d $(DESTDIR)$(sysconfdir)/$(pname)
	install -t $(DESTDIR)$(sysconfdir)/$(pname) -m 644 config.yaml

.PHONY: uninstall
uninstall:
#	rm -f $(prefix)/lib/$(TARGET).so
	rm -f $(prefix)$(bindir)/$(TARGET)
	rm -rf $(prefix)$(sysconfdir)/$(pname)
