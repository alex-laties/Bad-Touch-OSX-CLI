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

void SoundAnalyzer::ProcessBuffer(AudioQueueBufferRef buf) {
  unsigned long t = SoundAnalyzer::GetCurrentTime();
  
  if (t - last_peak_time > 800) {
    ProcessPeaks();
  }
  
  AUDIO_BLOCK val = 0, max = 0;
  AUDIO_BLOCK *position = (AUDIO_BLOCK *)buf->mAudioData;
  AUDIO_BLOCK totalVals = buf->mAudioDataByteSize / sizeof(AUDIO_BLOCK);
  
  for(AUDIO_BLOCK i = 0;  i < totalVals; ++i)
  {
    val = *position;
    
    if (peaks->empty()) {
      start_peak_time = last_peak_time = 0;
    }
    
    if (max < val) {
      max = val;
    }
    
    if (val > threshold)
    {
      unsigned long time = SoundAnalyzer::GetCurrentTime();
      unsigned long dif = time - last_peak_time;
      
      if (start_peak_time == 0) {
        start_peak_time = time;
      }
      
      if (last_peak_time - start_peak_time > 800) {
        ProcessPeaks();
      }
      
      if (dif >= 80) {
        printf("tap at %lu\n", time);
        peaks->push_back(val);
        last_peak_time = time;
      }
    }
    ++position;
  }
}

void SoundAnalyzer::ProcessPeaks() {
  if (peaks->empty()) {
    return;
  }
  
  switch (peaks->size()) {
    case 1:
      printf("1 tap detected\n");
      break;
    case 2:
      printf("2 taps detected\n");
      break;
    case 3:
      printf("3 taps detected\n");
      break;
    default:
      printf("swipe?\n");
      break;
  }
  peaks->clear();
}


SoundAnalyzer::SoundAnalyzer(AUDIO_BLOCK _threshold, AudioStreamBasicDescription* _format){
  last_peak_time = 0;
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