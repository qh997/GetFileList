OBJECT := gfl.o
OBJECT += common.o
OBJECT += options.o
OBJECT += dirbox.o
OBJECT += filebox.o
OBJECT += queuelist.o

ifneq ($(CC), gcc)
CC := gcc
endif
CFLAGS := -std=gnu99

.PHONY: all clean

all: clean gfl

clean:
	-rm $(OBJECT) gfl
 
gfl: $(OBJECT)
	@echo
	@echo 'make gfl'
	$(CC) $(CFLAGS) $^ -o $@
	@echo
