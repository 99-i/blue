INCS = $(ROOT)/include $(ROOT)/lib/libuv/include
LIBS = libuv

LIBDIRS = lib/libuv/build

CFLAGS = -std=c89 -Wall -Wextra -MMD -MP -g $(addprefix -L, $(LIBDIRS)) $(addprefix -I, $(INCS)) $(addprefix -l, $(LIBS)) 

LINKFLAGS = -MMD -MP -g 

PROJECT := blue

SUBDIR_MAKEFILE = $(ROOT)/subdir_makefile.mk
