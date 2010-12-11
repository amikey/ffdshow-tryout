/*
 * Copyright (c) 2004-2006 Milan Cutka
 * based of CSubpicInputPin by Gabest
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
#include "TsubtitleDVD.h"
#include "TsubtitlesTextpinDVD.h"
#include "TspuImage.h"
#include "Tstream.h"
#include "Tconfig.h"
#include "simd.h"

//=================================== TsubtitleDVDparent ==================================
TsubtitleDVDparent::TsubtitleDVDparent(void)
{
    psphli=NULL;
    fsppal=false;
    spon=true;
    sppal[0].Y=0x00;
    sppal[0].U=sppal[0].V=0x80;
    sppal[1].Y=0xe0;
    sppal[1].U=sppal[1].V=0x80;
    sppal[2].Y=0x80;
    sppal[2].U=sppal[2].V=0x80;
    sppal[3].Y=0x20;
    sppal[3].U=sppal[3].V=0x80;
    delay=0;
    forced_subs=false;
    custom_colors=false;
    tridx=0;
}

TspuPlane* TsubtitleDVDparent::allocPlanes(const CRect &r, int csp)
{
    planes[0].alloc(r.Size(),1,csp);
    if ((csp & FF_CSPS_MASK) == FF_CSP_420P) {
        planes[1].alloc(r.Size(),2,csp);
        planes[2].alloc(r.Size(),2,csp);
    }
    return planes;
}

const char* TsubtitleDVDparent::parseTimestamp(const char* &line,int &ms)
{
    while (isspace(*line)) {
        ++line;
    }
    int forward=1;
    if (line[0]=='+') {
        forward = 1;
        line++;
    } else if (line[0]=='-') {
        forward = -1;
        line++;
    }
    const char *p = line;
    while (isdigit(*p)) {
        ++p;
    }
    if (p - line == 0) {
        return NULL;
    }
    int h = atoi(line);
    if (*p != ':') {
        return NULL;
    }
    line = ++p;
    while (isdigit(*p)) {
        ++p;
    }
    if (p - line == 0) {
        return NULL;
    }
    int m = atoi(line);
    if (*p != ':') {
        return NULL;
    }
    line = ++p;
    while (isdigit(*p)) {
        ++p;
    }
    if (p - line == 0) {
        return NULL;
    }
    int s = atoi(line);
    if (*p != ':') {
        return NULL;
    }
    line = ++p;
    while (isdigit(*p)) {
        ++p;
    }
    if (p - line == 0) {
        return NULL;
    }
    ms = (atoi(line) + 1000 * (s + 60 * (m + 60 * h)))*forward;
    return p;
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse_delay(const char *line)
{
    return parseTimestamp(line,delay)?PARSE_OK:PARSE_ERROR;
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse_size(const char *line)
{
    // size: WWWxHHH
    char *p;
    while (isspace(*line)) {
        ++line;
    }
    if (!isdigit(*line)) {
        return PARSE_ERROR;
    }
    rectOrig.dx=strtoul(line, &p, 10);
    if (*p != 'x') {
        return PARSE_ERROR;
    }
    ++p;
    rectOrig.dy=strtoul(p, NULL, 10);
    return PARSE_OK;
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse_origin(const char *line)
{
    // org: X,Y
    char *p;
    while (isspace(*line)) {
        ++line;
    }
    if (!isdigit(*line)) {
        return PARSE_ERROR;
    }
    rectOrig.x = strtoul(line, &p, 10);
    if (*p != ',') {
        return PARSE_ERROR;
    }
    ++p;
    rectOrig.y = strtoul(p, NULL, 10);
    return PARSE_OK;
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse_forced_subs(const char *line)
{
    const char *p;

    p  = line;
    while (isspace(*p)) {
        ++p;
    }

    if (_strnicmp("on",p,2) == 0) {
        forced_subs=true;
        return PARSE_OK;
    } else if (_strnicmp("off",p,3) == 0) {
        forced_subs=false;
        return PARSE_OK;
    } else {
        return PARSE_ERROR;
    }
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse_palette(const char *line)
{
    // palette: XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX, XXXXXX
    unsigned int n=0;
    PARSE_RES res=PARSE_OK;;
    while (1) {
        const char *p;
        int r, g, b, tmp;
        while (isspace(*line)) {
            ++line;
        }
        p = line;
        while (isxdigit(*p)) {
            ++p;
        }
        if (p - line != 6) {
            res=PARSE_ERROR;
            break;
        }
        tmp = strtoul(line, NULL, 16);
        r = tmp >> 16 & 0xff;
        g = tmp >> 8 & 0xff;
        b = tmp & 0xff;
        sppal[n]= YUVcolorA(RGB(r,g,b), YUVcolorA::vobsubWeirdCsp_t());
        n++;
        if (n == 16) {
            break;
        }
        if (*p == ',') {
            ++p;
        }
        line = p;
    }
    if (n>0) {
        fsppal=true;
    }
    return res;
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse_custom(const char *line)
{
    //custom colors: OFF/ON(0/1)
    if ((strncmp("ON", line + 15, 2) == 0)||strncmp("1", line + 15, 1) == 0) {
        custom_colors=1;
        return PARSE_OK;
    } else if ((strncmp("OFF", line + 15, 3) == 0)||strncmp("0", line + 15, 1) == 0) {
        custom_colors=0;
        return PARSE_OK;
    } else {
        return PARSE_ERROR;
    }
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse_tridx(const char *line)
{
    //tridx: XXXX
    tridx = strtoul((line + 26), NULL, 16);
    tridx = ((tridx&0x1000)>>12) | ((tridx&0x100)>>7) | ((tridx&0x10)>>2) | ((tridx&1)<<3);
    return PARSE_OK;
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse_cuspal(const char *line)
{
    //colors: XXXXXX, XXXXXX, XXXXXX, XXXXXX
    unsigned int n;
    n = 0;
    line += 40;
    while(1) {
        const char *p;
        while (isspace(*line)) {
            ++line;
        }
        p=line;
        while (isxdigit(*p)) {
            ++p;
        }
        if (p - line !=6) {
            return PARSE_ERROR;
        }
        unsigned int tmp = strtoul(line, NULL, 16);
        int r = tmp >> 16 & 0xff;
        int g = tmp >> 8 & 0xff;
        int b = tmp & 0xff;
        cuspal[n] = YUVcolorA(RGB(r,g,b));
        n++;
        if (n==4) {
            break;
        }
        if(*p == ',') {
            ++p;
        }
        line = p;
    }
    return PARSE_OK;
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse_one_line(const char *line)
{
    if (strncmp("delay:", line, 6) == 0) {
        return idx_parse_delay( line + 6);
    } else if (strncmp("size:", line, 5) == 0) {
        return idx_parse_size( line + 5);
    } else if (strncmp("org:", line, 4) == 0) {
        return idx_parse_origin( line + 4);
    } else if (strncmp("palette:", line, 8) == 0) {
        return idx_parse_palette( line + 8);
    } else if (strncmp("custom colors:", line, 14) == 0) //custom colors: ON/OFF, tridx: XXXX, colors: XXXXXX, XXXXXX, XXXXXX,XXXXXX
        if (idx_parse_custom(line)==PARSE_OK &&
                idx_parse_tridx(line)==PARSE_OK &&
                idx_parse_cuspal(line)==PARSE_OK) {
            return PARSE_OK;
        } else {
            return PARSE_ERROR;
        }
    else if (strncmp("forced subs:", line, 12) == 0) {
        return idx_parse_forced_subs( line + 12);
    } else {
        return PARSE_IGNORE;
    }
}

TsubtitleDVDparent::PARSE_RES TsubtitleDVDparent::idx_parse(Tstream &fs)
{
    while (1) {
        char line[4096];
        if (!fs.fgets(line,4096)) {
            return PARSE_OK;
        }
        if (*line == 0 || *line == '\r' || *line == '\n' || *line == '#') {
            continue;
        }
        PARSE_RES res=idx_parse_one_line(line);
        if (res==PARSE_IGNORE) {
            DPRINTF(_l("idx: ignoring %s"), line);
            continue;
        } else if (res==PARSE_ERROR) {
            DPRINTF(_l("idx: ERROR in %s"), line);
            return PARSE_ERROR;
        }
    }
}

//===================================== TsubtitleDVD ======================================
TsubtitleDVD::TsubtitleDVD(REFERENCE_TIME Istart,const unsigned char *Idata,unsigned int Idatalen,TsubtitleDVDparent *Iparent):parent(Iparent)
{
    start=Istart;
    stop=_I64_MAX;
    memset(&sphli,0,sizeof(sphli));
    forced=false;
    data.append(Idata,Idatalen);
    if (parent->psphli && start==pts2rt(parent->psphli->StartPTM)) {
        psphli=new AM_PROPERTY_SPHLI(*parent->psphli);
    } else {
        psphli=NULL;
    }
    offset[0]=DWORD(-1);
    parse();
    changed=true;
    image=NULL;
}

TsubtitleDVD::TsubtitleDVD(REFERENCE_TIME Istart,TsubtitleDVDparent *Iparent):parent(Iparent)
{
    start=Istart;
    stop=_I64_MAX;
    memset(&sphli,0,sizeof(sphli));
    forced=false;
    if (parent->psphli && start==pts2rt(parent->psphli->StartPTM)) {
        psphli=new AM_PROPERTY_SPHLI(*parent->psphli);
    } else {
        psphli=NULL;
    }
    offset[0]=DWORD(-1);
    changed=true;
    image=NULL;
}

TsubtitleDVD::~TsubtitleDVD()
{
    lines.clear();
}

BYTE TsubtitleDVD::getNibble(const BYTE *p,DWORD *offset,int &nField,int &fAligned)
{
    BYTE ret=BYTE((p[offset[nField]]>>(fAligned<<2))&0x0f);
    offset[nField]+=1-fAligned;
    fAligned=!fAligned;
    return ret;
}
BYTE TsubtitleDVD::getHalfNibble(const BYTE *p,DWORD *offset,int &nField,int &n)
{
    BYTE ret=BYTE((p[offset[nField]]>>(n<<1))&0x03);
    if (!n) {
        offset[nField]++;
    }
    n=(n-1+4)&3;
    return ret;
}

void TsubtitleDVD::drawPixel(const CPoint &pt,const YUVcolorA &color, const CRect &rc, CRect &rectReal,TspuPlane plane[3],bool skipEdge) const
{
    /*
     if (c.Reserved==0) return;

     BYTE *p=&prefs.dst[0][pt.y*prefs.stride[0]+pt.x];
     *p-=(*p-c.Y)*c.Reserved>>4;

     // U/V is exchanged? weird but looks true when comparing the outputted colors from other decoders
     p=&prefs.dst[1][(pt.y/2)*prefs.stride[1]+(pt.x+1)/2];
     *p-=(*p-c.V)*c.Reserved>>4;
     p=&prefs.dst[2][(pt.y/2)*prefs.stride[2]+(pt.x+1)/2];
     *p-=(*p-c.U)*c.Reserved>>4;
    */
    int ptx=pt.x,pty=pt.y;
    if (color.A != 0) {
        if (ptx<rectReal.left  && (!skipEdge || ptx>2)) {
            rectReal.left=ptx;
        }
        if (ptx>rectReal.right && (!skipEdge || ptx!=(rc.Width())-1)) {
            rectReal.right=ptx;
        }
    }
    if (csp == FF_CSP_420P) {
        plane[0].c[pty*plane[0].stride+ptx]=color.Y;
        plane[0].r[pty*plane[0].stride+ptx]=color.A;

        if (pty&1) {
            return;
        }

        ptx /= 2;
        pty /= 2;
        plane[1].c[pty*plane[1].stride+ptx]=color.V;
        plane[1].r[pty*plane[1].stride+ptx]=color.A;

        plane[2].c[pty*plane[2].stride+ptx]=color.U;
        plane[2].r[pty*plane[2].stride+ptx]=color.A;
    } else {
        uint32_t *c = (uint32_t *)&plane[0].c[pty * plane[0].stride];
        uint32_t *r = (uint32_t *)&plane[0].r[pty * plane[0].stride];
        c[ptx] = /*(color.A<<24)|*/color.m_rgb;
        r[ptx] = (color.A<<16)|(color.A<<8)|(color.A);///*(0x00404040) - */color.m_aaa64;
    }
}

