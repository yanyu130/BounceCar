#ifndef AUDIO_H
#define AUDIO_H

#include "stdint.h"

#define AUDIO_WELCOME 2
#define AUDIO_GOODBYE 3

void Audio_Init(void);
void AudioPlay(uint8_t i);

#endif
