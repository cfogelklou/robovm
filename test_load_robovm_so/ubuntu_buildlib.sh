#!/bin/bash
export ROBOVM_DEV_ROOT=/home/appsupport/Projects/robovm/robovm
cd $ROBOVM_DEV_ROOT
vm/build.sh
vm/copy-binaries.sh
cd test_load_robovm_so
rm -rf src_lib/classes
mkdir src_lib/classes
javac -d src_lib/classes/ src_lib/TestLib.java
$ROBOVM_DEV_ROOT/bin/robovm -verbose -dump-intermediates -os linux -binaryType dynamic_lib -forcelinkclasses TestLib -skiprt -bcp $ROBOVM_DEV_ROOT/rt/target/classes:/robovm-rt/target/test-classes -cp $ROBOVM_DEV_ROOT/test_load_robovm_so/src_lib/classes TestLib 
./Debug/test_load_robovm_so


