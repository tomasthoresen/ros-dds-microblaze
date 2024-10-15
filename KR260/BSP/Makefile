#/******************************************************************************
# * Copyright (C) 2013 - 2021 Xilinx, Inc.  All rights reserved.
# * Copyright (C) 2024, Advanced Micro Devices, Inc
# * SPDX-License-Identifier: MIT
# ******************************************************************************/



SHELL := /bin/bash

TOOL=xsct
VDIR=vitis_kr260_ws
EDIR=embeddedsw

help:

	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  help		Show this help message"
	@echo "  repo		Clone embeddedsw BSP and apply patches"
	@echo "  clean	Clean"
	@echo "  "


repo:
	@echo "Cloning embeddedsw repo"
	git clone  https://github.com/Xilinx/embeddedsw
	cd embeddedsw && git checkout xlnx_rel_v2022.1 && git apply ../patch/*.patch


clean:
	@echo "Deleting repo embeddedsw*"; \
		rm -rf embeddedsw*






.PHONY: help repo clean

