//
//  main.c
//  BadTouchCLI
//
//  Created by Alex Laties on 1/19/12.
//  Copyright (c) 2012 University of Pennsylvania. All rights reserved.
//
#include <AudioToolbox/AudioToolbox.h>
#include "CAXException.h"
#include <stdio.h>

#define kNumberRecordBuffers  3

typedef struct AudioRecorder{
  AudioQueueRef queue;
  CFAbsoluteTime queueStartStopTime;
  AudioFileID recordFile;
  SInt64  recordPacket;
  Boolean running;
  Boolean verbose;
} AudioRecorder;

// ____________________________________________________________________________________
// Determine the size, in bytes, of a buffer necessary to represent the supplied number
// of seconds of audio data.
static int ComputeRecordBufferSize(const AudioStreamBasicDescription *format, AudioQueueRef queue, float seconds)
{
	int packets, frames, bytes;
	
	frames = (int)ceil(seconds * format->mSampleRate);
	
	if (format->mBytesPerFrame > 0)
		bytes = frames * format->mBytesPerFrame;
	else {
		UInt32 maxPacketSize;
		if (format->mBytesPerPacket > 0)
			maxPacketSize = format->mBytesPerPacket;	// constant packet size
		else {
			UInt32 propertySize = sizeof(maxPacketSize); 
			XThrowIfError(AudioQueueGetProperty(queue, kAudioConverterPropertyMaximumOutputPacketSize, &maxPacketSize,
                                          &propertySize), "couldn't get queue's maximum output packet size");
		}
		if (format->mFramesPerPacket > 0)
			packets = frames / format->mFramesPerPacket;
		else
			packets = frames;	// worst-case scenario: 1 frame in a packet
		if (packets == 0)		// sanity check
			packets = 1;
		bytes = packets * maxPacketSize;
	}
	return bytes;
}

//____________________________________________
//CALLBACK: IMPORTANT THINGS GO HERE
static void PropertyListener(void *userData, AudioQueueRef queue, AudioQueuePropertyID propID){
  AudioRecorder *ar = (AudioRecorder *)userData;
  if(propID == kAudioQueueProperty_IsRunning)
    ar->queueStartStopTime = CFAbsoluteTimeGetCurrent();
}

static void InputBufferHandler( void * inUserData,
                               AudioQueueRef inAQ,
                               AudioQueueBufferRef inBuffer,
                               const AudioTimeStamp * inStartTime,
                               UInt32 inNumPackets,
                               const AudioStreamPacketDescription *inDesc)
{
  AudioRecorder *ar = (AudioRecorder *)inUserData;
  
  try {
    if (ar->verbose) {
      printf("buf data %p, 0x%x bytes, 0x%x packets\n", inBuffer->mAudioData,
             (int)inBuffer->mAudioDataByteSize, (int)inNumPackets);
    }
    
    if (inNumPackets > 0) {
      //TODO pass data to sound processing function
      printf("Got %d packets of sound data!\n", inNumPackets);
      ar->recordPacket += inNumPackets;
    }
    
    if(ar->running)
      XThrowIfError(AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL), "AudioQueueEnqueueBuffer failed!");
  }
  catch (CAXException e) {
    char buf[256];
    fprintf(stderr, "InputBufferHandler: %s (%s)\n", e.mOperation, e.FormatError(buf));
  }
}

//_____________________________________________
//UTILITY FUNCTION
OSStatus GetDefaultInputDeviceSampleRate(Float64 *outSampleRate)
{
	OSStatus err;
	AudioDeviceID deviceID = 0;
  
	// get the default input device
	AudioObjectPropertyAddress addr;
	UInt32 size;
	addr.mSelector = kAudioHardwarePropertyDefaultInputDevice;
	addr.mScope = kAudioObjectPropertyScopeGlobal;
	addr.mElement = 0;
	size = sizeof(AudioDeviceID);
	err = AudioHardwareServiceGetPropertyData(kAudioObjectSystemObject, &addr, 0, NULL, &size, &deviceID);
	if (err) return err;
  
	// get its sample rate
	addr.mSelector = kAudioDevicePropertyNominalSampleRate;
	addr.mScope = kAudioObjectPropertyScopeGlobal;
	addr.mElement = 0;
	size = sizeof(Float64);
	err = AudioHardwareServiceGetPropertyData(deviceID, &addr, 0, NULL, &size, outSampleRate);
  
	return err;
}

int main (int argc, const char * argv[])
{
  int i, bufferByteSize;
  AudioStreamBasicDescription recordFormat;
  AudioRecorder ar;
  UInt32 size;
  
  memset(&recordFormat, 0, sizeof(recordFormat));
  memset(&ar, 0, sizeof(ar));
  
  if (recordFormat.mSampleRate == 0.)
    GetDefaultInputDeviceSampleRate(&recordFormat.mSampleRate);
  
  if (recordFormat.mChannelsPerFrame == 0)
    recordFormat.mChannelsPerFrame = 2;
  
  recordFormat.mFormatID = kAudioFormatLinearPCM;
  recordFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
  recordFormat.mBitsPerChannel = 16;
  recordFormat.mBytesPerPacket = recordFormat.mBytesPerFrame = 
    (recordFormat.mBitsPerChannel / 8) * recordFormat.mChannelsPerFrame;
  recordFormat.mFramesPerPacket = 1;
  recordFormat.mReserved = 0;
  
  try {
    XThrowIfError(AudioQueueNewInput(
                                     &recordFormat, 
                                     InputBufferHandler,
                                     &ar,
                                     NULL,
                                     NULL,
                                     0,
                                     &ar.queue), "AudioQueueNewInputFailed");
    size = sizeof(recordFormat);
    XThrowIfError(AudioQueueGetProperty(
                                        ar.queue, 
                                        kAudioConverterCurrentOutputStreamDescription, 
                                        &recordFormat, 
                                        &size), "couldn't get queue's format");
    bufferByteSize = ComputeRecordBufferSize(&recordFormat, ar.queue, 0.5);
    for(i = 0; i < kNumberRecordBuffers; ++i) {
      AudioQueueBufferRef buffer;
      XThrowIfError(AudioQueueAllocateBuffer(ar.queue, bufferByteSize, &buffer), "AudioQueueAllocateBuffer failed!");
      XThrowIfError(AudioQueueEnqueueBuffer(ar.queue, buffer, 0, NULL), "AudioQueueEnqueueBuffer failed!");
    }
    
    //start listening
    ar.running = TRUE;
    XThrowIfError(AudioQueueStart(ar.queue, NULL), "AudioQueueStart failed");
    printf("Listening... (press <return> to to stop):\n");
    getchar();
    
    printf("Cleaning up...\n");
    ar.running = FALSE;
    XThrowIfError(AudioQueueStop(ar.queue, TRUE), "AudioQueueStop failed");
    
  cleanup:
    AudioQueueDispose(ar.queue, TRUE);
  }
  catch (CAXException e)
  {
    char buf[256];
    fprintf(stderr, "InputBufferHandler: %s (%s)\n", e.mOperation, e.FormatError(buf));
    return e.mError;
  }
  
  printf("Done!\n");
  return 0;
}