template<class _mm> void TsubtitleDVD::drawPixelSimd(const CPoint &pt,const YUVcolorA &color, int length, const CRect &rc, CRect &rectReal,TspuPlane plane[3],bool skipEdge) const
{
    // Draw input color on (length) pixels on the same line starting at pt(x,y)
    int ptx=pt.x,pty=pt.y;
    if (color.A != 0) {
        if (ptx<rectReal.left && (!skipEdge || pt.x!=0)) {
            rectReal.left=ptx;
        }
        if (ptx+length>rectReal.right && (!skipEdge || pt.x+length!=rc.Width()-1)) {
            rectReal.right=ptx+length;
        }
    }

    if (csp == FF_CSP_420P) {
        typename _mm::__m colorY=_mm::set1_pi8(uint8_t(color.Y));
        typename _mm::__m colorU=_mm::set1_pi8(uint8_t(color.U));
        typename _mm::__m colorV=_mm::set1_pi8(uint8_t(color.V));
        typename _mm::__m colorA=_mm::set1_pi8(uint8_t(color.A));
        int length0=length;
        int cnt=length-_mm::size/2+1;

        for (; cnt>0; cnt-=_mm::size/2,length-=_mm::size/2,ptx+=_mm::size/2) {
            _mm::store2(plane[0].c+pty*plane[0].stride+ptx, colorY);
            _mm::store2(plane[0].r+pty*plane[0].stride+ptx, colorA);
        }
        for (; length>0; length--,ptx++) {
            plane[0].c[pty*plane[0].stride+ptx] = (uint8_t)color.Y;
            plane[0].r[pty*plane[0].stride+ptx] = (uint8_t)color.A;
        }

        if (pty&1) {
            _mm::empty();
            return;
        }

        // Reset values for next planes
        length=length0;
        ptx=pt.x;

        ptx /= 2;
        pty /= 2;
        length /= 2;
        cnt=length-_mm::size/2+1;
        for (; cnt>0; cnt-=_mm::size/2,length-=_mm::size/2,ptx+=_mm::size/2) {
            _mm::store2(plane[1].c+pty*plane[1].stride+ptx, colorV);
            _mm::store2(plane[1].r+pty*plane[1].stride+ptx, colorA);
            _mm::store2(plane[2].c+pty*plane[2].stride+ptx, colorU);
            _mm::store2(plane[2].r+pty*plane[2].stride+ptx, colorA);
        }
        for (; length>0; length--,ptx++) {
            plane[1].c[pty*plane[1].stride+ptx] = (uint8_t)color.V;
            plane[1].r[pty*plane[1].stride+ptx] = (uint8_t)color.A;
            plane[2].c[pty*plane[2].stride+ptx] = (uint8_t)color.U;
            plane[2].r[pty*plane[2].stride+ptx] = (uint8_t)color.A;
        }
    } else {
        // The following gives ARGB,ARGB,ARGB,ARGB on the 128 bits register
        //typename _mm::__m colorRGB=_mm::set_pi32(((color.A<<24)|color.m_rgb),((color.A<<24)|color.m_rgb));
        // The following gives 0RGB,0RGB,0RGB,0RGB on the 128 bits register (libswscale problem with alpha scaling)
        typename _mm::__m colorRGB=_mm::set_pi32(color.m_rgb,color.m_rgb);
        typename _mm::__m alpha=_mm::set_pi32(((color.A<<16)|(color.A<<8)|color.A),((color.A<<16)|(color.A<<8)|color.A));
        /* We write 128 by 128 bits (=_mm::size=16 bytes) which represents 4 pixels at a time in RGB32
           so we can write cnt times =  using SSE instructions */
        int cnt=length-_mm::size/4+1;

        for (; cnt>0; cnt-=_mm::size/4,length-=_mm::size/4,ptx+=_mm::size/4) {
            _mm::storeU(plane[0].c+pty*plane[0].stride+ptx*4, colorRGB);
            _mm::storeU(plane[0].r+pty*plane[0].stride+ptx*4, alpha);
        }

        for (; length>0; length--,ptx++) {
            *(uint32_t*)(plane[0].c+pty*plane[0].stride+ptx*4) = color.m_rgb;//(color.A<<24|color.m_rgb);
            *(uint32_t*)(plane[0].r+pty*plane[0].stride+ptx*4) = (color.A<<16)|(color.A<<8)|color.A;
        }
    }
    _mm::empty();
}




