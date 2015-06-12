#!/bin/bash
export ROBOVM_DEV_ROOT=/home/appsupport/Projects/robovm/robovm
cd $ROBOVM_DEV_ROOT
# Build the runtime libraries
vm/build.sh
vm/copy-binaries.sh

cd test_load_robovm_so
rm -rf HelloWorld
rm -rf src_exe/classes
mkdir src_exe/classes
javac -d src_exe/classes/ src_exe/HelloWorld.java
$ROBOVM_DEV_ROOT/bin/robovm -dump-intermediates -os linux -binaryType executable -forcelinkclasses HelloWorld -skiprt -bcp $ROBOVM_DEV_ROOT/rt/target/classes:/robovm-rt/target/test-classes -cp $ROBOVM_DEV_ROOT/test_load_robovm_so/src_exe/classes HelloWorld

./HelloWorld/HelloWorld -rvm:log=debug


