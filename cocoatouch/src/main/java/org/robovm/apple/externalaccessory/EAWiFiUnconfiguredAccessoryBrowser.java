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
package org.robovm.apple.externalaccessory;

/*<imports>*/
import java.io.*;
import java.nio.*;
import java.util.*;
import org.robovm.objc.*;
import org.robovm.objc.annotation.*;
import org.robovm.objc.block.*;
import org.robovm.rt.*;
import org.robovm.rt.annotation.*;
import org.robovm.rt.bro.*;
import org.robovm.rt.bro.annotation.*;
import org.robovm.rt.bro.ptr.*;
import org.robovm.apple.foundation.*;
import org.robovm.apple.dispatch.*;
import org.robovm.apple.uikit.*;
/*</imports>*/

/*<javadoc>*/
/**
 * @since Available in iOS 8.0 and later.
 */
/*</javadoc>*/
/*<annotations>*/@Library("ExternalAccessory") @NativeClass/*</annotations>*/
/*<visibility>*/public/*</visibility>*/ class /*<name>*/EAWiFiUnconfiguredAccessoryBrowser/*</name>*/ 
    extends /*<extends>*/NSObject/*</extends>*/ 
    /*<implements>*//*</implements>*/ {

    /*<ptr>*/public static class EAWiFiUnconfiguredAccessoryBrowserPtr extends Ptr<EAWiFiUnconfiguredAccessoryBrowser, EAWiFiUnconfiguredAccessoryBrowserPtr> {}/*</ptr>*/
    /*<bind>*/static { ObjCRuntime.bind(EAWiFiUnconfiguredAccessoryBrowser.class); }/*</bind>*/
    /*<constants>*//*</constants>*/
    /*<constructors>*/
    public EAWiFiUnconfiguredAccessoryBrowser() {}
    protected EAWiFiUnconfiguredAccessoryBrowser(SkipInit skipInit) { super(skipInit); }
    /**
     * @since Available in iOS 8.0 and later.
     */
    @WeaklyLinked
    public EAWiFiUnconfiguredAccessoryBrowser(EAWiFiUnconfiguredAccessoryBrowserDelegate delegate, DispatchQueue queue) { super((SkipInit) null); initObject(init(delegate, queue)); }
    /*</constructors>*/
    /*<properties>*/
    @Property(selector = "delegate")
    public native EAWiFiUnconfiguredAccessoryBrowserDelegate getDelegate();
    @Property(selector = "setDelegate:", strongRef = true)
    public native void setDelegate(EAWiFiUnconfiguredAccessoryBrowserDelegate v);
    @Property(selector = "unconfiguredAccessories")
    public native NSSet<EAWiFiUnconfiguredAccessory> getUnconfiguredAccessories();
    /*</properties>*/
    /*<members>*//*</members>*/
    /*<methods>*/
    /**
     * @since Available in iOS 8.0 and later.
     */
    @WeaklyLinked
    @Method(selector = "initWithDelegate:queue:")
    protected native @Pointer long init(EAWiFiUnconfiguredAccessoryBrowserDelegate delegate, DispatchQueue queue);
    /**
     * @since Available in iOS 8.0 and later.
     */
    @Method(selector = "startSearchingForUnconfiguredAccessoriesMatchingPredicate:")
    public native void startSearchingForUnconfiguredAccessories(NSPredicate predicate);
    /**
     * @since Available in iOS 8.0 and later.
     */
    @Method(selector = "stopSearchingForUnconfiguredAccessories")
    public native void stopSearchingForUnconfiguredAccessories();
    /**
     * @since Available in iOS 8.0 and later.
     */
    @WeaklyLinked
    @Method(selector = "configureAccessory:withConfigurationUIOnViewController:")
    public native void configureAccessory(EAWiFiUnconfiguredAccessory accessory, UIViewController viewController);
    /*</methods>*/
}