void TsubtitleDVD::drawPixels(CPoint pt,int len,const YUVcolorA &c,const CRect &rc,CRect &rectReal,TspuPlane plane[3],bool skipEdge) const
{
    if (pt.y < rc.top || pt.y >= rc.bottom) {
        return;
    }
    if (pt.x < rc.left) {
        len -= rc.left - pt.x;
        pt.x = rc.left;
    }
    if (pt.x + len > rc.right) {
        len = rc.right - pt.x;
    }
    if (len <= 0 || pt.x >= rc.right) {
        return;
    }

    if (c.A==0) {
        if (IsRectEmpty(&rc)) {
            return;
        }

        if (pt.y < rc.top || pt.y >= rc.bottom  || pt.x+len < rc.left || pt.x >= rc.right) {
            return;
        }
    }

    int y=pt.y-rc.top;
    if (c.A!=0 && (!skipEdge || (y!=rc.top && y!=rc.bottom-1))) {
        if (y<rectReal.top) {
            rectReal.top=y;
        }
        if (y>rectReal.bottom) {
            rectReal.bottom=y;
        }
    }
    /*if (Tconfig::cpu_flags&FF_CPU_SSE2)
     drawPixelSimd<Tsse2>(CPoint(pt.x - rc.left, y),c,len,rc,rectReal,plane,skipEdge);
    else
     drawPixelSimd<Tmmx>(CPoint(pt.x - rc.left, y),c,len,rc,rectReal,plane,skipEdge);
     pt.x+=len;*/

    while (len-->0) {
        drawPixel(CPoint(pt.x - rc.left, y),c, rc, rectReal,plane, skipEdge);
        pt.x++;
    }
}

