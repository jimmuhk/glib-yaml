PKGS = glib-2.0 yaml-0.1

CC = gcc
CFLAGS  = -g -Wall $(shell pkg-config --cflags $(PKGS))
LDFLAGS = $(shell pkg-config --libs $(PKGS))

EXE = parse-yaml

$(EXE):

clean:
	-rm -f $(EXE)
