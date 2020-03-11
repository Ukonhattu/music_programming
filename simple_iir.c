#include <stdio.h>
#include <stdlib.h>
#include "sndutil.h"
#include "iir.h"
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

  double sample;

  for (nsamples = 0; getbig16(&iosample, ifp) == 0; nsamples++)
  {
    sample = iosample / 32768.0;
    if (sample < -1.0)
      sample = -1.0;
    else if (sample > 1.0)
      sample = 1.0;

    iosample = iirfilter(sample) * 32768.0;
    if (putbig16(iosample, ofp) < 0)
    {
      fprintf(stderr, "samp: I/O error: "
                      "cannot write sound data to %s\n",
              argv[3]);
      return 1;
    }
  }

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