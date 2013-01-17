###############################################################################
#
# Name:        SCLPL Compiler Driver
# Author:      Mike Lowis
# License:     BSD 2-Clause
# Description: The main driver application that compiles SCLPL source code to
#              native binaries using a suite of command line tools.
#
###############################################################################

# Utility Function Definitions
#-----------------------------
# Function for generating a file list
flist = $(shell env find $(1) -name "*.$(strip $(2))" -print)

# Project and Artifact Names
#---------------------------
BIN_NAME    = sclpl
TEST_RUNNER = $(BIN_NAME)-test

# File and Directory Settings
#----------------------------
# Root Directories
PROJ_ROOT = ../../
DIST_ROOT = $(PROJ_ROOT)/dist
SRC_ROOT  = src/
TEST_ROOT = test/

# File Extensions
SRC_EXT  = scm
TEST_EXT = scm

# Source File Lists
SRC_FILES = $(call flist, $(SRC_ROOT), $(SRC_EXT))
TEST_FILES = $(call flist, $(TEST_ROOT), $(TEST_EXT))

# Object File Lists
SRC_OBJS  = $(SRC_FILES:%.$(SRC_EXT)=%.o)
TEST_OBJS = $(TEST_FILES:%.$(TEST_EXT)=%.o)

# Include Directories
SRC_INCS  = -I inc
TEST_INCS = -I inc

# Compiler and Linker Options
#----------------------------
CSC = csc
CSCFLAGS = -c -explicit-use -compile-syntax

# Build Rules
#------------

# List all rules not named for files/folders on disk
.PHONY: all release

all: release

#release: test $(BIN_NAME)
release: $(DIST_ROOT) $(BIN_NAME)
	@echo Copying to $(DIST_ROOT)...
	@cp $(BIN_NAME)* $(DIST_ROOT)

#test: $(TEST_RUNNER)
#	@echo Running unit tests...
#	@./$(TEST_RUNNER)

# Binaries
$(BIN_NAME): $(SRC_OBJS)
	@echo Linking $@...
	@$(CSC) -o $@ $(SRC_OBJS) $(LIBS)

$(TEST_RUNNER): $(SRC_OBJS) $(TEST_OBJS)
	@echo Linking $@...
	@$(CSC) -o $@ $(TEST_OBJS) $(filter-out src/main.o,$(SRC_OBJS))

# Object Files
$(SRC_OBJS): %.o : %.$(SRC_EXT)
	@echo $<
	@$(CSC) $(CSCFLAGS) $(SRC_INCS) -o $@ $<

#$(TEST_OBJS): %.o : %.$(TEST_EXT)
#	@echo $<
#	@$(CSC) $(CSCFLAGS) $(TEST_INCS) -o $@ $<

$(DIST_ROOT):
	mkdir $(DIST_ROOT)

# Cleanup
clean:
	@echo Cleaning up...
	@$(RM) $(TEST_OBJS)
	@$(RM) $(SRC_OBJS)
	@$(RM) $(BIN_NAME)*
#	@$(RM) $(TEST_RUNNER)*

