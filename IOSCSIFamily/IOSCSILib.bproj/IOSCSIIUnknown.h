/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
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
#ifndef _IOKIT_IOSCSIIUNKNOWN_H
#define _IOKIT_IOSCSIIUNKNOWN_H

#include <IOKit/IOCFPlugIn.h>

__BEGIN_DECLS
extern void *IOSCSILibFactory(CFAllocatorRef allocator, CFUUIDRef typeID);
__END_DECLS

class IOSCSIIUnknown {

public:
    struct InterfaceMap {
        IUnknownVTbl *pseudoVTable;
        IOSCSIIUnknown *obj;
    };

private:
    IOSCSIIUnknown(IOSCSIIUnknown &src);	// Disable copy constructor
    void operator =(IOSCSIIUnknown &src);
    IOSCSIIUnknown() : refCount(1) { };

protected:

    static int factoryRefCount;
    static void factoryAddRef();
    static void factoryRelease();

    IOSCSIIUnknown(void *unknownVTable);
    virtual ~IOSCSIIUnknown(); // Also virtualise destructor

    static HRESULT genericQueryInterface(void *self, REFIID iid, void **ppv);
    static unsigned long genericAddRef(void *self);
    static unsigned long genericRelease(void *self);

protected:

    unsigned long refCount;
    InterfaceMap iunknown;

public:
    virtual HRESULT queryInterface(REFIID iid, void **ppv) = 0;
    virtual unsigned long addRef();
    virtual unsigned long release();
};

#endif /* !_IOKIT_IOSCSIIUNKNOWN_H */