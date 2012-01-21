//
//  SoundAnalyzer.cpp
//  BadTouchCLI
//
//  Created by Alex Laties on 1/20/12.
//  Copyright (c) 2012 University of Pennsylvania. All rights reserved.
//
#include "SoundAnalyzer.h"
#include <ctype.h>
#ifdef __APPLE__
#include <sys/time.h>
#endif

void SoundAnalyzer::ProcessPacket(AudioQueueBufferRef buf) {
  UInt32 val = 0;
  UInt32 *position = (UInt32 *)buf->mAudioData;
  UInt32 totalVals = buf->mAudioDataByteSize / 4;
  for( UInt32 i = 0;  i < totalVals; ++i)
  {
    val = *position;
    if (peaks->empty()) {
      start_peak_time = last_peak_time = 0;
    }
    
    if (last_peak_time - start_peak_time > 500) {
      ProcessPeaks();
    }
    
    if (val > threshold)
    {
      unsigned long time = SoundAnalyzer::GetCurrentTime();
      unsigned long dif = time - last_peak_time;
      if (start_peak_time == 0) {
        start_peak_time = time;
      }
      if (dif >= 100) {
        peaks->push_back(val);
        last_peak_time = time;
      }
    }
    ++position;
  }
}

void SoundAnalyzer::ProcessPeaks() {
  //TODO determining # of peaks/scratching
}


SoundAnalyzer::SoundAnalyzer(UInt32 _threshold, AudioStreamBasicDescription* _format){
  last_peak_time = 0;
  max_amplitude = 0.;
  clip_length = 0.;
  threshold = _threshold;
  format = _format;
  peaks = new std::vector<AUDIO_BLOCK>();
}

unsigned long SoundAnalyzer::GetCurrentTime() {
#ifdef __APPLE__
  timeval current;
  gettimeofday(&current, NULL);
  return current.tv_sec * 1000 + current.tv_usec / 1000;
#endif
#ifdef _WIN32
  //TODO windows get time
#endif
#ifdef __linux__
  //TODO linux get time
#endif
}

SoundAnalyzer::~SoundAnalyzer() {
  delete peaks;
}