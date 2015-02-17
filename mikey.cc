#include <nan.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/IOKitLib.h>

using namespace v8;

namespace mikey {
  // Singleton Manager for Apple MiKey HID
  class MikeyManager {
  public:
    void SendKeyEvent(const char *keyEvent) {
      NanScope();

      if (hasCallback) {
        Handle<Value> argv[] = { NanNew<String>(keyEvent) };
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
      InitHIDManager();
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

    void InitHIDManager() {
      IOHIDManagerRef hidManager;

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
    static void ValueCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value)
    {
      uint32_t usage = IOHIDElementGetUsage(IOHIDValueGetElement(value));
      long val = IOHIDValueGetIntegerValue(value);
      if (usage == 0x89 && val == 1) {
        MikeyManager::GetInstance()->SendKeyEvent("playPause");
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

  // emulate 
  NAN_METHOD(SendKeyEvent) {
    NanScope();

    String::Utf8Value keyEvent(args[0]);
    MikeyManager::GetInstance()->SendKeyEvent(*keyEvent);

    NanReturnUndefined();
  }

  void Init(Handle<Object> exports) {
    NODE_SET_METHOD(exports, "setListener", SetListener);
    // exports->Set(NanNew("addListener"), NanNew<FunctionTemplate>(SetListener)->GetFunction());
    NODE_SET_METHOD(exports, "sendKeyEvent", SendKeyEvent);
  }

  NODE_MODULE(mikey, Init)
}
