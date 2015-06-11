#!/bin/bash
export ROBOVM_DEV_ROOT=/home/appsupport/Projects/robovm/robovm
cd $ROBOVM_DEV_ROOT
#mvn install
vm/build.sh
vm/copy-binaries.sh
cd test_load_robovm_so
rm -rf classes
mkdir classes
javac -d classes/ HelloWorld.java
$ROBOVM_DEV_ROOT/bin/robovm -verbose -dump-intermediates -os linux -binaryType dynamic_lib -forcelinkclasses HelloWorld -skiprt -bcp $ROBOVM_DEV_ROOT/rt/target/classes:/robovm-rt/target/test-classes -cp $ROBOVM_DEV_ROOT/test_load_robovm_so/classes HelloWorld 
./Debug/test_load_robovm_so

