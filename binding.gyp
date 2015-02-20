{
  "targets": [
    {
      "target_name": "mikey",
      "sources": [
        "mikey.cc",
        "sound_up.cc"
      ],
      "include_dirs": [ 
        "<!(node -e \"require('nan')\")",
        "System/Library/Frameworks/IOKit.framework/Headers",
        "System/Library/Frameworks/Carbon.framework/Headers"
      ]
    }
  ]
}