TspuImage* TsubtitleDVD::createNewImage(const TspuPlane src[3],const CRect &rcclip,CRect rectReal, CRect finalRect,
                                        const TprintPrefs &prefs)
{
    TspuImage *image = NULL;
    lines.clear();
    //rectReal.top++;
    //rectReal.bottom++;
    //rectReal.right++;
    if (Tconfig::cpu_flags&FF_CPU_SSE2) {
        image=new TspuImageSimd<Tsse2>(src,rcclip,rectReal,parent->rectOrig, finalRect, prefs, prefs.csp);
    } else {
        image=new TspuImageSimd<Tmmx>(src,rcclip,rectReal,parent->rectOrig, finalRect, prefs, prefs.csp);
    }
    lines.push_back(new TrenderedSubtitleLine(image));
    return image;
}

TspuImage* TsubtitleDVD::createNewImage(const TspuPlane src[3],const CRect &rcclip,CRect rectReal,const TprintPrefs &prefs)
{
    TspuImage *image = NULL;
    lines.clear();
    //rectReal.top++;
    //rectReal.bottom++;
    //rectReal.right++;
    if (Tconfig::cpu_flags&FF_CPU_SSE2) {
        image=new TspuImageSimd<Tsse2>(src,rcclip,rectReal,parent->rectOrig,prefs, prefs.csp);
    } else {
        image=new TspuImageSimd<Tmmx>(src,rcclip,rectReal,parent->rectOrig,prefs, prefs.csp);
    }
    lines.push_back(new TrenderedSubtitleLine(image));
    return image;
}

