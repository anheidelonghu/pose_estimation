# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/hxx/code/hxx_proj

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hxx/code/hxx_proj/build

# Include any dependencies generated for this target.
include CMakeFiles/getPic.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/getPic.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/getPic.dir/flags.make

CMakeFiles/getPic.dir/getPic.cpp.o: CMakeFiles/getPic.dir/flags.make
CMakeFiles/getPic.dir/getPic.cpp.o: ../getPic.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hxx/code/hxx_proj/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/getPic.dir/getPic.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/getPic.dir/getPic.cpp.o -c /home/hxx/code/hxx_proj/getPic.cpp

CMakeFiles/getPic.dir/getPic.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/getPic.dir/getPic.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hxx/code/hxx_proj/getPic.cpp > CMakeFiles/getPic.dir/getPic.cpp.i

CMakeFiles/getPic.dir/getPic.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/getPic.dir/getPic.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hxx/code/hxx_proj/getPic.cpp -o CMakeFiles/getPic.dir/getPic.cpp.s

CMakeFiles/getPic.dir/getPic.cpp.o.requires:

.PHONY : CMakeFiles/getPic.dir/getPic.cpp.o.requires

CMakeFiles/getPic.dir/getPic.cpp.o.provides: CMakeFiles/getPic.dir/getPic.cpp.o.requires
	$(MAKE) -f CMakeFiles/getPic.dir/build.make CMakeFiles/getPic.dir/getPic.cpp.o.provides.build
.PHONY : CMakeFiles/getPic.dir/getPic.cpp.o.provides

CMakeFiles/getPic.dir/getPic.cpp.o.provides.build: CMakeFiles/getPic.dir/getPic.cpp.o


# Object files for target getPic
getPic_OBJECTS = \
"CMakeFiles/getPic.dir/getPic.cpp.o"

# External object files for target getPic
getPic_EXTERNAL_OBJECTS =

getPic: CMakeFiles/getPic.dir/getPic.cpp.o
getPic: CMakeFiles/getPic.dir/build.make
getPic: /usr/lib/x86_64-linux-gnu/libGLU.so
getPic: /usr/lib/x86_64-linux-gnu/libGL.so
getPic: /usr/lib/x86_64-linux-gnu/libglfw.so
getPic: CMakeFiles/getPic.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hxx/code/hxx_proj/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable getPic"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/getPic.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/getPic.dir/build: getPic

.PHONY : CMakeFiles/getPic.dir/build

CMakeFiles/getPic.dir/requires: CMakeFiles/getPic.dir/getPic.cpp.o.requires

.PHONY : CMakeFiles/getPic.dir/requires

CMakeFiles/getPic.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/getPic.dir/cmake_clean.cmake
.PHONY : CMakeFiles/getPic.dir/clean

CMakeFiles/getPic.dir/depend:
	cd /home/hxx/code/hxx_proj/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hxx/code/hxx_proj /home/hxx/code/hxx_proj /home/hxx/code/hxx_proj/build /home/hxx/code/hxx_proj/build /home/hxx/code/hxx_proj/build/CMakeFiles/getPic.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/getPic.dir/depend
