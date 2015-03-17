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
package org.robovm.apple.scenekit;

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
import org.robovm.apple.uikit.*;
import org.robovm.apple.coreanimation.*;
import org.robovm.apple.coregraphics.*;
import org.robovm.apple.dispatch.*;
import org.robovm.apple.glkit.*;
import org.robovm.apple.spritekit.*;
import org.robovm.apple.opengles.*;
/*</imports>*/

/*<javadoc>*/
/*</javadoc>*/
@Marshaler(SCNPhysicsTestSearchMode.Marshaler.class)
/*<annotations>*/@Library("SceneKit")/*</annotations>*/
/*<visibility>*/public/*</visibility>*/ class /*<name>*/SCNPhysicsTestSearchMode/*</name>*/ 
    extends /*<extends>*/CocoaUtility/*</extends>*/ 
    /*<implements>*//*</implements>*/ {

    public static class Marshaler {
        @MarshalsPointer
        public static SCNPhysicsTestSearchMode toObject(Class<SCNPhysicsTestSearchMode> cls, long handle, long flags) {
            NSString o = (NSString) NSObject.Marshaler.toObject(NSString.class, handle, flags);
            if (o == null) {
                return null;
            }
            return SCNPhysicsTestSearchMode.valueOf(o);
        }
        @MarshalsPointer
        public static long toNative(SCNPhysicsTestSearchMode o, long flags) {
            if (o == null) {
                return 0L;
            }
            return NSObject.Marshaler.toNative(o.value(), flags);
        }
    }
    
    /*<ptr>*/
    /*</ptr>*/
    /*<bind>*/static { Bro.bind(SCNPhysicsTestSearchMode.class); }/*</bind>*/
    /*<constants>*//*</constants>*/
    public static final SCNPhysicsTestSearchMode Any = new SCNPhysicsTestSearchMode("AnyValue");
    public static final SCNPhysicsTestSearchMode Closest = new SCNPhysicsTestSearchMode("ClosestValue");
    public static final SCNPhysicsTestSearchMode All = new SCNPhysicsTestSearchMode("AllValue");
    
    private static SCNPhysicsTestSearchMode[] values = new SCNPhysicsTestSearchMode[] {Any, Closest, All};
    private final LazyGlobalValue<NSString> lazyGlobalValue;
    
    private SCNPhysicsTestSearchMode(String getterName) {
        lazyGlobalValue = new LazyGlobalValue<>(getClass(), getterName);
    }
    /*<constructors>*//*</constructors>*/
    /*<properties>*//*</properties>*/
    /*<members>*//*</members>*/
    public NSString value() {
        return lazyGlobalValue.value();
    }
    
    public static SCNPhysicsTestSearchMode valueOf(NSString value) {
        for (SCNPhysicsTestSearchMode v : values) {
            if (v.value().equals(value)) {
                return v;
            }
        }
        throw new IllegalArgumentException("No constant with value " + value + " found in " 
            + /*<name>*/SCNPhysicsTestSearchMode/*</name>*/.class.getName());
    }
    /*<methods>*/
    @GlobalValue(symbol="SCNPhysicsTestSearchModeAny", optional=true)
    protected static native NSString AnyValue();
    @GlobalValue(symbol="SCNPhysicsTestSearchModeClosest", optional=true)
    protected static native NSString ClosestValue();
    @GlobalValue(symbol="SCNPhysicsTestSearchModeAll", optional=true)
    protected static native NSString AllValue();
    /*</methods>*/
}