void TsubtitleDVD::createImage(const TspuPlane src[3],const CRect &rcclip,CRect rectReal,const TprintPrefs &prefs) const
{
    lines.clear();
    rectReal.bottom++;
    rectReal.right++;
    if (Tconfig::cpu_flags&FF_CPU_SSE2) {
        image=new TspuImageSimd<Tsse2>(src,rcclip,rectReal,parent->rectOrig,prefs,prefs.csp);
    } else {
        image=new TspuImageSimd<Tmmx>(src,rcclip,rectReal,parent->rectOrig,prefs,prefs.csp);
    }
    lines.push_back(new TrenderedSubtitleLine(image));
}

void TsubtitleDVD::linesprint(
    const TprintPrefs &prefs,
    unsigned char **dst,
    const stride_t *stride) const
{
    if (prefs.dvd || !prefs.vobchangeposition) {
        image->ownprint(prefs,dst,stride);
    } else {
        lines.print(prefs,dst,stride);
    }
}

void TsubtitleDVD::print(
    REFERENCE_TIME time,
    bool wasseek,
    Tfont &f,
    bool forceChange,
    TprintPrefs &prefs,
    unsigned char **dst,
    const stride_t *stride)
{
    csp = prefs.csp & FF_CSPS_MASK;
    DPRINTF(_l("TsubtitleDVD::print"));
    if (this->offset[0]==DWORD(-1)) {
        return;
    }

    if (lines.empty() || changed) {
        changed=false;

        const BYTE *p=&*data.begin();
        DWORD offset[2]= {this->offset[0],this->offset[1]};

        AM_PROPERTY_SPHLI sphli=this->sphli;
        CPoint pt(sphli.StartX, sphli.StartY);
        CRect rc(pt, CPoint(sphli.StopX, sphli.StopY));

        if (parent->rectOrig.dx==0) {
            parent->rectOrig = Trect(0,0,prefs.dx,prefs.dy);
        }
        CRect rcclip=parent->rectOrig;
        rcclip&=rc;

        if (psphli) {
            rcclip&=CRect(psphli->StartX,psphli->StartY,psphli->StopX,psphli->StopY);
            sphli=*psphli;
        }

        TspuPlane *planes=parent->allocPlanes(rcclip, prefs.csp);
        DPRINTF(_l("rect: [%i,%i] - [%i,%i]"),rcclip.left,rcclip.top,rcclip.Width(),rcclip.Height());

        YUVcolorA pal[4];
        pal[0]=parent->sppal[parent->fsppal?sphli.ColCon.backcol :0];
        pal[0].setAlpha(sphli.ColCon.backcon*16);
        pal[1]=parent->sppal[parent->fsppal?sphli.ColCon.patcol  :1];
        pal[1].setAlpha(sphli.ColCon.patcon*16);
        pal[2]=parent->sppal[parent->fsppal?sphli.ColCon.emph1col:2];
        pal[2].setAlpha(sphli.ColCon.emph1con*16);
        pal[3]=parent->sppal[parent->fsppal?sphli.ColCon.emph2col:3];
        pal[3].setAlpha(sphli.ColCon.emph2con*16);

        int nField=0;
        int fAligned=1;

        DWORD end[2]= {offset[1],(p[2]<<8)|p[3]};

        CRect rectReal(INT_MAX/2,INT_MAX/2,INT_MIN/2,INT_MIN/2);
        while ((nField==0 && offset[0]<end[0]) || (nField==1 && offset[1]<end[1])) {
            DWORD code;
            if ((code=getNibble(p,offset,nField,fAligned))>=0x4 ||
                    (code=(code<<4) | getNibble(p,offset,nField,fAligned))>=0x10 ||
                    (code=(code<<4) | getNibble(p,offset,nField,fAligned))>=0x40 ||
                    (code=(code<<4) | getNibble(p,offset,nField,fAligned))>=0x100) {
                drawPixels(pt,code>>2,pal[code&3],rcclip,rectReal,planes);
                if ((pt.x+=code>>2)<rc.right) {
                    continue;
                }
            }

            drawPixels(pt,rc.right-pt.x,pal[code&3],rcclip,rectReal,planes);

            if (!fAligned) {
                getNibble(p,offset,nField,fAligned);    // align to byte
            }

            pt.x=rc.left;
            pt.y++;
            nField=1-nField;
        }
        if (csp == FF_CSP_420P) {
            rectReal.left &= ~1;
            rectReal.top &= ~1;
        }
        createImage(planes,rcclip,rectReal,prefs);
    }
    linesprint(prefs,dst,stride);
}

