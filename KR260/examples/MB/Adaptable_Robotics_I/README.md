#/******************************************************************************
# * Copyright (C) 2013 - 2021 Xilinx, Inc.  All rights reserved.
# * Copyright (C) 2024, Advanced Micro Devices, Inc
# * SPDX-License-Identifier: MIT
# ******************************************************************************/

To build this application, do the following:

1. source the XSCT environment. (I.e. VITIS)

2. Build the BSP
    >cd ros-dds-microblaze/KR260/BSP
    >make repo

2. Build the application:
    >cd ros-dds-microblaze/KR260/examples/MB/Adeptable_Robotics_I
    >make example

3. Clean workspace
    >make clean