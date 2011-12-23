#ifndef _TVIDEOCODECWMV9_H_
#define _TVIDEOCODECWMV9_H_

#include "TvideoCodec.h"

class Tdll;
struct Tff_wmv9codecInfo;
struct Iff_wmv9;
class TvideoCodecWmv9 :public TvideoCodecDec
{
private:
    void create(void);
    Tdll *dll;
public:
    TvideoCodecWmv9(IffdshowBase *Ideci,IdecVideoSink *IsinkD);
    virtual ~TvideoCodecWmv9();
    static const char_t *dllname;
private:
    Tff_wmv9codecInfo *infos;
    Iff_wmv9 *wmv9;
    Iff_wmv9* (__stdcall *createWmv9)(void);
    void (__stdcall *destroyWmv9)(Iff_wmv9 *self);
    uint64_t csp;
    Trect rd;
    int codecIndex;
    mutable char_t codecName[100];
protected:
    virtual bool beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags);
public:
    virtual int getType(void) const {
        return IDFF_MOVIE_WMV9;
    }
    virtual const char_t* getName(void) const;

    virtual bool supExtradata(void) {
        return true;
    }
    virtual bool getExtradata(const void* *ptr,size_t *len);

    virtual bool testMediaType(FOURCC fcc,const CMediaType &mt);
    virtual HRESULT decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn);
};

#endif
