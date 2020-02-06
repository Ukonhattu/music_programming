#include <stdio.h>
#include <stdlib.h>
#include "sndutil.h"
#include "fir.h"
#define SAMPLESIZE 80
#define FILTER_LEN 63
int main(int argc, char *argv[])
{
  SNDSoundStruct s;
  FILE *ifp, *ofp, *fopen();
  short iosample;
  int i, nsamples;

  if (argc != 3)
  {
    fprintf(stderr, "Usage: source dest\n");
    return 1;
  }

  /*
   * Open source file and read soundfile header
   */
  ifp = fopen(argv[1], "r");
  if (ifp == NULL)
  {
    fprintf(stderr, "samp: cannot open %s\n", argv[2]);
    return 1;
  }

  if (getsndheader(&s, ifp) < 0)
  {
    fprintf(stderr, "samp: I/O error: "
                    "cannot read soundfile header from %s\n",
            argv[2]);
    return 1;
  }

  if (s.magic != SND_MAGIC)
  {
    fprintf(stderr, "samp: %s: not a SUN/NeXT soundfile\n", argv[2]);
    return 1;
  }
  if (s.dataFormat != 3)
  {
    fprintf(stderr, "samp: unsupported data format: %d\n", s.dataFormat);
    return 1;
  }

  /*
   * Set info to zero length
   */
  s.dataLocation = sizeof(SNDSoundStruct);
  for (i = 0; i < 4; i++)
    s.info[i] = '\0';

  /*
   * Write soundfile header
   */
  ofp = fopen(argv[2], "w");
  if (ofp == NULL)
  {
    fprintf(stderr, "samp: cannot open %s\n", argv[2]);
    return 1;
  }
  if (putsndheader(&s, ofp) < 0)
  {
    fprintf(stderr, "samp: I/O error: "
                    "cannot write soundfile header to %s\n",
            argv[3]);
    return 1;
  }

  /*
   * Signal processing section
   */

  /* placeholder */

  double *inputbuffer = calloc(SAMPLESIZE, sizeof(double));
  double *outputbuffer = calloc(SAMPLESIZE, sizeof(double));
  int j;
  double sample;

  double coeffs[FILTER_LEN] =
      {
          -0.0448093, 0.0322875, 0.0181163, 0.0087615, 0.0056797,
          0.0086685, 0.0148049, 0.0187190, 0.0151019, 0.0027594,
          -0.0132676, -0.0232561, -0.0187804, 0.0006382, 0.0250536,
          0.0387214, 0.0299817, 0.0002609, -0.0345546, -0.0525282,
          -0.0395620, 0.0000246, 0.0440998, 0.0651867, 0.0479110,
          0.0000135, -0.0508558, -0.0736313, -0.0529380, -0.0000709,
          0.0540186, 0.0766746, 0.0540186, -0.0000709, -0.0529380,
          -0.0736313, -0.0508558, 0.0000135, 0.0479110, 0.0651867,
          0.0440998, 0.0000246, -0.0395620, -0.0525282, -0.0345546,
          0.0002609, 0.0299817, 0.0387214, 0.0250536, 0.0006382,
          -0.0187804, -0.0232561, -0.0132676, 0.0027594, 0.0151019,
          0.0187190, 0.0148049, 0.0086685, 0.0056797, 0.0087615,
          0.0181163, 0.0322875, -0.0448093};
  nsamples = 0;
  firFloatInit();
  while (1)
  {
    for (i = 0; getbig16(&iosample, ifp) == 0 && i < SAMPLESIZE; nsamples++ && i++) /* nice hack m8 */
    {
      sample = iosample / 32768.0;
      if (sample < -1.0)
        sample = -1.0;
      else if (sample > 1.0)
        sample = 1.0;
      inputbuffer[i] = sample;
    }
    if (firFloat(coeffs, inputbuffer, outputbuffer, SAMPLESIZE - (SAMPLESIZE - i), FILTER_LEN) == 1)
      return 1;
    for (j = 0; j < SAMPLESIZE - (SAMPLESIZE - i); j++)
    {
      iosample = sample * 32768.0;
      if (putbig16(iosample, ofp) < 0)
      {
        fprintf(stderr, "samp: I/O error: "
                        "cannot write sound data to %s\n",
                argv[3]);
        return 1;
      }
    }
    if (i < SAMPLESIZE)
      break;
  }
  firFloatFree();
  free(inputbuffer);
  free(outputbuffer);

  if (ferror(ifp))
  {
    fprintf(stderr, "samp: I/O error: "
                    "cannot read sound data from %s\n",
            argv[3]);
    return 1;
  }

  /*
   * Update the soundfile header of the destination file
   */
  s.dataSize = nsamples * 2;
  if (updatesndheader(&s, ofp) < 0)
  {
    fprintf(stderr, "samp: write error: "
                    "cannot update soundfile header on %s\n",
            argv[3]);
    return 1;
  }

  /*
   * Close files and return with "no error".
   */
  fclose(ofp);
  fclose(ifp);

  return 0;
}