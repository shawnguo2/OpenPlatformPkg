/** @file
  UfsHcDxe driver is used to provide platform-dependent info, mainly UFS host controller
  MMIO base, to upper layer UFS drivers.

  Copyright (c) 2014 - 2015, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _DW_UFS_HOST_CONTROLLER_H_
#define _DW_UFS_HOST_CONTROLLER_H_

#include <Uefi.h>

#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/UfsHostController.h>

#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DmaLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>

//
// Host Capabilities Register Offsets
//
#define UFS_HC_CAP_OFFSET          0x0000  // Controller Capabilities
#define UFS_HC_VER_OFFSET          0x0008  // Version
#define UFS_HC_DDID_OFFSET         0x0010  // Device ID and Device Class
#define UFS_HC_PMID_OFFSET         0x0014  // Product ID and Manufacturer ID
#define UFS_HC_AHIT_OFFSET         0x0018  // Auto-Hibernate Idle Timer
//
// Operation and Runtime Register Offsets
//
#define UFS_HC_IS_OFFSET           0x0020  // Interrupt Status
#define UFS_HC_IE_OFFSET           0x0024  // Interrupt Enable
#define UFS_HC_STATUS_OFFSET       0x0030  // Host Controller Status
#define UFS_HC_ENABLE_OFFSET       0x0034  // Host Controller Enable
#define UFS_HC_UECPA_OFFSET        0x0038  // Host UIC Error Code PHY Adapter Layer
#define UFS_HC_UECDL_OFFSET        0x003c  // Host UIC Error Code Data Link Layer
#define UFS_HC_UECN_OFFSET         0x0040  // Host UIC Error Code Network Layer
#define UFS_HC_UECT_OFFSET         0x0044  // Host UIC Error Code Transport Layer
#define UFS_HC_UECDME_OFFSET       0x0048  // Host UIC Error Code DME
#define UFS_HC_UTRIACR_OFFSET      0x004c  // UTP Transfer Request Interrupt Aggregation Control Register
//
// UTP Transfer Register Offsets
//
#define UFS_HC_UTRLBA_OFFSET       0x0050  // UTP Transfer Request List Base Address
#define UFS_HC_UTRLBAU_OFFSET      0x0054  // UTP Transfer Request List Base Address Upper 32-Bits
#define UFS_HC_UTRLDBR_OFFSET      0x0058  // UTP Transfer Request List Door Bell Register
#define UFS_HC_UTRLCLR_OFFSET      0x005c  // UTP Transfer Request List CLear Register
#define UFS_HC_UTRLRSR_OFFSET      0x0060  // UTP Transfer Request Run-Stop Register
//
// UTP Task Management Register Offsets
//
#define UFS_HC_UTMRLBA_OFFSET      0x0070  // UTP Task Management Request List Base Address
#define UFS_HC_UTMRLBAU_OFFSET     0x0074  // UTP Task Management Request List Base Address Upper 32-Bits
#define UFS_HC_UTMRLDBR_OFFSET     0x0078  // UTP Task Management Request List Door Bell Register
#define UFS_HC_UTMRLCLR_OFFSET     0x007c  // UTP Task Management Request List CLear Register
#define UFS_HC_UTMRLRSR_OFFSET     0x0080  // UTP Task Management Run-Stop Register
//
// UIC Command Register Offsets
//
#define UFS_HC_UIC_CMD_OFFSET      0x0090  // UIC Command Register
#define UFS_HC_UCMD_ARG1_OFFSET    0x0094  // UIC Command Argument 1
#define UFS_HC_UCMD_ARG2_OFFSET    0x0098  // UIC Command Argument 2
#define UFS_HC_UCMD_ARG3_OFFSET    0x009c  // UIC Command Argument 3
//
// UMA Register Offsets
//
#define UFS_HC_UMA_OFFSET          0x00b0  // Reserved for Unified Memory Extension

#define UFS_HC_HCLKDIV_OFFSET      0x00fc

#define UFS_HC_HCE_EN              BIT0
#define UFS_HC_HCS_DP              BIT0
#define UFS_HC_HCS_UCRDY           BIT3
#define UFS_HC_IS_UE               BIT2
#define UFS_HC_IS_ULSS             BIT8
#define UFS_HC_IS_UCCS             BIT10
#define UFS_HC_CAP_64ADDR          BIT24
#define UFS_HC_CAP_NUTMRS          (BIT16 | BIT17 | BIT18)
#define UFS_HC_CAP_NUTRS           (BIT0 | BIT1 | BIT2 | BIT3 | BIT4)
#define UFS_HC_UTMRLRSR            BIT0
#define UFS_HC_UTRLRSR             BIT0

#define UFS_UIC_DME_GET            1
#define UFS_UIC_DME_SET            2

extern EFI_DRIVER_BINDING_PROTOCOL                gUfsHcDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL                gUfsHcComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL               gUfsHcComponentName2;

//
// Unique signature for private data structure.
//
#define UFS_HC_PRIVATE_DATA_SIGNATURE             SIGNATURE_32 ('U','F','S','H')

typedef struct _UFS_HOST_CONTROLLER_PRIVATE_DATA  UFS_HOST_CONTROLLER_PRIVATE_DATA;

//
// Nvme private data structure.
//
struct _UFS_HOST_CONTROLLER_PRIVATE_DATA {
  UINT32                             Signature;
  EFI_HANDLE                         Handle;

  EDKII_UFS_HOST_CONTROLLER_PROTOCOL UfsHc;
  UINTN                              RegBase;
};

#define UFS_HOST_CONTROLLER_PRIVATE_DATA_FROM_UFSHC(a) \
  CR (a, \
      UFS_HOST_CONTROLLER_PRIVATE_DATA, \
      UfsHc, \
      UFS_HC_PRIVATE_DATA_SIGNATURE \
      )

/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a
  Unicode string. If the driver specified by This has a user readable name in
  the language specified by Language, then a pointer to the driver name is
  returned in DriverName, and EFI_SUCCESS is returned. If the driver specified
  by This does not support the language specified by Language,
  then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language. This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified
                                in RFC 4646 or ISO 639-2 language code format.

  @param  DriverName[out]       A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                driver specified by This in the language
                                specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by
                                This and the language specified by Language was
                                returned in DriverName.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER DriverName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
UfsHcComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  );

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by
  ControllerHandle and ChildHandle in the form of a Unicode string. If the
  driver specified by This has a user readable name in the language specified by
  Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently
  managing the controller specified by ControllerHandle and ChildHandle,
  then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  ControllerHandle[in]  The handle of a controller that the driver
                                specified by This is managing.  This handle
                                specifies the controller whose name is to be
                                returned.

  @param  ChildHandle[in]       The handle of the child controller to retrieve
                                the name of.  This is an optional parameter that
                                may be NULL.  It will be NULL for device
                                drivers.  It will also be NULL for a bus drivers
                                that wish to retrieve the name of the bus
                                controller.  It will not be NULL for a bus
                                driver that wishes to retrieve the name of a
                                child controller.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language.  This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified in
                                RFC 4646 or ISO 639-2 language code format.

  @param  ControllerName[out]   A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                controller specified by ControllerHandle and
                                ChildHandle in the language specified by
                                Language from the point of view of the driver
                                specified by This.

  @retval EFI_SUCCESS           The Unicode string for the user readable name in
                                the language specified by Language for the
                                driver specified by This was returned in
                                DriverName.

  @retval EFI_INVALID_PARAMETER ControllerHandle is NULL.

  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
                                EFI_HANDLE.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER ControllerName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This is not currently
                                managing the controller specified by
                                ControllerHandle and ChildHandle.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
UfsHcComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  );

/**
  Tests to see if this driver supports a given controller. If a child device is provided,
  it further tests to see if this driver supports creating a handle for the specified child device.

  This function checks to see if the driver specified by This supports the device specified by
  ControllerHandle. Drivers will typically use the device path attached to
  ControllerHandle and/or the services from the bus I/O abstraction attached to
  ControllerHandle to determine if the driver supports ControllerHandle. This function
  may be called many times during platform initialization. In order to reduce boot times, the tests
  performed by this function must be very small, and take as little time as possible to execute. This
  function must not change the state of any hardware devices, and this function must be aware that the
  device specified by ControllerHandle may already be managed by the same driver or a
  different driver. This function must match its calls to AllocatePages() with FreePages(),
  AllocatePool() with FreePool(), and OpenProtocol() with CloseProtocol().
  Since ControllerHandle may have been previously started by the same driver, if a protocol is
  already in the opened state, then it must not be closed with CloseProtocol(). This is required
  to guarantee the state of ControllerHandle is not modified by this function.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For bus drivers, if this parameter is not NULL, then
                                   the bus driver must determine if the bus controller specified
                                   by ControllerHandle and the child controller specified
                                   by RemainingDevicePath are both supported by this
                                   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
                                   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by the driver
                                   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by a different
                                   driver or an application that requires exclusive access.
                                   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
                                   RemainingDevicePath is not supported by the driver specified by This.
**/
EFI_STATUS
EFIAPI
UfsHcDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

