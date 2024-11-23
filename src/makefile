include $(CONFIG_MK)

SRCS = $(wildcard *.c)
OBJS = $(addprefix $(ROOT)/bin/,$(patsubst %.c, %.o, $(SRCS)))
DEPS = $(addprefix $(ROOT)/bin/,$(patsubst %.c, %.d, $(SRCS)))

SUBDIRS = packet net util nbt game log

.PHONY: all subdirs $(SUBDIRS)

all: subdirs $(OBJS)

subdirs: $(SUBDIRS)

-include $(DEPS)

$(SUBDIRS):
	@$(MAKE) -C $@ OBJ_PREFIX=$@

$(ROOT)/bin/%.o: %.c
	@tput setaf 2
	@echo COMPILING $<
	@tput sgr0
ifeq ($(VERBOSE), ON)
	gcc $(CFLAGS) -c $< -o $@
else
	@gcc $(CFLAGS) -c $< -o $@
endif
	@echo true > $(ROOT)/bin/recompile
	@tput setaf 2
	@echo SUCCESSFULLY COMPILED $<
	@tput sgr0
