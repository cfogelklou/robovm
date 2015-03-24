#!/bin/bash
export ROBOVM_DEV_ROOT=~/Projects/robovm/robovm
cd $ROBOVM_DEV_ROOT
vm/build.sh
vm/copy-binaries.sh
cd robovm_test
rm -rf classes
mkdir classes
javac -d classes/ HelloWorld.java
robovm -verbose -use-debug-libs -os linux -cp classes/ HelloWorld


