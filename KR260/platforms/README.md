#/******************************************************************************
# * Copyright (C) 2013 - 2021 Xilinx, Inc.  All rights reserved.
# * Copyright (C) 2024, Advanced Micro Devices, Inc
# * SPDX-License-Identifier: MIT
# ******************************************************************************/


Make notes about the different hardware implementations you are using here. I.e.

Platforms:
The directory named "xilinx_kr260_4mb_4pmod_202210_1" is the one used.

xilinx_kr260_4mb_4pmod_202210_1 - has the extra GPIO PHY reset implemented.
xilinx_kr260_4mb_4pmod_202210_1_10.07.24 works but does not have the extra
reset circuitry for the PHY reset. 
