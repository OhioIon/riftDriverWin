// ********************* Includes       ********************* //
#include "rift.h"

extern "C" {
#include <hidsdi.h>
}

// ********************* Defines        ********************* //

// ********************* Types          ********************* //

// ********************* Data           ********************* //

// ********************* Prototypes     ********************* //

// ********************* Implementation ********************* //

// Send Keep-Alive feature report to Rift
bool rift_send_keep_alive(HANDLE handle)
{
  // Prepare buffer
  uint8_t buf_au8[5] = { 0 };
  const uint16_t tiKeepAlive_ms_cu16 = 10 * 1000;
  buf_au8[0] = 0x08; // Keep alive command
  buf_au8[1] = 0x00;
  buf_au8[2] = 0x00;
  buf_au8[3] = tiKeepAlive_ms_cu16 & 0xFF; // Keep alive interval
  buf_au8[4] = tiKeepAlive_ms_cu16 >> 8;   // Keep alive interval

  // Send feature report
  bool res_l = HidD_SetFeature(handle, buf_au8, sizeof(buf_au8));
  if( !res_l ) printf("Error: HidD_SetFeature() failed with %d\n", GetLastError() );

  return res_l;
}

// Send Enable-Components feature report to Rift
bool rift_send_enable_components(HANDLE handle, bool on_l)
{
  // Prepare buffer
  uint8_t buf_au8[4] = { 0 };
  buf_au8[0] = 0x1d; // Enable components command
  buf_au8[1] = 0x00;
  buf_au8[2] = 0x00;
  buf_au8[3] = (on_l != 0)? 0x01 : 0x00; // Flags: 1 = Display, 2 = Audio, 4 = LEDs

  // Send feature report
  bool res_l = HidD_SetFeature(handle, buf_au8, sizeof(buf_au8) );
  if( !res_l ) printf("Error: HidD_SetFeature() failed with %d\n", GetLastError());

  return res_l;
}

// EOF
