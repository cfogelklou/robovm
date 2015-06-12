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

#Delete the executable and the library.
rm -rf src_lib/classes
mkdir src_lib/classes
rm -rf HelloWorld
rm -rf src_exe/classes
mkdir src_exe/classes

# Compile the test library
javac -d src_lib/classes/ src_lib/TestLib.java

# Compile HelloWorld
javac -d src_exe/classes/ src_exe/HelloWorld.java

#Use our debug compiler to build the test library.
$ROBOVM_DEV_ROOT/bin/robovm -dump-intermediates -os linux -binaryType dynamic_lib -forcelinkclasses TestLib -skiprt -bcp $ROBOVM_DEV_ROOT/rt/target/classes:/robovm-rt/target/test-classes -cp $ROBOVM_DEV_ROOT/test_load_robovm_so/src_lib/classes TestLib 

#Use our debug compiler to build the HelloWorld application.
$ROBOVM_DEV_ROOT/bin/robovm -dump-intermediates -os linux -binaryType executable -forcelinkclasses HelloWorld -skiprt -bcp $ROBOVM_DEV_ROOT/rt/target/classes:/robovm-rt/target/test-classes -cp $ROBOVM_DEV_ROOT/test_load_robovm_so/src_exe/classes HelloWorld

#Run the  test library.
./Debug/test_load_robovm_so

#Run HelloWorld.
./HelloWorld/HelloWorld -rvm:log=debug

