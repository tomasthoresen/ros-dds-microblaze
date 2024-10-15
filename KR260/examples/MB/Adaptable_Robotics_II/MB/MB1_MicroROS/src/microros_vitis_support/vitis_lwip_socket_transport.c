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

#include <uxr/client/transport.h>

#include <rmw_microxrcedds_c/config.h>

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include <lwip/sockets.h>

#define UDP_PORT        8888
static int sock_fd = -1;

bool vitis_lwip_socket_transport_open(struct uxrCustomTransport * transport)
{
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        return false;
    }

    return true;
}

bool vitis_lwip_socket_transport_close(struct uxrCustomTransport * transport)
{
    if (sock_fd != -1)
    {
        closesocket(sock_fd);
        sock_fd = -1;
    }
    return true;
}

size_t vitis_lwip_socket_transport_write(struct uxrCustomTransport* transport, uint8_t * buf, size_t len, uint8_t * err)
{
    if (sock_fd == -1)
    {
        return 0;
    }
    const char * ip_addr = (const char*) transport->args;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    int ret = 0;
    ret = sendto(sock_fd, buf, len, 0, (struct sockaddr *)&addr, sizeof(addr));
    size_t writed = ret>0? ret:0;

    return writed;
}

size_t vitis_lwip_socket_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err)
{


	  int ret = 0;
	    //set timeout
	    struct timeval tv_out;
	    tv_out.tv_sec = timeout / 1000;
	    tv_out.tv_usec = (timeout % 1000) * 1000;

	    if( tv_out.tv_sec == 0 && tv_out.tv_usec == 0 )
	    {
	        tv_out.tv_usec = 1000;
	    }

	    ret = setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO,&tv_out, sizeof(tv_out));
	    if(ret != 0){
	    	xil_printf("setsockopt returned an error=%d\r\n", ret);
	    }
	    ret = recv(sock_fd, buf, len, MSG_WAITALL);
	    // if(ret != 0){
	    // 		//commented out as this always generate an error. lwip bug
	   	//     	xil_printf("recv returned an error=%d\r\n", ret);
	   	//     }
	    size_t readed = ret > 0 ? ret : 0;
	    return readed;


//    int ret = 0;
//    //set timeout
//    struct timeval tv_out;
//    tv_out.tv_sec = timeout / 1000;
//    tv_out.tv_usec = (timeout % 1000) * 1000;
//    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO,&tv_out, sizeof(tv_out));
//    ret = recv(sock_fd, buf, len, MSG_WAITALL);
//    size_t readed = ret > 0 ? ret : 0;
//    return readed;
}
