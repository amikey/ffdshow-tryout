/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Tsubreader.h"
#include "TsubtitleText.h"
#include "Tfont.h"
#include "TfontSettings.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "TfontManager.h"
#include "simd.h"
#include "Tconfig.h"
#include "ffdebug.h"
#include "postproc/swscale.h"
#include "Tlibmplayer.h"
#include <mbstring.h>
#pragma warning(disable:4244)

//============================ The matrix for outline =============================
static const short matrix0[]={
 000,000,000,000,000,
 000,100,200,100,000, 
 000,200,800,200,000,
 000,100,200,100,000, 
 000,000,000,000,000
};

static const short matrix0RGB[]={
 000,000,000,000,000,
 000,000,000,000,000, 
 000,000,000,000,000,
 000,000,000,000,000, 
 000,000,000,000,000
};

static const short matrix1[]={
 000,000,000,000,000,
 000,200,500,200,000, 
 000,500,800,500,000,
 000,200,500,200,000, 
 000,000,000,000,000
};

static const short matrix2[]={
 000,200,300,200,000,
 200,400,500,400,200,
 300,500,800,500,300,
 200,400,500,400,200,
 000,200,300,200,000
};

static const short matrix3[]={
 800,800,800,800,800,
 800,800,800,800,800,
 800,800,800,800,800,
 800,800,800,800,800,
 800,800,800,800,800
};

//============================ TrenderedSubtitleWordBase =============================
TrenderedSubtitleWordBase::~TrenderedSubtitleWordBase()
{
 if (own)
  for (int i=0;i<3;i++)
   {
    aligned_free(bmp[i]);
    aligned_free(msk[i]);
    aligned_free(outline[i]);
    aligned_free(shadow[i]);
   }
}

//============================== TrenderedSubtitleWord ===============================

