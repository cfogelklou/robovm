#!/bin/bash
export ROBOVM_DEV_ROOT=/home/appsupport/Projects/robovm/robovm
cd $ROBOVM_DEV_ROOT
# Build the runtime libraries
vm/build.sh
vm/copy-binaries.sh

#Enter and build the test program that will load the libray.
cd test_load_robovm_so
cd Debug
make clean all
cd ..

#Build the test library that the test program will load.
rm -rf src_lib/classes
mkdir src_lib/classes

# Compile the test library
javac -d src_lib/classes/ src_lib/TestLib.java

#Use our debug compiler to build the test library.
$ROBOVM_DEV_ROOT/bin/robovm -verbose -dump-intermediates -os linux -binaryType dynamic_lib -forcelinkclasses TestLib -skiprt -bcp $ROBOVM_DEV_ROOT/rt/target/classes:/robovm-rt/target/test-classes -cp $ROBOVM_DEV_ROOT/test_load_robovm_so/src_lib/classes TestLib 

#Run the  test library.
./Debug/test_load_robovm_so


