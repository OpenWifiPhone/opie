#ifndef __ppm_expander_h
#define __ppm_expander_h

#include "CExpander.h"
#include <sys/stat.h>


#include "utypes.h"
#include "ppm.h"

class PPM_ReadBuf;

#define SYM_EOF 256

class ppm_expander : public CExpander {
  unsigned int bufsize;
  UCHAR *buf_in,*buf_out;
  unsigned int outbytes;
  unsigned long blocksize;
  unsigned short numblocks;
  unsigned short curblock;
  unsigned short maxnode;
  bool needppmend;
  FILE* my_file_in;
  PPM_ReadBuf* my_read_buf;
  ppm_worker ppm;

  int home();
public:
  QString about() { return QString("ppms Codec (c) Tim Wentford\nCompression code (c) Fabrice Bellard"); }
#ifdef USEQPE
	void suspend()
      {
	  CExpander::suspend(my_file_in);
      }
  void unsuspend()
      {
	  CExpander::unsuspend(my_file_in);
      }
#else
      void suspend() {}
      void unsuspend() {}
#endif
  ppm_expander()
      : bufsize(1024)
      , buf_in(new UCHAR[bufsize])
      , buf_out(new UCHAR[bufsize])
      , outbytes(0)
      , blocksize(0)
      , numblocks(0)
      , curblock(0)
      , maxnode(0)
      , needppmend(false)
      , my_file_in(0)
      , my_read_buf(0)
  {}

  virtual ~ppm_expander();
  int OpenFile(const char* infile);
  int getch();
  void locate(unsigned short block, unsigned int n);
  unsigned int locate() { return outbytes; }
  void locate(unsigned int n);
  bool hasrandomaccess() { return (numblocks > 1); }
  void sizes(unsigned long& file, unsigned long& text);
  MarkupType PreferredMarkup() { return cTEXT; }
};

#endif
