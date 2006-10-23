/*
 * Copyright (c) 2002-2006 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	Includes
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

// Libkern includes
#include <libkern/c++/OSData.h>
#include <libkern/c++/OSDictionary.h>

// General IOKit includes
#include <IOKit/IOBufferMemoryDescriptor.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IODeviceTreeSupport.h>

// IOKit storage includes
#include <IOKit/storage/IOStorageDeviceCharacteristics.h>
#include <IOKit/storage/IOStorageProtocolCharacteristics.h>

// SCSI Architecture Model Family includes
#include <IOKit/scsi/SCSICommandOperationCodes.h>
#include <IOKit/scsi/SCSICmds_INQUIRY_Definitions.h>

// SCSI Parallel Family includes
#include "IOSCSIParallelInterfaceDevice.h"


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	Macros
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

#define DEBUG 												0
#define DEBUG_ASSERT_COMPONENT_NAME_STRING					"SPI Device"

#if DEBUG
#define SCSI_PARALLEL_DEVICE_DEBUGGING_LEVEL				0
#endif

#include "IOSCSIParallelFamilyDebugging.h"

#if ( SCSI_PARALLEL_DEVICE_DEBUGGING_LEVEL >= 1 )
#define PANIC_NOW(x)           IOPanic x
#else
#define PANIC_NOW(x)
#endif

#if ( SCSI_PARALLEL_DEVICE_DEBUGGING_LEVEL >= 2 )
#define ERROR_LOG(x)           IOLog x
#else
#define ERROR_LOG(x)
#endif

#if ( SCSI_PARALLEL_DEVICE_DEBUGGING_LEVEL >= 3 )
#define STATUS_LOG(x)          IOLog x
#else
#define STATUS_LOG(x)
#endif


#define super IOSCSIProtocolServices
OSDefineMetaClassAndStructors ( IOSCSIParallelInterfaceDevice, IOSCSIProtocolServices );


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	Constants
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

#define kIOPropertyIOUnitKey		"IOUnit"
#define kIODeviceLocationKey		"io-device-location"

enum
{
	kWorldWideNameDataSize 		= 8,
	kAddressIdentifierDataSize 	= 3,
	kALPADataSize				= 1,
	kSASAddressDataSize			= 8
};


#if 0
#pragma mark -
#pragma mark ₯ IOKit Member Routines
#pragma mark -
#endif


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SetInitialTargetProperties									   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::SetInitialTargetProperties (
										OSDictionary * properties )
{
	
	OSDictionary *	protocolDict	= NULL;
	OSObject *		value			= NULL;
	bool			result			= false;
	
	protocolDict = OSDictionary::withCapacity ( properties->getCount ( ) );
	require_nonzero ( protocolDict, INIT_FAILURE );
	
	setProperty ( kIOPropertyProtocolCharacteristicsKey, protocolDict );
	protocolDict->release ( );
	protocolDict = NULL;
	
	// Set the properties from the dictionary
	value = properties->getObject ( kIOPropertyFibreChannelNodeWorldWideNameKey );
	SetTargetProperty ( kIOPropertyFibreChannelNodeWorldWideNameKey, value );
	
	value = properties->getObject ( kIOPropertyFibreChannelPortWorldWideNameKey );
	SetTargetProperty ( kIOPropertyFibreChannelPortWorldWideNameKey, value );
	
	value = properties->getObject ( kIOPropertyFibreChannelAddressIdentifierKey );
	SetTargetProperty ( kIOPropertyFibreChannelAddressIdentifierKey, value );
	
	value = properties->getObject ( kIOPropertyFibreChannelALPAKey );
	SetTargetProperty ( kIOPropertyFibreChannelALPAKey, value );
	
	value = properties->getObject ( kIOPropertySASAddressKey );
	SetTargetProperty ( kIOPropertySASAddressKey, value );

	result = true;
	
	
INIT_FAILURE:
	
	
	return result;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ start															   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::start ( IOService * provider )
{
	
	OSDictionary *	protocolDict	= NULL;
	OSDictionary *	copyDict		= NULL;
	bool			result			= false;
	char			unit[10];
	
	// Save access to the controller object so that Tasks can be sent
	// for execution.
	fController = OSDynamicCast ( IOSCSIParallelInterfaceController, provider );
	require_nonzero ( fController, PROVIDER_CAST_FAILURE );
	
	// Execute the inherited start
	result = super::start ( provider );
	require ( result, PROVIDER_START_FAILURE );
	
	// Open the controller, the provider.
	result = fController->open ( this );
	require ( result, CONTROLLER_OPEN_FAILURE );
	
	result = fController->InitializeTargetForID ( fTargetIdentifier );
	require ( result, CONTROLLER_INIT_FAILURE );
	
	// Setup power management for this object.
	InitializePowerManagement ( provider );
	
	copyDict = OSDynamicCast ( OSDictionary, copyProperty ( kIOPropertyProtocolCharacteristicsKey ) );
	if ( copyDict != NULL )
	{
		
		protocolDict = ( OSDictionary * ) copyDict->copyCollection ( );
		copyDict->release ( );
		
	}
	
	if ( protocolDict != NULL )
	{
		
		OSNumber *	targetID = NULL;
		
		// Create an OSNumber object with the SCSI Target Identifier
		targetID = OSNumber::withNumber ( fTargetIdentifier, 64 );
		if ( targetID != NULL )
		{
			
			protocolDict->setObject ( kIOPropertySCSITargetIdentifierKey, targetID );
			
			// Set the Unit number used to build the device tree path
			setProperty ( kIOPropertyIOUnitKey, targetID );
			
			targetID->release ( );
			
		}
		
		setProperty ( kIOPropertyProtocolCharacteristicsKey, protocolDict );
		protocolDict->release ( );
		protocolDict = NULL;
		
	}
	
	// Set the location to allow booting 
    sprintf ( unit, "%x", ( int ) fTargetIdentifier );
    setLocation ( unit );
	
	// The device and this driver have been succesfully configured
	// and are ready to provide their services, call CreateSCSITargetDevice().
	CreateSCSITargetDevice ( );
	
	return true;
	
	
CONTROLLER_INIT_FAILURE:
CONTROLLER_OPEN_FAILURE:
PROVIDER_START_FAILURE:
PROVIDER_CAST_FAILURE:
	
	
	return false;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ stop															   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::stop ( IOService * provider )
{
	super::stop ( provider );
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ finalize													       [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::finalize ( IOOptionBits options )
{
	
	if ( fController != NULL )
	{
		
		fController->close ( this );
		fController = NULL;
		
	}
	
	return super::finalize ( options );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ free															   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::free ( void )
{
	
	// Release the HBA specific data
	if ( fHBAData != NULL )
	{
		
		IOFree ( fHBAData, fHBADataSize );
		fHBAData		= NULL;
		fHBADataSize	= 0;
		
	}
	
	// Release the lock for the Task Queue.
	if ( fQueueLock != NULL )
	{
		
		// Free the SCSI Task queue access lock.
		IOSimpleLockFree ( fQueueLock );
		fQueueLock = NULL;
		
	}
	
	// Release the lock for the Task Queue.
	if ( fResendQueueLock != NULL )
	{
		
		// Free the SCSI Task queue access lock.
		IOSimpleLockFree ( fResendQueueLock );
		fResendQueueLock = NULL;
		
	}
	
	super::free ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ message														   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

IOReturn
IOSCSIParallelInterfaceDevice::message ( 
							UInt32 						type, 
							IOService * 				provider, 
							void * 						argument )
{
	
	IOReturn	result = kIOReturnSuccess;
	
	switch ( type )
	{
		
		case kSCSIControllerNotificationBusReset:
		{
			
			// Bus reset occurred, disavow all negotiation settings
			// and force renegotiation
			for ( int index = 0; index < kSCSIParallelFeature_TotalFeatureCount; index++ )
			{
				
				// Set each one to false.
				fFeatureIsNegotiated[index] = false;
				
			}
			
			// Message the SAM drivers to verify their device's state
			SendNotification_VerifyDeviceState ( );
			
		}
		break;
		
		case kSCSIPort_NotificationStatusChange:
		{
			
			// Port status is changing, let target device object know
			// about it.
			messageClients ( kSCSIPort_NotificationStatusChange, argument );
			
		}
		
		default:
		{
			result = super::message ( type, provider, argument );
		}
		break;
		
	}
	
	return result;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ requestProbe													   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

IOReturn
IOSCSIParallelInterfaceDevice::requestProbe ( IOOptionBits options )
{
	
	// See if this device already has any opens on it.
	if ( isOpen ( ) == false )
	{
		
		// The device and this driver have been succesfully configured
		// and are ready to provide their services, call CreateSCSITargetDevice().
		CreateSCSITargetDevice ( );
		
		return kIOReturnSuccess;
		
	}
	
	else
	{
		return kIOReturnNotPermitted;
	}
	
}


#if 0
#pragma mark -
#pragma mark ₯ Device Object Management Member routines
#pragma mark -
#endif


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ CreateTarget	- 	Creates an IOSCSIParallelInterfaceDevice for the
//						specified target ID.				   [STATIC][PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

IOSCSIParallelInterfaceDevice *
IOSCSIParallelInterfaceDevice::CreateTarget (
							SCSITargetIdentifier 		targetID, 
							UInt32 						sizeOfHBAData,
							IORegistryEntry *			entry )
{
	
	IOSCSIParallelInterfaceDevice * newDevice	= NULL;
	OSObject *						value		= NULL;
	bool							result		= false;
	
	newDevice = OSTypeAlloc ( IOSCSIParallelInterfaceDevice );
	require_nonzero ( newDevice, DEVICE_CREATION_FAILURE );
	
	if ( entry != NULL )
	{
		
		result = newDevice->init ( 0 );
		require ( result, RELEASE_DEVICE );
		
		newDevice->lockForArbitration ( );
		result = newDevice->attachToParent ( entry, gIODTPlane );
		newDevice->unlockForArbitration ( );
		
		value = entry->copyProperty ( kIODeviceLocationKey );
		if ( value != NULL )
		{
			newDevice->setProperty ( kIODeviceLocationKey, value );
		}
		
	}
	
	else
	{
		result = newDevice->init ( 0 );
	}
	
	require ( result, RELEASE_DEVICE );
	
	// Set all of the fields to their defaults
	newDevice->fHBAData					= NULL;
	newDevice->fHBADataSize				= sizeOfHBAData;
	newDevice->fTargetIdentifier		= targetID;
	newDevice->fPreviousParallelDevice	= NULL;
	newDevice->fNextParallelDevice 		= NULL;
	newDevice->fOutstandingTaskList		= NULL;
	newDevice->fQueueLock				= NULL;
	newDevice->fResendTaskList			= NULL;
	newDevice->fResendQueueLock			= NULL;
	
	bzero ( newDevice->fFeatureIsNegotiated,
			kSCSIParallelFeature_TotalFeatureCount * sizeof ( bool ) );
	
	bzero ( newDevice->fITNexusSupportsFeature,
			kSCSIParallelFeature_TotalFeatureCount * sizeof ( bool ) );
	
	// Allocate the lock for the Task Queue
	newDevice->fQueueLock = IOSimpleLockAlloc ( );
	require_nonzero ( newDevice->fQueueLock, LOCK_ALLOC_FAILURE );
	
	newDevice->fResendQueueLock = IOSimpleLockAlloc ( );
	require_nonzero ( newDevice->fResendQueueLock, LOCK_ALLOC_FAILURE );
	
	newDevice->fAllowResends = true;
	
	if ( sizeOfHBAData != 0 )
	{
		
		// Allocate the HBA specific data for the device object
		newDevice->fHBAData = IOMalloc ( sizeOfHBAData );
		require_nonzero ( newDevice->fHBAData, HBA_DATA_ALLOC_FAILURE );		
		bzero ( newDevice->fHBAData, sizeOfHBAData );
		
	}
	
	
	return newDevice;
	
	
HBA_DATA_ALLOC_FAILURE:
	
	
	// Release the lock for the Task Queue.
	if ( newDevice->fQueueLock != NULL )
	{
		
		// Free the SCSI Task queue access lock.
		IOSimpleLockFree ( newDevice->fQueueLock );
		newDevice->fQueueLock = NULL;
		
	}
	
	// Release the lock for the Task Queue.
	if ( newDevice->fResendQueueLock != NULL )
	{
		
		// Free the SCSI Task queue access lock.
		IOSimpleLockFree ( newDevice->fResendQueueLock );
		newDevice->fResendQueueLock = NULL;
		
	}
	
	
LOCK_ALLOC_FAILURE:
RELEASE_DEVICE:
	
	
	require_nonzero_quiet ( newDevice, DEVICE_CREATION_FAILURE );
	newDevice->release ( );
	newDevice = NULL;
	
	
DEVICE_CREATION_FAILURE:
	
	
	return NULL;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ DestroyTarget	- 	Destroys an IOSCSIParallelInterfaceDevice.	   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void 
IOSCSIParallelInterfaceDevice::DestroyTarget ( void )
{
	
	IORegistryEntry *		parent = NULL;
	
	SendNotification_DeviceRemoved ( );
	
	// Get rid of the io-device-location property first.
	removeProperty ( kIODeviceLocationKey );
	
	// Remove this entry from the IODeviceTree plane.
	lockForArbitration ( );
	
	parent = getParentEntry ( gIODTPlane );
	
	if ( parent != NULL )
	{
		detachFromParent ( parent, gIODTPlane );
	}
	
	unlockForArbitration ( );	
	
	// Remove anything from the "resend queue".
	IOSimpleLockLock ( fResendQueueLock );
	
	fResendTaskList = NULL;
	fAllowResends = false;
	
	IOSimpleLockUnlock ( fResendQueueLock );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetPreviousDeviceInList - Retrieves previous device in linked list.
//																	   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

IOSCSIParallelInterfaceDevice *
IOSCSIParallelInterfaceDevice::GetPreviousDeviceInList ( void )
{
	return fPreviousParallelDevice;
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SetPreviousDeviceInList - Sets previous device in linked list.   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::SetPreviousDeviceInList (
							IOSCSIParallelInterfaceDevice * newPrev )
{
	fPreviousParallelDevice = newPrev;
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetNextDeviceInList - Retrieves next device in linked list.	   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

IOSCSIParallelInterfaceDevice *
IOSCSIParallelInterfaceDevice::GetNextDeviceInList ( void )
{
	return fNextParallelDevice;
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SetNextDeviceInList - Sets next device in linked list.		   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::SetNextDeviceInList ( 
							IOSCSIParallelInterfaceDevice * 	newNext )
{
	fNextParallelDevice = newNext;
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ DetermineParallelFeatures - 	Determines parallel protocol features based
//									on INQUIRY data.				  [PRIVATE]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::DetermineParallelFeatures ( UInt8 * inqData )
{
	
	OSDictionary *	dict			= NULL;
	OSDictionary *	copyDict		= NULL;
	OSNumber *		features		= NULL;
	UInt64			deviceFeatures	= 0;
	UInt64			ITNexusFeatures	= 0;
	bool			supported		= false;
	UInt8			inqSCSIVersion	= 0;
	UInt8			inqDataLength	= 0;
	
	inqSCSIVersion = ( ( SCSICmd_INQUIRY_StandardData * ) inqData )->VERSION & kINQUIRY_ANSI_VERSION_Mask;
	inqDataLength = ( ( SCSICmd_INQUIRY_StandardData * ) inqData )->ADDITIONAL_LENGTH + 5;
	
	// Verify that the device is SCSI-2 compliant and the INQUIRY data is large 
	// enough to contain the SCSI-2 feature flags
	if ( ( inqSCSIVersion >= kINQUIRY_ANSI_VERSION_SCSI_2_Compliant ) &&
		 ( inqDataLength > kINQUIRY_Byte7_Offset ) )
	{
		
		if ( inqData[kINQUIRY_Byte7_Offset] & kINQUIRY_Byte7_SYNC_Mask )
		{
			
			deviceFeatures |= (1 << kSCSIParallelFeature_SynchronousDataTransfer);
			supported = DoesHBASupportSCSIParallelFeature ( kSCSIParallelFeature_SynchronousDataTransfer );
			if ( supported == true )
			{
				
				fITNexusSupportsFeature[kSCSIParallelFeature_SynchronousDataTransfer] = true;
				ITNexusFeatures |= (1 << kSCSIParallelFeature_SynchronousDataTransfer);
				
			}
			
		}
		
		if ( inqData[kINQUIRY_Byte7_Offset] & kINQUIRY_Byte7_WBUS16_Mask )
		{
			
			deviceFeatures |= (1 << kSCSIParallelFeature_WideDataTransfer);
			supported = DoesHBASupportSCSIParallelFeature ( kSCSIParallelFeature_WideDataTransfer );
			if ( supported == true )
			{
				
				fITNexusSupportsFeature[kSCSIParallelFeature_WideDataTransfer] = true;
				ITNexusFeatures |= (1 << kSCSIParallelFeature_WideDataTransfer);
				
			}
			
		}
		
	}
	
	// Verify that the device is SPC compliant and the INQUIRY data is large 
	// enough to contain the SPI-3 feature flags
	if ( ( inqSCSIVersion >= kINQUIRY_ANSI_VERSION_SCSI_SPC_Compliant ) &&
		 ( inqDataLength > kINQUIRY_Byte56_Offset ) )
	{
		
		if ( inqData[kINQUIRY_Byte56_Offset] & kINQUIRY_Byte56_IUS_Mask )
		{
			
			deviceFeatures |= (1 << kSCSIParallelFeature_InformationUnitTransfers);
			supported = DoesHBASupportSCSIParallelFeature ( kSCSIParallelFeature_InformationUnitTransfers );
			if ( supported == true )
			{
				
				fITNexusSupportsFeature[kSCSIParallelFeature_InformationUnitTransfers] = true;
				ITNexusFeatures |= (1 << kSCSIParallelFeature_InformationUnitTransfers);
				
			}
			
		}
		
		if ( inqData[kINQUIRY_Byte56_Offset] & kINQUIRY_Byte56_QAS_Mask )
		{
			
			deviceFeatures |= (1 << kSCSIParallelFeature_QuickArbitrationAndSelection);
			supported = DoesHBASupportSCSIParallelFeature( kSCSIParallelFeature_QuickArbitrationAndSelection );
			if ( supported == true )
			{
				
				fITNexusSupportsFeature[kSCSIParallelFeature_QuickArbitrationAndSelection] = true;
				ITNexusFeatures |= (1 << kSCSIParallelFeature_QuickArbitrationAndSelection);
				
			}
			
		}
		
		if ( ( ( inqData[kINQUIRY_Byte56_Offset] & kINQUIRY_Byte56_CLOCKING_Mask ) == kINQUIRY_Byte56_CLOCKING_ONLY_DT ) ||
			 ( ( inqData[kINQUIRY_Byte56_Offset] & kINQUIRY_Byte56_CLOCKING_Mask ) == kINQUIRY_Byte56_CLOCKING_ST_AND_DT ) )
		{
			
			deviceFeatures |= (1 << kSCSIParallelFeature_DoubleTransitionDataTransfers);
			supported = DoesHBASupportSCSIParallelFeature ( kSCSIParallelFeature_DoubleTransitionDataTransfers );
			if ( supported == true )
			{
				
				fITNexusSupportsFeature[kSCSIParallelFeature_DoubleTransitionDataTransfers] = true;
				ITNexusFeatures |= (1 << kSCSIParallelFeature_DoubleTransitionDataTransfers);
				
			}
			
		}
		
	}
	
	copyDict = ( OSDictionary * ) copyProperty ( kIOPropertyProtocolCharacteristicsKey );
	if ( copyDict != NULL )
	{
		
		dict = ( OSDictionary * ) copyDict->copyCollection ( );
		copyDict->release ( );
		
	}
	
	if ( dict != NULL )
	{
		
		features = OSNumber::withNumber ( deviceFeatures, 64 );
		if ( features != NULL )
		{
			
			dict->setObject ( kIOPropertySCSIDeviceFeaturesKey, features );
			features->release ( );
			features = NULL;
			
		}
		
		features = OSNumber::withNumber ( ITNexusFeatures, 64 );
		if ( features != NULL )
		{
			
			dict->setObject ( kIOPropertySCSI_I_T_NexusFeaturesKey, features );
			features->release ( );
			features = NULL;
			
		}
		
		setProperty ( kIOPropertyProtocolCharacteristicsKey, dict );
		dict->release ( );
		dict = NULL;
		
	}
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetTargetIdentifier - Retrieves the SCSITargetIdentifier for this device.
//																	   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSITargetIdentifier
IOSCSIParallelInterfaceDevice::GetTargetIdentifier ( void )
{
	return fTargetIdentifier;
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetHBADataPointer - Retrieves the pointer to the HBA Data for this device.
//																	   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void *
IOSCSIParallelInterfaceDevice::GetHBADataPointer ( void )
{
	return fHBAData;
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetHBADataSize - Retrieves the HBA Data size for this device.    [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt32
IOSCSIParallelInterfaceDevice::GetHBADataSize ( void )
{
	return fHBADataSize;
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ IsFeatureNegotiationNecessary - 	Checks if a feature negotiation is
//										necessary.					   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::IsFeatureNegotiationNecessary (
							SCSIParallelFeature			feature )
{
	// Verify that the requested feature is one that is known to
	// the device object.
	if ( feature >= kSCSIParallelFeature_TotalFeatureCount )
	{
		return false;
	}
	
	return ( fITNexusSupportsFeature[feature] &&
			 ( fFeatureIsNegotiated[feature] == false ) );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ FindTaskForAddress - 	Find the outstanding task for the Task Address of
//							this Target and the specified Lun and Tag.
//																	   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSIParallelTaskIdentifier
IOSCSIParallelInterfaceDevice::FindTaskForAddress (
							SCSILogicalUnitNumber		theL,
							SCSITaggedTaskIdentifier	theQ )
{
	
	SCSIParallelTask *	tempTask;
	
	IOSimpleLockLock ( fQueueLock );
	
	// Make sure that the queue lock is being held before setting
	// tempTask to the head element.
	tempTask = fOutstandingTaskList;
	
	while ( tempTask != NULL )
	{
		
		if ( ( GetLogicalUnitNumber ( tempTask ) == theL ) &&
			 ( GetTaggedTaskIdentifier ( tempTask ) == theQ ) )
		{
			break;
		}
		
		tempTask = tempTask->GetNextTaskInList ( );
		
	}
	
	IOSimpleLockUnlock ( fQueueLock );
	
	return tempTask;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ FindTaskForControllerIdentifier - Find the outstanding task for the
//										identifier. 				   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSIParallelTaskIdentifier	
IOSCSIParallelInterfaceDevice::FindTaskForControllerIdentifier (
							UInt64						theIdentifier )
{
	
	SCSIParallelTask *	tempTask;
	
	IOSimpleLockLock ( fQueueLock );
	
	// Make sure that the queue lock is being held before setting
	// tempTask to the head element.
	tempTask = fOutstandingTaskList;
	
	while ( tempTask != NULL )
	{
		
		// Check if the request is to return the first element on the queue
		if ( theIdentifier == kSCSIParallelTaskControllerIDQueueHead )
		{
			
			// The request is for the first element on the queue, this will
			// break the first time through the while loop.
			break;
			
		}
		
		if ( GetControllerTaskIdentifier ( tempTask ) == theIdentifier )
		{
			break;
		}
		
		tempTask = tempTask->GetNextTaskInList ( );
		
	}
	
	IOSimpleLockUnlock ( fQueueLock );
	
	return tempTask;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SetTargetProperty - Sets a target property. 					   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool	
IOSCSIParallelInterfaceDevice::SetTargetProperty ( 
									const char * 		key,
									OSObject *			value )
{
	
	bool			result			= false;
	OSDictionary *	protocolDict	= NULL;
	OSDictionary *	copyDict		= NULL;
	
	require_nonzero ( key, ErrorExit );
	require_nonzero ( value, ErrorExit );
	
	copyDict = OSDynamicCast ( OSDictionary, copyProperty ( kIOPropertyProtocolCharacteristicsKey ) );
	require_nonzero ( copyDict, ErrorExit );
	
	protocolDict = ( OSDictionary * ) copyDict->copyCollection ( );
	copyDict->release ( );
	
	require_nonzero ( protocolDict, ErrorExit );
	
	if ( strcmp ( key, kIOPropertyFibreChannelPortWorldWideNameKey ) == 0 )
	{
		
		OSData * data = OSDynamicCast ( OSData, value );
		
		require_nonzero ( data, ErrorExit );
		require ( ( data->getLength ( ) == kWorldWideNameDataSize ), ErrorExit );
		result = protocolDict->setObject ( key, value );
		
	}
	
	else if ( strcmp ( key, kIOPropertyFibreChannelNodeWorldWideNameKey ) == 0 )
	{
		
		OSData *	data		= OSDynamicCast ( OSData, value );
		char		name[27]	= { 0 };
		
		require_nonzero ( data, ErrorExit );
		require ( ( data->getLength ( ) == kWorldWideNameDataSize ), ErrorExit );
		result = protocolDict->setObject ( key, value );
		
		snprintf ( name, sizeof ( name ), "FC Target %016qX", OSSwapHostToBigInt64 ( *( UInt64 * ) data->getBytesNoCopy ( ) ) );
		setName ( name, gIOServicePlane );
		
	}
	
	else if ( strcmp ( key, kIOPropertyFibreChannelAddressIdentifierKey ) == 0 )
	{
		
		OSData * data = OSDynamicCast ( OSData, value );
		
		require_nonzero ( data, ErrorExit );
		require ( ( data->getLength ( ) == kAddressIdentifierDataSize ), ErrorExit );
		result = protocolDict->setObject ( key, value );
		
	}
	
	else if ( strcmp ( key, kIOPropertyFibreChannelALPAKey ) == 0 )
	{
		
		OSData * data = OSDynamicCast ( OSData, value );
		
		require_nonzero ( data, ErrorExit );
		require ( ( data->getLength ( ) == kALPADataSize ), ErrorExit );
		result = protocolDict->setObject ( key, value );
		
	}

	else if ( strcmp ( key, kIOPropertySASAddressKey ) == 0 )
	{
		
		OSData *	data		= OSDynamicCast ( OSData, value );
		char		name[28]	= { 0 };
		
		require_nonzero ( data, ErrorExit );
		require ( ( data->getLength ( ) == kSASAddressDataSize ), ErrorExit );
		result = protocolDict->setObject ( key, value );
		
		snprintf ( name, sizeof ( name ), "SAS Target %016qX", OSSwapHostToBigInt64 ( *( UInt64 * ) data->getBytesNoCopy ( ) ) );
		setName ( name, gIOServicePlane );
		
	}
	
	setProperty ( kIOPropertyProtocolCharacteristicsKey, protocolDict );
	protocolDict->release ( );
	protocolDict = NULL;
	
	
ErrorExit:
	
	
	return result;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ RemoveTargetProperty - Removes a property for this object. 	   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void	
IOSCSIParallelInterfaceDevice::RemoveTargetProperty ( const char * key )
{
	
	OSDictionary *	protocolDict	= NULL;
	OSDictionary *	copyDict		= NULL;
	
	require_nonzero ( key, ErrorExit );
	
	copyDict = OSDynamicCast ( OSDictionary, copyProperty ( kIOPropertyProtocolCharacteristicsKey ) );
	require_nonzero ( copyDict, ErrorExit );
	
	protocolDict = ( OSDictionary * ) copyDict->copyCollection ( );
	copyDict->release ( );
	
	require_nonzero ( protocolDict, ErrorExit );
	
	if ( protocolDict->getObject ( key ) != NULL )
	{
		
		protocolDict->removeObject ( key );
		
	}
	
	setProperty ( kIOPropertyProtocolCharacteristicsKey, protocolDict );
	protocolDict->release ( );
	protocolDict = NULL;
	
	
ErrorExit:
	
	
	return;
	
}


#if 0
#pragma mark -
#pragma mark ₯ SCSI Protocol Services Member Routines
#pragma mark -
#endif


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SendSCSICommand - Sends a command to the controller.			   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::SendSCSICommand (
							SCSITaskIdentifier			request,
							SCSIServiceResponse * 		serviceResponse,
							SCSITaskStatus *			taskStatus )
{
	
	SCSIParallelTaskIdentifier		parallelTask	= NULL;
	IOMemoryDescriptor *			buffer			= NULL;
	IOReturn						status			= kIOReturnBadArgument;
	IOWorkLoop *					workLoop		= NULL;
	bool							block			= true;
	
	// Set the defaults to an error state.		
	*taskStatus			= kSCSITaskStatus_No_Status;
	*serviceResponse	= kSCSIServiceResponse_SERVICE_DELIVERY_OR_TARGET_FAILURE;
	
	if ( isInactive ( ) == true )
	{
		
		return false;
		
	}
	
	// Check if there is an SCSIParallelTask available to allow the request
	// to be sent to the device. If we don't block on the client thread, we
	// risk the chance of never being able to send an I/O to the controller for
	// this device.
	//
	// But, we can't block the ISR either. Depending on what thread we're on,
	// we have to make the right decision here.
	workLoop = getWorkLoop ( );
	if ( workLoop != NULL )
	{
		
		if ( workLoop->onThread ( ) )
		{
			block = false;
		}
		
	}
	
	parallelTask = GetSCSIParallelTask ( block );
	if ( parallelTask == NULL )
	{
		
		// A SCSI Parallel Task could not be obtained, report
		// that the task was not executed and wait for a task to complete.
		return false;
		
	}
	
	SetTargetIdentifier ( parallelTask, fTargetIdentifier );
	SetDevice ( parallelTask, this );
	SetSCSITaskIdentifier ( parallelTask, request );
	
	// Set the Parallel SCSI transfer features.	
	for ( UInt32 index = 0; index < kSCSIParallelFeature_TotalFeatureCount; index++ )
	{
		
		// Set each one to false.
		if ( IsFeatureNegotiationNecessary ( ( SCSIParallelFeature ) index ) == true )
		{
			
			SetSCSIParallelFeatureNegotiation (
								parallelTask, 
								( SCSIParallelFeature ) index, 
								kSCSIParallelFeature_AttemptNegotiation );
			
		}
		
	}
	
	// Add the task to the outstanding task list.
	AddToOutstandingTaskList ( parallelTask );
	
	// Set the buffer for IODMACommand.
	buffer = GetDataBuffer ( parallelTask );
	if ( buffer != NULL )
	{
		
		status = SetDMABuffer ( parallelTask, buffer );
		if ( status != kIOReturnSuccess )
		{
			
			ERROR_LOG ( ( "SetDMABuffer failed, status = 0x%08x\n", status ) );
			
			RemoveFromOutstandingTaskList ( parallelTask );
			
			// Release the SCSI Parallel Task object
			FreeSCSIParallelTask ( parallelTask );
			
			CommandCompleted ( request, *serviceResponse, *taskStatus );
			
			return true;
			
		}
		
	}
	
	*serviceResponse = ExecuteParallelTask ( parallelTask );
	if ( *serviceResponse != kSCSIServiceResponse_Request_In_Process )
	{
		
		// The task has already completed
		RemoveFromOutstandingTaskList ( parallelTask );
		
		// Release the SCSI Parallel Task object
		FreeSCSIParallelTask ( parallelTask );
		
		CommandCompleted ( request, *serviceResponse, *taskStatus );
		
	}
	
	return true;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ CompleteSCSITask - Completes a command from the controller.	   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::CompleteSCSITask ( 	
							SCSIParallelTaskIdentifier 	completedTask,
							SCSIServiceResponse 		serviceResponse,
							SCSITaskStatus 				completionStatus )
{
	
	SCSITaskIdentifier	clientRequest	= NULL;
	
	if ( completedTask == NULL )
	{
		
		// The driver was asked to complete an invalid task,
		// there is nothing it can do so just return.
		return;
		
	}
	
	// Check if the device rejected the task because its queue is full
	if ( ( serviceResponse == kSCSIServiceResponse_TASK_COMPLETE ) &&
		 ( completionStatus == kSCSITaskStatus_TASK_SET_FULL ) &&
		 ( fAllowResends == true ) )
	{
		
		// The task was not executed because the device reported
		// a TASK_SET_FULL, place it on the resend queue and wait for 
		// a task to complete with a status other than TASK_SET_FULL.
		AddToResendTaskList ( completedTask );
		
		// Done for now.
		return;
		
	} 
		
	// Make sure that the task is removed from the outstanding task list
	// so that the driver no longer sees this task as outstanding.
	RemoveFromOutstandingTaskList ( completedTask );
			
	// Retrieve the original SCSI Task
	clientRequest = GetSCSITaskIdentifier ( completedTask );
	if ( clientRequest == NULL )
	{
		IOPanic ( "clientRequest is NULL in CompleteSCSITask\n" );
	}
	
	// Set the appropriate fields in the SCSI Task
	IOSCSIProtocolServices::SetRealizedDataTransferCount ( clientRequest, GetRealizedDataTransferCount ( completedTask ) );
	
	// Store any negotiations that were done.
	for ( UInt32 index = 0; index < kSCSIParallelFeature_TotalFeatureCount; index++ )
	{
		
		// Set each one to false.
		if ( IsFeatureNegotiationNecessary ( ( SCSIParallelFeature ) index ) == true )
		{
			
			if ( GetSCSIParallelFeatureNegotiationResult ( completedTask, ( SCSIParallelFeature ) index ) ==
				kSCSIParallelFeature_NegotitiationSuccess )
			{
				fFeatureIsNegotiated[index] = true;
			}
			
		}
		
	}
	
	// Release the SCSI Parallel Task object
	FreeSCSIParallelTask ( completedTask );
	
	// If there are requests on the resend queue, send them first
	// Currently only the element at the head of the queue will be sent
	// If the desire is to allow all elements to be sent, the break
	// statement can be removed
	while ( fResendTaskList != NULL )
	{
		
		SCSIParallelTaskIdentifier 	parallelTask;
		
		parallelTask = fResendTaskList;
		
		RemoveFromResendTaskList ( parallelTask);
		
		if ( ExecuteParallelTask ( parallelTask ) != kSCSIServiceResponse_Request_In_Process )
		{
			
			SCSITaskIdentifier		nextRequest	= NULL;
			
			// The task has already completed
			RemoveFromOutstandingTaskList ( parallelTask );
			
			nextRequest = GetSCSITaskIdentifier ( completedTask );
			
			// Release the SCSI Parallel Task object
			FreeSCSIParallelTask ( parallelTask );
			
			CommandCompleted ( nextRequest, kSCSIServiceResponse_SERVICE_DELIVERY_OR_TARGET_FAILURE, kSCSITaskStatus_No_Status );
			
			// Send this command has already completed, start the next
			// one on the queue.
			continue;
			
		}
		
		// A command was successfully sent, wait for it to complete
		// before sending the next one.
		break;
		
	}
	
	// Inform the client that the task has been executed.
	CommandCompleted ( clientRequest, serviceResponse, completionStatus );
	
}


#if 0
#pragma mark -
#pragma mark ₯ SCSI Protocol Service Feature routines
#pragma mark -
#endif


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ IsProtocolServiceSupported - 	Called by SCSI Application Layer to determine
//									if the protocol layer driver supports a
//									SCSIProtocolFeature.	   		   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::IsProtocolServiceSupported (
							SCSIProtocolFeature 		feature,
							void * 						value )
{
	
	bool	isSupported = false;
	
	switch ( feature )
	{
		
		case kSCSIProtocolFeature_GetMaximumLogicalUnitNumber:
		{
			
			isSupported = true;
			*( UInt32 * ) value = ReportHBAHighestLogicalUnitNumber ( );
			
		}
		break;
		
		case kSCSIProtocolFeature_SubmitDefaultInquiryData:
		{
			isSupported = true;
		}
		break;
		
		case kSCSIProtocolFeature_ProtocolAlwaysReportsAutosenseData:
		{
			isSupported = fController->DoesHBAPerformAutoSense ( );
		}
		break;
		
		default:
		{
			// Since isSupported is set to false by default, there is
			// nothing that needs to be done for the default case.
		}
		break;
		
	}
	
	return isSupported;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ HandleProtocolServiceFeature - Called by SCSI Application Layer to handle
//									 a SCSIProtocolFeature.	   		   [PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
bool
IOSCSIParallelInterfaceDevice::HandleProtocolServiceFeature (
							SCSIProtocolFeature 		feature, 
							void * 						serviceValue )
{
	
	bool	wasHandled = false;
	
	switch ( feature )
	{
		
		case kSCSIProtocolFeature_SubmitDefaultInquiryData:
		{
			
			DetermineParallelFeatures ( ( UInt8 * ) serviceValue );
			wasHandled = true;
			
			// Put us in the IORegistry so we can be found by utilities like
			// System Profiler easily.
			registerService ( );
			
		}
		break;
		
		default:
		{
			break;
		}
		
	}
	
	return wasHandled;
	
}


#if 0
#pragma mark -
#pragma mark ₯ SCSI Management Functions
#pragma mark -
#endif


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ AbortSCSICommand - Not used.	   		   			   [₯OBSOLETE₯][PUBLIC]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSIServiceResponse
IOSCSIParallelInterfaceDevice::AbortSCSICommand ( 
							SCSITaskIdentifier 			request )
{
	return kSCSIServiceResponse_FUNCTION_REJECTED;
}


#if 0
#pragma mark -
#pragma mark ₯ Controller Object Accessors
#pragma mark -
#endif


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ ExecuteParallelTask - Called to issue a task to the controller.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSIServiceResponse
IOSCSIParallelInterfaceDevice::ExecuteParallelTask (
							SCSIParallelTaskIdentifier	parallelRequest )
{
	return fController->ExecuteParallelTask ( parallelRequest );
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetSCSIParallelTask - Gets a SCSIParallelTaskIdentifier from the
//							controller's command pool.				[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSIParallelTaskIdentifier
IOSCSIParallelInterfaceDevice::GetSCSIParallelTask ( bool blockForCommand )
{
	return fController->GetSCSIParallelTask ( blockForCommand );
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ FreeSCSIParallelTask - Returns a SCSIParallelTaskIdentifier to the
//							 controller's command pool.				[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::FreeSCSIParallelTask (
							SCSIParallelTaskIdentifier	returnTask )
{
	return fController->FreeSCSIParallelTask ( returnTask );
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ ReportHBAHighestLogicalUnitNumber - 	Retrieves the highest supported
//											Logical Unit.			[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSILogicalUnitNumber
IOSCSIParallelInterfaceDevice::ReportHBAHighestLogicalUnitNumber ( void )
{
	return fController->ReportHBAHighestLogicalUnitNumber ( );
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ DoesHBASupportSCSIParallelFeature - 	Queries the controller if a
//											specific SCSIParallelFeature is
//											supported.				[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::DoesHBASupportSCSIParallelFeature ( 
							SCSIParallelFeature			theFeature )
{
	return fController->DoesHBASupportSCSIParallelFeature ( theFeature );
}


#if 0
#pragma mark -
#pragma mark ₯ SCSI Parallel Task Object Accessors
#pragma mark -
#endif


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ AddToOutstandingTaskList - Adds a task to the outstanding task list.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::AddToOutstandingTaskList ( 
							SCSIParallelTaskIdentifier	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return false;
	}
	
	IOSimpleLockLock ( fQueueLock );
	
	if ( fOutstandingTaskList == NULL )
	{
		
		// There are no tasks currently in the list, add this one to the
		// front.
		fOutstandingTaskList = tempTask;
		tempTask->SetPreviousTaskInList ( NULL );
		tempTask->SetNextTaskInList ( NULL );
		
	}
	
	else
	{
		
		// Add this task to the end of the list
		SCSIParallelTask *	listTask;
		
		listTask = fOutstandingTaskList;
		while ( listTask->GetNextTaskInList ( ) != NULL )
		{
			listTask = listTask->GetNextTaskInList ( );
		}
		
		listTask->SetNextTaskInList ( tempTask );
		tempTask->SetPreviousTaskInList ( listTask );
		tempTask->SetNextTaskInList ( NULL );
		
	}
	
	IOSimpleLockUnlock ( fQueueLock );
	
	return true;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ RemoveFromOutstandingTaskList - 	Removes a task from the outstanding
//										task list.					[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::RemoveFromOutstandingTaskList ( 
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask	= ( SCSIParallelTask * ) parallelTask;
	SCSIParallelTask *	nextTask	= NULL;
	SCSIParallelTask *	prevTask	= NULL;
	bool				inList		= false;
	
	IOSimpleLockLock ( fQueueLock );
	
	// Validate that this command is in the outstanding list first (avoid
	// errant drivers doing double completions).
	nextTask = fOutstandingTaskList;
	while ( nextTask != NULL )
	{
		
		if ( nextTask == tempTask )
		{
			
			inList = true;
			break;
			
		}
		
		nextTask = nextTask->GetNextTaskInList ( );
		
	}
	
	require ( inList, ErrorExit );
	
	nextTask = tempTask->GetNextTaskInList ( );
	prevTask = tempTask->GetPreviousTaskInList ( );
	
	if ( prevTask != NULL )
	{
		
		// There is a previous task, set it to the victim's next task
		prevTask->SetNextTaskInList ( nextTask );
		
	}
	
	else
	{
		fOutstandingTaskList = nextTask;
	}
	
	if ( nextTask != NULL )
	{
		
		// The next task is not NULL, set it to the victim's previous
		nextTask->SetPreviousTaskInList ( prevTask );
		
	}
	
	// Clear out the victim's previous and next pointers
	tempTask->SetNextTaskInList ( NULL );
	tempTask->SetPreviousTaskInList ( NULL );
	
	
ErrorExit:
	
	
	IOSimpleLockUnlock ( fQueueLock );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ AddToResendTaskList - Adds a task to the resend (TASK_SET_FULL) task list.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::AddToResendTaskList ( 
							SCSIParallelTaskIdentifier	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return false;
	}
	
	IOSimpleLockLock ( fResendQueueLock );
	
	if ( fResendTaskList == NULL )
	{
		
		// There are no tasks currently in the list, add this one to the
		// front.
		fResendTaskList = tempTask;
		tempTask->SetPreviousResendTaskInList ( NULL );
		tempTask->SetNextResendTaskInList ( NULL );
		
	}
	
	else
	{
		
		// Add this task to the end of the list
		SCSIParallelTask *	listTask;
		
		listTask = fResendTaskList;
		while ( listTask->GetNextResendTaskInList ( ) != NULL )
		{
			listTask = listTask->GetNextResendTaskInList ( );
		}
		
		listTask->SetNextResendTaskInList ( tempTask );
		tempTask->SetPreviousResendTaskInList ( listTask );
		tempTask->SetNextResendTaskInList ( NULL );
		
	}
	
	IOSimpleLockUnlock ( fResendQueueLock );
	
	return true;
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ RemoveFromOutstandingTaskList - 	Removes a task from the resend task
//										(TASK_SET_FULL) list.		[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::RemoveFromResendTaskList ( 
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	SCSIParallelTask *	nextTask = NULL;
	SCSIParallelTask *	prevTask = NULL;
	
	IOSimpleLockLock ( fResendQueueLock );
	
	nextTask = tempTask->GetNextResendTaskInList ( );
	prevTask = tempTask->GetPreviousResendTaskInList ( );
	
	if ( prevTask != NULL )
	{
		
		// There is a previous task, set it to the victim's next task
		prevTask->SetNextResendTaskInList ( nextTask );
		
	}
	
	else
	{
		fResendTaskList = nextTask;
	}
	
	if ( nextTask != NULL )
	{
		
		// The next task is not NULL, set it to the victim's previous
		nextTask->SetPreviousResendTaskInList ( prevTask );
		
	}
	
	// Clear out the victim's previous and next pointers
	tempTask->SetNextResendTaskInList ( NULL );
	tempTask->SetPreviousResendTaskInList ( NULL );
	
	IOSimpleLockUnlock ( fResendQueueLock );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SetSCSITaskIdentifier - 	Sets the SCSITaskIdentifier in the
//								parallelTask.						[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool					
IOSCSIParallelInterfaceDevice::SetSCSITaskIdentifier ( 
							SCSIParallelTaskIdentifier 	parallelTask, 
							SCSITaskIdentifier 			scsiRequest )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return false;
	}
	
	return tempTask->SetSCSITaskIdentifier ( scsiRequest );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetSCSITaskIdentifier - 	Retrieves the SCSITaskIdentifier from the
//								parallelTask.						[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSITaskIdentifier
IOSCSIParallelInterfaceDevice::GetSCSITaskIdentifier ( 
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return NULL;
	}
	
	return tempTask->GetSCSITaskIdentifier ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SetDevice - Sets the device in the parallelTask.				[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::SetDevice ( 
							SCSIParallelTaskIdentifier			parallelTask,
							IOSCSIParallelInterfaceDevice * 	device )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return false;
	}
	
	return tempTask->SetDevice ( device );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SetTargetIdentifier - 	Sets the SCSITargetIdentifier in the
//								parallelTask.						[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::SetTargetIdentifier ( 
							SCSIParallelTaskIdentifier 	parallelTask,
							SCSITargetIdentifier 		theTargetID )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return false;
	}
	
	return tempTask->SetTargetIdentifier ( theTargetID );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetTargetIdentifier - 	Retrieves the SCSITargetIdentifier from the
//								parallelTask.						[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSITargetIdentifier
IOSCSIParallelInterfaceDevice::GetTargetIdentifier ( 
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return NULL;
	}
	
	return tempTask->GetTargetIdentifier ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SetDMABuffer - Sets the DMA buffer in the task.				[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

IOReturn
IOSCSIParallelInterfaceDevice::SetDMABuffer ( 
							SCSIParallelTaskIdentifier 	parallelTask,
							IOMemoryDescriptor *		buffer )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return NULL;
	}
	
	return tempTask->SetBuffer ( buffer );
	
}


// ---- Methods for Accessing data in the client's SCSI Task Object ----	
// Method to retrieve the LUN that identifies the Logical Unit whose Task
// Set to which this task is to be added.
// --> Currently this only supports Level 1 Addressing, complete
// Hierachal LUN addressing will need to be added to the SCSI Task object
// and the Peripheral Device Type objects which will represent Logical Units.
// Since that will be completed before this is released, this method will be
// changed at that time.

//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetLogicalUnitNumber - 	Retrieves the SCSILogicalUnitNumber from the
//								parallelTask.						[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSILogicalUnitNumber
IOSCSIParallelInterfaceDevice::GetLogicalUnitNumber ( 
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return 0;
	}
	
	return tempTask->GetLogicalUnitNumber ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetTaggedTaskIdentifier - Retrieves the SCSITaggedTaskIdentifier from the
//								parallelTask.						[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSITaggedTaskIdentifier 
IOSCSIParallelInterfaceDevice::GetTaggedTaskIdentifier (
							SCSIParallelTaskIdentifier		parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return kSCSIUntaggedTaskIdentifier;
	}
	
	return tempTask->GetTaggedTaskIdentifier ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetTaskAttribute - 		Retrieves the SCSITaskAttribute from the
//								parallelTask.						[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSITaskAttribute
IOSCSIParallelInterfaceDevice::GetTaskAttribute (
							SCSIParallelTaskIdentifier 		parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return kSCSITask_SIMPLE;
	}
	
	return tempTask->GetTaskAttribute ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetCommandDescriptorBlockSize - 	Retrieves the CDB size from the
//										parallelTask.				[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt8
IOSCSIParallelInterfaceDevice::GetCommandDescriptorBlockSize ( 
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return 0;
	}
	
	return tempTask->GetCommandDescriptorBlockSize ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetCommandDescriptorBlockSize - 	Retrieves the CDB from the parallelTask.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::GetCommandDescriptorBlock ( 
							SCSIParallelTaskIdentifier 		parallelTask,
							SCSICommandDescriptorBlock *	cdbData )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return false;
	}
	
	return tempTask->GetCommandDescriptorBlock ( cdbData );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetDataTransferDirection - Retrieves the data transfer direction from
//								 the parallelTask.					[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt8
IOSCSIParallelInterfaceDevice::GetDataTransferDirection ( 
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return kSCSIDataTransfer_NoDataTransfer;
	}
	
	return tempTask->GetDataTransferDirection ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetRequestedDataTransferCount - Retrieves the requested data transfer
//									  count from the parallelTask.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt64
IOSCSIParallelInterfaceDevice::GetRequestedDataTransferCount ( 
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return 0;
	}
	
	return tempTask->GetRequestedDataTransferCount ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetRequestedDataTransferCount - Retrieves the realized data transfer
//									  count from the parallelTask.	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt64
IOSCSIParallelInterfaceDevice::GetRealizedDataTransferCount (
							SCSIParallelTaskIdentifier 		parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return 0;
	}
	
	return tempTask->GetRealizedDataTransferCount ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetRequestedDataTransferCount - Sets the realized data transfer
//									  count in the parallelTask.	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

bool
IOSCSIParallelInterfaceDevice::SetRealizedDataTransferCount ( 
							SCSIParallelTaskIdentifier 	parallelTask,
							UInt64 						realizedTransferCountInBytes )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return false;
	}
	
	return tempTask->SetRealizedDataTransferCount ( realizedTransferCountInBytes );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetRequestedDataTransferCount - Increments the realized data transfer
//									  count in the parallelTask.	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::IncrementRealizedDataTransferCount (
							SCSIParallelTaskIdentifier 	parallelTask,
							UInt64 						realizedTransferCountInBytes )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return;
	}
	
	return tempTask->IncrementRealizedDataTransferCount ( realizedTransferCountInBytes );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetRequestedDataTransferCount - Retrieves the data buffer in the
//									  parallelTask.					[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

IOMemoryDescriptor *
IOSCSIParallelInterfaceDevice::GetDataBuffer (
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return NULL;
	}
	
	return tempTask->GetDataBuffer ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetDataBufferOffset - Retrieves the data buffer offset in the parallelTask.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt64
IOSCSIParallelInterfaceDevice::GetDataBufferOffset (
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return 0;
	}
	
	return tempTask->GetDataBufferOffset ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetTimeoutDuration - Retrieves the timeout duration in the parallelTask.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt32
IOSCSIParallelInterfaceDevice::GetTimeoutDuration (
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return 0;
	}
	
	return tempTask->GetTimeoutDuration ( );
	
}

//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ SetSCSIParallelFeatureNegotiation - 	Sets a feature negotiation request
//											in the specified task.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void
IOSCSIParallelInterfaceDevice::SetSCSIParallelFeatureNegotiation ( 
							SCSIParallelTaskIdentifier 		parallelTask,
							SCSIParallelFeature 			requestedFeature, 
							SCSIParallelFeatureRequest 		newRequest )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return;
	}
	
	return tempTask->SetSCSIParallelFeatureNegotiation ( requestedFeature, newRequest );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetSCSIParallelFeatureNegotiation - 	Gets a feature negotiation request
//											in the specified task.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSIParallelFeatureRequest
IOSCSIParallelInterfaceDevice::GetSCSIParallelFeatureNegotiation (
							SCSIParallelTaskIdentifier 		parallelTask,
							SCSIParallelFeature 			requestedFeature )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return kSCSIParallelFeature_NoNegotiation;
	}
	
	return tempTask->GetSCSIParallelFeatureNegotiation ( requestedFeature );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetSCSIParallelFeatureNegotiationResult - Gets a feature negotiation
//												result in the specified task.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

SCSIParallelFeatureResult
IOSCSIParallelInterfaceDevice::GetSCSIParallelFeatureNegotiationResult (
							SCSIParallelTaskIdentifier 		parallelTask,
							SCSIParallelFeature 			requestedFeature )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return kSCSIParallelFeature_NegotitiationUnchanged;
	}
	
	return tempTask->GetSCSIParallelFeatureNegotiationResult ( requestedFeature );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetControllerTaskIdentifier - Gets the identifier associated with the
//									task.							[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt64
IOSCSIParallelInterfaceDevice::GetControllerTaskIdentifier (
							SCSIParallelTaskIdentifier 		parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return 0;
	}
	
	return tempTask->GetControllerTaskIdentifier ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetHBADataSize - Gets the size of HBA Data associated with a command.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

UInt32
IOSCSIParallelInterfaceDevice::GetHBADataSize (
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return 0;
	}
	
	return tempTask->GetHBADataSize ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetHBADataPointer - Gets the HBA Data pointer associated with a command.
//																	[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void *
IOSCSIParallelInterfaceDevice::GetHBADataPointer (
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return NULL;
	}
	
	return tempTask->GetHBADataPointer ( );
	
}


//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯ GetHBADataDescriptor - Gets the HBA memory descriptor associated with
//							 a command.								[PROTECTED]
//ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

IOMemoryDescriptor *
IOSCSIParallelInterfaceDevice::GetHBADataDescriptor (
							SCSIParallelTaskIdentifier 	parallelTask )
{
	
	SCSIParallelTask *	tempTask = ( SCSIParallelTask * ) parallelTask;
	
	if ( tempTask == NULL )
	{
		return NULL;
	}
	
	return tempTask->GetHBADataDescriptor ( );
	
}