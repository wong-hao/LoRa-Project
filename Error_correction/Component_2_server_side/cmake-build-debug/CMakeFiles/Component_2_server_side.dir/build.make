# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /cygdrive/c/Users/229077035/AppData/Local/JetBrains/CLion2021.1/cygwin_cmake/bin/cmake.exe

# The command to remove a file.
RM = /cygdrive/c/Users/229077035/AppData/Local/JetBrains/CLion2021.1/cygwin_cmake/bin/cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/Component_2_server_side.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Component_2_server_side.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Component_2_server_side.dir/flags.make

CMakeFiles/Component_2_server_side.dir/main.cpp.o: CMakeFiles/Component_2_server_side.dir/flags.make
CMakeFiles/Component_2_server_side.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Component_2_server_side.dir/main.cpp.o"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Component_2_server_side.dir/main.cpp.o -c "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/main.cpp"

CMakeFiles/Component_2_server_side.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Component_2_server_side.dir/main.cpp.i"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/main.cpp" > CMakeFiles/Component_2_server_side.dir/main.cpp.i

CMakeFiles/Component_2_server_side.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Component_2_server_side.dir/main.cpp.s"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/main.cpp" -o CMakeFiles/Component_2_server_side.dir/main.cpp.s

# Object files for target Component_2_server_side
Component_2_server_side_OBJECTS = \
"CMakeFiles/Component_2_server_side.dir/main.cpp.o"

# External object files for target Component_2_server_side
Component_2_server_side_EXTERNAL_OBJECTS =

Component_2_server_side.exe: CMakeFiles/Component_2_server_side.dir/main.cpp.o
Component_2_server_side.exe: CMakeFiles/Component_2_server_side.dir/build.make
Component_2_server_side.exe: CMakeFiles/Component_2_server_side.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Component_2_server_side.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Component_2_server_side.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Component_2_server_side.dir/build: Component_2_server_side.exe

.PHONY : CMakeFiles/Component_2_server_side.dir/build

CMakeFiles/Component_2_server_side.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Component_2_server_side.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Component_2_server_side.dir/clean

CMakeFiles/Component_2_server_side.dir/depend:
	cd "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side" "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side" "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/cmake-build-debug" "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/cmake-build-debug" "/cygdrive/e/LoRa Github/LoRa-Project/Error_correction/Component_2_server_side/cmake-build-debug/CMakeFiles/Component_2_server_side.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/Component_2_server_side.dir/depend

