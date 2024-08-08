include $(CONFIG_MK)

SRCS = $(wildcard *.c)

OBJS = $(addprefix $(ROOT)/bin/$(OBJ_PREFIX)_,$(patsubst %.c, %.o, $(SRCS)))
DEPS = $(addprefix $(ROOT)/bin/$(OBJ_PREFIX)_,$(patsubst %.c, %.d, $(SRCS)))

.PHONY: all

all: $(OBJS)

-include $(DEPS)

include $(ROOT)/compile_command.mk
