.PHONY: download_libs
.PHONY: compile_libs
.PHONY: compile
.PHONY: all_libraries

lib:
	@mkdir lib

download_libs: lib/libuv

lib/libuv:
	@tput setaf 2
	@echo Downloading libuv.
	@tput sgr0
	@cd lib && git clone https://github.com/libuv/libuv.git


compile_libs: lib/libuv/build

lib/libuv/build:
	@tput setaf 2
	@echo Building libuv.
	@tput sgr0
	@cd lib/libuv && mkdir build && cd build && cmake .. -G "Unix Makefiles" -DBUILD_TESTING=OFF && make

all_libraries: lib download_libs compile_libs
