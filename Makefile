CC      = gcc
CFLAGS  = -g -O2 -pipe -std=c99 -Wall -Wextra -fPIC -Iinclude -D_POSIX_C_SOURCE=199309L
LDFLAGS = -shared

NAME    = libtui
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:.c=.o)

PREFIX      = /usr/local
LIBDIR      = $(PREFIX)/lib
INCLUDEDIR  = $(PREFIX)/include/tui

# Build targets
STATIC  = $(NAME).a
SHARED  = $(NAME).so

.PHONY: all clean install uninstall

all: $(STATIC) $(SHARED)

$(STATIC): $(OBJ)
	ar rcs $@ $^

$(SHARED): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	install -d $(LIBDIR)
	install -d $(INCLUDEDIR)
	install -m 644 $(STATIC) $(LIBDIR)
	install -m 755 $(SHARED) $(LIBDIR)
	install -m 644 include/*.h $(INCLUDEDIR)
	install -m 644 include/tui.h $(PREFIX)/include/tui.h
	ldconfig

uninstall:
	rm -f $(LIBDIR)/$(STATIC)
	rm -f $(LIBDIR)/$(SHARED)
	rm -f $(PREFIX)/include/tui.h
	rm -rf $(INCLUDEDIR)
	ldconfig

clean:
	rm -f $(OBJ) $(STATIC) $(SHARED)
