# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cata/INRIA/damaris/trunk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cata/INRIA/damaris/trunk

# Include any dependencies generated for this target.
include server/CMakeFiles/damaris-server.dir/depend.make

# Include the progress variables for this target.
include server/CMakeFiles/damaris-server.dir/progress.make

# Include the compile flags for this target's objects.
include server/CMakeFiles/damaris-server.dir/flags.make

server/CMakeFiles/damaris-server.dir/Initiator.cpp.o: server/CMakeFiles/damaris-server.dir/flags.make
server/CMakeFiles/damaris-server.dir/Initiator.cpp.o: server/Initiator.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/cata/INRIA/damaris/trunk/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object server/CMakeFiles/damaris-server.dir/Initiator.cpp.o"
	cd /home/cata/INRIA/damaris/trunk/server && /usr/local/bin/mpicxx   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/damaris-server.dir/Initiator.cpp.o -c /home/cata/INRIA/damaris/trunk/server/Initiator.cpp

server/CMakeFiles/damaris-server.dir/Initiator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/damaris-server.dir/Initiator.cpp.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_PREPROCESSED_SOURCE

server/CMakeFiles/damaris-server.dir/Initiator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/damaris-server.dir/Initiator.cpp.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_ASSEMBLY_SOURCE

server/CMakeFiles/damaris-server.dir/Initiator.cpp.o.requires:
.PHONY : server/CMakeFiles/damaris-server.dir/Initiator.cpp.o.requires

server/CMakeFiles/damaris-server.dir/Initiator.cpp.o.provides: server/CMakeFiles/damaris-server.dir/Initiator.cpp.o.requires
	$(MAKE) -f server/CMakeFiles/damaris-server.dir/build.make server/CMakeFiles/damaris-server.dir/Initiator.cpp.o.provides.build
.PHONY : server/CMakeFiles/damaris-server.dir/Initiator.cpp.o.provides

server/CMakeFiles/damaris-server.dir/Initiator.cpp.o.provides.build: server/CMakeFiles/damaris-server.dir/Initiator.cpp.o

server/CMakeFiles/damaris-server.dir/Server.cpp.o: server/CMakeFiles/damaris-server.dir/flags.make
server/CMakeFiles/damaris-server.dir/Server.cpp.o: server/Server.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/cata/INRIA/damaris/trunk/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object server/CMakeFiles/damaris-server.dir/Server.cpp.o"
	cd /home/cata/INRIA/damaris/trunk/server && /usr/local/bin/mpicxx   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/damaris-server.dir/Server.cpp.o -c /home/cata/INRIA/damaris/trunk/server/Server.cpp

server/CMakeFiles/damaris-server.dir/Server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/damaris-server.dir/Server.cpp.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_PREPROCESSED_SOURCE

server/CMakeFiles/damaris-server.dir/Server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/damaris-server.dir/Server.cpp.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_ASSEMBLY_SOURCE

server/CMakeFiles/damaris-server.dir/Server.cpp.o.requires:
.PHONY : server/CMakeFiles/damaris-server.dir/Server.cpp.o.requires

server/CMakeFiles/damaris-server.dir/Server.cpp.o.provides: server/CMakeFiles/damaris-server.dir/Server.cpp.o.requires
	$(MAKE) -f server/CMakeFiles/damaris-server.dir/build.make server/CMakeFiles/damaris-server.dir/Server.cpp.o.provides.build
.PHONY : server/CMakeFiles/damaris-server.dir/Server.cpp.o.provides

server/CMakeFiles/damaris-server.dir/Server.cpp.o.provides.build: server/CMakeFiles/damaris-server.dir/Server.cpp.o

server/CMakeFiles/damaris-server.dir/ServerC.cpp.o: server/CMakeFiles/damaris-server.dir/flags.make
server/CMakeFiles/damaris-server.dir/ServerC.cpp.o: server/ServerC.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/cata/INRIA/damaris/trunk/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object server/CMakeFiles/damaris-server.dir/ServerC.cpp.o"
	cd /home/cata/INRIA/damaris/trunk/server && /usr/local/bin/mpicxx   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/damaris-server.dir/ServerC.cpp.o -c /home/cata/INRIA/damaris/trunk/server/ServerC.cpp

server/CMakeFiles/damaris-server.dir/ServerC.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/damaris-server.dir/ServerC.cpp.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_PREPROCESSED_SOURCE

server/CMakeFiles/damaris-server.dir/ServerC.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/damaris-server.dir/ServerC.cpp.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_ASSEMBLY_SOURCE

server/CMakeFiles/damaris-server.dir/ServerC.cpp.o.requires:
.PHONY : server/CMakeFiles/damaris-server.dir/ServerC.cpp.o.requires

server/CMakeFiles/damaris-server.dir/ServerC.cpp.o.provides: server/CMakeFiles/damaris-server.dir/ServerC.cpp.o.requires
	$(MAKE) -f server/CMakeFiles/damaris-server.dir/build.make server/CMakeFiles/damaris-server.dir/ServerC.cpp.o.provides.build
.PHONY : server/CMakeFiles/damaris-server.dir/ServerC.cpp.o.provides

server/CMakeFiles/damaris-server.dir/ServerC.cpp.o.provides.build: server/CMakeFiles/damaris-server.dir/ServerC.cpp.o

