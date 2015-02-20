#include <nan.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/IOKitLib.h>
#include <Carbon/Carbon.h>
#include "sound_up.h"

using namespace v8;

namespace mikey {
  const int kMikeyPlayPauseUsageCode  = 0x89;
  const int kMikeyNextUsageCode       = 0x8A;
  const int kMikeyPrevUsageCode       = 0x8B;
  const int kMikeySoundUpUsageCode    = 0x8C;
  const int kMikeySoundDownUsageCode  = 0x8D;

  // Singleton Manager for Apple MiKey HID
  class MikeyManager {
  public:
    void SendKeyEvent(uint32_t usage) {
      NanScope();

      if (hasCallback) {
        Handle<Value> argv[] = { NanNew<Number>(usage) };
        callback->Call(1, argv);
      }
    }

    void setCallback(NanCallback *cb) {
      hasCallback = TRUE;
      callback = cb;
    }

    // Returns singletone instance of the MikeyManager
    static MikeyManager* GetInstance() {
      if (!instance) {
        instance = new MikeyManager;
      }
      return instance;
    }

  private:
    // Constructor
    MikeyManager() { 
      hasCallback = FALSE;
      StartHIDManager();
      StartSecureEventInputCheckTimer();
    }
    // Destructor
    ~MikeyManager() {}
    // Copy constructor
    MikeyManager(MikeyManager const&) {}
    // Assignament operator
    MikeyManager& operator=(MikeyManager const&) {}

    // Singleton reference
    static MikeyManager *instance;

    bool hasCallback;
    NanCallback *callback;

    IOHIDManagerRef hidManager;
    CFRunLoopTimerRef secureTimer;
    bool secureEventInputWasEnabled;

    void StartHIDManager() {
      hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
      if (!hidManager) {
        fprintf(stderr, "%s: Failed to instantiate IOHIDManager\n", __PRETTY_FUNCTION__);
      }

      IOHIDManagerSetDeviceMatching(hidManager, IOServiceMatching("AppleMikeyHIDDriver"));
      IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

      if (IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeSeizeDevice) == kIOReturnError) {
        fprintf(stderr, "%s: Failed to open IOHIDManager\n", __PRETTY_FUNCTION__);
      }

      IOHIDManagerRegisterInputValueCallback(hidManager, ValueCallback, NULL);
    }
    void StopHIDManager() {
      if(IOHIDManagerClose(hidManager, kIOHIDOptionsTypeNone) == kIOReturnError) {
        fprintf(stderr, "%s: Failed to close IOHIDManager\n", __PRETTY_FUNCTION__);
      }
      CFRelease(hidManager);
      hidManager = NULL;
    }
    /*
     Every time Apple's SecureEventInputs gets enabled, Mikey IOHIDManager loses
     exclusive access, so other apps (read iTunes) gets notified as well from
     Mikey device related events. Hence, Mikey IOHIDManger is restarted to re-acquire
     exclusive access.
   */
    void StartSecureEventInputCheckTimer() {
      secureTimer = CFRunLoopTimerCreate(kCFAllocatorDefault,
        CFAbsoluteTimeGetCurrent(), 1, 0, 0, CheckIsSecureEventInputEnabled, NULL);
      CFRunLoopAddTimer(CFRunLoopGetCurrent(), secureTimer, kCFRunLoopDefaultMode);
    }
    static void CheckIsSecureEventInputEnabled(CFRunLoopTimerRef timer, void *info) {
      bool secureEventInputIsEnabled = IsSecureEventInputEnabled();
      bool secureEventInputWasEnabled = MikeyManager::GetInstance()->secureEventInputWasEnabled;
      if (secureEventInputIsEnabled != secureEventInputWasEnabled) {
        MikeyManager::GetInstance()->secureEventInputWasEnabled = secureEventInputIsEnabled;
        MikeyManager::GetInstance()->StopHIDManager();
        MikeyManager::GetInstance()->StartHIDManager();
      }
    }
    static void ValueCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value) {
      uint32_t usage = IOHIDElementGetUsage(IOHIDValueGetElement(value));
      long val = IOHIDValueGetIntegerValue(value);
      // val can be in [0, 1, 2, 3], but 1 is always the first data coming; 'down' event.
      if (val == 1 && kMikeyPlayPauseUsageCode <= usage && usage <= kMikeySoundDownUsageCode) {
        MikeyManager::GetInstance()->SendKeyEvent(usage);
      }
    }
  };

  // initializing singleton reference pointer (not the instance)
  MikeyManager *MikeyManager::instance = 0;

  NAN_METHOD(SetListener) {
    NanScope();

    Local<Function> callbackHandle = args[0].As<Function>();
    NanCallback *callback = new NanCallback(callbackHandle);

    MikeyManager::GetInstance()->setCallback(callback);

    NanReturnUndefined();
  }

  NAN_METHOD(SendBackKeyEvent) {
    NanScope();

    uint32_t usage = args[0]->Uint32Value();
    switch (usage) {
      case kMikeyPlayPauseUsageCode:
        HIDPostAuxKey(NX_KEYTYPE_PLAY);
        break;
      case kMikeyPrevUsageCode:
        HIDPostAuxKey(NX_KEYTYPE_FAST);
        break;
      case kMikeyNextUsageCode:
        HIDPostAuxKey(NX_KEYTYPE_REWIND);
        break;
      case kMikeySoundUpUsageCode:
        HIDPostAuxKey(NX_KEYTYPE_SOUND_UP);
        break;
      case kMikeySoundDownUsageCode:
        HIDPostAuxKey(NX_KEYTYPE_SOUND_DOWN);
        break;
    }

    NanReturnUndefined();
  }

  // emulate 
  NAN_METHOD(SendKeyEvent) {
    NanScope();

    uint32_t usage = args[0]->Uint32Value();
    MikeyManager::GetInstance()->SendKeyEvent(usage);

    NanReturnUndefined();
  }

  void Init(Handle<Object> exports) {
    NODE_SET_METHOD(exports, "setListener", SetListener);
    // exports->Set(NanNew("addListener"), NanNew<FunctionTemplate>(SetListener)->GetFunction());
    NODE_SET_METHOD(exports, "sendBackKeyEvent", SendBackKeyEvent);
    NODE_SET_METHOD(exports, "sendKeyEvent", SendKeyEvent);
  }

  NODE_MODULE(mikey, Init)
}
