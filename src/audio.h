#ifndef AUDIO_H
#define AUDIO_H

#include <nusys.h>
#include <ultra64.h>
#include <nualstl_n.h>

extern u8 _bank_sfx_wbankSegmentRomStart[];
extern u8 _bank_sfx_wbankSegmentRomEnd[];
extern u8 _bank_sfx_pbankSegmentRomStart[];
extern u8 _bank_sfx_pbankSegmentRomEnd[];
extern u8 _bank_sfx_sfxSegmentRomStart[];
extern u8 _bank_sfx_sfxSegmentRomEnd[];

#define WBANK_START _bank_sfx_wbankSegmentRomStart
#define WBANK_END   _bank_sfx_wbankSegmentRomEnd
#define PBANK_START _bank_sfx_pbankSegmentRomStart
#define PBANK_END   _bank_sfx_pbankSegmentRomEnd
#define SFX_START   _bank_sfx_sfxSegmentRomStart
#define SFX_END     _bank_sfx_sfxSegmentRomEnd

static musHandle sndHandle = 0;

#define SND_DRUM        0
#define SND_MUSIC       1

int ptr_buf[NU_AU_SAMPLE_SIZE];
int sfx_buf[NU_AU_SE_SIZE];

void Rom2Ram(void *from_addr, void *to_addr, s32 seq_size);
void initAudio(void);


#endif