/**
  Starts a device controller or a bus controller.

  The Start() function is designed to be invoked from the EFI boot service ConnectController().
  As a result, much of the error checking on the parameters to Start() has been moved into this
  common boot service. It is legal to call Start() from other locations,
  but the following calling restrictions must be followed or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE.
  2. If RemainingDevicePath is not NULL, then it must be a pointer to a naturally aligned
     EFI_DEVICE_PATH_PROTOCOL.
  3. Prior to calling Start(), the Supported() function for the driver specified by This must
     have been called with the same calling parameters, and Supported() must have returned EFI_SUCCESS.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For a bus driver, if this parameter is NULL, then handles
                                   for all the children of Controller are created by this driver.
                                   If this parameter is not NULL and the first Device Path Node is
                                   not the End of Device Path Node, then only the handle for the
                                   child device specified by the first Device Path Node of
                                   RemainingDevicePath is created by this driver.
                                   If the first Device Path Node of RemainingDevicePath is
                                   the End of Device Path Node, no child handle is created by this
                                   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
UfsHcDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

/**
  Stops a device controller or a bus controller.

  The Stop() function is designed to be invoked from the EFI boot service DisconnectController().
  As a result, much of the error checking on the parameters to Stop() has been moved
  into this common boot service. It is legal to call Stop() from other locations,
  but the following calling restrictions must be followed or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE that was used on a previous call to this
     same driver's Start() function.
  2. The first NumberOfChildren handles of ChildHandleBuffer must all be a valid
     EFI_HANDLE. In addition, all of these handles must have been created in this driver's
     Start() function, and the Start() function must have called OpenProtocol() on
     ControllerHandle with an Attribute of EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
                                support a bus specific I/O protocol for the driver
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
UfsHcDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer
  );

/**
  Get the MMIO base of the UFS host controller.

  @param[in]   This             A pointer to the EFI_UFS_HOST_CONTROLLER_PROTOCOL instance.
  @param[out]  MmioBar          The MMIO base address of UFS host controller.

  @retval EFI_SUCCESS           The operation succeeds.
  @retval others                The operation fails.
**/
EFI_STATUS
EFIAPI
UfsHcGetMmioBar (
  IN     EDKII_UFS_HOST_CONTROLLER_PROTOCOL *This,
     OUT UINTN                              *MmioBar
  );

