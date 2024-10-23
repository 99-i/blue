INCS = $(ROOT)/include $(ROOT)/lib/libuv/include /usr/include
LIBS = uv ssl uuid

LIBDIRS = lib/libuv/build /usr/lib

CFLAGS = -std=c89 -Wall -Wextra -MMD -MP -g $(addprefix -L, $(LIBDIRS)) $(addprefix -I, $(INCS)) $(addprefix -l, $(LIBS)) -D_GNU_SOURCE

LINKFLAGS = -MMD -MP -g

PROJECT := blue

SUBDIR_MAKEFILE = $(ROOT)/make/subdir_makefile.mk
SUBDIR_WITH_SUBDIRS_MAKEFILE = $(ROOT)/make/subdir_with_subdirs_makefile.mk
