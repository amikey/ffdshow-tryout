#ifndef _TFFPICT_H_
#define _TFFPICT_H_

#include "TffRect.h"
#include "ffImgfmt.h"
#include "rational.h"
#include "interfaces.h"
//#include "ffmpeg/libavcodec/avcodec.h"

struct FRAME_TYPE {
    static int fromSample(IMediaSample *pIn);
    enum {
        UNKNOWN=0,
        I    =1,
        P    =2,
        B    =3,
        GMC  =4,
        SKIP =5,
        PAD  =6,
        DELAY=7,
        SI   =8,
        SP   =9,
        typemask=15,
        QPEL=16
    };
    static const char_t *name(int type) {
        switch (type) {
            case P:
                return _l("p");
            case I:
                return _l("i");
            case B:
                return _l("b");
            case GMC:
                return _l("s");
            case SKIP:
                return _l("skipped");
            case PAD:
                return _l("padded");
            case DELAY:
                return _l("delayed");
            case SI:
                return _l("si");
            case SP:
                return _l("sp");
            case UNKNOWN:
            default:
                return _l("unknown");
        }
    }
};

struct FIELD_TYPE {
    static int fromSample(IMediaSample *pIn,bool isInterlacedRawVideo);
    enum {
        PROGRESSIVE_FRAME=1,
        PROGRESSIVE_TOP=2,
        PROGRESSIVE_BOTTOM=4,
        MASK_PROG=PROGRESSIVE_FRAME+PROGRESSIVE_TOP+PROGRESSIVE_BOTTOM,
        INT_TFF=8,
        INT_BFF=16,
        MASK_INT=INT_TFF+INT_BFF,
        MASK_INT_PROG=MASK_PROG+MASK_INT,
        SEQ_START=32,
        SEQ_END=64,
        MASK_SEQ=SEQ_START+SEQ_END
    };
};

enum AVColorRange;
struct Tpalette {
    Tpalette(void):pal(NULL),numcolors(0) {}
    Tpalette(const unsigned char *Ipal,unsigned int Inumcolors):pal(Ipal),numcolors(Inumcolors) {}
    Tpalette(const RGBQUAD *Ipal,unsigned int Inumcolors):pal((const unsigned char*)Ipal),numcolors(Inumcolors) {}
    Tpalette(const uint32_t *palette);
    const unsigned char *pal;
    unsigned int numcolors;
};

struct TffPictBase {
    TffPictBase(void) {
        csp=0;
    }
    TffPictBase(unsigned int Idx,unsigned int Idy);
    uint64_t csp;
    Trect rectFull,rectClip;
    stride_t diff[4];
    Trect getRect(int full,int half) const;
    void setSize(unsigned int Idx,unsigned int Idy);
    void setDar(const Rational &dar),setSar(const Rational &sar);
};

class Tconvert;
struct TffPict :TffPictBase {
private:
    void clear(const Trect &r, unsigned int plane, int brightness=0, int blackcutoff=0);
    static void asm_BitBlt_MMX(BYTE* dstp, stride_t dst_pitch, const BYTE* srcp, stride_t src_pitch, int row_size, int height);
    static void asm_BitBlt_C(BYTE* dstp, stride_t dst_pitch, const BYTE* srcp, stride_t src_pitch, int row_size, int height);
    void copyRect(const TffPict &src,const Trect &r,unsigned int plane);
    void common_init(void);
    void init(void);
    void init(uint64_t Icsp,unsigned char *Idata[4],const stride_t Istride[4],const Trect &r,bool Iro,int Iframetype,int Ifieldtype,size_t IsrcSize,const Tpalette &Ipalette);
    static void calcDiff(const TcspInfo &cspInfo,const Trect &rectClip,const stride_t stride[4],stride_t diff[4]);
    void readLibavcodec(uint64_t Icsp,const char_t *flnm,const char_t *ext,Tbuffer &buf,IffdshowBase *deci);
    void readOle(uint64_t Icsp,const char_t *flnm,const char_t *ext,Tbuffer &buf,IffdshowBase *deci);
    unsigned int edge;
    static void draw_edges(uint8_t *buf, stride_t wrap, int width, int height, int w);
public:
    TffPict(void);
    TffPict(uint64_t Icsp,unsigned char *data[4],const stride_t stride[4],const Trect &r,bool ro,int Iframetype,int Ifieldtype,size_t IsrcSize,IMediaSample *pIn,const Tpalette &Ipalette=Tpalette());
    TffPict(uint64_t Icsp,unsigned char *data[4],const stride_t stride[4],const Trect &r,bool ro,IMediaSample *pIn,const Tpalette &Ipalette=Tpalette(),bool isInterlacedRawVideo=false);
    TffPict(uint64_t Icsp,const char_t *flnm,Tbuffer &buf,IffdshowBase *deci);
    void setTimestamps(IMediaSample *pIn);
    void setDiscontinuity(IMediaSample *pIn);
    void setFullRange(AVColorRange color_range);
    ~TffPict() {}

    TcspInfo cspInfo;
    int frametype,fieldtype;
    bool film,repeat_first_field;
    bool discontinuity;
    int video_full_range_flag;
    enum AVColorSpace YCbCr_RGB_matrix_coefficients;
    REFERENCE_TIME rtStart,rtStop;
    LONGLONG mediatimeStart,mediatimeStop;
    int gmcWarpingPoints,gmcWarpingPointsReal;
    size_t srcSize;
#ifdef OSD_H264POC
    int h264_poc;
#endif

    unsigned char *data[4];
    bool ro[4];
    void setRO(bool Iro);
    stride_t stride[4];
    void calcDiff(void);
    Trect rectEdge;
    unsigned char *edgeData[4];
    Tpalette palette;

    static const unsigned int PLANE_ALL=UINT_MAX;
    void clear(int full,unsigned int plane=PLANE_ALL);
    void clear(int Bpp,unsigned int black,unsigned char *dst,stride_t stride,unsigned int row_size,unsigned int height,int brightness, int blackcutoff);
    static void clear(int Bpp,unsigned int black,unsigned char *dst,stride_t stride,unsigned int row_size,unsigned int height);
    void clearBorder(int brightness, int blackcutoff);
    void copyBorder(const TffPict &src,unsigned int plane);
    void histogram(unsigned int histogram[256],int full,int half) const;
    void copyFrom(const TffPict &p,Tbuffer &buf,const Trect *rectCopy=NULL);
    static void copy(unsigned char *dst, stride_t dstStride, const unsigned char *src, stride_t srcStride, int bytesPerLine, int height, bool flip=false);
    void convertCSP(uint64_t Icsp,Tbuffer &buf,Tconvert *convert,int edge=0);
    void convertCSP(uint64_t Icsp,Tbuffer &buf,int edge=0);
    void setCSP(uint64_t Icsp);
    void alloc(unsigned int dx,unsigned int dy,uint64_t Icsp,Tbuffer &buf,int edge=0);
    void createEdge(unsigned int edge,Tbuffer &buf);
    void md5sum(uint8_t sum[16]) const;
};

// called from TffPict::convertCSP(uint64_t Icsp,Tbuffer &buf,int edge) and image filters that have to be compatible with it.
static inline stride_t get_stride_YUV_planar(const TcspInfo &cspInfo, unsigned int dx, int plane, int edge)
{
    return (((cspInfo.Bpp * dx>>cspInfo.shiftX[plane])+edge)/16+2)*16; 
}
#endif
