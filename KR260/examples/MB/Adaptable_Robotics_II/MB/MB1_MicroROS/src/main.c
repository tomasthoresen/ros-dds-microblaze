/*
 * Copyright (C) 2024, Advanced Micro Devices, Inc
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2016 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform_config.h"
#include "xil_printf.h"
#include <xil_io.h>
#include <stdbool.h>
#include <xiltimer.h>
#include "xgpio_l.h"
#include <xio_switch.h>

#include "microros_vitis_support/microros_vitis_support.h"

#include <FreeRTOS.h>
#include <task.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>

#if LWIP_IPV6==1
#include "lwip/ip.h"
#else
#if LWIP_DHCP==1
#include "lwip/dhcp.h"
#endif
#endif

#ifdef XPS_BOARD_ZCU102
#ifdef XPAR_XIICPS_0_DEVICE_ID
int IicPhyReset(void);
#endif
#endif

int main_thread();
void main_network(void *);
void print_echo_app_header();
void echo_application_thread(void *);
void timer_callback(rcl_timer_t * timer, int64_t last_call_time);

void lwip_init();

#if LWIP_IPV6==0
#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
#endif

#define THREAD_STACKSIZE 2048

static struct netif server_netif;
struct netif *echo_netif;

int sock;

rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){while(true){xil_printf("ERROR ... \n\r");};}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

/*
 * gettimeofday -- go out via exit...
 */
__attribute__((weak))int gettimeofday(struct timeval *__restrict __p,
		void *__restrict __tz) {
	while (1)
		;
}


void timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
//  xil_printf("Enter timer_callback function ... \n\r");
	RCLC_UNUSED(last_call_time);
	if (timer != NULL) {
		RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
		msg.data++;
		//  xil_printf("Publishing ... \n\r");
	}
}