void TsubtitleDVD::append(const unsigned char *Idata,unsigned int Idatalen)
{
    data.append(Idata,Idatalen);
    parse();
}

bool TsubtitleDVD::parse(void)
{
    BYTE *p=&*data.begin();
    DPRINTF(_l("TsubtitleDVD::parse"));

    WORD packetsize=short((p[0]<<8)|p[1]);
    WORD datasize=short((p[2]<<8)|p[3]);

    if (packetsize>data.size() || datasize>packetsize) {
        return false;
    }

    int i,next=datasize;

    do {
        i=next;
        uint32_t pts=(p[i]<<8)|p[i+1];
        i+=2;
        next=(p[i]<<8)|p[i+1];
        i+=2;

        if (next>packetsize || next<datasize) {
            return false;
        }

        for (bool fBreak=false; !fBreak;) {
            int len;
            switch(p[i]) {
                case 0x00:
                    len=0;
                    break;
                case 0x01:
                    len=0;
                    break;
                case 0x02:
                    len=0;
                    break;
                case 0x03:
                    len=2;
                    break;
                case 0x04:
                    len=2;
                    break;
                case 0x05:
                    len=6;
                    break;
                case 0x06:
                    len=4;
                    break;
                default  :
                    len=0;
                    break;
            }
            if (i+len>=packetsize) {
                DPRINTF(_l("Warning: Wrong subpicture parameter block ending"));
                break;
            }

            switch (p[i++]) {
                case 0x00: // forced start displaying
                    forced=true;
                    break;
                case 0x01: // normal start displaying
                    forced=false;
                    break;
                case 0x02: // stop displaying
                    stop=start+1024*pts2rt(pts);
                    break;
                case 0x03:
                    sphli.ColCon.emph2col=p[i]>>4;
                    sphli.ColCon.emph1col=p[i]&0xf;
                    sphli.ColCon.patcol =p[i+1]>>4;
                    sphli.ColCon.backcol=p[i+1]&0xf;
                    i += 2;
                    break;
                case 0x04:
                    sphli.ColCon.emph2con=p[i]>>4;
                    sphli.ColCon.emph1con=p[i]&0xf;
                    sphli.ColCon.patcon =p[i+1]>>4;
                    sphli.ColCon.backcon=p[i+1]&0xf;
                    i += 2;
                    break;
                case 0x05:
                    sphli.StartX=USHORT((p[i]<<4) + (p[i+1]>>4));
                    sphli.StopX =USHORT(((p[i+1]&0x0f)<<8) + p[i+2]+1);
                    sphli.StartY=USHORT((p[i+3]<<4) + (p[i+4]>>4));
                    sphli.StopY =USHORT(((p[i+4]&0x0f)<<8) + p[i+5]+1);
                    i += 6;
                    break;
                case 0x06:
                    offset[0]=(p[i]<<8)|p[i+1];
                    i+=2;
                    offset[1]=(p[i]<<8)|p[i+1];
                    i+=2;
                    break;
                case 0xff: // end of ctrlblk
                    fBreak=true;
                    continue;
                default: // skip this ctrlblk
                    fBreak=true;
                    break;
            }
        }
    } while (i<=next && i<packetsize);
    return true;
}