/**                                                                 
  Provides the UFS controller-specific addresses needed to access system memory.
            
  @param  This                  A pointer to the EFI_UFS_HOST_CONTROLLER_PROTOCOL instance.
  @param  Operation             Indicates if the bus master is going to read or write to system memory.
  @param  HostAddress           The system memory address to map to the UFS controller.
  @param  NumberOfBytes         On input the number of bytes to map. On output the number of bytes
                                that were mapped.                                                 
  @param  DeviceAddress         The resulting map address for the bus master UFS controller to use to
                                access the hosts HostAddress.                                        
  @param  Mapping               A resulting value to pass to Unmap().
                                  
  @retval EFI_SUCCESS           The range was mapped for the returned NumberOfBytes.
  @retval EFI_UNSUPPORTED       The HostAddress cannot be mapped as a common buffer.                                
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_DEVICE_ERROR      The system hardware could not map the requested address.
                                   
**/
EFI_STATUS
EFIAPI
UfsHcMap (
  IN     EDKII_UFS_HOST_CONTROLLER_PROTOCOL   *This,
  IN     EDKII_UFS_HOST_CONTROLLER_OPERATION  Operation,
  IN     VOID                                 *HostAddress,
  IN OUT UINTN                                *NumberOfBytes,
     OUT EFI_PHYSICAL_ADDRESS                 *DeviceAddress,
     OUT VOID                                 **Mapping
  );

/**                                                                 
  Completes the Map() operation and releases any corresponding resources.
            
  @param  This                  A pointer to the EFI_UFS_HOST_CONTROLLER_PROTOCOL instance.                                      
  @param  Mapping               The mapping value returned from Map().
                                  
  @retval EFI_SUCCESS           The range was unmapped.
  @retval EFI_DEVICE_ERROR      The data was not committed to the target system memory.
                                   
**/
EFI_STATUS
EFIAPI
UfsHcUnmap (
  IN  EDKII_UFS_HOST_CONTROLLER_PROTOCOL   *This,
  IN  VOID                                 *Mapping
  );

