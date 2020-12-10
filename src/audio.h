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

#define SND_CHARSEL         0
#define SND_DIE1            1
#define SND_DIE2            2
#define SND_DRMVDRM         3
#define SND_DRUMHRD1        4
#define SND_DRUMHRD2        5
#define SND_DRUMHRD3        6
#define SND_DRUMHRD4        7
#define SND_DRUMHRD5        8
#define SND_DRUMHRD6        9
#define SND_DRUMSFT1        10
#define SND_DRUMSFT2        11
#define SND_E_SPAWN1        12
#define SND_E_SPAWN2        13
#define SND_E_SPAWN3        14
#define SND_E_SPAWN4        15
#define SND_MODESET         14
#define SND_P_SPAWN         15

int ptr_buf[NU_AU_SAMPLE_SIZE];
int sfx_buf[NU_AU_SE_SIZE];

float softSoundPlaying;

void Rom2Ram(void *from_addr, void *to_addr, s32 seq_size);
void initAudio();
void updateAudio(float deltaTime);

void playRandomDrumHard();
void playRandomDrumSoft();
void playRandomDeath();
void playRandomEnemySpawn();


#endif
