# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kimi/FIUBA/7559-TP2/picoDB

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kimi/FIUBA/7559-TP2/picoDB

# Include any dependencies generated for this target.
include shared/CMakeFiles/shared_lib.dir/depend.make

# Include the progress variables for this target.
include shared/CMakeFiles/shared_lib.dir/progress.make

# Include the compile flags for this target's objects.
include shared/CMakeFiles/shared_lib.dir/flags.make

shared/CMakeFiles/shared_lib.dir/src/person.cpp.o: shared/CMakeFiles/shared_lib.dir/flags.make
shared/CMakeFiles/shared_lib.dir/src/person.cpp.o: shared/src/person.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kimi/FIUBA/7559-TP2/picoDB/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object shared/CMakeFiles/shared_lib.dir/src/person.cpp.o"
	cd /home/kimi/FIUBA/7559-TP2/picoDB/shared && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/shared_lib.dir/src/person.cpp.o -c /home/kimi/FIUBA/7559-TP2/picoDB/shared/src/person.cpp

shared/CMakeFiles/shared_lib.dir/src/person.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/shared_lib.dir/src/person.cpp.i"
	cd /home/kimi/FIUBA/7559-TP2/picoDB/shared && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kimi/FIUBA/7559-TP2/picoDB/shared/src/person.cpp > CMakeFiles/shared_lib.dir/src/person.cpp.i

shared/CMakeFiles/shared_lib.dir/src/person.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/shared_lib.dir/src/person.cpp.s"
	cd /home/kimi/FIUBA/7559-TP2/picoDB/shared && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kimi/FIUBA/7559-TP2/picoDB/shared/src/person.cpp -o CMakeFiles/shared_lib.dir/src/person.cpp.s

shared/CMakeFiles/shared_lib.dir/src/person.cpp.o.requires:
.PHONY : shared/CMakeFiles/shared_lib.dir/src/person.cpp.o.requires

shared/CMakeFiles/shared_lib.dir/src/person.cpp.o.provides: shared/CMakeFiles/shared_lib.dir/src/person.cpp.o.requires
	$(MAKE) -f shared/CMakeFiles/shared_lib.dir/build.make shared/CMakeFiles/shared_lib.dir/src/person.cpp.o.provides.build
.PHONY : shared/CMakeFiles/shared_lib.dir/src/person.cpp.o.provides

shared/CMakeFiles/shared_lib.dir/src/person.cpp.o.provides.build: shared/CMakeFiles/shared_lib.dir/src/person.cpp.o

# Object files for target shared_lib
shared_lib_OBJECTS = \
"CMakeFiles/shared_lib.dir/src/person.cpp.o"

# External object files for target shared_lib
shared_lib_EXTERNAL_OBJECTS =

shared/libshared_lib.a: shared/CMakeFiles/shared_lib.dir/src/person.cpp.o
shared/libshared_lib.a: shared/CMakeFiles/shared_lib.dir/build.make
shared/libshared_lib.a: shared/CMakeFiles/shared_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libshared_lib.a"
	cd /home/kimi/FIUBA/7559-TP2/picoDB/shared && $(CMAKE_COMMAND) -P CMakeFiles/shared_lib.dir/cmake_clean_target.cmake
	cd /home/kimi/FIUBA/7559-TP2/picoDB/shared && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/shared_lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
shared/CMakeFiles/shared_lib.dir/build: shared/libshared_lib.a
.PHONY : shared/CMakeFiles/shared_lib.dir/build

shared/CMakeFiles/shared_lib.dir/requires: shared/CMakeFiles/shared_lib.dir/src/person.cpp.o.requires
.PHONY : shared/CMakeFiles/shared_lib.dir/requires

shared/CMakeFiles/shared_lib.dir/clean:
	cd /home/kimi/FIUBA/7559-TP2/picoDB/shared && $(CMAKE_COMMAND) -P CMakeFiles/shared_lib.dir/cmake_clean.cmake
.PHONY : shared/CMakeFiles/shared_lib.dir/clean

shared/CMakeFiles/shared_lib.dir/depend:
	cd /home/kimi/FIUBA/7559-TP2/picoDB && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kimi/FIUBA/7559-TP2/picoDB /home/kimi/FIUBA/7559-TP2/picoDB/shared /home/kimi/FIUBA/7559-TP2/picoDB /home/kimi/FIUBA/7559-TP2/picoDB/shared /home/kimi/FIUBA/7559-TP2/picoDB/shared/CMakeFiles/shared_lib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : shared/CMakeFiles/shared_lib.dir/depend

