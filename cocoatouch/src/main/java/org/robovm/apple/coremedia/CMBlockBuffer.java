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
package org.robovm.apple.coremedia;

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
import org.robovm.apple.coreaudio.*;
import org.robovm.apple.coreanimation.*;
import org.robovm.apple.coregraphics.*;
import org.robovm.apple.corevideo.*;
import org.robovm.apple.audiotoolbox.*;
/*</imports>*/

/*<javadoc>*/
/*</javadoc>*/
/*<annotations>*/@Library("CoreMedia")/*</annotations>*/
/*<visibility>*/public/*</visibility>*/ class /*<name>*/CMBlockBuffer/*</name>*/ 
    extends /*<extends>*/CFType/*</extends>*/ 
    /*<implements>*//*</implements>*/ {

    /*<ptr>*/public static class CMBlockBufferPtr extends Ptr<CMBlockBuffer, CMBlockBufferPtr> {}/*</ptr>*/
    /*<bind>*/static { Bro.bind(CMBlockBuffer.class); }/*</bind>*/
    /*<constants>*//*</constants>*/
    /*<constructors>*/
    protected CMBlockBuffer() {}
    /*</constructors>*/
    /*<properties>*//*</properties>*/
    /*<members>*//*</members>*/
    /**
     * @since Available in iOS 4.0 and later.
     */
    public static CMBlockBuffer create(int subBlockCapacity, CMBlockBufferFlags flags) {
        CMBlockBuffer.CMBlockBufferPtr ptr = new CMBlockBuffer.CMBlockBufferPtr();
        create(null, subBlockCapacity, flags, ptr);
        return ptr.get();
    }
    /**
     * @since Available in iOS 4.0 and later.
     */
    public static CMBlockBuffer create(VoidPtr memoryBlock, @MachineSizedUInt long blockLength, CFAllocator blockAllocator, CMBlockBufferCustomBlockSource customBlockSource, @MachineSizedUInt long offsetToData, @MachineSizedUInt long dataLength, CMBlockBufferFlags flags) {
        CMBlockBuffer.CMBlockBufferPtr ptr = new CMBlockBuffer.CMBlockBufferPtr();
        create(null, memoryBlock, blockLength, blockAllocator, customBlockSource, offsetToData, dataLength, flags, ptr);
        return ptr.get();
    }
    /**
     * @since Available in iOS 4.0 and later.
     */
    public static CMBlockBuffer create(CMBlockBuffer targetBuffer, @MachineSizedUInt long offsetToData, @MachineSizedUInt long dataLength, CMBlockBufferFlags flags) {
        CMBlockBuffer.CMBlockBufferPtr ptr = new CMBlockBuffer.CMBlockBufferPtr();
        create(null, targetBuffer, offsetToData, dataLength, flags, ptr);
        return ptr.get();
    }
    /**
     * @since Available in iOS 4.0 and later.
     */
    public static CMBlockBuffer create(CMBlockBuffer sourceBuffer, CFAllocator blockAllocator, CMBlockBufferCustomBlockSource customBlockSource, @MachineSizedUInt long offsetToData, @MachineSizedUInt long dataLength, CMBlockBufferFlags flags) {
        CMBlockBuffer.CMBlockBufferPtr ptr = new CMBlockBuffer.CMBlockBufferPtr();
        create(null, sourceBuffer, blockAllocator, customBlockSource, offsetToData, dataLength, flags, ptr);
        return ptr.get();
    }

    /**
     * @since Available in iOS 4.0 and later.
     */
    public BytePtr accessDataBytes(@MachineSizedUInt long offset, @MachineSizedUInt long length, VoidPtr temporaryBlock) {
        BytePtr.BytePtrPtr ptr = new BytePtr.BytePtrPtr();
        OSStatus err = accessDataBytes(this, offset, length, temporaryBlock, ptr);
        if (err.equals(CMBlockBufferError.No)) {
            return ptr.get();
        }
        return null;
    }
    /**
     * @since Available in iOS 4.0 and later.
     */
    private OSStatus replaceDataBytes(VoidPtr sourceBytes, @MachineSizedUInt long offsetIntoDestination, @MachineSizedUInt long dataLength) {
        return replaceDataBytes(sourceBytes, this, offsetIntoDestination, dataLength);
    }
    /**
     * @since Available in iOS 4.0 and later.
     */
    public OSStatus fillDataBytes(byte fillByte, @MachineSizedUInt long offsetIntoDestination, @MachineSizedUInt long dataLength) {
        return fillDataBytes(fillByte, this, offsetIntoDestination, dataLength);
    }
    /*<methods>*/
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferCreateEmpty", optional=true)
    private static native OSStatus create(CFAllocator structureAllocator, int subBlockCapacity, CMBlockBufferFlags flags, CMBlockBuffer.CMBlockBufferPtr newBBufOut);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferCreateWithMemoryBlock", optional=true)
    private static native OSStatus create(CFAllocator structureAllocator, VoidPtr memoryBlock, @MachineSizedUInt long blockLength, CFAllocator blockAllocator, CMBlockBufferCustomBlockSource customBlockSource, @MachineSizedUInt long offsetToData, @MachineSizedUInt long dataLength, CMBlockBufferFlags flags, CMBlockBuffer.CMBlockBufferPtr newBBufOut);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferCreateWithBufferReference", optional=true)
    private static native OSStatus create(CFAllocator structureAllocator, CMBlockBuffer targetBuffer, @MachineSizedUInt long offsetToData, @MachineSizedUInt long dataLength, CMBlockBufferFlags flags, CMBlockBuffer.CMBlockBufferPtr newBBufOut);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferCreateContiguous", optional=true)
    private static native OSStatus create(CFAllocator structureAllocator, CMBlockBuffer sourceBuffer, CFAllocator blockAllocator, CMBlockBufferCustomBlockSource customBlockSource, @MachineSizedUInt long offsetToData, @MachineSizedUInt long dataLength, CMBlockBufferFlags flags, CMBlockBuffer.CMBlockBufferPtr newBBufOut);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferGetTypeID", optional=true)
    public static native @MachineSizedUInt long getClassTypeID();
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferAppendMemoryBlock", optional=true)
    public native OSStatus appendMemoryBlock(VoidPtr memoryBlock, @MachineSizedUInt long blockLength, CFAllocator blockAllocator, CMBlockBufferCustomBlockSource customBlockSource, @MachineSizedUInt long offsetToData, @MachineSizedUInt long dataLength, CMBlockBufferFlags flags);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferAppendBufferReference", optional=true)
    public native OSStatus appendBufferReference(CMBlockBuffer targetBBuf, @MachineSizedUInt long offsetToData, @MachineSizedUInt long dataLength, CMBlockBufferFlags flags);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferAssureBlockMemory", optional=true)
    public native OSStatus assureBlockMemory();
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferAccessDataBytes", optional=true)
    private static native OSStatus accessDataBytes(CMBlockBuffer theBuffer, @MachineSizedUInt long offset, @MachineSizedUInt long length, VoidPtr temporaryBlock, BytePtr.BytePtrPtr returnedPointer);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferCopyDataBytes", optional=true)
    public native OSStatus copyDataBytes(@MachineSizedUInt long offsetToData, @MachineSizedUInt long dataLength, VoidPtr destination);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferReplaceDataBytes", optional=true)
    private static native OSStatus replaceDataBytes(VoidPtr sourceBytes, CMBlockBuffer destinationBuffer, @MachineSizedUInt long offsetIntoDestination, @MachineSizedUInt long dataLength);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferFillDataBytes", optional=true)
    private static native OSStatus fillDataBytes(byte fillByte, CMBlockBuffer destinationBuffer, @MachineSizedUInt long offsetIntoDestination, @MachineSizedUInt long dataLength);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferGetDataPointer", optional=true)
    public native OSStatus getDataPointer(@MachineSizedUInt long offset, MachineSizedUIntPtr lengthAtOffset, MachineSizedUIntPtr totalLength, BytePtr.BytePtrPtr dataPointer);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferGetDataLength", optional=true)
    public native @MachineSizedUInt long getDataLength();
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferIsRangeContiguous", optional=true)
    public native boolean isRangeContiguous(@MachineSizedUInt long offset, @MachineSizedUInt long length);
    /**
     * @since Available in iOS 4.0 and later.
     */
    @Bridge(symbol="CMBlockBufferIsEmpty", optional=true)
    public native boolean isEmpty();
    /*</methods>*/
}
