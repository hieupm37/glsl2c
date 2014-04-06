# Makefile

CC = cc
ARCH = -arch i386
CFLAGS = -c -Wall $(ARCH)
LDFLAGS = $(ARCH)
SRCS = main.c
OBJS = $(SRCS:.c=.o)
EXE  = glsl2c
INSTALL_DIR = /usr/local/bin

all: $(SRCS) $(EXE)

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXE)

install:
	cp $(EXE) $(INSTALL_DIR)
