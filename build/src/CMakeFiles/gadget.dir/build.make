# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.17.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.17.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/showanasyabi/Desktop/trash/caspGadget/Gadget

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build

# Include any dependencies generated for this target.
include src/CMakeFiles/gadget.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/gadget.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/gadget.dir/flags.make

src/CMakeFiles/gadget.dir/core/main.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/core/main.cpp.o: ../src/core/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/gadget.dir/core/main.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/core/main.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/core/main.cpp

src/CMakeFiles/gadget.dir/core/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/core/main.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/core/main.cpp > CMakeFiles/gadget.dir/core/main.cpp.i

src/CMakeFiles/gadget.dir/core/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/core/main.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/core/main.cpp -o CMakeFiles/gadget.dir/core/main.cpp.s

src/CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.o: ../src/core/PerformanceMetrics.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/core/PerformanceMetrics.cpp

src/CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/core/PerformanceMetrics.cpp > CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.i

src/CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/core/PerformanceMetrics.cpp -o CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.s

src/CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.o: ../src/operators/ycsbTrace.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/ycsbTrace.cpp

src/CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/ycsbTrace.cpp > CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.i

src/CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/ycsbTrace.cpp -o CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.s

src/CMakeFiles/gadget.dir/operators/flinkTrace.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/flinkTrace.cpp.o: ../src/operators/flinkTrace.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/CMakeFiles/gadget.dir/operators/flinkTrace.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/flinkTrace.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/flinkTrace.cpp

src/CMakeFiles/gadget.dir/operators/flinkTrace.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/flinkTrace.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/flinkTrace.cpp > CMakeFiles/gadget.dir/operators/flinkTrace.cpp.i

src/CMakeFiles/gadget.dir/operators/flinkTrace.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/flinkTrace.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/flinkTrace.cpp -o CMakeFiles/gadget.dir/operators/flinkTrace.cpp.s

src/CMakeFiles/gadget.dir/operators/continuousJoin.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/continuousJoin.cpp.o: ../src/operators/continuousJoin.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/CMakeFiles/gadget.dir/operators/continuousJoin.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/continuousJoin.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/continuousJoin.cpp

src/CMakeFiles/gadget.dir/operators/continuousJoin.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/continuousJoin.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/continuousJoin.cpp > CMakeFiles/gadget.dir/operators/continuousJoin.cpp.i

src/CMakeFiles/gadget.dir/operators/continuousJoin.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/continuousJoin.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/continuousJoin.cpp -o CMakeFiles/gadget.dir/operators/continuousJoin.cpp.s

src/CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.o: ../src/operators/slidingAllIncremental.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object src/CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingAllIncremental.cpp

src/CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingAllIncremental.cpp > CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.i

src/CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingAllIncremental.cpp -o CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.s

src/CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.o: ../src/operators/slidingKeyedIncremental.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object src/CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingKeyedIncremental.cpp

src/CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingKeyedIncremental.cpp > CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.i

src/CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingKeyedIncremental.cpp -o CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.s

src/CMakeFiles/gadget.dir/operators/joinTumbling.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/joinTumbling.cpp.o: ../src/operators/joinTumbling.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object src/CMakeFiles/gadget.dir/operators/joinTumbling.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/joinTumbling.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/joinTumbling.cpp

src/CMakeFiles/gadget.dir/operators/joinTumbling.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/joinTumbling.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/joinTumbling.cpp > CMakeFiles/gadget.dir/operators/joinTumbling.cpp.i

src/CMakeFiles/gadget.dir/operators/joinTumbling.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/joinTumbling.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/joinTumbling.cpp -o CMakeFiles/gadget.dir/operators/joinTumbling.cpp.s

src/CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.o: ../src/operators/slidingKeyedHolistic.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object src/CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingKeyedHolistic.cpp

src/CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingKeyedHolistic.cpp > CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.i

src/CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingKeyedHolistic.cpp -o CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.s

src/CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.o: ../src/operators/slidingAllHolistic.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object src/CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingAllHolistic.cpp

src/CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingAllHolistic.cpp > CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.i

src/CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/slidingAllHolistic.cpp -o CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.s

src/CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.o: ../src/operators/tumblingAllIncremental.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object src/CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingAllIncremental.cpp

src/CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingAllIncremental.cpp > CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.i

src/CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingAllIncremental.cpp -o CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.s

src/CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.o: ../src/operators/tumblingAllIHolistic.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object src/CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingAllIHolistic.cpp

src/CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingAllIHolistic.cpp > CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.i

