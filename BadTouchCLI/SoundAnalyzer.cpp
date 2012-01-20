//
//  SoundAnalyzer.cpp
//  BadTouchCLI
//
//  Created by Alex Laties on 1/20/12.
//  Copyright (c) 2012 University of Pennsylvania. All rights reserved.
//
#include "SoundAnalyzer.h"
#include <ctype.h>

void SoundAnalyzer::ProcessPacket(AudioQueueBufferRef buf) {
  UInt32 val;
  UInt32 *position = (UInt32 *)buf->mAudioData;
  UInt32 totalVals = sizeof(buf->mAudioData) / buf->mAudioDataByteSize;
  for( UInt32 i = 0;  i < totalVals; ++i)
  {
    val = *position;
    if (val > threshold)
    {
      
    }
    ++position;
  }
}

SoundAnalyzer::SoundAnalyzer(UInt32 _threshold){
  position = 0;
  last_peak = 0;
  num_peaks = 0;
  max_amplitude = 0.;
  time_since_last = 0.;
  clip_length = 0.;
  threshold = _threshold;
}

