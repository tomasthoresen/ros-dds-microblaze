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


#include "FreeRTOS.h"
#include "task.h"
#include <time.h>

#include <rmw_microros/rmw_microros.h>
#include <rcl/rcl.h>

#include "../platform_config.h"

// Transport signatures
bool vitis_lwip_socket_transport_open(struct uxrCustomTransport * transport);
bool vitis_lwip_socket_transport_close(struct uxrCustomTransport * transport);
size_t vitis_lwip_socket_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
size_t vitis_lwip_socket_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

// Memory allocation
void * vitis_allocate(size_t size, void * state)
{
    return malloc(size);
}

void vitis_deallocate(void * pointer, void * state)
{
    free(pointer);
}

void * vitis_reallocate(void * pointer, size_t size, void * state)
{
    return realloc(pointer, size);
}

void * vitis_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state)
{
    return calloc(number_of_elements, size_of_element);
}

// Clock handling
#define MICROSECONDS_PER_SECOND    ( 1000000LL )                                   /**< Microseconds per second. */
#define NANOSECONDS_PER_SECOND     ( 1000000000LL )                                /**< Nanoseconds per second. */
#define NANOSECONDS_PER_TICK       ( NANOSECONDS_PER_SECOND / configTICK_RATE_HZ ) /**< Nanoseconds per FreeRTOS tick. */

void UTILS_NanosecondsToTimespec( int64_t llSource,
                                  struct timespec * const pxDestination )
{
    long lCarrySec = 0;

    /* Convert to timespec. */
    pxDestination->tv_sec = ( time_t ) ( llSource / NANOSECONDS_PER_SECOND );
    pxDestination->tv_nsec = ( long ) ( llSource % NANOSECONDS_PER_SECOND );

    /* Subtract from tv_sec if tv_nsec < 0. */
    if( pxDestination->tv_nsec < 0L )
    {
        /* Compute the number of seconds to carry. */
        lCarrySec = ( pxDestination->tv_nsec / ( long ) NANOSECONDS_PER_SECOND ) + 1L;

        pxDestination->tv_sec -= ( time_t ) ( lCarrySec );
        pxDestination->tv_nsec += lCarrySec * ( long ) NANOSECONDS_PER_SECOND;
    }
}

int clock_gettime(clockid_t unused, struct timespec *tp)
{

    ( void ) unused;

    TimeOut_t xCurrentTime = { 0 };
    uint64_t ullTickCount = 0ULL;

    vTaskSetTimeOutState( &xCurrentTime );

    ullTickCount = ( uint64_t ) ( xCurrentTime.xOverflowCount ) << ( sizeof( TickType_t ) * 8 );

    ullTickCount += xCurrentTime.xTimeOnEntering;
    if(FREERTOS_TIME_DEBUG == 1){
    	xil_printf("clock_gettime shows %x\r\n",ullTickCount);
    }

    UTILS_NanosecondsToTimespec( ( int64_t ) ullTickCount * NANOSECONDS_PER_TICK, tp );

    return 0;
}

// Micro-ROS configuration
void configure_microros()
{
	rmw_uros_set_custom_transport(
		false,
		"192.168.2.114",
		vitis_lwip_socket_transport_open,
		vitis_lwip_socket_transport_close,
		vitis_lwip_socket_transport_write,
		vitis_lwip_socket_transport_read
	);

    rcl_allocator_t vitis_allocator = rcutils_get_zero_initialized_allocator();
    vitis_allocator.allocate = vitis_allocate;
    vitis_allocator.deallocate = vitis_deallocate;
    vitis_allocator.reallocate = vitis_reallocate;
    vitis_allocator.zero_allocate =  vitis_zero_allocate;

    if (!rcutils_set_default_allocator(&vitis_allocator)) {
        printf("Error on default allocators (line %d)\n", __LINE__);
    }
}