//===================================== TsubtitleSVCD =====================================
bool TsubtitleSVCD::parse(void)
{
    BYTE *p=&*data.begin(),*p0=p;
    if (data.size()<2) {
        return false;
    }

    WORD packetsize=WORD((p[0]<<8)|p[1]);
    p += 2;

    if (packetsize>data.size()) {
        return false;
    }

    bool duration=!!(*p++&0x04);

    *p++; // unknown

    if(duration) {
        stop=start+10000LL*((p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3])/90;
        p+=4;
    }
    sphli.StartX=sphli.StopX=USHORT((p[0]<<8)|p[1]);
    p+=2;
    sphli.StartY=sphli.StopY=USHORT((p[0]<<8)|p[1]);
    p+=2;
    sphli.StopX=USHORT(sphli.StopX+(p[0]<<8)|p[1]);
    p+=2;
    sphli.StopY=USHORT(sphli.StopY+(p[0]<<8)|p[1]);
    p+=2;

    for (int i=0; i<4; i++) {
        sppal[i].Y=*p++;
        sppal[i].U=*p++;
        sppal[i].V=*p++;
        sppal[i].setAlpha(UCHAR(*p++>>4)*16);
    }

    if (*p++&0xc0) {
        p+=4;    // duration of the shift operation should be here, but it is untested
    }

    offset[1]=(p[0]<<8)|p[1];
    p+=2;

    offset[0]=DWORD(p-p0);
    offset[1]+=offset[0];

    return true;
}
void TsubtitleSVCD::print(
    REFERENCE_TIME time,
    bool wasseek,
    Tfont &f,
    bool forceChange,
    const TprintPrefs &prefs,
    unsigned char **dst,
    const stride_t *stride)
{
    csp = prefs.csp & FF_CSPS_MASK;
    if (lines.empty() || changed) {
        changed=false;

        const BYTE *p=&*data.begin();
        DWORD offset[2]= {this->offset[0],this->offset[1]};

        CRect rcclip(0,0,prefs.dx,prefs.dy);

        /* FIXME: startx/y looks to be wrong in the sample I tested (yes, this one too!)
        CPoint pt(m_sphli.StartX, m_sphli.StartY);
        CRect rc(pt, CPoint(m_sphli.StopX, m_sphli.StopY));
        */

        CSize size(sphli.StopX - sphli.StartX, sphli.StopY - sphli.StartY);
        CPoint pt((rcclip.Width() - size.cx) / 2, (rcclip.Height()*3 - size.cy*1) / 4);
        CRect rc(pt, size);

        int nField=0;
        int n=3;

        DWORD end[2]= {offset[1],(p[2]<<8)|p[3]};

        CRect rectReal(INT_MAX/2,INT_MAX/2,INT_MIN/2,INT_MIN/2);
        TspuPlane *planes=parent->allocPlanes(rcclip, prefs.csp);
        while ((nField==0 && offset[0]<end[0]) || (nField==1 && offset[1]<end[1])) {
            BYTE code=getHalfNibble(p,offset,nField,n);
            BYTE repeat=BYTE(1+(code==0?getHalfNibble(p,offset,nField,n):0));

            drawPixels(pt,repeat,sppal[code&3],rcclip,rectReal,planes);
            if ((pt.x+=repeat)<rc.right) {
                continue;
            }
            while(n!=3) {
                getHalfNibble(p,offset,nField,n);    // align to byte
            }
            pt.x=rc.left;
            pt.y++;
            nField=1-nField;
        }
        createImage(planes,rcclip,rectReal,prefs);
    }
    linesprint(prefs,dst,stride);
}

