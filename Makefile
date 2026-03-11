CC ?= cc
AR ?= ar
RANLIB ?= ranlib

PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include

LIBNAME ?= tuilib
STATIC_LIB := lib$(LIBNAME).a

SRC_DIR := src
INC_DIR := include
OBJ_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
HDRS := $(wildcard $(INC_DIR)/*.h)

CFLAGS ?= -O2
CPPFLAGS += -I$(INC_DIR)

.PHONY: all clean install uninstall

all: $(STATIC_LIB)

$(STATIC_LIB): $(OBJS)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

install: $(STATIC_LIB)
	install -d $(DESTDIR)$(LIBDIR)
	install -m 644 $(STATIC_LIB) $(DESTDIR)$(LIBDIR)/$(STATIC_LIB)
	install -d $(DESTDIR)$(INCLUDEDIR)/wrntui
	install -m 644 $(HDRS) $(DESTDIR)$(INCLUDEDIR)/wrntui/

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/$(STATIC_LIB)
	for h in $(notdir $(HDRS)); do rm -f $(DESTDIR)$(INCLUDEDIR)/wrntui/$$h; done
	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(INCLUDEDIR)/wrntui 2>/dev/null || true

clean:
	rm -rf $(OBJ_DIR) $(STATIC_LIB)