// full rendering
template<class tchar> TrenderedSubtitleWord::TrenderedSubtitleWord(
                       HDC hdc,
                       const tchar *s0,
                       size_t strlens,
                       const short (*matrix)[5],
                       const YUVcolorA &YUV,
                       const YUVcolorA &outlineYUV,
                       const YUVcolorA &shadowYUV,
                       const TrenderedSubtitleLines::TprintPrefs &prefs,
                       int xscale):
 TrenderedSubtitleWordBase(true),
 shiftChroma(true)
{
 csp=prefs.csp;
 typedef typename tchar_traits<tchar>::ffstring ffstring;
 typedef typename tchar_traits<tchar>::strings strings;
 strings s1;
 strtok(ffstring(s0,strlens).c_str(),_L("\t"),s1);
 SIZE sz;sz.cx=sz.cy=0;ints cxs;
 for (typename strings::iterator s=s1.begin();s!=s1.end();s++)
  {
   SIZE sz0;
   prefs.config->getGDI<tchar>().getTextExtentPoint32(hdc,s->c_str(),(int)s->size(),&sz0);
   sz.cx+=sz0.cx;
   if (s+1!=s1.end())
    {
     int tabsize=prefs.tabsize*sz0.cy;
     int newpos=(sz.cx/tabsize+1)*tabsize;
     sz0.cx+=newpos-sz.cx;
     sz.cx=newpos;
    }
   cxs.push_back(sz0.cx);
   sz.cy=std::max(sz.cy,sz0.cy);
  }
 OUTLINETEXTMETRIC otm;
 GetOutlineTextMetrics(hdc,sizeof(otm),&otm);
 unsigned int shadowSize = getShadowSize(prefs,otm.otmTextMetrics.tmHeight);
 SIZE sz1=sz;
 if (otm.otmItalicAngle)
  sz1.cx-=LONG(sz1.cy*sin(otm.otmItalicAngle*M_PI/1800));
 else
  if (otm.otmTextMetrics.tmItalic)
   sz1.cx+=sz1.cy*0.35;
 dx[0]=(sz1.cx/4+2)*4;dy[0]=sz1.cy+4;
 unsigned char *bmp16=(unsigned char*)calloc(dx[0]*4,dy[0]);
 HBITMAP hbmp=CreateCompatibleBitmap(hdc,dx[0],dy[0]);
 HGDIOBJ old=SelectObject(hdc,hbmp);
 RECT r={0,0,dx[0],dy[0]};
 FillRect(hdc,&r,(HBRUSH)GetStockObject(BLACK_BRUSH));
 SetTextColor(hdc,RGB(255,255,255));
 SetBkColor(hdc,RGB(0,0,0));
 int x=2;
 ints::const_iterator cx=cxs.begin();
 for (typename strings::const_iterator s=s1.begin();s!=s1.end();s++,cx++)
  {
   const char *t=(const char *)s->c_str();
   int sz=(int)s->size();
   prefs.config->getGDI<tchar>().textOut(hdc,x,2,s->c_str(),sz/*(int)s->size()*/);
   x+=*cx;
  }
 drawShadow(hdc,hbmp,bmp16,old,xscale,sz,prefs,matrix,YUV,outlineYUV,shadowYUV,shadowSize);
}
extern "C"  DWORD maskffff;
void TrenderedSubtitleWord::drawShadow(
      HDC hdc,
      HBITMAP hbmp,
      unsigned char *bmp16,
      HGDIOBJ old,
      int xscale,
      const SIZE &sz,
      const TrenderedSubtitleLines::TprintPrefs &prefs,
      const short (*matrix)[5],
      const YUVcolorA &yuv,
      const YUVcolorA &outlineYUV,
      const YUVcolorA &shadowYUV,
      unsigned int shadowSize)
{
 m_bodyYUV=yuv;
 m_outlineYUV=outlineYUV;
 m_shadowYUV=shadowYUV;
 BITMAPINFO bmi;
 bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
 bmi.bmiHeader.biWidth=dx[0];
 bmi.bmiHeader.biHeight=-1*dy[0];
 bmi.bmiHeader.biPlanes=1;
 bmi.bmiHeader.biBitCount=32;
 bmi.bmiHeader.biCompression=BI_RGB;
 bmi.bmiHeader.biSizeImage=dx[0]*dy[0];
 bmi.bmiHeader.biXPelsPerMeter=75;
 bmi.bmiHeader.biYPelsPerMeter=75;
 bmi.bmiHeader.biClrUsed=0;
 bmi.bmiHeader.biClrImportant=0;
 GetDIBits(hdc,hbmp,0,dy[0],bmp16,&bmi,DIB_RGB_COLORS);  // copy bitmap, get it in bmp16
 SelectObject(hdc,old);
 DeleteObject(hbmp);

 if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
  {
   YV12_lum2chr_min=YV12_lum2chr_min_mmx2;
   YV12_lum2chr_max=YV12_lum2chr_max_mmx2;
  }
 else
  {
   YV12_lum2chr_min=YV12_lum2chr_min_mmx;
   YV12_lum2chr_max=YV12_lum2chr_max_mmx;
  }
#ifndef WIN64
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  {
#endif
   alignXsize=16;
   TrenderedSubtitleWord_printY=TrenderedSubtitleWord_printY_sse2;
   TrenderedSubtitleWord_printUV=TrenderedSubtitleWord_printUV_sse2;
#ifndef WIN64
  }
 else
  {
   alignXsize=8;
   TrenderedSubtitleWord_printY=TrenderedSubtitleWord_printY_mmx;
   TrenderedSubtitleWord_printUV=TrenderedSubtitleWord_printUV_mmx;
  }
#endif
 unsigned int _dx,_dy;
 _dx=(xscale*dx[0]/100)/4+4+shadowSize;
 _dy=dy[0]/4+4+shadowSize;
 dxCharY=xscale*sz.cx/400;dyCharY=sz.cy/4;
 unsigned int al=prefs.csp==FF_CSP_420P ? alignXsize : alignXsize/4;
 unsigned int _dxtemp=(_dx/al)*al;
 _dx=_dxtemp<_dx ? _dxtemp+al : _dxtemp;
 bmp[0]=(unsigned char*)aligned_calloc3(_dx,_dy,4,16);
 msk[0]=(unsigned char*)aligned_calloc3(_dx,_dy,4,16);
 outline[0]=(unsigned char*)aligned_calloc3(_dx,_dy,4,16);
 shadow[0]=(unsigned char*)aligned_calloc3(_dx,_dy,4,16);
 int dxCharYstart;
 switch (prefs.alignSSA)
  {
   case 1: // left(SSA)
   case 5:
   case 9:
    dxCharYstart=0;
    break;
   case 2: // center(SSA)
   case 6:
   case 10:
    dxCharYstart=(_dx-((sz.cx+3)/4+2+shadowSize)*xscale/100)/2;
    break;
   case 3: // right(SSA)
   case 7:
   case 11:
    dxCharYstart=_dx-((sz.cx+3)/4+2+shadowSize)*xscale/100;
    break;
   case -1:
   default:
    dxCharYstart=((xscale==100 && prefs.align!=ALIGN_LEFT)?(_dx-dx[0]/4)/2:0);
    break;
  }
 if (dxCharYstart<0) dxCharYstart=0;
 for (unsigned int y=2;y<dy[0]-2;y+=4)
    {
   unsigned char *dstBmpY=bmp[0]+(y/4+2)*_dx+2+dxCharYstart;
   for (unsigned int xstep=xscale==100?4*65536:400*65536/xscale,x=(2<<16)+xstep;x<((dx[0]-2)<<16);x+=xstep,dstBmpY++)
    {
     unsigned int sum=0;
     for (const unsigned char *bmp16src=bmp16+((y-2)*dx[0]+((x>>16)-2))*4,*bmp16srcEnd=bmp16src+5*dx[0]*4;bmp16src!=bmp16srcEnd;bmp16src+=dx[0]*4)
      {
       for (int i=0;i<=12;i+=4)
         sum+=bmp16src[i];
      }
     sum/=20; // average of 5x4=20pixels
     *dstBmpY=(unsigned char)sum;
    }
  }
 free(bmp16);

 if (prefs.blur || (prefs.shadowMode==0 && shadowSize>0))
  {
   // Blur characters. Edge gets more smooth, outline gets thicker.
   // This code is better adapted to bigger characters.
   if (prefs.deci)
    {
     unsigned char *blured_bmp=(unsigned char*)aligned_calloc(_dx,_dy,16);
     Tlibmplayer *libmplayer;
     SwsFilter filter;
     SwsParams params;
     prefs.deci->getPostproc(&libmplayer);
     filter.lumH = filter.lumV = filter.chrH = filter.chrV = libmplayer->sws_getGaussianVec(0.6, 3.0);
     libmplayer->sws_normalizeVec(filter.lumH, 1.0);

     Tlibmplayer::swsInitParams(&params,SWS_GAUSS);
     SwsContext *ctx=libmplayer->sws_getContext(_dx, _dy, IMGFMT_Y800, _dx, _dy, IMGFMT_Y800, &params, &filter, NULL);
     libmplayer->sws_scale_ordered(ctx,(const uint8_t**)&bmp[0],(const stride_t *)&_dx,0,_dy,(uint8_t**)&blured_bmp,(stride_t *)&_dx);
     libmplayer->sws_freeContext(ctx);
     libmplayer->sws_freeVec(filter.lumH);
     libmplayer->Release();
     aligned_free(bmp[0]);
     bmp[0]=blured_bmp;
    }
  }

 dx[0]=_dx;dy[0]=_dy;
 if (!matrix)
  memset(msk[0],255,dx[0]*dy[0]);
 else
  {
   memcpy(msk[0],bmp[0],dx[0]*dy[0]);

    #define MAKE_SHADOW(x,y)                                  \
    {                                                         \
     unsigned int s=0,cnt=0;                                  \
     for (int yy=-2;yy<=+2;yy++)                              \
      {                                                       \
       if (y+yy<0 || (unsigned int)(y+yy)>=dy[0]) continue;   \
       for (int xx=-2;xx<=+2;xx++)                            \
        {                                                     \
         if (x+xx<0 || (unsigned int)(x+xx)>=dx[0]) continue; \
         s+=bmp[0][dx[0]*(y+yy)+(x+xx)]*matrix[yy+2][xx+2];   \
         cnt++;                                               \
        }                                                     \
      }                                                       \
     s/=cnt*32;                                               \
     if (s>255) s=255;                                        \
     msk[0][dx[0]*y+x]=(unsigned char)s;                      \
    }

   int dyY1=dy[0]-1,dyY2=dy[0]-2;
   for (unsigned int x=0;x<dx[0];x++)
    {
     MAKE_SHADOW(x,0);
     MAKE_SHADOW(x,1);
     MAKE_SHADOW(x,dyY2);
     MAKE_SHADOW(x,dyY1);
    }
   int dxY1=dx[0]-1,dxY2=dx[0]-2;
   for (unsigned int y0=2;y0<dy[0]-2;y0++)
    {
     MAKE_SHADOW(0,y0);
     MAKE_SHADOW(1,y0);
     MAKE_SHADOW(dxY2,y0);
     MAKE_SHADOW(dxY1,y0);
    }

   __m64 matrix8_0=*(__m64*)matrix[0],matrix8_1=*(__m64*)matrix[1],matrix8_2=*(__m64*)matrix[2];
   const int matrix4[3]={matrix[0][4],matrix[1][4],matrix[2][4]};
   const unsigned char *bmpYsrc_2=bmp[0],*bmpYsrc_1=bmp[0]+1*dx[0],*bmpYsrc=bmp[0]+2*dx[0],*bmpYsrc1=bmp[0]+3*dx[0],*bmpYsrc2=bmp[0]+4*dx[0];
   unsigned char *mskYdst=msk[0]+dx[0]*2;
   __m64 m0=_mm_setzero_si64();
   for (unsigned int y=2;y<dy[0]-2;y++,bmpYsrc_2+=dx[0],bmpYsrc_1+=dx[0],bmpYsrc+=dx[0],bmpYsrc1+=dx[0],bmpYsrc2+=dx[0],mskYdst+=dx[0])
    for (unsigned int x=2;x<dx[0]-2;x++)
     {
      __m64 r_2=_mm_madd_pi16(_mm_unpacklo_pi8(*(__m64*)(bmpYsrc_2+x-2),m0),matrix8_0);
      __m64 r_1=_mm_madd_pi16(_mm_unpacklo_pi8(*(__m64*)(bmpYsrc_1+x-2),m0),matrix8_1);
      __m64 r  =_mm_madd_pi16(_mm_unpacklo_pi8(*(__m64*)(bmpYsrc  +x-2),m0),matrix8_2);
      __m64 r1 =_mm_madd_pi16(_mm_unpacklo_pi8(*(__m64*)(bmpYsrc1 +x-2),m0),matrix8_1);
      __m64 r2 =_mm_madd_pi16(_mm_unpacklo_pi8(*(__m64*)(bmpYsrc2 +x-2),m0),matrix8_0);
      r=_mm_add_pi32(_mm_add_pi32(_mm_add_pi32(_mm_add_pi32(r_2,r_1),r),r1),r2);
      r=_mm_add_pi32(_mm_srli_si64(r,32),r);
      int s=bmpYsrc_2[x+2]*matrix4[0];
      s+=   bmpYsrc_1[x+2]*matrix4[1];
      s+=   bmpYsrc  [x+2]*matrix4[2];
      s+=   bmpYsrc1 [x+2]*matrix4[1];
      s+=   bmpYsrc2 [x+2]*matrix4[0];
      mskYdst[x]=limit_uint8((_mm_cvtsi64_si32(r)+s)/(25*32));
     }
  }
 unsigned int count=_dx*_dy;
 for (unsigned int c=0;c<count;c++)
  {
   int b=bmp[0][c];
   int o=msk[0][c]-b;
   outline[0][c]=o*(255-b)>>8;
  }

 unsigned int _dxUV;
 if (csp==FF_CSP_420P)
  {
   dx[1]=dx[0]>>prefs.shiftX[1];
   dy[1]=dy[0]>>prefs.shiftY[1];
   _dxUV=dx[1];
   dx[1]=(dx[1]/alignXsize+1)*alignXsize;
   bmp[1]=(unsigned char*)aligned_calloc(dx[1],dy[1],16);
   outline[1]=(unsigned char*)aligned_calloc(dx[1],dy[1],16);
   shadow[1]=(unsigned char*)aligned_calloc(dx[1],dy[1],16);

   dx[2]=dx[0]>>prefs.shiftX[2];
   dy[2]=dy[0]>>prefs.shiftY[2];
   dx[2]=(dx[2]/alignXsize+1)*alignXsize;

   bmpmskstride[0]=dx[0];bmpmskstride[1]=dx[1];bmpmskstride[2]=dx[2];
  }
 else
  {
   dx[1]=dx[0]*4;
   dy[1]=dy[0];
   bmp[1]=(unsigned char*)aligned_malloc(dx[1]*dy[1]+16,16);
   outline[1]=(unsigned char*)aligned_malloc(dx[1]*dy[1]+16,16);
   shadow[1]=(unsigned char*)aligned_malloc(dx[1]*dy[1]+16,16);

   bmpmskstride[0]=dx[0];bmpmskstride[1]=dx[1];
  }

    unsigned int shadowAlpha = 255; //prefs.shadowAlpha;
    unsigned int shadowMode = prefs.shadowMode; // 0: glowing, 1:classic with gradient, 2: classic with no gradient, >=3: no shadow
    if (shadowSize > 0)
    if (shadowMode == 0) //Gradient glowing shadow (most complex)
    {
        _mm_empty();
        if (_dx<shadowSize) shadowSize=_dx;
        if (_dy<shadowSize) shadowSize=_dy;
        unsigned int circle[1089]; // 1089=(16*2+1)^2
        if (shadowSize>16) shadowSize=16;
        int circleSize=shadowSize*2+1;
        for (int y=0;y<circleSize;y++)
        {
            for (int x=0;x<circleSize;x++)
            {
                unsigned int rx=ff_abs(x-(int)shadowSize);
                unsigned int ry=ff_abs(y-(int)shadowSize);
                unsigned int r=(unsigned int)sqrt((double)(rx*rx+ry*ry));
                if (r>shadowSize)
                    circle[circleSize*y+x] = 0;
                else
                    circle[circleSize*y+x] = shadowAlpha*(shadowSize+1-r)/(shadowSize+1);
            }
        }
        for (unsigned int y=0; y<_dy;y++)
        {
            int starty = y>=shadowSize ? 0 : shadowSize-y;
            int endy = y+shadowSize<_dy ? circleSize : _dy-y+shadowSize;
            for (unsigned int x=0; x<_dx;x++)
            {
                unsigned int pos = _dx*y+x;
                int startx = x>=shadowSize ? 0 : shadowSize-x;
                int endx = x+shadowSize<_dx ? circleSize : _dx-x+shadowSize;
                if (msk[0][pos] == 0) continue;
                for (int ry=starty; ry<endy;ry++)
                {
                    for (int rx=startx; rx<endx;rx++)
                    {
                        unsigned int alpha = circle[circleSize*ry+rx];
                        if (alpha)
                        {
                            unsigned int dstpos = _dx*(y+ry-shadowSize)+x+rx-shadowSize;
                            unsigned int s = msk[0][pos] * alpha >> 8;
                            if (shadow[0][dstpos]<s)
                                shadow[0][dstpos] = (unsigned char)s;
                        }
                    }
                }
            }
        }
    }
    else if (shadowMode == 1) //Gradient classic shadow
    {
        unsigned int shadowStep = shadowAlpha/shadowSize;
        for (unsigned int y=0; y<_dy;y++)
        {
            for (unsigned int x=0; x<_dx;x++)
            {
                unsigned int pos = _dx*y+x;
                if (msk[0][pos] == 0) continue;

                unsigned int shadowAlphaGradient = shadowAlpha;
                for (unsigned int xx=1; xx<=shadowSize; xx++)
                {
                    unsigned int s = msk[0][pos]*shadowAlphaGradient>>8;
                    if (x + xx < _dx)
                    {
                        if (y+xx < _dy && 
                            shadow[0][_dx*(y+xx)+x+xx] <s)
                        {
                            shadow[0][_dx*(y+xx)+x+xx] = s;
                        }
                    }
                    shadowAlphaGradient -= shadowStep;
                }
            }
        }
    }
    else if (shadowMode == 2) //Classic shadow
    {
        for (unsigned int y=shadowSize; y<_dy;y++)
            memcpy(shadow[0]+_dx*y+shadowSize,msk[0]+_dx*(y-shadowSize),_dx-shadowSize);
    }

 if (csp==FF_CSP_420P)
  {
   const DWORD *fontmaskconstants=&fontMaskConstants;
   int isColorOutline=(outlineYUV.U!=128 || outlineYUV.V!=128);
   if (Tconfig::cpu_flags&FF_CPU_MMX)
    {
     unsigned int e_dx0=_dx & ~0xf;
     unsigned int e_dx1=e_dx0/2;
     for (unsigned int y=0;y<dy[1];y++)
      for (unsigned int x=0;x<e_dx1;x+=8)
       {
        unsigned int lum0=2*y*_dx+x*2;
        unsigned int lum1=(2*y+1)*_dx+x*2;
        unsigned int chr=y*dx[1]+x;
        YV12_lum2chr_max(&bmp[0][lum0],&bmp[0][lum1],&bmp[1][chr]);
        if (isColorOutline)
         YV12_lum2chr_max(&outline[0][lum0],&outline[0][lum1],&outline[1][chr]);
        else
         YV12_lum2chr_min(&outline[0][lum0],&outline[0][lum1],&outline[1][chr]);
        YV12_lum2chr_min(&shadow[0][lum0],&shadow [0][lum1],&shadow [1][chr]);
       }
     unsigned int e_dx2=_dx/2;
     if (e_dx2>e_dx1)
      {
       if (_dx>=16)
        {
         unsigned int ldx=_dx-16;
         unsigned int cdx=ldx/2;
         for (unsigned int y=0;y<dy[1];y++)
          {
           unsigned int lum0=2*y*_dx+ldx;
           unsigned int lum1=(2*y+1)*_dx+ldx;
           unsigned int chr=y*dx[1]+cdx;
           YV12_lum2chr_max(&bmp[0][lum0],&bmp[0][lum1],&bmp[1][chr]);
           if (isColorOutline)
            YV12_lum2chr_max(&outline[0][lum0],&outline[0][lum1],&outline[1][chr]);
           else
            YV12_lum2chr_min(&outline[0][lum0],&outline[0][lum1],&outline[1][chr]);
           YV12_lum2chr_min(&shadow[0][lum0],&shadow[0][lum1],&shadow[1][chr]);
          }
        }
       else
        {
          for (unsigned int y=0;y<dy[1];y++)
           for (unsigned int x=e_dx1;x<e_dx2;x++)
            {
             unsigned int lum0=2*y*_dx+x*2;
             unsigned int lum1=(2*y+1)*_dx+x*2;
             unsigned int chr=y*dx[1]+x;
             bmp[1][chr]=std::max(std::max(std::max(bmp[0][lum0],bmp[0][lum0+1]),bmp[0][lum1]),bmp[0][lum1+1]);
             if (isColorOutline)
              outline[1][chr]=std::max(std::max(std::max(outline[0][lum0],outline[0][lum0+1]),outline[0][lum1]),outline[0][lum1+1]);
             else
              outline[1][chr]=std::min(std::min(std::min(outline[0][lum0],outline[0][lum0+1]),outline[0][lum1]),outline[0][lum1+1]);
             shadow[1][chr]=std::min(std::min(std::min(shadow[0][lum0],shadow[0][lum0+1]),shadow[0][lum1]),shadow[0][lum1+1]);
            }
        }
      }
    }
   else
    {
     unsigned int _dx1=_dx/2;
     for (unsigned int y=0;y<dy[1];y++)
      for (unsigned int x=0;x<_dx1;x++)
       {
        unsigned int lum0=2*y*_dx+x*2;
        unsigned int lum1=(2*y+1)*_dx+x*2;
        unsigned int chr=y*dx[1]+x;
        bmp[1][chr]=std::max(std::max(std::max(bmp[0][lum0],bmp[0][lum0+1]),bmp[0][lum1]),bmp[0][lum1+1]);
        if (isColorOutline)
         outline[1][chr]=std::max(std::max(std::max(outline[0][lum0],outline[0][lum0+1]),outline[0][lum1]),outline[0][lum1+1]);
        else
         outline[1][chr]=std::min(std::min(std::min(outline[0][lum0],outline[0][lum0+1]),outline[0][lum1]),outline[0][lum1+1]);
        shadow[1][chr]=std::min(std::min(std::min(shadow[0][lum0],shadow[0][lum0+1]),shadow[0][lum1]),shadow[0][lum1+1]);
       }
    }
  }
 else
  {
   //RGB32
   unsigned int xy=(_dx*_dy)>>2;
   DWORD *bmpRGB=(DWORD *)bmp[1];
   unsigned char *bmpY=bmp[0];
   for (unsigned int i=xy;i;bmpRGB+=4,bmpY+=4,i--)
    {
     *(bmpRGB)      =*bmpY<<16         | *bmpY<<8         | *bmpY;
     *(bmpRGB+1)    =*(bmpY+1)<<16     | *(bmpY+1)<<8     | *(bmpY+1);
     *(bmpRGB+2)    =*(bmpY+2)<<16     | *(bmpY+2)<<8     | *(bmpY+2);
     *(bmpRGB+3)    =*(bmpY+3)<<16     | *(bmpY+3)<<8     | *(bmpY+3);
    }
   DWORD *outlineRGB=(DWORD *)outline[1];
   unsigned char *outlineY=outline[0];
   for (unsigned int i=xy;i;outlineRGB+=4,outlineY+=4,i--)
    {
     *(outlineRGB)  =*outlineY<<16     | *outlineY<<8     | *outlineY;
     *(outlineRGB+1)=*(outlineY+1)<<16 | *(outlineY+1)<<8 | *(outlineY+1);
     *(outlineRGB+2)=*(outlineY+2)<<16 | *(outlineY+2)<<8 | *(outlineY+2);
     *(outlineRGB+3)=*(outlineY+3)<<16 | *(outlineY+3)<<8 | *(outlineY+3);
    }
   DWORD *shadowRGB=(DWORD *)shadow[1];
   unsigned char *shadowY=shadow[0];
   for (unsigned int i=xy;i;shadowRGB+=4,shadowY+=4,i--)
    {
     *(shadowRGB)   =*shadowY<<16      | *shadowY<<8      | *shadowY;
     *(shadowRGB+1) =*(shadowY+1)<<16  | *(shadowY+1)<<8  | *(shadowY+1);
     *(shadowRGB+2) =*(shadowY+2)<<16  | *(shadowY+2)<<8  | *(shadowY+2);
     *(shadowRGB+3) =*(shadowY+3)<<16  | *(shadowY+3)<<8  | *(shadowY+3);
    }
  }
 _mm_empty();
 aligned_free(msk[0]);
 msk[0]=NULL;
}

