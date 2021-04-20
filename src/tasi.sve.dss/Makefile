#-----------------------------------------------------------------------------
#
# (C) 2014 European Space Agency
# European Space Operations Centre
# Darmstadt, Germany
#
#-----------------------------------------------------------------------------
#
# System:        UMF
#
# Sub-System:    esa.umf.cmake
#
# File Name:     Makefile
#
# Author:        VEGA
#
# Creation Date: July 28, 2011
#
#-----------------------------------------------------------------------------
# This file delegates standard CDT make targets to CMake generated Makefiles.
#-----------------------------------------------------------------------------

# Prerequisite: The following environment variables must be set
# UMF_CMAKE_MODULES_DIR - must point to CMake modules directory (from UMF)
# SMP2_HOME             - must point to SMP2 solution
# XXX_HOME              - must point to required solutions
SHELL := /bin/bash

# Build mode (may be overridden via environment variable)
BUILD_MODE ?= Debug

# Project name
PROJECT_NAME = $(notdir $(CURDIR))

# Build directory for CMake
SOLUTION_DIR = $(shell cat .settings/esa.umf.solution)
BUILD_ROOT_DIR = $(SOLUTION_DIR)/$(shell cat $(SOLUTION_DIR)/.settings/esa.umf.buildDir)
BUILD_DIR = $(BUILD_ROOT_DIR)/$(BUILD_MODE)

# Project root directory
PROJECT_ROOT_DIR = ..

# Solution-wide settings for Make
include $(SOLUTION_DIR)/Settings.mk

.PHONY: all
all: install

.PHONY: compile
compile: init
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME)

.PHONY: rules
rules: init
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME).rules

.PHONY: audit
audit: init
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME).audit

.PHONY: doxygen
doxygen: init
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME).doxygen

.PHONY: install
install: init
	+$(MAKE) -C $(BUILD_DIR)/$(PROJECT_NAME) install
	+$(MAKE) install.sh

.PHONY: clean
clean: init
	+$(MAKE) -C $(BUILD_DIR)/$(PROJECT_NAME) clean

.PHONY: simsat_deploy
simsat_deploy: init
	+$(MAKE) -C $(SOLUTION_DIR) simsat_deploy

.PHONY: coverage
coverage: init
	+$(MAKE) -C $(SOLUTION_DIR) coverage

.PHONY: coverage_html
coverage_html: init
	+$(MAKE) -C $(SOLUTION_DIR) coverage_html

.PHONY: genall
genall: gencpp gendoc genidl genxsd

.PHONY: gencpp
gencpp: init
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME).gencpp
	+$(MAKE) gencpp.sh

.PHONY: gendoc
gendoc: init
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME).gendoc
	+$(MAKE) gendoc.sh

.PHONY: genidl
genidl: init
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME).genidl

.PHONY: genxsd
genxsd: init
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME).genxsd

.PHONY: smp2cc
smp2cc: init
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME).smp2cc

.PHONY: install.sh gencpp.sh gendoc.sh
install.sh gencpp.sh gendoc.sh:
	@if [ -x $(CURDIR)/$@ ]; then \
		echo "++ Executing custom script $(CURDIR)/$@" ;\
		$(CURDIR)/$@ ;\
	fi

.PHONY: simsat_deploy.sh
simsat_deploy.sh:
	@if [ -x $(CURDIR)/$@ ]; then \
		echo "!! Custom script $(CURDIR)/$@ is deprecated and was not executed!" ;\
		echo "!! To fix this, use or create install.sh in the project directory and/or $@ in the solution directory." ;\
		exit 1;\
	fi

.PHONY: fast
fast:
	+$(MAKE) -C $(BUILD_DIR) $(PROJECT_NAME)/fast

.PHONY: init
init: $(BUILD_DIR)/CMakeFiles/Makefile2

$(BUILD_DIR)/CMakeFiles/Makefile2: \
		$(wildcard CMake*.txt) \
		$(wildcard .settings/esa.umf.*) \
		$(wildcard $(PROJECT_ROOT_DIR)/*/CMake*.txt) \
		$(wildcard $(PROJECT_ROOT_DIR)/*/.settings/esa.umf.*) \
		$(wildcard $(SOLUTION_DIR)/CMake*.txt) \
		$(wildcard $(SOLUTION_DIR)/.settings/esa.umf.*)
	+$(MAKE) -C $(SOLUTION_DIR) init

.PHONY: check_env
check_env:
