# Makefile to build the executable for the EQRWS windy plugin.
#
# make [all]
# make clean

.PHONY: clean all

# Executable filename
TARGET=EQRWS_windy_plugin

# Where to find the source code to be built
SRC_DIR=src

# Where to find the application source code
APP_DIR=$(SRC_DIR)

# Where we will put the generated output files
OUTPUT_DIR=bin

# Where we will place the target executable files
TARGET_DIR=$(OUTPUT_DIR)/target

# Where the find the libraries
LIB_DIR=lib \
        lib/websocketpp \
        lib/jwt-cpp/include

# Libraries to link to. Order matters, place dependent libraries first.
STATIC_LIBS= slink
DYN_LIBS= pthread

# The C++ and C compilers to be used
CROSS_COMPILE:=
CXX=$(CROSS_COMPILE)g++
CC=$(CROSS_COMPILE)gcc

# C++ compiler flags
# -g                    Adds debug information to the executable file
# -Wall                 Turns on most, but not all compiler flags
# -w                    Turns off all compiler warnings
# -On                   Compiler optimisation level, n=s optimises for size
# -std=c++2a            Use the c++20 standard
# -static-libstdc++     Statically links the std library set, to avoid compatibility issues when deploying to the BBB
# -ggdb3                Debug info useful for valgrind diagnostics
# -Wno-psabi			Ignores an ABI change warning between GCC compilers version 6 and 7.1
# -Wno-format-truncation Ignores warnings about truncating numbers with c-style formatting
CXXFLAGS= -Wall -Os -std=c++2a -static-libstdc++ -Wno-psabi -Wno-format-truncation

# Create a list of required objects from all the files with a ".cpp" or ".c" extension within the source directory
OBJS+=$(patsubst $(APP_DIR)/%.cpp, $(OUTPUT_DIR)/%.o, $(shell find $(APP_DIR) -name *.cpp))
OBJS+=$(patsubst $(APP_DIR)/%.c, $(OUTPUT_DIR)/%.o, $(shell find $(APP_DIR) -name *.c))

# Create a list of header file include paths containing all directories inside the source directory
INC_DIRS=$(shell find $(APP_DIR) -type d)
INC_DIRS+=$(LIB_DIR)

# Flags passed to the preprocessor. Add the include paths
CPPFLAGS+=$(foreach dir,$(INC_DIRS),-I$(dir))

# Linker flags. Link the libraries
LDFLAGS += $(foreach lib,$(STATIC_LIBS),-l:lib$(lib).a)
LDFLAGS += $(foreach lib,$(DYN_LIBS),-l$(lib))

# Set the makefile internal search to cover all sub-directories within the source directory
space :=
space +=
VPATH := $(subst $(space),:,$(shell find $(SRC_DIR) -type d))

#--------------------------------------------------------------------------------------------------#

default: all

all: $(TARGET_DIR)/$(TARGET)
	@echo Nothing to be done

clean:
	@echo Cleaning...
	rm -rf $(OUTPUT_DIR)/*
	@echo Output cleaned.

$(OUTPUT_DIR)/%.o : %.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OUTPUT_DIR)/%.o : %.c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET_DIR)/$(TARGET) : $(OBJS)
	mkdir -p $(TARGET_DIR)
	@echo Linking target $(TARGET_DIR)/$(TARGET)...
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $(TARGET_DIR)/$(TARGET) $(OBJS) $(LDFLAGS)
	@echo Target $(TARGET_DIR)/$(TARGET) built

