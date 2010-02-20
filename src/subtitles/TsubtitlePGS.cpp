/*
 * Copyright (c) 2004-2010 Damien Bain-Thouverez
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
#include "Tstream.h"
#include "Tconfig.h"
#include "TglobalSettings.h"
#include "ffdshow_mediaguids.h"
#include "TcodecSettings.h"
#include "rational.h"
#include "line.h"
#include "simd.h"
#include "TimgFilterSubtitles.h"
#include "TsubtitlePGS.h"
#include "TffPict.h"

TsubtitlePGS::TsubtitlePGS(IffdshowBase *Ideci,REFERENCE_TIME Istart, REFERENCE_TIME Istop, TcompositionObject *IpCompositionObject, TsubtitleDVDparent *Iparent):
 TsubtitleDVD(Istart, Iparent),
 deci(Ideci),
 m_pCompositionObject(IpCompositionObject),
 stride(0),
 ownimage(NULL)
{
 assert(m_pCompositionObject != NULL);
 start =Istart;
 stop = Istop;
 bitmap=NULL;
 readContext();
}

TsubtitlePGS::~TsubtitlePGS()
{
 if (m_pCompositionObject) delete m_pCompositionObject;
 //ownimage is freed by TsubtitleLines
}

void TsubtitlePGS::readContext(void)
{
 start=m_pCompositionObject->m_rtStart;
 stop=m_pCompositionObject->m_rtStop;
 stride=m_pCompositionObject->m_width;
 centerPoint = CPoint(m_pCompositionObject->m_horizontal_position+m_pCompositionObject->m_width/2,
  m_pCompositionObject->m_vertical_position+m_pCompositionObject->m_height/2); 
 size = CSize(m_pCompositionObject->m_width, m_pCompositionObject->m_height);
 videoWidth=m_pCompositionObject->m_pVideoDescriptor->nVideoWidth;
 videoHeight=m_pCompositionObject->m_pVideoDescriptor->nVideoHeight;
}



void TsubtitlePGS::print(
    REFERENCE_TIME time,
    bool wasseek,
    Tfont &f,
    bool forceChange,
    TprintPrefs &prefs,
    unsigned char **dst,
    const stride_t *stride)
{
 DPRINTF(_l("TsubtitlePGS::print %I64i"),start);

 //TsubPrintPrefs *subPrefs = (TsubPrintPrefs*)&prefs;

 csp = prefs.csp & FF_CSPS_MASK;
 if (ownimage == NULL || forceChange)
 {
  parent->rectOrig = Trect(0,0,prefs.dx,prefs.dy);
  if (videoWidth==0) videoWidth=prefs.dx;
  if (videoHeight==0) videoHeight=prefs.dy;

  prefs.pgs=true;
  // Calculate scale according to : 
  // 1/ difference of size between original and output size
  // 2/ scaling preference
  int scale100=(int)100*((float)prefs.dx/videoWidth);
  prefs.pgsscale=(int)prefs.vobscale*scale100/100;
  scale100=(int)((float)scale100*prefs.vobscale/256);

  // Recalculate the coordinates proportionally
  centerPoint.x=(int)((float)centerPoint.x*scale100/100);
  centerPoint.y=(int)((float)centerPoint.y*scale100/100);
  
  CSize newSize(size.cx*scale100/100, size.cy*scale100/100);

  // Starting point of our subtitles with the new coordinates
  CPoint pt(centerPoint.x-newSize.cx/2, centerPoint.y-newSize.cy/2);
  if (pt.x + newSize.cx > (LONG)prefs.dx) pt.x = prefs.dx-newSize.cx - 2;
  if (pt.y + newSize.cy > (LONG)prefs.dy) pt.y = prefs.dy-newSize.cy - 2;
  if (pt.x<0) pt.x=0;
  if (pt.y<0) pt.y=0;
  
  // Rectangle of our subtitles with the new position and new size
  CRect rcclip(pt, newSize);

  // Rectangle of our subtitles with the new position but original size
  CRect rc(pt, size);


  // Real size of out subtitles rectangle after it has been reduced (due to transparent aeras) = cropping rectangle
  CRect rectReal(INT_MAX/2,INT_MAX/2,INT_MIN/2,INT_MIN/2);

  // TspuPlane is size to the output picture because our subs rectangle can be resized up to it
  TspuPlane *planes=parent->allocPlanes(rcclip, prefs.csp);
  
  Tbitdata bitdata = Tbitdata(&m_pCompositionObject->data[0], m_pCompositionObject->data.size());
  BYTE			bTemp;
  BYTE			bSwitch;
  int nPaletteIndex = 0; //Index of RGBA color
  int nCount = 0; // Repetition count of the pixel

  pt.x=rc.left;
  pt.y=rc.top;

  while (pt.y<rc.bottom && bitdata.bitsleft>0)
  {
   bTemp = bitdata.readByte();
   if (bTemp != 0)
		 {
			 nPaletteIndex = bTemp;
			 nCount = 1;
		 }
		 else
		 {
			 bSwitch = bitdata.readByte();
			 if (!(bSwitch & 0x80))
			 {
				 if (!(bSwitch & 0x40))
				 {
					 nCount		= bSwitch & 0x3F;
					 if (nCount > 0)
						 nPaletteIndex	= 0;
				 }
				 else
				 {
					 nCount			= (bSwitch&0x3F) <<8 | (SHORT)bitdata.readByte();
					 nPaletteIndex	= 0;
				 }
			 }
			 else
			 {
				 if (!(bSwitch & 0x40))
				 {
					 nCount			= bSwitch & 0x3F;
					 nPaletteIndex	= bitdata.readByte();
				 }
				 else
				 {
					 nCount			= (bSwitch&0x3F) <<8 | (SHORT)bitdata.readByte();
					 nPaletteIndex	= bitdata.readByte();
				 }
			 }
		 }

   // 1 or more pixels to fill with the palette index
		 if (nCount>0) // Fill this series of pixels on this line
		 {
			 if (nPaletteIndex != 0xFF)		// 255 = Fully transparent
    {
     uint32_t color = m_pCompositionObject->m_Colors[nPaletteIndex];
     unsigned char alpha = (color>>24)&0xFF;
     //if (alpha>0)
     {
      YUVcolorA c(RGB((color>>16)&0xFF,
       (color>>8)&0xFF,
       (color)&0xFF), alpha);
      drawPixels(pt,nCount,c,rcclip,rectReal,planes);
     }
    }
			 pt.x += nCount;
		 }
		 else // This line is fully filled
		 {
    pt.y++;
    pt.x = rc.left;
		 }
	 }
  // Don't free the composition object : it may be needed if the subtitles settings change
  /* // Free the composition object, not used anymore
  delete m_pCompositionObject;
  m_pCompositionObject = NULL;*/
  rectReal.top++;

  DPRINTF(_l("TsubtitlePGS::print Build image (left,right,top,bottom)=(%ld,%ld,%ld,%ld)\nParent rect (%ld,%ld,%ld,%ld)"),rectReal.left, rectReal.right, rectReal.top, rectReal.bottom, rcclip.left, rcclip.right, rcclip.top, rcclip.bottom);
  ownimage=createNewImage(planes, rcclip, rectReal, prefs);
 }
 ownimage->ownprint(prefs, dst, stride);
}
 