/*
 * Copyright (C) 2013-2015 RoboVM AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.robovm.apple.corebluetooth;

/*<imports>*/
import java.io.*;
import java.nio.*;
import java.util.*;
import org.robovm.objc.*;
import org.robovm.objc.annotation.*;
import org.robovm.objc.block.*;
import org.robovm.rt.*;
import org.robovm.rt.bro.*;
import org.robovm.rt.bro.annotation.*;
import org.robovm.rt.bro.ptr.*;
import org.robovm.apple.foundation.*;
import org.robovm.apple.corefoundation.*;
import org.robovm.apple.dispatch.*;
/*</imports>*/

/*<javadoc>*/
/**
 * @since Available in iOS 5.0 and later.
 */
/*</javadoc>*/
/*<annotations>*/@Library("CoreBluetooth") @NativeClass/*</annotations>*/
/*<visibility>*/public/*</visibility>*/ class /*<name>*/CBService/*</name>*/ 
    extends /*<extends>*/CBAttribute/*</extends>*/ 
    /*<implements>*//*</implements>*/ {

    /*<ptr>*/public static class CBServicePtr extends Ptr<CBService, CBServicePtr> {}/*</ptr>*/
    /*<bind>*/static { ObjCRuntime.bind(CBService.class); }/*</bind>*/
    /*<constants>*//*</constants>*/
    /*<constructors>*/
    public CBService() {}
    protected CBService(SkipInit skipInit) { super(skipInit); }
    /*</constructors>*/
    /*<properties>*/
    @Property(selector = "peripheral")
    public native CBPeripheral getPeripheral();
    @Property(selector = "isPrimary")
    public native boolean isPrimary();
    @Property(selector = "includedServices")
    public native NSArray<CBService> getIncludedServices();
    @Property(selector = "characteristics")
    public native NSArray<CBCharacteristic> getCharacteristics();
    /*</properties>*/
    /*<members>*//*</members>*/
    /*<methods>*/
    
    /*</methods>*/
}
