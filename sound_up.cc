//
//  Created by
//      George Warner
//      Casey Fleser http://www.somegeekintn.com/blog/2006/03/universal-mediakeys/
//
//  Stolen from http://stackoverflow.com/questions/10273159
//

#include "sound_up.h"
#include <stdio.h>
#include <strings.h>

io_connect_t get_event_driver(void)
{
    static  mach_port_t sEventDrvrRef = 0;
    mach_port_t masterPort;
    io_iterator_t iter;
    io_object_t service;
    kern_return_t kr;
    
    if (!sEventDrvrRef)
    {
        // Get master device port
        kr = IOMasterPort( bootstrap_port, &masterPort );
        // NSCAssert((KERN_SUCCESS == kr), "IOMasterPort failed.");
        if (KERN_SUCCESS != kr)
        {
            fprintf(stderr, "%s\n", "IOMasterPort failed.");
            return 0;
        }
        
        kr = IOServiceGetMatchingServices( masterPort, IOServiceMatching( kIOHIDSystemClass ), &iter );
        // NSCAssert((KERN_SUCCESS == kr), @"IOServiceGetMatchingServices failed.");
        if (KERN_SUCCESS != kr)
        {
            fprintf(stderr, "%s\n", "IOServiceGetMatchingServices failed.");
            return 0;
        }

        if (!IOIteratorIsValid( iter ))
        {
          fprintf(stderr, "%s\n", "IOIterator is not valid");
          return 0;
        }
        
        service = IOIteratorNext( iter );
        // NSCAssert(service, @"IOIteratorNext failed.");
        if (!service)
        {
            fprintf(stderr, "%s\n", "IOIteratorNext failed.");
            return 0;
        }
        
        kr = IOServiceOpen( service, mach_task_self(),
                            kIOHIDParamConnectType, &sEventDrvrRef );
        // NSCAssert((KERN_SUCCESS == kr), @"IOServiceOpen failed.");
        if (KERN_SUCCESS != kr)
        {
            fprintf(stderr, "%s\n", "IOServiceOpen failed.");
            return 0;
        }
        
        IOObjectRelease( service );
        IOObjectRelease( iter );
    }
    return sEventDrvrRef;
}

void HIDPostAuxKey(const UInt8 auxKeyCode)
{
    NXEventData   event;
    kern_return_t kr;
    IOGPoint      loc = { 0, 0 };
    
    // Key press event
    UInt32      evtInfo = auxKeyCode << 16 | NX_KEYDOWN << 8;
    bzero(&event, sizeof(NXEventData));
    event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
    event.compound.misc.L[0] = evtInfo;
    kr = IOHIDPostEvent( get_event_driver(), NX_SYSDEFINED, loc, &event, kNXEventDataVersion, 0, FALSE );
    // NSCAssert((KERN_SUCCESS == kr), @"IOHIDPostEvent pressing failed.");
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "%s\n", "IOHIDPostEvent pressing failed.");
    }
    
    // Key release event
    evtInfo = auxKeyCode << 16 | NX_KEYUP << 8;
    bzero(&event, sizeof(NXEventData));
    event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
    event.compound.misc.L[0] = evtInfo;
    kr = IOHIDPostEvent( get_event_driver(), NX_SYSDEFINED, loc, &event, kNXEventDataVersion, 0, FALSE );
    // NSCAssert((KERN_SUCCESS == kr), @"IOHIDPostEvent releasing failed.");
    if (KERN_SUCCESS != kr)
    {
        fprintf(stderr, "%s\n", "IOHIDPostEvent releasing failed.");
    }
}
