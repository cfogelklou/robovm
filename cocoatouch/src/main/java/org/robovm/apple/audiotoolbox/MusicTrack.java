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
package org.robovm.apple.audiotoolbox;

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
import org.robovm.apple.coregraphics.*;
import org.robovm.apple.opengles.*;
import org.robovm.apple.audiounit.*;
import org.robovm.apple.coreaudio.*;
import org.robovm.apple.coremedia.*;
/*</imports>*/

/*<javadoc>*/
/*</javadoc>*/
/*<annotations>*/@Library("AudioToolbox")/*</annotations>*/
/*<visibility>*/public/*</visibility>*/ class /*<name>*/MusicTrack/*</name>*/ 
    extends /*<extends>*/NativeObject/*</extends>*/ 
    /*<implements>*//*</implements>*/ {

    /*<ptr>*/public static class MusicTrackPtr extends Ptr<MusicTrack, MusicTrackPtr> {}/*</ptr>*/
    /*<bind>*/static { Bro.bind(MusicTrack.class); }/*</bind>*/
    /*<constants>*//*</constants>*/
    /*<constructors>*/
    protected MusicTrack() {}
    /*</constructors>*/
    /*<properties>*//*</properties>*/
    /*<members>*//*</members>*/
    /*<methods>*/
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackGetSequence", optional=true)
    public native OSStatus getSequence(MusicSequence.MusicSequencePtr outSequence);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackSetDestNode", optional=true)
    public native OSStatus setDestNode(int inNode);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackSetDestMIDIEndpoint", optional=true)
    public native OSStatus setDestMIDIEndpoint(int inEndpoint);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackGetDestNode", optional=true)
    public native OSStatus getDestNode(IntPtr outNode);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackGetDestMIDIEndpoint", optional=true)
    public native OSStatus getDestMIDIEndpoint(IntPtr outEndpoint);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackSetProperty", optional=true)
    public native OSStatus setProperty(int inPropertyID, VoidPtr inData, int inLength);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackGetProperty", optional=true)
    public native OSStatus getProperty(int inPropertyID, VoidPtr outData, IntPtr ioLength);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackMoveEvents", optional=true)
    public native OSStatus moveEvents(double inStartTime, double inEndTime, double inMoveTime);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackClear", optional=true)
    public native OSStatus clear(double inStartTime, double inEndTime);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackCut", optional=true)
    public native OSStatus cut(double inStartTime, double inEndTime);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackCopyInsert", optional=true)
    public native OSStatus copyInsert(double inSourceStartTime, double inSourceEndTime, MusicTrack inDestTrack, double inDestInsertTime);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackMerge", optional=true)
    public native OSStatus merge(double inSourceStartTime, double inSourceEndTime, MusicTrack inDestTrack, double inDestInsertTime);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackNewMIDINoteEvent", optional=true)
    public native OSStatus newMIDINoteEvent(double inTimeStamp, MIDINoteMessage inMessage);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackNewMIDIChannelEvent", optional=true)
    public native OSStatus newMIDIChannelEvent(double inTimeStamp, MIDIChannelMessage inMessage);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackNewMIDIRawDataEvent", optional=true)
    public native OSStatus newMIDIRawDataEvent(double inTimeStamp, MIDIRawData inRawData);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackNewExtendedNoteEvent", optional=true)
    public native OSStatus newExtendedNoteEvent(double inTimeStamp, ExtendedNoteOnEvent inInfo);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackNewParameterEvent", optional=true)
    public native OSStatus newParameterEvent(double inTimeStamp, ParameterEvent inInfo);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackNewExtendedTempoEvent", optional=true)
    public native OSStatus newExtendedTempoEvent(double inTimeStamp, double inBPM);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackNewMetaEvent", optional=true)
    public native OSStatus newMetaEvent(double inTimeStamp, MIDIMetaEvent inMetaEvent);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackNewUserEvent", optional=true)
    public native OSStatus newUserEvent(double inTimeStamp, MusicEventUserData inUserData);
    /**
     * @since Available in iOS 5.0 and later.
     */
    @Bridge(symbol="MusicTrackNewAUPresetEvent", optional=true)
    public native OSStatus newAUPresetEvent(double inTimeStamp, AUPresetEvent inPresetEvent);
    /*</methods>*/
}
