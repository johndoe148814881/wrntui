CC      = gcc
CFLAGS  = -g -O2 -pipe -std=c99 -Wall -Wextra -fPIC -Iinclude -D_POSIX_C_SOURCE=199309L
LDFLAGS = -shared

NAME    = libtui
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:.c=.o)

PREFIX      = /usr/local
LIBDIR      = $(PREFIX)/lib
INCLUDEDIR  = $(PREFIX)/include/tui
PUBLIC_HEADER = wrntui.h

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
	install -d $(PREFIX)/include
	install -m 644 $(STATIC) $(LIBDIR)
	install -m 755 $(SHARED) $(LIBDIR)
	install -m 644 include/$(PUBLIC_HEADER) $(PREFIX)/include/$(PUBLIC_HEADER)
	ldconfig

uninstall:
	rm -f $(LIBDIR)/$(STATIC)
	rm -f $(LIBDIR)/$(SHARED)
	rm -f $(PREFIX)/include/$(PUBLIC_HEADER)
	rm -f $(PREFIX)/include/tui.h
	rm -f $(PREFIX)/include/box.h $(PREFIX)/include/msg.h $(PREFIX)/include/info.h
	rm -f $(PREFIX)/include/graph.h $(PREFIX)/include/bind.h $(PREFIX)/include/cmd.h
	rm -rf $(INCLUDEDIR)
	ldconfig

clean:
	rm -f $(OBJ) $(STATIC) $(SHARED)
