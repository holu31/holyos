include ../common.mk

LDFLAGS	+= -r
CFLAGS	+= -I../include -I../limine

override SRCFILES := $(shell find -L * -type f | LC_ALL=C sort)
override CFILES := $(filter %.c,$(SRCFILES))
override ASFILES := $(filter %.S,$(SRCFILES))
override OBJS := $(addprefix ,$(CFILES:.c=.c.o) $(ASFILES:.s=.s.o))

.PHONY: all
all: mm.o

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.s.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

mm.o: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

clean:
	@find . -type f -name "*.o" -delete
