CC = gcc

CFLAGS = -Wall -pedantic -I.

LDLIBS = -lglib-2.0 -lyaml -lcyaml
# TODO use pkg-config?
# pkg-config --libs libcyaml yaml-0.1 glib-2.0

pname = spotify-adblock-linux
TARGET = yaml-parse

prefix ?= /usr/local
sysconfdir ?= /etc
bindir ?= /bin
libdir ?= /lib

CDEFINES = -DSYSCONFDIR=\"$(sysconfdir)\"

.PHONY: all
#all: $(TARGET).so
all: yaml-parse-freeform yaml-parse-schema

#$(TARGET).so: $(TARGET).c whitelist.h blacklist.h
#	$(CC) $(CFLAGS) -shared -fPIC -o $@ $(LDFLAGS) $(LDLIBS) $^

yaml-parse-freeform: src/yaml-parse-freeform.c
	$(CC) $(CFLAGS) $(CDEFINES) -o $@ $(LDFLAGS) $(LDLIBS) $^

yaml-parse-schema: src/yaml-parse-schema.c
	$(CC) $(CFLAGS) $(CDEFINES) -o $@ $(LDFLAGS) $(LDLIBS) $^

.PHONY: clean
clean:
#	rm -f $(TARGET).so
#	rm -f $(TARGET)
	rm -f yaml-parse-freeform yaml-parse-schema

.PHONY: install
#install: $(TARGET)
install:
	install -d $(DESTDIR)$(prefix)$(bindir)
	install -t $(DESTDIR)$(prefix)$(bindir) --strip -m 755 yaml-parse-freeform
	install -t $(DESTDIR)$(prefix)$(bindir) --strip -m 755 yaml-parse-schema
	install -d $(DESTDIR)$(sysconfdir)/$(pname)
	install -t $(DESTDIR)$(sysconfdir)/$(pname) -m 644 src/config.yaml # copy config file to /etc

# ^ comment last line to test missing config file

.PHONY: uninstall
uninstall:
#	rm -f $(prefix)/lib/$(TARGET).so
	rm -f $(prefix)$(bindir)/$(TARGET)
	rm -f $(prefix)$(bindir)/yaml-parse-libcyaml
	rm -rf $(prefix)$(sysconfdir)/$(pname)
