CC = cc
MODULE = datastructure
INCLUDES = $(addprefix -I,$(MODULE))
CFLAGS = -Wall -O3
OBJECTS = $(addsuffix .o,$(MODULE)) scheduler.o
DEFINE = SS_READYQUEUESIZE=16 TIMER_INTERVAL=2 MAX_TRIGGER_TIMES=20 DELAYJOB=3
DEFINES = $(addprefix -D,$(DEFINE))
TARGET = demo.bin

# Check the dependent module exist or not.
ifeq ($(wildcard $(MODULE)),)
	URL=https://github.com/starnight/simple-data-structure.git 
	$(MODULE)DEP=git clone $(URL) $(MODULE)
endif

demo: $(OBJECTS)
	$(CC) $(INCLUDES) $(CFLAGS) $(DEFINES) $(OBJECTS) demo.c -o $(TARGET)

scheduler.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DEFINES) -o $@ -c $(subst .o,.c,$@)

datastructure.o:
	$($(subst .o,DEP,$@))
	$(CC) $(CFLAGS) -o $@ -c $(MODULE)/$(subst .o,.c,$@)	

.PHONY: clean
clean:
	rm -rf *.o *.bin *.s
clean_all:
	rm -rf *.o *.bin *.s $(MODULE)
