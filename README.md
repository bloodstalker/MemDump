# MemDump
Dump Memory

# Building

If you are building for Android and your host is linux:<br/>
```bash
export NDK_PROJECT_DIR=`pwd`
make android
```

If you are building for android and your host is windows and you use cygwin, you can't use `pwd` to get the path since that would return a nix style path and your windows ndk won't recognize that. Just give it a windows path.<br/>

For Linux:
```bash
make
```

if you are building on linux for linux or android on android just run `make` and you're good to go. you obviously need sudo to attach to a pid so it wont work on your android device if its not rooted.<br/>

For windows:
You can't use this to get RAM dumps on windows.<br/>
