#!/data/data/com.termux/files/usr/bin/bash
#/usr/bin/bash

./main $(pgrep "$1") > ramdump
