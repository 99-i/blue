include config.mk
.PHONY: all
.PHONY: clean
.PHONY: gen_cc
.PHONY: test

all: bin all_libraries compile bin/$(PROJECT).exe

tests: compile bin/lib$(PROJECT).a
	@tput setaf 2
	@echo Running tests
	@tput sgr0
	@$(MAKE) --no-print-directory -C test CONFIG_MK="`pwd`/config.mk" ROOT="`pwd`"

bin:
	@mkdir bin

include libs.mk

compile:
	@$(MAKE) -C src CONFIG_MK="`pwd`/config.mk" ROOT="`pwd`"

OBJS = $(filter-out bin/main.o, $(wildcard bin/*.o))

MAIN_OBJ = bin/main.o

bin/$(PROJECT).exe: $(MAIN_OBJ) bin/lib$(PROJECT).a bin/recompile
	gcc $(LINKFLAGS) -o $@ $(MAIN_OBJ) -Lbin $(addprefix -L, $(LIBDIRS)) $(addprefix -l, $(LIBS)) -l$(PROJECT) -pthread

bin/lib$(PROJECT).a: $(OBJS)
	ar rcs $@ $(OBJS)

run: bin/$(PROJECT).exe
	@echo
	@bin/$(PROJECT).exe

clean:
	rm -rf bin/*.o
	rm -rf bin/*.a
	rm -rf bin/*.d
	rm -rf bin/*.exe
	cd test && $(MAKE) clean
	@echo true > bin/recompile
	@tput setaf 2
	@echo Cleaned the output directories.
	@tput sgr0

full_clean:
	rm -rf bin
	rm -rf lib
	rm -rf testbin

gen_cc: clean
	tsx util/compile_commands/compile_commands.ts

gdb: bin/$(PROJECT).exe
	gdb $<

memcheck: bin/$(PROJECT).exe
	valgrind --leak-check=full --show-leak-kinds=all $<
