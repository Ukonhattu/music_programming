/* soundstruct.h */

#ifndef _SOUNDSTRUCT_H_
#define _SOUNDSTRUCT_H_

typedef struct {
  int magic;
  int dataLocation;
  int dataSize;
  int dataFormat;
  int samplingRate;
  int channelCount;
  char info[4];
} SNDSoundStruct;

#define SND_MAGIC 0x2e736e64

#endif /* _SOUNDSTRUCT_H_ */