unsigned int TrenderedSubtitleWord::getShadowSize(const TrenderedSubtitleLines::TprintPrefs &prefs,LONG fontHeight)
{
 if (prefs.shadowSize==0 || prefs.shadowMode==3)
  return 0;
 if (prefs.shadowSize < 0) // SSA/ASS/ASS2
 return -1 * prefs.shadowSize;
 unsigned int shadowSize = prefs.shadowSize*fontHeight/180.0+2.6;
 if (prefs.shadowMode==0)
  shadowSize*=0.6;
 else if (prefs.shadowMode==1)
  shadowSize/=1.4142;  // 1.4142 = sqrt(2.0)
 else if (prefs.shadowMode==2)
  shadowSize*=0.4;

 if (shadowSize==0)
  shadowSize = 1;
 if (shadowSize>16)
  shadowSize = 16;
 return shadowSize;
}

// fast rendering
// YV12 only. RGB32 is not supported.
template<class tchar> TrenderedSubtitleWord::TrenderedSubtitleWord(TcharsChache *charsChache,const tchar *s,size_t strlens,const TrenderedSubtitleLines::TprintPrefs &prefs):TrenderedSubtitleWordBase(true),shiftChroma(true)
{
 csp=prefs.csp;
 m_bodyYUV=(charsChache->getBodyYUV());
 m_outlineYUV=(charsChache->getOutlineYUV());
 m_shadowYUV=(charsChache->getShadowYUV());
 const TrenderedSubtitleWord **chars=(const TrenderedSubtitleWord**)_alloca(strlens*sizeof(TrenderedSubtitleLine*));
 for (int i=0;i<3;i++)
  dx[i]=dy[i]=0;
 for (size_t i=0;i<strlens;i++)
  {
   chars[i]=charsChache->getChar(&s[i],prefs);
   dx[0]+=chars[i]->dxCharY;
   if (s[i]=='\t')
    {
     int tabsize=prefs.tabsize*std::max(chars[0]->dyCharY,1U);
     dx[0]=(dx[0]/tabsize+1)*tabsize;
    }
   for (int p=0;p<3;p++)
    dy[p]=std::max(dy[p],chars[i]->dy[p]);
   if(sizeof(tchar)==sizeof(char))
    if(_mbclen((const unsigned char *)&s[i])==2)
     i++;
  }
 alignXsize=chars[0]->alignXsize;
 TrenderedSubtitleWord_printY=chars[0]->TrenderedSubtitleWord_printY;
 TrenderedSubtitleWord_printUV=chars[0]->TrenderedSubtitleWord_printUV;
 dx[0]=(dx[0]/alignXsize+2)*alignXsize;dx[1]=dx[2]=(dx[0]/alignXsize/2+1)*alignXsize;
 dxCharY=dx[0];
 dyCharY=chars[0]->dyCharY;
 for (int i=0;i<2;i++)
  {
   bmp[i]=(unsigned char*)aligned_calloc(dx[i],dy[i],16);
   shadow[i]=(unsigned char*)aligned_calloc(dx[i],dy[i],16);
   outline[i]=(unsigned char*)aligned_calloc(dx[i],dy[i],16);
   bmpmskstride[i]=dx[i];
  }
 unsigned int x=0;
 for (size_t i=0;i<strlens;i++)
  {
   if (s[i]=='\t')
    {
     int tabsize=prefs.tabsize*std::max(chars[0]->dyCharY,1U);
     x=(x/tabsize+1)*tabsize;
    }
   for (unsigned int p=0;p<2;p++)
    {
     const unsigned char *charbmpptr=chars[i]->bmp[p];
     const unsigned char *charshadowptr=chars[i]->shadow[p],*charoutlineptr=chars[i]->outline[p];
     unsigned char *bmpptr=bmp[p]+roundRshift(x,prefs.shiftX[p]);
     unsigned char *shadowptr=shadow[p]+roundRshift(x,prefs.shiftX[p]),*outlineptr=outline[p]+roundRshift(x,prefs.shiftX[p]);
     for (unsigned int y=0;y<chars[i]->dy[p];y++,
       bmpptr+=bmpmskstride[p],
       shadowptr+=bmpmskstride[p],outlineptr+=bmpmskstride[p],
       charbmpptr+=chars[i]->bmpmskstride[p],
       charshadowptr+=chars[i]->bmpmskstride[p],charoutlineptr+=chars[i]->bmpmskstride[p])
      {
       memadd(bmpptr,charbmpptr,chars[i]->dx[p]);
       memadd(shadowptr,charshadowptr,chars[i]->dx[p]);
       memadd(outlineptr,charoutlineptr,chars[i]->dx[p]);
      }
    }
   x+=chars[i]->dxCharY;
   if(sizeof(tchar)==sizeof(char))
    if(_mbclen((const unsigned char *)&s[i])==2)
     i++;
  }
 _mm_empty();
}
void TrenderedSubtitleWord::print(unsigned int sdx[3],unsigned char *dstLn[3],const stride_t stride[3],const unsigned char *bmp[3],const unsigned char *msk[3]) const
{
 if (shadow[0])
  {
#ifdef WIN64
   if (Tconfig::cpu_flags&FF_CPU_SSE2)
    {
     unsigned char xmmregs[16*16];
     storeXmmRegs(xmmregs);
#else
   if (Tconfig::cpu_flags&(FF_CPU_SSE2|FF_CPU_MMX))
    {
#endif
     if (csp==FF_CSP_420P)
      {
       //YV12
       unsigned int halfAlingXsize=alignXsize>>1;
       unsigned short* colortbl=(unsigned short *)aligned_malloc(192,16);
       for (unsigned int i=0;i<halfAlingXsize;i++)
        {
         colortbl[i]   =(short)m_bodyYUV.Y;
         colortbl[i+8] =(short)m_bodyYUV.U;
         colortbl[i+16]=(short)m_bodyYUV.V;
         colortbl[i+24]=(short)m_bodyYUV.A;
         colortbl[i+32]=(short)m_outlineYUV.Y;
         colortbl[i+40]=(short)m_outlineYUV.U;
         colortbl[i+48]=(short)m_outlineYUV.V;
         colortbl[i+56]=(short)m_outlineYUV.A;
         colortbl[i+64]=(short)m_shadowYUV.Y;
         colortbl[i+72]=(short)m_shadowYUV.U;
         colortbl[i+80]=(short)m_shadowYUV.V;
         colortbl[i+88]=(short)m_shadowYUV.A;
        }
       // Y
       unsigned int endx=sdx[0] & ~(alignXsize-1);
       for (unsigned int y=0;y<dy[0];y++)
        for (unsigned int x=0;x<endx;x+=alignXsize)
         {
          int srcPos=y*dx[0]+x;
          int dstPos=y*stride[0]+x;
          TrenderedSubtitleWord_printY(&bmp[0][srcPos],&outline[0][srcPos],&shadow[0][srcPos],colortbl,&dstLn[0][dstPos]);
         }
       if (endx<sdx[0])
        {
         for (unsigned int y=0;y<dy[0];y++)
          for (unsigned int x=endx;x<sdx[0];x++)
           {
            int srcPos=y*dx[0]+x;
            int dstPos=y*stride[0]+x;
            int s=m_shadowYUV.A *shadow [0][srcPos]>>8;
            int d=((256-s)*dstLn[0][dstPos]>>8)+(s*m_shadowYUV.Y>>8);
            int b=m_bodyYUV.A   *bmp    [0][srcPos]>>8;
                d=((256-b)*d>>8)+(b*m_bodyYUV.Y>>8);
            int o=m_outlineYUV.A*outline[0][srcPos]>>8;
                dstLn[0][dstPos]=((256-o)*d>>8)+(o*m_outlineYUV.Y>>8);
           }
         }
       // UV
       endx=sdx[1] & ~(alignXsize-1);
       for (unsigned int y=0;y<dy[1];y++)
        for (unsigned int x=0;x<endx;x+=alignXsize)
         {
          int srcPos=y*dx[1]+x;
          int dstPos=y*stride[1]+x;
          TrenderedSubtitleWord_printUV(&bmp[1][srcPos],&outline[1][srcPos],&shadow[1][srcPos],colortbl,&dstLn[1][dstPos],&dstLn[2][dstPos]);
         }
       if (endx<sdx[1])
        {
         for (unsigned int y=0;y<dy[1];y++)
          for (unsigned int x=0;x<sdx[1];x++)
           {
            int srcPos=y*dx[1]+x;
            int dstPos=y*stride[1]+x;
            // U
            int s=m_shadowYUV.A *shadow [1][srcPos]>>8;
            int d=((256-s)*dstLn[1][dstPos]>>8)+(s*m_shadowYUV.U>>8);
            int b=m_bodyYUV.A   *bmp    [1][srcPos]>>8;
                d=((256-b)*d>>8)+(b*m_bodyYUV.U>>8);
            int o=m_outlineYUV.A*outline[1][srcPos]>>8;
                dstLn[1][dstPos]=((256-o)*d>>8)+(o*m_outlineYUV.U>>8);
            // V
                d=((256-s)*dstLn[2][dstPos]>>8)+(s*m_shadowYUV.V>>8);
                d=((256-b)*d>>8)+(b*m_bodyYUV.V>>8);
                dstLn[2][dstPos]=((256-o)*d>>8)+(o*m_outlineYUV.V>>8);
           }
        }
       aligned_free(colortbl);
      }
     else
      {
       //RGB32
       const DWORD *fontmaskconstants=&fontMaskConstants;
       unsigned int halfAlingXsize=alignXsize>>1;
       unsigned short* colortbl=(unsigned short *)aligned_malloc(192,16);
       colortbl[ 0]=colortbl[ 4]=(short)m_bodyYUV.b;
       colortbl[ 1]=colortbl[ 5]=(short)m_bodyYUV.g;
       colortbl[ 2]=colortbl[ 6]=(short)m_bodyYUV.r;
       colortbl[ 3]=colortbl[ 7]=0;
       colortbl[32]=colortbl[36]=(short)m_outlineYUV.b;
       colortbl[33]=colortbl[37]=(short)m_outlineYUV.g;
       colortbl[34]=colortbl[38]=(short)m_outlineYUV.r;
       colortbl[35]=colortbl[39]=0;
       colortbl[64]=colortbl[67]=(short)m_shadowYUV.b;
       colortbl[65]=colortbl[68]=(short)m_shadowYUV.b;
       colortbl[66]=colortbl[69]=(short)m_shadowYUV.b;
       colortbl[70]=colortbl[71]=0;
       for (unsigned int i=0;i<halfAlingXsize;i++)
        {
         colortbl[i+24]=(short)m_bodyYUV.A;
         colortbl[i+56]=(short)m_outlineYUV.A;
         colortbl[i+88]=(short)m_shadowYUV.A;
        }

       unsigned int endx2=sdx[0]*4;
       unsigned int endx=endx2 & ~(alignXsize-1);
       for (unsigned int y=0;y<dy[0];y++)
        for (unsigned int x=0;x<endx;x+=alignXsize)
         {
          int srcPos=y*dx[1]+x;
          int dstPos=y*stride[0]+x;
          TrenderedSubtitleWord_printY(&bmp[1][srcPos],&outline[1][srcPos],&shadow[1][srcPos],colortbl,&dstLn[0][dstPos]);
         }
       if (endx<endx2)
        {
         for (unsigned int y=0;y<dy[1];y++)
          for (unsigned int x=endx;x<endx2;x+=4)
           {
            int srcPos=y*dx[1]+x;
            int dstPos=y*stride[0]+x;
            // B
            int s=m_shadowYUV.A *shadow [1][srcPos]>>8;
            int d=((256-s)*dstLn[0][dstPos]>>8)+(s*m_shadowYUV.b>>8);
            int b=m_bodyYUV.A   *bmp    [1][srcPos]>>8;
                d=((256-b)*d>>8)+(b*m_bodyYUV.b>>8);
            int o=m_outlineYUV.A*outline[1][srcPos]>>8;
                dstLn[0][dstPos]=((256-o)*d>>8)+(o*m_outlineYUV.b>>8);
            // G
                d=((256-s)*dstLn[0][dstPos+1]>>8)+(s*m_shadowYUV.g>>8);
                d=((256-b)*d>>8)+(b*m_bodyYUV.g>>8);
                dstLn[0][dstPos+1]=((256-o)*d>>8)+(o*m_outlineYUV.g>>8);
            // R
                d=((256-s)*dstLn[0][dstPos+2]>>8)+(s*m_shadowYUV.r>>8);
                d=((256-b)*d>>8)+(b*m_bodyYUV.r>>8);
                dstLn[0][dstPos+2]=((256-o)*d>>8)+(o*m_outlineYUV.r>>8);
           }
        }
       aligned_free(colortbl);
      }
#ifdef WIN64
     restoreXmmRegs(xmmregs);
#endif
    }
   else
    {
     if (csp==FF_CSP_420P)
      {
       // YV12-Y
       for (unsigned int y=0;y<dy[0];y++)
        for (unsigned int x=0;x<sdx[0];x++)
         {
          int srcPos=y*dx[0]+x;
          int dstPos=y*stride[0]+x;
          int s=m_shadowYUV.A *shadow [0][srcPos]>>8;
          int d=((256-s)*dstLn[0][dstPos]>>8)+(s*m_shadowYUV.Y>>8);
          int b=m_bodyYUV.A   *bmp    [0][srcPos]>>8;
              d=((256-b)*d>>8)+(b*m_bodyYUV.Y>>8);
          int o=m_outlineYUV.A*outline[0][srcPos]>>8;
              dstLn[0][dstPos]=((256-o)*d>>8)+(o*m_outlineYUV.Y>>8);
         }
       for (unsigned int y=0;y<dy[1];y++)
        for (unsigned int x=0;x<sdx[1];x++)
         {
          int srcPos=y*dx[1]+x;
          int dstPos=y*stride[1]+x;
          // U
          int s=m_shadowYUV.A *shadow [1][srcPos]>>8;
          int d=((256-s)*dstLn[1][dstPos]>>8)+(s*m_shadowYUV.U>>8);
          int b=m_bodyYUV.A   *bmp    [1][srcPos]>>8;
              d=((256-b)*d>>8)+(b*m_bodyYUV.U>>8);
          int o=m_outlineYUV.A*outline[1][srcPos]>>8;
              dstLn[1][dstPos]=((256-o)*d>>8)+(o*m_outlineYUV.U>>8);
          // V
              d=((256-s)*dstLn[2][dstPos]>>8)+(s*m_shadowYUV.V>>8);
              d=((256-b)*d>>8)+(b*m_bodyYUV.V>>8);
              dstLn[2][dstPos]=((256-o)*d>>8)+(o*m_outlineYUV.V>>8);
         }
      }
     else
      {
       //RGB32
       for (unsigned int y=0;y<dy[1];y++)
        for (unsigned int x=0;x<sdx[0]*4;x+=4)
         {
          int srcPos=y*dx[1]+x;
          int dstPos=y*stride[0]+x;
          // B
          int s=m_shadowYUV.A *shadow [1][srcPos]>>8;
          int d=((256-s)*dstLn[0][dstPos]>>8)+(s*m_shadowYUV.b>>8);
          int b=m_bodyYUV.A   *bmp    [1][srcPos]>>8;
              d=((256-b)*d>>8)+(b*m_bodyYUV.b>>8);
          int o=m_outlineYUV.A*outline[1][srcPos]>>8;
              dstLn[0][dstPos]=((256-o)*d>>8)+(o*m_outlineYUV.b>>8);
          // G
              d=((256-s)*dstLn[0][dstPos+1]>>8)+(s*m_shadowYUV.g>>8);
              d=((256-b)*d>>8)+(b*m_bodyYUV.g>>8);
              dstLn[0][dstPos+1]=((256-o)*d>>8)+(o*m_outlineYUV.g>>8);
          // R
              d=((256-s)*dstLn[0][dstPos+2]>>8)+(s*m_shadowYUV.r>>8);
              d=((256-b)*d>>8)+(b*m_bodyYUV.r>>8);
              dstLn[0][dstPos+2]=((256-o)*d>>8)+(o*m_outlineYUV.r>>8);
         }
      }
    }
  }
 else
  {
   int sdx15=sdx[0]-15;
   for (unsigned int y=0;y<dy[0];y++,dstLn[0]+=stride[0],msk[0]+=bmpmskstride[0],bmp[0]+=bmpmskstride[0])
    {
     int x=0;
     for (;x<sdx15;x+=16)
      {
       __m64 mm0=*(__m64*)(dstLn[0]+x),mm1=*(__m64*)(dstLn[0]+x+8);
       mm0=_mm_subs_pu8(mm0,*(__m64*)(msk[0]+x));mm1=_mm_subs_pu8(mm1,*(__m64*)(msk[0]+x+8));
       mm0=_mm_adds_pu8(mm0,*(__m64*)(bmp[0]+x));mm1=_mm_adds_pu8(mm1,*(__m64*)(bmp[0]+x+8));
       *(__m64*)(dstLn[0]+x)=mm0;*(__m64*)(dstLn[0]+x+8)=mm1;
      }
     for (;x<int(sdx[0]);x++)
      {
       int c=dstLn[0][x];
       c-=msk[0][x];if (c<0) c=0;
       c+=bmp[0][x];if (c>255) c=255;
       dstLn[0][x]=(unsigned char)c;
      }
    }
   __m64 m128=_mm_set1_pi8((char)128),m0=_mm_setzero_si64(),mAdd=shiftChroma?m128:m0;
   int add=shiftChroma?128:0;
   int sdx7=sdx[1]-7;
   for (unsigned int y=0;y<dy[1];y++,dstLn[1]+=stride[1],dstLn[2]+=stride[2],msk[1]+=bmpmskstride[1],bmp[1]+=bmpmskstride[1],bmp[2]+=bmpmskstride[2])
    {
     int x=0;
     for (;x<sdx7;x+=8)
      {
       __m64 mm0=*(__m64*)(dstLn[1]+x);
       __m64 mm1=*(__m64*)(dstLn[2]+x);

       psubb(mm0,m128);
       psubb(mm1,m128);

       const __m64 msk8=*(const __m64*)(msk[1]+x);

       __m64 mskU=_mm_cmpgt_pi8(m0,mm0); //what to be negated
       mm0=_mm_or_si64(_mm_and_si64(mskU,_mm_adds_pu8(mm0,msk8)),_mm_andnot_si64(mskU,_mm_subs_pu8(mm0,msk8)));

       __m64 mskV=_mm_cmpgt_pi8(m0,mm1);
       mm1=_mm_or_si64(_mm_and_si64(mskV,_mm_adds_pu8(mm1,msk8)),_mm_andnot_si64(mskV,_mm_subs_pu8(mm1,msk8)));

       mm0=_mm_add_pi8(_mm_add_pi8(mm0,*(__m64*)(bmp[1]+x)),mAdd);
       mm1=_mm_add_pi8(_mm_add_pi8(mm1,*(__m64*)(bmp[2]+x)),mAdd);

       *(__m64*)(dstLn[1]+x)=mm0;
       *(__m64*)(dstLn[2]+x)=mm1;
      }
     for (;x<int(sdx[1]);x++)
      {
       int m=msk[1][x],c;
       c=dstLn[1][x];
       c-=128;
       if (c<0) {c+=m;if (c>0) c=0;}
       else     {c-=m;if (c<0) c=0;}
       c+=bmp[1][x];
       c+=add;
       dstLn[1][x]=c;//(unsigned char)limit(c,0,255);

       c=dstLn[2][x];
       c-=128;
       if (c<0) {c+=m;if (c>0) c=0;}
       else     {c-=m;if (c<0) c=0;};
       c+=bmp[2][x];
       c+=add;
       dstLn[2][x]=c;//(unsigned char)limit(c,0,255);
      }
    }
  }

/* for (int x=0;x<dx[0];x++)
     for (int y=0;y<dy[0];y++)
     {
         if (bmp[0][dy[0]*y+x] !=0)
         {
             dstLn[0][x]=c
         }
     }*/
 _mm_empty();
}

//============================== TrenderedSubtitleLine ===============================
unsigned int TrenderedSubtitleLine::width(void) const
{
 unsigned int dx=0;
 for (const_iterator w=begin();w!=end();w++)
  dx+=(*w)->dx[0];
 return dx;
}
unsigned int TrenderedSubtitleLine::height(void) const
{
 unsigned int dy=0;
 for (const_iterator w=begin();w!=end();w++)
  dy=std::max(dy,(*w)->dy[0]);
 return dy;
}
unsigned int TrenderedSubtitleLine::charHeight(void) const
{
 unsigned int dy=0;
 for (const_iterator w=begin();w!=end();w++)
  dy=std::max(dy,(*w)->dyCharY);
 return dy;
}
void TrenderedSubtitleLine::print(int startx,int starty,const TrenderedSubtitleLines::TprintPrefs &prefs,unsigned int prefsdx,unsigned int prefsdy) const
{
 for (const_iterator w=begin();w!=end() && startx<(int)prefsdx;startx+=(*w)->dxCharY,w++)
  {
   const unsigned char *msk[3],*bmp[3];
   unsigned char *dstLn[3];
   int x[3];
   unsigned int dx[3];
   for (int i=0;i<3;i++)
    {
     x[i]=startx>>prefs.shiftX[i];
     msk[i]=(*w)->msk[i];
     bmp[i]=(*w)->bmp[i];
     if (prefs.align!=ALIGN_FFDSHOW && x[i]<0)
      {
       msk[i]+=-x[i];
       bmp[i]+=-x[i];
      }
     dstLn[i]=prefs.dst[i]+prefs.stride[i]*(starty>>prefs.shiftY[i]);if (x[i]>0) dstLn[i]+=x[i]*prefs.cspBpp;

     if (x[i]+(*w)->dx[i]>(prefsdx>>prefs.shiftX[i])) dx[i]=(prefsdx>>prefs.shiftX[i])-x[i];
     else if (x[i]<0) dx[i]=(*w)->dx[i]+x[i];
     else dx[i]=(*w)->dx[i];
     dx[i]=std::min(dx[i],prefsdx>>prefs.shiftX[i]);
    }
   (*w)->print(dx,dstLn,prefs.stride,bmp,msk);
  }
}
void TrenderedSubtitleLine::clear(void)
{
 for (iterator w=begin();w!=end();w++)
  delete *w;
 std::vector<value_type>::clear();
}

//============================== TrenderedSubtitleLines ==============================
void TrenderedSubtitleLines::print(const TprintPrefs &prefs)
{
 if (empty()) return;
 unsigned int prefsdx,prefsdy;
 if (prefs.sizeDx && prefs.sizeDy)
  {
   prefsdx=prefs.sizeDx;
   prefsdy=prefs.sizeDy;
  }
 else
  {
   prefsdx=prefs.dx;
   prefsdy=prefs.dy;
  }
 unsigned int h=0;
 for (const_iterator i=begin();i!=end();i++)
  h+=prefs.linespacing*(*i)->charHeight()/100;

 const_reverse_iterator ri=rbegin();
 unsigned int h1 = h - prefs.linespacing*(*ri)->charHeight()/100 + (*ri)->height();

 double y=(prefs.ypos<0) ? -(double)prefs.ypos : ((double)prefs.ypos*prefsdy)/100-h/2;
 if (y+h1 >= (double)prefsdy) y=(double)prefsdy-h1-1;
 if (y<0) y=0;

 int old_alignment=-1;
 int old_marginTop=-1,old_marginL=-1;

 for (const_iterator i=begin();i!=end();y+=(double)prefs.linespacing*(*i)->charHeight()/100,i++)
  {
   if (y<0) continue;

   unsigned int marginTop=(*i)->props.get_marginTop(prefsdy);
   unsigned int marginBottom=(*i)->props.get_marginBottom(prefsdy);

   // When the alignment or marginTop or marginL changes, it's a new paragraph.
   if ((*i)->props.alignment>0 && ((*i)->props.alignment!=old_alignment || old_marginTop!=(*i)->props.marginTop || old_marginL!=(*i)->props.marginL))
    {
     old_alignment=(*i)->props.alignment;
     old_marginTop=(*i)->props.marginTop;
     old_marginL=(*i)->props.marginL;
     // calculate the height of the paragraph
     double paragraphHeight=0,h1=0;
     for (const_iterator pi=i;pi!=end();pi++)
      {
       double h2;
       if ((*pi)->props.alignment!=old_alignment || (*pi)->props.marginTop!=old_marginTop || (*pi)->props.marginL!=old_marginL)
        break;
       h2=h1+(*pi)->height();
       h1+=(double)prefs.linespacing*(*pi)->charHeight()/100;
       if (h2>paragraphHeight) paragraphHeight=h2;
      }
     h1+=2.0; // FIXME: should plus the outline width of the top line + that of the botom line.

     switch ((*i)->props.alignment)
      {
       case 1: // SSA bottom
       case 2:
       case 3:
        y=(double)prefsdy-h1-marginBottom;
        break;
       case 9: // SSA mid
       case 10:
       case 11:
        y=((double)prefsdy-h1)/2.0;
        break;
       case 5: // SSA top
       case 6:
       case 7:
        y=marginTop;
        break;
       default:
        break;
      }

     if (y+paragraphHeight>=(double)prefsdy)
      y=(double)prefsdy-paragraphHeight-1;
     if (y<0) y=0;
    }

   if (y+(*i)->height()>=(double)prefsdy) break;
   //TODO: cleanup
   int x;
   unsigned int cdx=(*i)->width();
   if (prefs.xpos<0) x=-prefs.xpos;
   else
    {
     switch ((*i)->props.alignment)
      {
       case 1: // left(SSA)
       case 5:
       case 9:
        x=(*i)->props.get_marginL(prefsdx);
        break;
       case 2: // center(SSA)
       case 6:
       case 10:
        x=(prefsdx-cdx)/2;
        if (x<0) x=0;
        if (x+cdx>=prefsdx) x=prefsdx-cdx;
        break;
       case 3: // right(SSA)
       case 7:
       case 11:
        x=prefsdx-cdx-(*i)->props.get_marginR(prefsdx)+2;
        break;
       default: // -1 (non SSA)
        x=(prefs.xpos*prefsdx)/100+prefs.posXpix;
        switch (prefs.align)
         {
          case ALIGN_FFDSHOW:x=x-cdx/2;if (x<0) x=0;if (x+cdx>=prefsdx) x=prefsdx-cdx;break;
          case ALIGN_LEFT:break;
          case ALIGN_CENTER:x=x-cdx/2;break;
          case ALIGN_RIGHT:x=x-cdx;break;
         }
        break;
      }
    }
   if (x+cdx>=prefsdx) x=prefsdx-cdx-1;
   if (x<0) x=0;
   (*i)->print(x,y,prefs,prefsdx,prefsdy); // print a line (=print words).
  }
}
void TrenderedSubtitleLines::add(TrenderedSubtitleLine *ln,unsigned int *height)
{
 push_back(ln);
 if (height)
  *height+=ln->height();
}
void TrenderedSubtitleLines::clear(void)
{
 for (iterator l=begin();l!=end();l++)
  {
   (*l)->clear();
   delete *l;
  }
 std::vector<value_type>::clear();
}

//================================= TcharsChache =================================
TcharsChache::TcharsChache(HDC Ihdc,const short (*Imatrix)[5],const YUVcolorA &Iyuv,const YUVcolorA &Ioutline,const YUVcolorA &Ishadow,int Ixscale,IffdshowBase *Ideci):
 hdc(Ihdc),
 matrix(Imatrix),
 yuv(Iyuv),
 outlineYUV(Ioutline),
 shadowYUV(Ishadow),
 xscale(Ixscale),
 deci(Ideci)
{
}
TcharsChache::~TcharsChache()
{
 for (Tchars::iterator c=chars.begin();c!=chars.end();c++)
  delete c->second;
}
template<> const TrenderedSubtitleWord* TcharsChache::getChar(const wchar_t *s,const TrenderedSubtitleLines::TprintPrefs &prefs)
{
 int key=(int)*s;
 Tchars::iterator l=chars.find(key);
 if (l!=chars.end()) return l->second;
 TrenderedSubtitleWord *ln=new TrenderedSubtitleWord(hdc,s,1,matrix,yuv,outlineYUV,shadowYUV,prefs,xscale);
 chars[key]=ln;
 return ln;
}

template<> const TrenderedSubtitleWord* TcharsChache::getChar(const char *s,const TrenderedSubtitleLines::TprintPrefs &prefs)
{
 if(_mbclen((unsigned char *)s)==1)
  {
   int key=(int)*s;
   Tchars::iterator l=chars.find(key);
   if (l!=chars.end()) return l->second;
   TrenderedSubtitleWord *ln=new TrenderedSubtitleWord(hdc,s,1,matrix,yuv,outlineYUV,shadowYUV,prefs,xscale);
   chars[key]=ln;
   return ln;
  }
 else
  {
   const wchar_t *mbcs=(wchar_t *)s; // ANSI-MBCS
   int key=(int)*mbcs;
   Tchars::iterator l=chars.find(key);
   if (l!=chars.end()) return l->second;
   TrenderedSubtitleWord *ln=new TrenderedSubtitleWord(hdc,s,2,matrix,yuv,outlineYUV,shadowYUV,prefs,xscale);
   chars[key]=ln;
   return ln;
  }
}

//==================================== Tfont ====================================
Tfont::Tfont(IffdshowBase *Ideci):
 fontManager(NULL),
 deci(Ideci),
 oldsub(NULL),
 hdc(NULL),oldFont(NULL),
 charsCache(NULL),
 height(0),
 fontSettings((TfontSettings*)malloc(sizeof(TfontSettings)))
{
}
Tfont::~Tfont()
{
 done();
 free(fontSettings);
}
void Tfont::init(const TfontSettings *IfontSettings)
{
 done();
 memcpy(fontSettings,IfontSettings,sizeof(TfontSettings));
 hdc=CreateCompatibleDC(NULL);
 if (!hdc) return;
 yuvcolor=YUVcolorA(fontSettings->color,fontSettings->bodyAlpha);
 outlineYUV=YUVcolorA(fontSettings->outlineColor,fontSettings->outlineAlpha);
 shadowYUV=YUVcolorA(fontSettings->shadowColor,fontSettings->shadowAlpha);
 //if (fontSettings->outlineStrength<100)
 // for (int y=-2;y<=2;y++)
 //  for (int x=-2;x<=2;x++)
 //   {
 //    double d=8-(x*x+y*y);
 //    matrix[y+2][x+2]=short(3.57*fontSettings->outlineStrength*pow(d/8,2-fontSettings->outlineRadius/50.0)+0.5);
 //   }
 switch (fontSettings->outlineWidth)
  {
   case 0:
    memcpy(matrix,matrix0,50);
    break;
   case 1:
    memcpy(matrix,matrix1,50);
    break;
   case 2:
    memcpy(matrix,matrix2,50);
    break;
   case 3:
    memcpy(matrix,matrix3,50);
    break;
  }

 if (fontSettings->fast)
  charsCache=new TcharsChache(hdc,fontSettings->opaqueBox?NULL:matrix,yuvcolor,outlineYUV,shadowYUV,fontSettings->xscale,deci);
}
void Tfont::done(void)
{
 lines.clear();
 if (hdc)
  {
   if (oldFont) SelectObject(hdc,oldFont);oldFont=NULL;
   DeleteDC(hdc);hdc=NULL;
  }
 oldsub=NULL;
 if (charsCache) delete charsCache;charsCache=NULL;
}

template<class tchar> TrenderedSubtitleWord* Tfont::newWord(const tchar *s,size_t slen,TrenderedSubtitleLines::TprintPrefs prefs,const TsubtitleWord<tchar> *w,bool trimRightSpaces)
{
 typedef typename tchar_traits<tchar>::ffstring ffstring;
 OUTLINETEXTMETRIC otm;
 GetOutlineTextMetrics(hdc,sizeof(otm),&otm);

 ffstring s1(s);
 if (trimRightSpaces)
  {
   while (s1.size() && s1.at(s1.size()-1)==' ')
    s1.erase(s1.size()-1,1);
  }

 if (w->props.shadowDepth != -1) // SSA/ASS/ASS2
  {
   if (w->props.shadowDepth == 0)
    {
     prefs.shadowMode = 3;
     prefs.shadowSize = 0;
    }
   else
    {
     prefs.shadowMode = 2;
     prefs.shadowSize = -1 * w->props.shadowDepth;
    }
  }

 prefs.alignSSA=w->props.alignment;

 short (*mat)[5];
 if (w->props.outlineWidth==0)
  {
   if (prefs.csp==FF_CSP_420P)
    mat=(short (*)[5])matrix0;
   else
    mat=(short (*)[5])matrix0RGB;
  }
 else if (w->props.outlineWidth==1)
  mat=(short (*)[5])matrix1;
 else if (w->props.outlineWidth==2)
  mat=(short (*)[5])matrix2;
 else if (w->props.outlineWidth>=3)
  {
   mat=(short (*)[5])matrix3;
   prefs.blur=true;
  }
 else
  {
   mat=fontSettings->opaqueBox?NULL:matrix;
   if (fontSettings->outlineWidth==3)
    prefs.blur=true;
  }

 if (prefs.shadowMode==-1) // OSD
  {
   prefs.shadowMode=fontSettings->shadowMode;
   prefs.shadowSize=fontSettings->shadowSize;
  }

 YUVcolorA shadowYUV1;
 if (!w->props.isColor)
  {
   shadowYUV1=shadowYUV;
   if (prefs.shadowMode<=1)
    shadowYUV1.A=256*sqrt((double)shadowYUV1.A/256.0);
  }

 if (!w->props.isColor && fontSettings->fast && !otm.otmItalicAngle && !otm.otmTextMetrics.tmItalic && !(prefs.shadowSize!=0 && prefs.shadowMode!=3) && prefs.csp==FF_CSP_420P)
  return new TrenderedSubtitleWord(charsCache,s1.c_str(),slen,prefs); // fast rendering
 else
  return new TrenderedSubtitleWord(hdc,                      // full rendering
                                   s1.c_str(),
                                   slen,
                                   mat,
                                   w->props.isColor ? YUVcolorA(w->props.color,w->props.colorA) : yuvcolor,
                                   w->props.isColor ? YUVcolorA(w->props.OutlineColour,w->props.OutlineColourA) : outlineYUV,
                                   w->props.isColor ? YUVcolorA(w->props.ShadowColour,w->props.ShadowColourA) : shadowYUV1,
                                   prefs,
                                   w->props.scaleX!=-1?w->props.scaleX:fontSettings->xscale
                                   );
}

template<class tchar> int Tfont::get_splitdx1(const TsubtitleWord<tchar> &w,int splitdx,int dx) const
{
 if (w.props.get_marginR(dx) || w.props.get_marginL(dx))
  return (dx- w.props.get_marginR(dx) - w.props.get_marginL(dx))*4;
 else
  return splitdx;
}

template<class tchar> void Tfont::prepareC(const TsubtitleTextBase<tchar> *sub,const TrenderedSubtitleLines::TprintPrefs &prefs,bool forceChange)
{
 if (oldsub!=sub || forceChange || oldCsp!=prefs.csp)
  {
   oldsub=sub;
   oldCsp=prefs.csp;

   unsigned int dx,dy;
   if (prefs.sizeDx && prefs.sizeDy)
    {
     dx=prefs.sizeDx;
     dy=prefs.sizeDy;
    }
   else
    {
     dx=prefs.dx;
     dy=prefs.dy;
    }

   lines.clear();height=0;
   if (!sub) return;
   if (!fontManager)
    comptrQ<IffdshowDecVideo>(deci)->getFontManager(&fontManager);
   bool nosplit=!fontSettings->split && !(prefs.fontchangesplit && prefs.fontsplit);
   int splitdx0=nosplit?0:(prefs.textBorderLR-40>(int)dx?dx:dx-prefs.textBorderLR)*4;
   for (typename TsubtitleTextBase<tchar>::const_iterator l=sub->begin();l!=sub->end();l++)
    {
     TrenderedSubtitleLine *line=NULL;
     int splitdx=splitdx0;
     int splitdx1=-1;
     for (typename TsubtitleLine<tchar>::const_iterator w=l->begin();w!=l->end();w++)
      {
       if (splitdx1==-1)
        splitdx1=get_splitdx1(*w,splitdx,dx);
       LOGFONT lf;
       w->props.toLOGFONT(lf,*fontSettings,dx,dy);
       HFONT font=fontManager->getFont(lf);
       HGDIOBJ old=SelectObject(hdc,font);
       if (!oldFont) oldFont=old;
       SetTextCharacterExtra(hdc,w->props.spacing>=0?w->props.spacing:fontSettings->spacing);
       if (!line)
        {
         line=new TrenderedSubtitleLine(w->props);
         splitdx1=get_splitdx1(*w,splitdx,dx);
        }
       if (nosplit)
        line->push_back(newWord<tchar>(*w,strlen(*w),prefs,&*w));
       else
        {
         const tchar *p=*w;
         while (*p)
          {
           int nfit;
           SIZE sz;
           size_t strlenp=strlenp=strlen(p);
           int *pwidths=(int*)_alloca(sizeof(int)*(strlenp+1));
           if (!prefs.config->getGDI<tchar>().getTextExtentExPoint(hdc,p,(int)strlenp,splitdx1,&nfit,pwidths,&sz) || nfit>=(int)strlen(p))
            {
             TrenderedSubtitleWord *rw;
             if (w+1==l->end())
              rw=newWord(p,strlen(p),prefs,&*w,true); // trim right space
             else
              rw=newWord(p,strlen(p),prefs,&*w,false);
             line->push_back(rw);
             splitdx1-=rw->dxCharY*4;
             if (splitdx1<0) splitdx1=0;
             splitdx-=rw->dxCharY*4;
             if (splitdx<0) splitdx=0;
             break;
            }

           bool spaceOK=false;
           if (nfit==0) nfit=1;
           for (int j=nfit;j>0;j--)
            if (tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)p[j]))
             {
              spaceOK=true;
              nfit=j;
              break;
             }

           if (spaceOK || /* to avoid infinity loop */ splitdx1==get_splitdx1(*w,splitdx,dx))
            {
             TrenderedSubtitleWord *rw=newWord(p,nfit,prefs,&*w,true);
             line->push_back(rw);
             p+=nfit;
            }
           lines.add(line,&height);
           line=new TrenderedSubtitleLine(w->props);
           splitdx1=get_splitdx1(*w,splitdx,dx);
           splitdx=splitdx0;
           while (*p && tchar_traits<tchar>::isspace((typename tchar_traits<tchar>::uchar_t)*p))
            p++;
          }
        }
      }

     if (line)
      if (!line->empty())
       lines.add(line,&height);
      else
       delete line;
    }
  }
}

template<class tchar> void Tfont::print(const TsubtitleTextBase<tchar> *sub,bool forceChange,const TrenderedSubtitleLines::TprintPrefs &prefs,unsigned int *y)
{
 if (!sub) return;
 prepareC(sub,prefs,forceChange);if (y) *y+=height;
 lines.print(prefs);
}

template void Tfont::print(const TsubtitleTextBase<char> *sub,bool forceChange,const TrenderedSubtitleLines::TprintPrefs &prefs,unsigned int *y);
template void Tfont::print(const TsubtitleTextBase<wchar_t> *sub,bool forceChange,const TrenderedSubtitleLines::TprintPrefs &prefs,unsigned int *y);
