/******************************************************************************
 * Copyright (C) 2024, Advanced Micro Devices, Inc
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

/******************************************************************************
 *
 * Copyright (C) 2014 - 2018 Xilinx, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

#include <stdio.h>

/*
 * Include Xilinx headers
 */

#include <xparameters.h>
#include <xiic.h>
#include <xil_io.h>
#include <xil_printf.h>
#include <xio_switch.h>


#ifdef __cplusplus
extern "C"
{
}
#endif

extern char pins[XPAR_IO_SWITCH_0_IO_SWITCH_WIDTH];


/****************************************************************************/
/**
 * @brief	Print through PMOD example
 *
 * @param	None.
 *
 * @return	None.
 *
 *
 * @note	
 * 
 * This is just a simple print routine to show connectivity between the PMOD
 * and the MicroBlaze. 
 *
 ****************************************************************************/
int main() {

	/*
	 * Setup IO switch to configure the PMOD one.
	 */

	init_io_switch(XPAR_IO_SWITCH_0_IO_SWITCH_WIDTH,
	0x80100000);

	pins[0] = UART0_TX;
	pins[1] = UART0_RX;
	pins[4] = UART1_TX;
	pins[5] = UART1_RX;
	pins[2] = SDA0;
	pins[3] = SCL0;


	config_io_switch(XPAR_IO_SWITCH_0_IO_SWITCH_WIDTH,
		0x80100000);


	xil_printf("MicroBlaze #1 - Starting MicroBlaze example\r\n\n");
	xil_printf("MicroBlaze #1 - configured PMOD # 1 io_switch\r\n");

	while(1){
    	xil_printf("MicroBlaze #1 - configured PMOD # 1 io_switch\r\n");
        printf("Hello from MicroBlaze #1 \r\n");
    }
	

	return(0);
}

