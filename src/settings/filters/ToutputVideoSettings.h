#ifndef _TOUTPUTVIDEOSETTINGS_H_
#define _TOUTPUTVIDEOSETTINGS_H_

#include "TfilterSettings.h"

#include "ffImgfmt.h"
struct ToutputVideoSettings :TfilterSettingsVideo {
private:
    static const TfilterIDFF idffs;
protected:
    const int* getResets(unsigned int pageId);
    virtual int getDefault(int id);
public:
    ToutputVideoSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);

    int yv12,yuy2,uyvy,nv12,rgb32,rgb24,p016;
    int dv,dvNorm;
    void getDVsize(unsigned int *dx,unsigned int *dy) const;
    int get_cspOptionsBlackCutoff(int video_full_range_flag) const;
    int get_cspOptionsWhiteCutoff(int video_full_range_flag) const;
    int get_cspOptionsChromaCutoff(int video_full_range_flag) const;
    int brightness2luma(int brightness, int video_full_range_flag) const;
    // int luma2brightness(int luma) const;
    static int get_cspOptionsChromaCutoffStatic(int blackCutoff, int whiteCutoff, int chromaCutoff);
    static const char_t *dvNorms[];
    static const char_t *deintMethods[];
    static const char_t *deintFieldOrder[];
    static const char_t *rgbInterlaceMethods[];

    struct ToutputColorspace {
        ToutputColorspace(DWORD IbiCompression,const GUID *Ig,WORD IbiBitCount,WORD IbiPlanes):biCompression(IbiCompression),g(Ig),biBitCount(IbiBitCount),biPlanes(IbiPlanes) {}
        DWORD biCompression;
        const GUID *g;
        WORD biBitCount,biPlanes;
    };
    void getOutputColorspaces(ints &ocsps),getOutputColorspaces(TcspInfos &ocsps);
    int hwOverlayAspect;
    int hwDeinterlace,hwDeintMethod, hwDeintFieldOrder;
    int highQualityRGB,dithering;
    int cspOptionsIturBt, cspOptionsInputLevelsMode;
    int cspOptionsBlackCutoff, cspOptionsWhiteCutoff, cspOptionsChromaCutoff;
    int cspOptionsRgbInterlaceMode;
    int cspOptionsOutputLevelsMode;
    int flip;

    void reg_op_outcsps(TregOp &t);

    virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const {}
    virtual void createPages(TffdshowPageDec *parent) const {}
};

struct ToutputVideoConvertSettings :TfilterSettingsVideo {
    ToutputVideoConvertSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL):TfilterSettingsVideo(sizeof(*this),Icoll,filters,NULL,false) {}

    uint64_t csp;

    virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const {}
    virtual void createPages(TffdshowPageDec *parent) const {}
};

#endif
