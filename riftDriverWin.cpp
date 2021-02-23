// riftDriverWin.cpp
//

// Includes
#include <iostream>
#include <wtypes.h>

extern "C" {
#include <initguid.h>
#include <hidsdi.h>
#include <setupapi.h>
}

#include "rift.h"

// Types
typedef struct _HID_DEVICE
{
    WCHAR              DevicePath[1024];  // Device path string
    HANDLE             HidDevice;         // A file handle to the hid device.
    HIDD_ATTRIBUTES    Attributes;        // Device attributes (vendor ID, product ID)

} HID_DEVICE;

// Data
static volatile bool keep_running = true;

// Prototypes

// Implementation

// Thread to check if return key was hit on keyboard
DWORD WINAPI userInput_thread( void* pContext )
{
    // Wait for any user input (return key)
    std::cin.get();

    // Stop loop in main
    keep_running = false;

    return 0;
}

// Open a HID device that matches the Vendor-ID and Product-ID
bool OpenHidDevice( HID_DEVICE* Hid_ps, uint16_t vid_u16, uint16_t pid_u16 )
{
    GUID*    hidGuid_p = new GUID();
    HDEVINFO hwDevInfoSet = new HDEVINFO();
    SP_DEVICE_INTERFACE_DATA* deviceInfoData_p = new SP_DEVICE_INTERFACE_DATA();
    SP_DEVICE_INTERFACE_DETAIL_DATA* deviceDetails_p;
    SP_DEVINFO_DATA* devInfoData_p = new SP_DEVINFO_DATA();

    // Get the GUID for HID devices
    HidD_GetHidGuid( hidGuid_p );

    // Setup a list of connected HID devices based on the GUID
    SetupDiDestroyDeviceInfoList( hwDevInfoSet );
    hwDevInfoSet = SetupDiGetClassDevs( hidGuid_p, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
    
    // Loop over HID devices
    deviceInfoData_p->cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    uint32_t idxDev_u32 = 0;
    bool foundDev_l = false;
    while( SetupDiEnumDeviceInterfaces(hwDevInfoSet, NULL, hidGuid_p, idxDev_u32, deviceInfoData_p) )
    {
        idxDev_u32++;

        // Check required buffer length to receive device path
        DWORD RequiredLength;
        devInfoData_p->cbSize = sizeof(SP_DEVINFO_DATA);
        SetupDiGetDeviceInterfaceDetail(hwDevInfoSet, deviceInfoData_p, NULL, 0, &RequiredLength, devInfoData_p );
  
        // Allocate memory for device path
        deviceDetails_p = (SP_DEVICE_INTERFACE_DETAIL_DATA*)calloc( 1, (int)RequiredLength );
        if (deviceDetails_p == NULL) break;

        // Get device path
        deviceDetails_p->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SetupDiGetDeviceInterfaceDetail(hwDevInfoSet, deviceInfoData_p, deviceDetails_p, RequiredLength, &RequiredLength, devInfoData_p);
        wcscpy_s(Hid_ps->DevicePath, _countof(Hid_ps->DevicePath), deviceDetails_p->DevicePath);
        free(deviceDetails_p);

        //  Open HID device
        Hid_ps->HidDevice = CreateFile(Hid_ps->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, NULL);
        if( Hid_ps->HidDevice == INVALID_HANDLE_VALUE )
        {
            // Try next device
            continue;
        }

        // Get HID device attributes
        HidD_GetAttributes(Hid_ps->HidDevice, &Hid_ps->Attributes);

        // Check Vendor ID and Product ID match
        if ((Hid_ps->Attributes.VendorID == vid_u16) &&
            (Hid_ps->Attributes.ProductID == pid_u16))
        {
            // Found device -> stop search
            foundDev_l = true;
            break;
        }
        else
        {
            // No match, close device
            CloseHandle(Hid_ps->HidDevice);
        }
    }

    // Report if device was found
    return foundDev_l;
}

// Main function
int main( void )
{
    int res;
    HID_DEVICE* Hid_ps = new HID_DEVICE();

    // Start thread to check for 'return' keyboard hit
    HANDLE kbhit_h = CreateThread( NULL, 0, userInput_thread, NULL, 0L, NULL );

    printf( "/// riftDriverWin V1.0 ///\n" );

    // Open device (Oculus Rift CV1)
    res = OpenHidDevice( Hid_ps, 0x2833, 0x0031 );
    if( res == 0 )
    {
        printf( "Error: OpenHidDevice() failed. Oculus Rift CV1 connected?\n" );
        goto abort;
    }
    else
    { 
        printf("Found Oculus Rift CV1\n");
    }
    
    // Turn the CV1 screens on
    res = rift_send_enable_components( Hid_ps->HidDevice, 1 );
    if (res == 0) goto cleanup;
    printf( "Screen turned ON\n" );
    
    // Send keep alive
    while( keep_running )
    {
        res = rift_send_keep_alive( Hid_ps->HidDevice );
        if (res == 0) break;
        Sleep( 1000 );
    }
    
    // Turn the CV1 screen off
    res = rift_send_enable_components( Hid_ps->HidDevice, 0 );
    if (res == 0) goto cleanup;
    printf( "Screen turned OFF\n" );

cleanup: // Close down connection to HID device
    CloseHandle(Hid_ps->HidDevice);
    
abort:
    printf( "/// Exit ///\n" );

    return 0;
}

// EOF
