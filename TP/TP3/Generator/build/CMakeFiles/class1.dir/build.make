# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/build

# Include any dependencies generated for this target.
include CMakeFiles/class1.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/class1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/class1.dir/flags.make

CMakeFiles/class1.dir/main.cpp.o: CMakeFiles/class1.dir/flags.make
CMakeFiles/class1.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/class1.dir/main.cpp.o"
	/usr/bin/g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/class1.dir/main.cpp.o -c /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/main.cpp

CMakeFiles/class1.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/class1.dir/main.cpp.i"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/main.cpp > CMakeFiles/class1.dir/main.cpp.i

CMakeFiles/class1.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/class1.dir/main.cpp.s"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/main.cpp -o CMakeFiles/class1.dir/main.cpp.s

CMakeFiles/class1.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/class1.dir/main.cpp.o.requires

CMakeFiles/class1.dir/main.cpp.o.provides: CMakeFiles/class1.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/class1.dir/build.make CMakeFiles/class1.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/class1.dir/main.cpp.o.provides

CMakeFiles/class1.dir/main.cpp.o.provides.build: CMakeFiles/class1.dir/main.cpp.o


# Object files for target class1
class1_OBJECTS = \
"CMakeFiles/class1.dir/main.cpp.o"

# External object files for target class1
class1_EXTERNAL_OBJECTS =

class1: CMakeFiles/class1.dir/main.cpp.o
class1: CMakeFiles/class1.dir/build.make
class1: /usr/lib/x86_64-linux-gnu/libGL.so
class1: /usr/lib/x86_64-linux-gnu/libGLU.so
class1: /usr/lib/x86_64-linux-gnu/libglut.so
class1: CMakeFiles/class1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable class1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/class1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/class1.dir/build: class1

.PHONY : CMakeFiles/class1.dir/build

CMakeFiles/class1.dir/requires: CMakeFiles/class1.dir/main.cpp.o.requires

.PHONY : CMakeFiles/class1.dir/requires

CMakeFiles/class1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/class1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/class1.dir/clean

CMakeFiles/class1.dir/depend:
	cd /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/build /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/build /home/shifu/MEI/CG/Trabalho1/Projeto_CG/P1/Generator/build/CMakeFiles/class1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/class1.dir/depend