server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o: server/CMakeFiles/damaris-server.dir/flags.make
server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o: server/ServerFortran.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/cata/INRIA/damaris/trunk/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o"
	cd /home/cata/INRIA/damaris/trunk/server && /usr/local/bin/mpicxx   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/damaris-server.dir/ServerFortran.cpp.o -c /home/cata/INRIA/damaris/trunk/server/ServerFortran.cpp

server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/damaris-server.dir/ServerFortran.cpp.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_PREPROCESSED_SOURCE

server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/damaris-server.dir/ServerFortran.cpp.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_ASSEMBLY_SOURCE

server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o.requires:
.PHONY : server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o.requires

server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o.provides: server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o.requires
	$(MAKE) -f server/CMakeFiles/damaris-server.dir/build.make server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o.provides.build
.PHONY : server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o.provides

server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o.provides.build: server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o

server/CMakeFiles/damaris-server.dir/Options.cpp.o: server/CMakeFiles/damaris-server.dir/flags.make
server/CMakeFiles/damaris-server.dir/Options.cpp.o: server/Options.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/cata/INRIA/damaris/trunk/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object server/CMakeFiles/damaris-server.dir/Options.cpp.o"
	cd /home/cata/INRIA/damaris/trunk/server && /usr/local/bin/mpicxx   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/damaris-server.dir/Options.cpp.o -c /home/cata/INRIA/damaris/trunk/server/Options.cpp

server/CMakeFiles/damaris-server.dir/Options.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/damaris-server.dir/Options.cpp.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_PREPROCESSED_SOURCE

server/CMakeFiles/damaris-server.dir/Options.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/damaris-server.dir/Options.cpp.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_CXX_CREATE_ASSEMBLY_SOURCE

server/CMakeFiles/damaris-server.dir/Options.cpp.o.requires:
.PHONY : server/CMakeFiles/damaris-server.dir/Options.cpp.o.requires

server/CMakeFiles/damaris-server.dir/Options.cpp.o.provides: server/CMakeFiles/damaris-server.dir/Options.cpp.o.requires
	$(MAKE) -f server/CMakeFiles/damaris-server.dir/build.make server/CMakeFiles/damaris-server.dir/Options.cpp.o.provides.build
.PHONY : server/CMakeFiles/damaris-server.dir/Options.cpp.o.provides

server/CMakeFiles/damaris-server.dir/Options.cpp.o.provides.build: server/CMakeFiles/damaris-server.dir/Options.cpp.o

# Object files for target damaris-server
damaris__server_OBJECTS = \
"CMakeFiles/damaris-server.dir/Initiator.cpp.o" \
"CMakeFiles/damaris-server.dir/Server.cpp.o" \
"CMakeFiles/damaris-server.dir/ServerC.cpp.o" \
"CMakeFiles/damaris-server.dir/ServerFortran.cpp.o" \
"CMakeFiles/damaris-server.dir/Options.cpp.o"

# External object files for target damaris-server
damaris__server_EXTERNAL_OBJECTS =

server/libdamaris-server.a: server/CMakeFiles/damaris-server.dir/Initiator.cpp.o
server/libdamaris-server.a: server/CMakeFiles/damaris-server.dir/Server.cpp.o
server/libdamaris-server.a: server/CMakeFiles/damaris-server.dir/ServerC.cpp.o
server/libdamaris-server.a: server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o
server/libdamaris-server.a: server/CMakeFiles/damaris-server.dir/Options.cpp.o
server/libdamaris-server.a: server/CMakeFiles/damaris-server.dir/build.make
server/libdamaris-server.a: server/CMakeFiles/damaris-server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libdamaris-server.a"
	cd /home/cata/INRIA/damaris/trunk/server && $(CMAKE_COMMAND) -P CMakeFiles/damaris-server.dir/cmake_clean_target.cmake
	cd /home/cata/INRIA/damaris/trunk/server && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/damaris-server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
server/CMakeFiles/damaris-server.dir/build: server/libdamaris-server.a
.PHONY : server/CMakeFiles/damaris-server.dir/build

server/CMakeFiles/damaris-server.dir/requires: server/CMakeFiles/damaris-server.dir/Initiator.cpp.o.requires
server/CMakeFiles/damaris-server.dir/requires: server/CMakeFiles/damaris-server.dir/Server.cpp.o.requires
server/CMakeFiles/damaris-server.dir/requires: server/CMakeFiles/damaris-server.dir/ServerC.cpp.o.requires
server/CMakeFiles/damaris-server.dir/requires: server/CMakeFiles/damaris-server.dir/ServerFortran.cpp.o.requires
server/CMakeFiles/damaris-server.dir/requires: server/CMakeFiles/damaris-server.dir/Options.cpp.o.requires
.PHONY : server/CMakeFiles/damaris-server.dir/requires

server/CMakeFiles/damaris-server.dir/clean:
	cd /home/cata/INRIA/damaris/trunk/server && $(CMAKE_COMMAND) -P CMakeFiles/damaris-server.dir/cmake_clean.cmake
.PHONY : server/CMakeFiles/damaris-server.dir/clean

server/CMakeFiles/damaris-server.dir/depend:
	cd /home/cata/INRIA/damaris/trunk && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cata/INRIA/damaris/trunk /home/cata/INRIA/damaris/trunk/server /home/cata/INRIA/damaris/trunk /home/cata/INRIA/damaris/trunk/server /home/cata/INRIA/damaris/trunk/server/CMakeFiles/damaris-server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : server/CMakeFiles/damaris-server.dir/depend

