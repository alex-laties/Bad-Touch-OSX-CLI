//
//  SoundAnalyzer.cpp
//  BadTouchCLI
//
//  Created by Alex Laties on 1/20/12.
//  Copyright (c) 2012 University of Pennsylvania. All rights reserved.
//
#include "SoundAnalyzer.h"

void SoundAnalyzer::ProcessPacket(AudioQueueBufferRef buf) {
  //TODO something
}

SoundAnalyzer::SoundAnalyzer(){
  position = 0;
  last_peak = 0;
  num_peaks = 0;
  max_amplitude = 0.;
  time_since_last = 0.;
  clip_length = 0.;
}

