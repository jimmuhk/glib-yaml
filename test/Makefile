PKGS = glib-2.0 yaml-0.1 libglib-yaml

CC = gcc
CFLAGS  = -g -Wall $(shell pkg-config --cflags $(PKGS))
LDFLAGS = $(shell pkg-config --libs $(PKGS))

EXES = dump-yaml parse-yaml find-node

all: $(EXES)

clean:
	-rm -f $(EXES)