#if LWIP_IPV6==1
void print_ip6(char *msg, ip_addr_t *ip)
{
	print(msg);
	xil_printf(" %x:%x:%x:%x:%x:%x:%x:%x\n\r",
			IP6_ADDR_BLOCK1(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK2(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK3(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK4(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK5(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK6(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK7(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK8(&ip->u_addr.ip6));
}

#else
void print_ip(char *msg, ip_addr_t *ip) {
	xil_printf(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip),
			ip4_addr4(ip));
}

void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw) {

	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}

#endif

void microros_int32_publisher() {

	configure_microros();

	rcl_allocator_t allocator = rcl_get_default_allocator();
	rclc_support_t support;

	// init micro_ros
	rcl_ret_t ret = rclc_support_init(&support, 0, NULL, &allocator);
	if (ret != RCL_RET_OK) {
		printf("Error rclc_support_init (line %d)\n", __LINE__);
	}

	// create node
	rcl_node_t node;
	ret = rclc_node_init_default(&node, "microros_node1", "", &support);
	if (ret != RCL_RET_OK) {
		printf("Error rclc_node_init_default (line %d)\n", __LINE__);
	}

	// create publisher
	ret = rclc_publisher_init_default(&publisher, &node,
			ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
			"micro_ros_MicroBlaze1_node_publisher");
	if (ret != RCL_RET_OK) {
		printf("Error rclc_node_init_default (line %d)\n", __LINE__);
	}

	// create timer,
	rcl_timer_t timer;
	ret = rclc_timer_init_default(&timer, &support,
	RCL_MS_TO_NS(100), timer_callback);
	if (ret != RCL_RET_OK) {
		printf("Error rclc_timer_init_default (line %d)\n", __LINE__);
	}

	// create executor
	rclc_executor_t executor;

	ret = rclc_executor_init(&executor, &support.context, 1, &allocator);
	if (ret != RCL_RET_OK) {
		printf("Error rclc_timer_init_default (line %d)\n", __LINE__);
	}
	ret = rclc_executor_add_timer(&executor, &timer);
	if (ret != RCL_RET_OK) {
		printf("Error rclc_timer_init_default (line %d)\n", __LINE__);
	}
	msg.data = 0;

	while (true) {
		RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
	}
}

int main() {

	/*
	 * Setting up the IO_Switch to enable serial data.
	 */

	init_io_switch(XPAR_IO_SWITCH_0_IO_SWITCH_WIDTH,
	MB_XPAR_IO_SWITCH_0_S_AXI_BASEADDR); //sets all pins as GPIO
	/*
	 * Configure UART
	 */
	set_pin(0, UART0_TX, MB_XPAR_IO_SWITCH_0_S_AXI_BASEADDR); //out pmod pin 1
	set_pin(1, UART0_RX, MB_XPAR_IO_SWITCH_0_S_AXI_BASEADDR); //out pmod pin 1

	/*
	 * Configure SPI
	 */
#ifdef SET_SPI_SWITCH
	set_pin(6, SS0, MB_XPAR_IO_SWITCH_0_S_AXI_BASEADDR);
	set_pin(7, MOSI0, MB_XPAR_IO_SWITCH_0_S_AXI_BASEADDR);
	set_pin(8, MISO0, MB_XPAR_IO_SWITCH_0_S_AXI_BASEADDR);
	set_pin(9, SPICLK0, MB_XPAR_IO_SWITCH_0_S_AXI_BASEADDR);
#endif

	/*
	 * Configure I2C
	 */
	set_pin(9, SDA0, MB_XPAR_IO_SWITCH_0_S_AXI_BASEADDR);
	set_pin(10, SCL0, MB_XPAR_IO_SWITCH_0_S_AXI_BASEADDR);
//	PIN11 = GND
//	PIN12 = VCC

	xil_printf("MicroBlaze1 freertos lwip example started\r\n\n");

	xil_printf("Resetting the Ethernet PHY.\r\n\n");
	/*
	 * Resetting the Ethernet PHY
	 */
	reset_phy();

	/*
	 * Start FreeRTOS
	 */
	sys_thread_new("main_thrd", (void (*)(void*)) main_thread, 0,
	THREAD_STACKSIZE,
	DEFAULT_THREAD_PRIO);
	vTaskStartScheduler();
	while (1)
		;
	return 0;
}

void network_thread(void *p) {
	struct netif *netif;
	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] =
			{ 0x00, 0x0a, 0x35, 0x00, 0x01, 0x07 };
#if LWIP_IPV6==0
	ip_addr_t ipaddr, netmask, gw;
#if LWIP_DHCP==1
	int mscnt = 0;
#endif
#endif

	netif = &server_netif;


#if LWIP_IPV6==0
#if LWIP_DHCP==0
	/* initialize IP addresses to be used */
	IP4_ADDR(&ipaddr, 192, 168, 2, 160);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 2, 1);
#endif

	/* print out IP settings of the board */

#if LWIP_DHCP==0
	print_ip_settings(&ipaddr, &netmask, &gw);
	/* print all application headers */
#endif

#if LWIP_DHCP==1
	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#endif
#endif

#if LWIP_IPV6==0
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address,
	PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\r\n");
		return;
	}
#else
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, NULL, NULL, NULL, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\r\n");
		return;
	}

	netif->ip6_autoconfig_enabled = 1;

	netif_create_ip6_linklocal_address(netif, 1);
	netif_ip6_addr_set_state(netif, 0, IP6_ADDR_VALID);

	print_ip6("\n\rBoard IPv6 address ", &netif->ip6_addr[0].u_addr.ip6);
#endif

	netif_set_default(netif);

	/* specify that the network if is up */
	netif_set_up(netif);

	/* start packet receive thread - required for lwIP operation */
	sys_thread_new("xemacif_input_thread",
			(void (*)(void*)) xemacif_input_thread, netif,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO);

#if LWIP_IPV6==0
#if LWIP_DHCP==1
	dhcp_start(netif);
	while (1) {
		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
		dhcp_fine_tmr();
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
			dhcp_coarse_tmr();
			mscnt = 0;
		}
	}
#else

	xil_printf("\r\n");
	xil_printf("Starting Micro-ROS app\r\n\n");
	

	print_echo_app_header();
	xil_printf("\r\n");

	sys_thread_new("echod", echo_application_thread, 0,
	THREAD_STACKSIZE,
	DEFAULT_THREAD_PRIO);
	vTaskDelete(NULL);
#endif
#else
	print_echo_app_header();
	xil_printf("\r\n");
	sys_thread_new("echod",echo_application_thread, 0,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO);
	vTaskDelete(NULL);
#endif
	return;
}

int main_thread() {
#if LWIP_DHCP==1
	int mscnt = 0;
#endif

#ifdef XPS_BOARD_ZCU102
	IicPhyReset();
#endif

	/* initialize lwIP before calling sys_thread_new */
	lwip_init();

	/* any thread using lwIP should be created using sys_thread_new */
	sys_thread_new("NW_THRD", network_thread, NULL,
	THREAD_STACKSIZE,
	DEFAULT_THREAD_PRIO);

#if LWIP_IPV6==0
#if LWIP_DHCP==1
	while (1) {
		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
		if (server_netif.ip_addr.addr) {
			xil_printf("DHCP request success\r\n");
			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			print_echo_app_header();
			xil_printf("\r\n");
			sys_thread_new("echod", echo_application_thread, 0,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			break;
		}
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS * 2000) {
			xil_printf("ERROR: DHCP request timed out\r\n");
			xil_printf("Configuring default IP of 192.168.2.160\r\n");
			IP4_ADDR(&(server_netif.ip_addr), 192, 168, 2, 160);
			IP4_ADDR(&(server_netif.netmask), 255, 255, 255, 0);
			IP4_ADDR(&(server_netif.gw), 192, 168, 2, 1);
			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			/* print all application headers */
			xil_printf("\r\n");
			xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
			xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");

			print_echo_app_header();
			xil_printf("\r\n");
			sys_thread_new("echod", echo_application_thread, 0,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			break;
		}
	}
#endif
#endif
	vTaskDelete(NULL);
	return 0;
}
