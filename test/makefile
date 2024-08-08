include $(CONFIG_MK)

SRCS = $(wildcard *.c)

EXES = $(addprefix testbin/,$(patsubst %.c, %.exe, $(SRCS)))
DEPS = $(addprefix testbin/,$(patsubst %.c, %.d, $(SRCS)))

.PHONY: all
.PHONY: run

all: testbin $(EXES) run


testbin/%.exe: %.c $(ROOT)/bin/lib$(PROJECT).a
	gcc $(CFLAGS) -o $@ $< -L$(ROOT)/bin -l$(PROJECT)

run: $(addsuffix _run, $(EXES))

error_level = $(shell echo `%errorlevel%`)

$(addsuffix _run, $(EXES)):
	@echo
	@tput setaf 2
	@echo "RUNNING TEST $(subst .exe_run,,$(subst testbin/,,$@))"
	@tput sgr0
	@($(@:_run=)) && (echo && tput setaf 2 && echo TEST PASS && tput sgr0) || (echo && tput setaf 9 && echo TEST FAIL && tput sgr0)



-include $(DEPS)

clean:
	rm -rf testbin
	@mkdir testbin
