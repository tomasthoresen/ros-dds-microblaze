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
	@echo "  help       	Show this help message"
	@echo "  clean	    	Delete vitis workspace and embeddedsw repo"
	@echo "  example   	Build the workspace and applications"
	@echo "  "
	@echo "  "

#
# This target builds the workspace and the applications
#
all: check-tool clean example

#
# This target converts the bitstream of the FPGA to a binary which is compatible with fpgautil tool
#
bin: check-tool
	@echo "bootgen Required image file kr260_4mb_4pmod_wrapper.bit";\
	#check if kr260_4mb_4pmod_wrapper.bit exist
	@if [ -f kr260_4mb_4pmod_wrapper.bit ]; then \
		bootgen -image kr260_bit_bin.bif -arch zynqmp -o kr260_4mb_4pmod.bit.bin -w;\
		echo "Generated kr260_4mb_4pmod.bit.bin";\
	else \
		echo "kr260_4mb_4pmod_wrapper.bit does not exist";\
	fi
#
# This target builds the workspace and the applications
#
example: check-tool
	@echo "Building workspace and targets"; \
	#if workspace ./vitis_kr260_MB_ws exsists, delete it; \
	if [ -d $(VDIR) ]; then \
		rm -rf $(VDIR); \
	fi; \
	    xsct ./buildExample.ini

#
# This target checks if the tool is available
#
check-tool:
	@if command -v $(TOOL) > /dev/null; then \
		echo "$(TOOL) is available"; \
	else \
		echo "$(TOOL) is not available"; \
		echo "Xilinx Scripting Lanugage(XSCT) needs to be available and sourced"; \
		echo "XSCT can be standalone or it comes with VITIS."; \
		exit 1; \
	fi


#
# This target cleans the workspace and the bitstream
#
clean:
# check if the workspace vitis_kr260_ws-* exsists
	@if [ -d vitis_kr260_ws* ]; then \
		echo "Deleting workspace vitis_kr260_ws*"; \
		rm -rf vitis_kr260_*; \
	fi




.PHONY: help example all check-tool clean

