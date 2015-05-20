#!/bin/bash
export ROBOVM_DEV_ROOT=~/Projects/robovm/robovm
cd $ROBOVM_DEV_ROOT
#mvn install
vm/build.sh
vm/copy-binaries.sh
cd robovm_test
rm -rf classes
mkdir classes
javac -d classes/ HelloWorld.java
$ROBOVM_DEV_ROOT/bin/robovm -verbose -use-debug-libs -os linux -cp classes/ HelloWorld
