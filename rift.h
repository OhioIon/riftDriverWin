#ifndef _RIFT_H
#define _RIFT_H

// ********************* Includes       ********************* //
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

// ********************* Defines        ********************* //

// ********************* Types          ********************* //

// ********************* Data           ********************* //

// ********************* Interface      ********************* //

bool rift_send_keep_alive(HANDLE handle);
bool rift_send_enable_components(HANDLE handle, bool on_l);

#endif
