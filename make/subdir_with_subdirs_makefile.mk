include $(CONFIG_MK)

SRCS = $(wildcard *.c)

OBJS = $(addprefix $(ROOT)/bin/$(OBJ_PREFIX)_,$(patsubst %.c, %.o, $(SRCS)))
DEPS = $(addprefix $(ROOT)/bin/$(OBJ_PREFIX)_,$(patsubst %.c, %.d, $(SRCS)))

.PHONY: all subdirs $(SUBDIRS)

all: subdirs $(OBJS)
	@:

subdirs: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ OBJ_PREFIX=$@

-include $(DEPS)

include $(ROOT)/make/compile_command.mk