//===================================== TsubtitleCVD ======================================
bool TsubtitleCVD::parse(void)
{
    const BYTE *p=&*data.begin();
    WORD packetsize=WORD((p[0]<<8)|p[1]);
    WORD datasize=WORD((p[2]<<8)|p[3]);

    if (packetsize>data.size() || datasize>packetsize) {
        return false;
    }

    p=&*data.begin()+datasize;

    for (int i=datasize,j=packetsize-4; i<=j; i+=4,p+=4)
        switch(p[0]) {
            case 0x0c:
                break;
            case 0x04:
                stop=start+10000LL*((p[1]<<16)|(p[2]<<8)|p[3])/90;
                break;
            case 0x17:
                sphli.StartX=USHORT(((p[1]&0x0f)<<6) + (p[2]>>2));
                sphli.StartY=USHORT(((p[2]&0x03)<<8) + p[3]);
                break;
            case 0x1f:
                sphli.StopX=USHORT(((p[1]&0x0f)<<6) + (p[2]>>2));
                sphli.StopY=USHORT(((p[2]&0x03)<<8) + p[3]);
                break;
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
                sppal[0][p[0]-0x24].Y=p[1];
                sppal[0][p[0]-0x24].U=p[2];
                sppal[0][p[0]-0x24].V=p[3];
                break;
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
                sppal[1][p[0]-0x2c].Y=p[1];
                sppal[1][p[0]-0x2c].U=p[2];
                sppal[1][p[0]-0x2c].V=p[3];
                break;
            case 0x37:
                sppal[0][3].setAlpha(UCHAR(p[2]>>4)*16);
                sppal[0][2].setAlpha(UCHAR(p[2]&0xf)*16);
                sppal[0][1].setAlpha(UCHAR(p[3]>>4)*16);
                sppal[0][0].setAlpha(UCHAR(p[3]&0xf)*16);
                break;
            case 0x3f:
                sppal[1][3].setAlpha(UCHAR(p[2]>>4)*16);
                sppal[1][2].setAlpha(UCHAR(p[2]&0xf)*16);
                sppal[1][1].setAlpha(UCHAR(p[3]>>4)*16);
                sppal[1][0].setAlpha(UCHAR(p[3]&0xf)*16);
                break;
            case 0x47:
                offset[0] = (p[2]<<8)|p[3];
                break;
            case 0x4f:
                offset[1] = (p[2]<<8)|p[3];
                break;
            default:
                break;
        }
    return true;
}
void TsubtitleCVD::print(
    REFERENCE_TIME time,
    bool wasseek,
    Tfont &f,
    bool forceChange,
    const TprintPrefs &prefs,
    unsigned char **dst,
    const stride_t *stride)
{
    csp = prefs.csp & FF_CSPS_MASK;
    if (lines.empty() || changed) {
        changed=false;

        const BYTE *p=&*data.begin();
        DWORD offset[2]= {this->offset[0],this->offset[1]};

        CRect rcclip(0,0,prefs.dx,prefs.dy);

        /* FIXME: startx/y looks to be wrong in the sample I tested
        CPoint pt(m_sphli.StartX, m_sphli.StartY);
        CRect rc(pt, CPoint(m_sphli.StopX, m_sphli.StopY));
        */

        CSize size(sphli.StopX-sphli.StartX,sphli.StopY-sphli.StartY);
        CPoint pt((rcclip.Width() - size.cx) / 2, (rcclip.Height()*3 - size.cy*1) / 4);
        CRect rc(pt,size);

        int nField=0;
        int fAligned=1;

        DWORD end[2]= {offset[1],(p[2]<<8)|p[3]};

        CRect rectReal(INT_MAX/2,INT_MAX/2,INT_MIN/2,INT_MIN/2);
        TspuPlane *planes=parent->allocPlanes(rcclip, prefs.csp);
        while ((nField==0 && offset[0]<end[0]) || (nField==1 && offset[1]<end[1])) {
            BYTE code;

            if ((code=getNibble(p,offset,nField,fAligned))>=0x4) {
                drawPixels(pt,code>>2,sppal[0][code&3],rcclip,rectReal,planes);
                pt.x+=code>>2;
                continue;
            }

            code=getNibble(p,offset,nField,fAligned);
            drawPixels(pt,rc.right-pt.x,sppal[0][code&3],rcclip,rectReal,planes);

            if (!fAligned) {
                getNibble(p,offset,nField,fAligned);    // align to byte
            }

            pt.x=rc.left;
            pt.y++;
            nField=1-nField;
        }
        createImage(planes,rcclip,rectReal,prefs);
    }
    linesprint(prefs,dst,stride);
}