src/CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingAllIHolistic.cpp -o CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.s

src/CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.o: ../src/operators/tumblingKeyedIncremental.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object src/CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingKeyedIncremental.cpp

src/CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingKeyedIncremental.cpp > CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.i

src/CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingKeyedIncremental.cpp -o CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.s

src/CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.o: ../src/operators/tumblingKeyedHolistic.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object src/CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingKeyedHolistic.cpp

src/CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingKeyedHolistic.cpp > CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.i

src/CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/tumblingKeyedHolistic.cpp -o CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.s

src/CMakeFiles/gadget.dir/operators/joinSliding.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/joinSliding.cpp.o: ../src/operators/joinSliding.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object src/CMakeFiles/gadget.dir/operators/joinSliding.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/joinSliding.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/joinSliding.cpp

src/CMakeFiles/gadget.dir/operators/joinSliding.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/joinSliding.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/joinSliding.cpp > CMakeFiles/gadget.dir/operators/joinSliding.cpp.i

src/CMakeFiles/gadget.dir/operators/joinSliding.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/joinSliding.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/joinSliding.cpp -o CMakeFiles/gadget.dir/operators/joinSliding.cpp.s

src/CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.o: ../src/operators/continuousAggregation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object src/CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/continuousAggregation.cpp

src/CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/continuousAggregation.cpp > CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.i

src/CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/continuousAggregation.cpp -o CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.s

src/CMakeFiles/gadget.dir/operators/intervalJoin.cpp.o: src/CMakeFiles/gadget.dir/flags.make
src/CMakeFiles/gadget.dir/operators/intervalJoin.cpp.o: ../src/operators/intervalJoin.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object src/CMakeFiles/gadget.dir/operators/intervalJoin.cpp.o"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gadget.dir/operators/intervalJoin.cpp.o -c /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/intervalJoin.cpp

src/CMakeFiles/gadget.dir/operators/intervalJoin.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gadget.dir/operators/intervalJoin.cpp.i"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/intervalJoin.cpp > CMakeFiles/gadget.dir/operators/intervalJoin.cpp.i

src/CMakeFiles/gadget.dir/operators/intervalJoin.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gadget.dir/operators/intervalJoin.cpp.s"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src/operators/intervalJoin.cpp -o CMakeFiles/gadget.dir/operators/intervalJoin.cpp.s

# Object files for target gadget
gadget_OBJECTS = \
"CMakeFiles/gadget.dir/core/main.cpp.o" \
"CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.o" \
"CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.o" \
"CMakeFiles/gadget.dir/operators/flinkTrace.cpp.o" \
"CMakeFiles/gadget.dir/operators/continuousJoin.cpp.o" \
"CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.o" \
"CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.o" \
"CMakeFiles/gadget.dir/operators/joinTumbling.cpp.o" \
"CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.o" \
"CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.o" \
"CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.o" \
"CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.o" \
"CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.o" \
"CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.o" \
"CMakeFiles/gadget.dir/operators/joinSliding.cpp.o" \
"CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.o" \
"CMakeFiles/gadget.dir/operators/intervalJoin.cpp.o"

# External object files for target gadget
gadget_EXTERNAL_OBJECTS =

src/gadget: src/CMakeFiles/gadget.dir/core/main.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/core/PerformanceMetrics.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/ycsbTrace.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/flinkTrace.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/continuousJoin.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/slidingAllIncremental.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/slidingKeyedIncremental.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/joinTumbling.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/slidingKeyedHolistic.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/slidingAllHolistic.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/tumblingAllIncremental.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/tumblingAllIHolistic.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/tumblingKeyedIncremental.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/tumblingKeyedHolistic.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/joinSliding.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/continuousAggregation.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/operators/intervalJoin.cpp.o
src/gadget: src/CMakeFiles/gadget.dir/build.make
src/gadget: /usr/local/lib/librocksdb.dylib
src/gadget: src/CMakeFiles/gadget.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Linking CXX executable gadget"
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gadget.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/gadget.dir/build: src/gadget

.PHONY : src/CMakeFiles/gadget.dir/build

src/CMakeFiles/gadget.dir/clean:
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src && $(CMAKE_COMMAND) -P CMakeFiles/gadget.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/gadget.dir/clean

src/CMakeFiles/gadget.dir/depend:
	cd /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/showanasyabi/Desktop/trash/caspGadget/Gadget /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/src /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src /Users/showanasyabi/Desktop/trash/caspGadget/Gadget/build/src/CMakeFiles/gadget.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/gadget.dir/depend

