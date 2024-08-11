INCS = $(ROOT)/include $(ROOT)/lib/libuv/include
LIBS = uv

LIBDIRS = lib/libuv/build

CFLAGS = -std=c89 -Wall -Wextra -MMD -MP -g $(addprefix -L, $(LIBDIRS)) $(addprefix -I, $(INCS)) $(addprefix -l, $(LIBS)) -D_GNU_SOURCE

LINKFLAGS = -MMD -MP -g

PROJECT := blue

SUBDIR_MAKEFILE = $(ROOT)/subdir_makefile.mk
SUBDIR_WITH_SUBDIRS_MAKEFILE = $(ROOT)/subdir_with_subdirs_makefile.mk