/**                                                                 
  Allocates pages that are suitable for an EfiUfsHcOperationBusMasterCommonBuffer
  mapping.                                                                       
            
  @param  This                  A pointer to the EFI_UFS_HOST_CONTROLLER_PROTOCOL instance.
  @param  Type                  This parameter is not used and must be ignored.
  @param  MemoryType            The type of memory to allocate, EfiBootServicesData or
                                EfiRuntimeServicesData.                               
  @param  Pages                 The number of pages to allocate.                                
  @param  HostAddress           A pointer to store the base system memory address of the
                                allocated range.                                        
  @param  Attributes            The requested bit mask of attributes for the allocated range.
                                  
  @retval EFI_SUCCESS           The requested memory pages were allocated.
  @retval EFI_UNSUPPORTED       Attributes is unsupported. The only legal attribute bits are
                                MEMORY_WRITE_COMBINE and MEMORY_CACHED.                     
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The memory pages could not be allocated.  
                                   
**/
EFI_STATUS
EFIAPI
UfsHcAllocateBuffer (
  IN     EDKII_UFS_HOST_CONTROLLER_PROTOCOL *This,
  IN     EFI_ALLOCATE_TYPE                  Type,
  IN     EFI_MEMORY_TYPE                    MemoryType,
  IN     UINTN                              Pages,
     OUT VOID                               **HostAddress,
  IN     UINT64                             Attributes
  );

/**                                                                 
  Frees memory that was allocated with AllocateBuffer().
            
  @param  This                  A pointer to the EFI_UFS_HOST_CONTROLLER_PROTOCOL instance.  
  @param  Pages                 The number of pages to free.                                
  @param  HostAddress           The base system memory address of the allocated range.                                    
                                  
  @retval EFI_SUCCESS           The requested memory pages were freed.
  @retval EFI_INVALID_PARAMETER The memory range specified by HostAddress and Pages
                                was not allocated with AllocateBuffer().
                                     
**/
EFI_STATUS
EFIAPI
UfsHcFreeBuffer (
  IN  EDKII_UFS_HOST_CONTROLLER_PROTOCOL    *This,
  IN  UINTN                                 Pages,
  IN  VOID                                  *HostAddress
  );

/**                                                                 
  Flushes all posted write transactions from the UFS bus to attached UFS device.
            
  @param  This                  A pointer to the EFI_UFS_HOST_CONTROLLER_PROTOCOL instance.  
                                  
  @retval EFI_SUCCESS           The posted write transactions were flushed from the UFS bus
                                to attached UFS device.                                      
  @retval EFI_DEVICE_ERROR      The posted write transactions were not flushed from the UFS
                                bus to attached UFS device due to a hardware error.                           
                                     
**/
EFI_STATUS
EFIAPI
UfsHcFlush (
  IN  EDKII_UFS_HOST_CONTROLLER_PROTOCOL   *This
  );

/**                                                                 
  Enable a UFS bus driver to access UFS MMIO registers in the UFS Host Controller memory space.

  @param  This                  A pointer to the EDKII_UFS_HOST_CONTROLLER_PROTOCOL instance.
  @param  Width                 Signifies the width of the memory operations.
  @param  Offset                The offset within the UFS Host Controller MMIO space to start the
                                memory operation.
  @param  Count                 The number of memory operations to perform.
  @param  Buffer                For read operations, the destination buffer to store the results.
                                For write operations, the source buffer to write data from.

  @retval EFI_SUCCESS           The data was read from or written to the UFS host controller.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the UFS Host Controller memory space.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.

**/
EFI_STATUS
EFIAPI
UfsHcMmioRead (
  IN     EDKII_UFS_HOST_CONTROLLER_PROTOCOL        *This,
  IN     EDKII_UFS_HOST_CONTROLLER_PROTOCOL_WIDTH  Width,
  IN     UINT64                                    Offset,
  IN     UINTN                                     Count,
  IN OUT VOID                                      *Buffer
  );

/**                                                                 
  Enable a UFS bus driver to access UFS MMIO registers in the UFS Host Controller memory space.

  @param  This                  A pointer to the EDKII_UFS_HOST_CONTROLLER_PROTOCOL instance.
  @param  Width                 Signifies the width of the memory operations.
  @param  Offset                The offset within the UFS Host Controller MMIO space to start the
                                memory operation.
  @param  Count                 The number of memory operations to perform.
  @param  Buffer                For read operations, the destination buffer to store the results.
                                For write operations, the source buffer to write data from.

  @retval EFI_SUCCESS           The data was read from or written to the UFS host controller.
  @retval EFI_UNSUPPORTED       The address range specified by Offset, Width, and Count is not
                                valid for the UFS Host Controller memory space.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.

**/
EFI_STATUS
EFIAPI
UfsHcMmioWrite (
  IN     EDKII_UFS_HOST_CONTROLLER_PROTOCOL        *This,
  IN     EDKII_UFS_HOST_CONTROLLER_PROTOCOL_WIDTH  Width,
  IN     UINT64                                    Offset,
  IN     UINTN                                     Count,
  IN OUT VOID                                      *Buffer
  );

EFI_STATUS
EFIAPI
UfsHcPhyInit (
  IN     EDKII_UFS_HOST_CONTROLLER_PROTOCOL        *This
  );
#endif /* _DW_UFS_HOST_CONTROLLER_H_ */
