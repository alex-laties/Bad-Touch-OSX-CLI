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

class SoundAnalyzer {
private:
  int position;
  int last_peak;
  int num_peaks;
  float max_amplitude;
  float time_since_last;
  float clip_length;
public:
  void ProcessPacket(AudioQueueBufferRef buf);
  
  SoundAnalyzer();
  
};


#endif
