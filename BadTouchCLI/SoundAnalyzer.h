//
//  SoundAnalyzer.h
//  BadTouchCLI
//
//  Created by Alex Laties on 1/20/12.
//  Copyright (c) 2012 University of Pennsylvania. All rights reserved.
//

#ifndef BadTouchCLI_SoundAnalyzer_h
#define BadTouchCLI_SoundAnalyzer_h
#include <AudioToolbox/AudioToolbox.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#ifndef AUDIO_BLOCK
#define AUDIO_BLOCK SInt32
#endif

class SoundAnalyzer {
private:
  unsigned long start_peak_time, last_peak_time;
  SInt32 threshold;
  AudioStreamBasicDescription* format;
  std::vector<AUDIO_BLOCK>* peaks;
public:
  void ProcessBuffer(AudioQueueBufferRef buf);
  void ProcessPeaks();
  static unsigned long GetCurrentTime();
  
  SoundAnalyzer(SInt32 threshold, AudioStreamBasicDescription* recordFormat);
  ~SoundAnalyzer();
};


#endif
