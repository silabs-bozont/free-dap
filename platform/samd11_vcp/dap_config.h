/*
 * Copyright (c) 2017, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DAP_CONFIG_H_
#define _DAP_CONFIG_H_

/*- Includes ----------------------------------------------------------------*/
#include "samd11.h"
#include "hal_gpio.h"

/*- Definitions -------------------------------------------------------------*/
HAL_GPIO_PIN(SWCLK_TCK,    A, 3)
HAL_GPIO_PIN(SWDIO_TMS,    A, 2)
HAL_GPIO_PIN(nRESET,       A, 4)

#define DAP_CONFIG_ENABLE_SWD
//#define DAP_CONFIG_ENABLE_JTAG

#define DAP_CONFIG_DEFAULT_PORT        DAP_PORT_SWD
#define DAP_CONFIG_DEFAULT_CLOCK       20000000 // Hz

#define DAP_CONFIG_PACKET_SIZE         64
#define DAP_CONFIG_PACKET_COUNT        1

// Set the value to NULL if you want to disable a string
// DAP_CONFIG_PRODUCT_STR must contain "CMSIS-DAP" to be compatible with the standard
#define DAP_CONFIG_VENDOR_STR          "Arduino"
#define DAP_CONFIG_PRODUCT_STR         "Si917 CMSIS-DAP Adapter"
#define DAP_CONFIG_SER_NUM_STR         usb_serial_number
#define DAP_CONFIG_FW_VER_STR          "v1.0"
#define DAP_CONFIG_DEVICE_VENDOR_STR   NULL
#define DAP_CONFIG_DEVICE_NAME_STR     NULL

//#define DAP_CONFIG_RESET_TARGET_FN     target_specific_reset_function

// A value at which dap_clock_test() produces 1 kHz output on the SWCLK pin
#define DAP_CONFIG_DELAY_CONSTANT      3400

// A threshold for switching to fast clock (no added delays)
// This is the frequency produced by dap_clock_test(1) on the SWCLK pin 
#define DAP_CONFIG_FAST_CLOCK          20000 // Hz

/*- Prototypes --------------------------------------------------------------*/
extern char usb_serial_number[16];

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWCLK_TCK_write(int value)
{
  HAL_GPIO_SWCLK_TCK_write(value);
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWDIO_TMS_write(int value)
{
  HAL_GPIO_SWDIO_TMS_write(value);
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_TDO_write(int value)
{
  (void)value;
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_nTRST_write(int value)
{
  (void)value;
}

//-----------------------------------------------------------------------------
// PA04 is resistor-bridged to the Si917 RESET_N / POC_IN network because the
// inverting MOSFET is not fitted.  Pulling that net low powers the Si917 down
// permanently: neither releasing to high-Z, driving it high, nor powering up
// with it held low brings the device back, and recovery needs the SJ1 ISP
// jumper.  Until the reset circuit is repaired the line is left alone.
static inline void DAP_CONFIG_nRESET_write(int value)
{
  /*
   * PA04 is resistor-bridged to the Si917 RESET_N / POC_IN pin because the
   * board's MOSFET was removed, so this line gates the target's power rather
   * than resetting it: driving it low powers the Si917 down and driving it
   * high boots it, which takes a couple of seconds.  Both edges have to be
   * driven; letting the pin float leaves the target off.
   */
  HAL_GPIO_nRESET_write(value);
  HAL_GPIO_nRESET_out();
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_SWCLK_TCK_read(void)
{
  return HAL_GPIO_SWCLK_TCK_read();
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_SWDIO_TMS_read(void)
{
  return HAL_GPIO_SWDIO_TMS_read();
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_TDI_read(void)
{
  return 0;
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_TDO_read(void)
{
  return 0;
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_nTRST_read(void)
{
  return 0;
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_nRESET_read(void)
{
  return HAL_GPIO_nRESET_read();
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWCLK_TCK_set(void)
{
  HAL_GPIO_SWCLK_TCK_set();
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWCLK_TCK_clr(void)
{
  HAL_GPIO_SWCLK_TCK_clr();
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWDIO_TMS_in(void)
{
  HAL_GPIO_SWDIO_TMS_in();
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWDIO_TMS_out(void)
{
  HAL_GPIO_SWDIO_TMS_out();
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SETUP(void)
{
  HAL_GPIO_SWCLK_TCK_in();
  HAL_GPIO_SWDIO_TMS_in();

  /* Release the target so it boots whenever the board is plugged in. */
  HAL_GPIO_nRESET_set();
  HAL_GPIO_nRESET_out();

  HAL_GPIO_SWDIO_TMS_pullup();
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_DISCONNECT(void)
{
  HAL_GPIO_SWCLK_TCK_in();
  HAL_GPIO_SWDIO_TMS_in();
  /* Leave nRESET as the debugger left it, so a running target keeps running. */
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_CONNECT_SWD(void)
{
  HAL_GPIO_SWDIO_TMS_out();
  HAL_GPIO_SWDIO_TMS_set();

  HAL_GPIO_SWCLK_TCK_out();
  HAL_GPIO_SWCLK_TCK_set();

  /* Leave the target powered; only an explicit reset request may drive it. */
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_CONNECT_JTAG(void)
{
  HAL_GPIO_SWDIO_TMS_out();
  HAL_GPIO_SWDIO_TMS_set();

  HAL_GPIO_SWCLK_TCK_out();
  HAL_GPIO_SWCLK_TCK_set();
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_LED(int index, int state)
{
  (void)index;
  (void)state;
}

#endif // _DAP_CONFIG_H_

