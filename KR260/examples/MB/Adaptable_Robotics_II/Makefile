#/******************************************************************************
# * Copyright (C) 2013 - 2021 Xilinx, Inc.  All rights reserved.
# * Copyright (C) 2024, Advanced Micro Devices, Inc
# * SPDX-License-Identifier: MIT
# ******************************************************************************/



SHELL := /bin/bash

TOOL=xsct
VDIR=vitis_kr260_ws
EDIR=embeddedsw
arg1=
arg2=

help:

	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  help       	    Show this help message"
	@echo "  clean	    Delete vitis workspace and embeddedsw repo"
	@echo "  example   Build the workspace and applications"
	@echo "  "


all: clean example


example: check-tool
	@echo "Building workspace and targets"; \
	if [ -d $(VDIR) ]; then \
		rm -rf $(VDIR); \
	fi; \
        xsct ./buildExample.ini


check-tool:
	@if command -v $(TOOL) > /dev/null; then \
		echo "$(TOOL) is available"; \
	else \
		echo "$(TOOL) is not available"; \
		echo "Xilinx Scripting Lanugage(XSCT) needs to be available and sourced"; \
		echo "XSCT can be standalone or it comes with VITIS."; \
		exit 1; \
	fi

clean:
	@echo "Deleting workspace vitis_kr260_ws-*"; \
		rm -rf vitis_kr260_*


.PHONY: help example all check-tool clean

