$(ROOT)/bin/$(OBJ_PREFIX)_%.o: %.c
	@tput setaf 2
	@echo COMPILING $(OBJ_PREFIX)/$<
	@tput sgr0
ifeq ($(VERBOSE), ON)
	gcc $(CFLAGS) -c $< -o $@
else
	@gcc $(CFLAGS) -c $< -o $@
endif
	@echo true > $(ROOT)/bin/recompile
	@tput setaf 2
	@echo SUCCESSFULLY COMPILED $(OBJ_PREFIX)/$<
	@tput sgr0
