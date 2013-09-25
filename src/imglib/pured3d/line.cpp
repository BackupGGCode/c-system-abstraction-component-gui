/*
 *  Crafted by Jim Buckeyne
 *   (c)1999-2006++ Freedom Collective
 *
 * Simple Line operations on Images.  Single pixel, no anti aliasing.
 *  There is a line routine which steps through each point and calls
 *  a user defined function, which may be used to extend straight lines
 *  with a smarter antialiasing renderer. (isntead of plot/putpixel)
 *
 *
 *
 *  consult doc/image.html
 *
 */


#ifndef IMAGE_LIBRARY_SOURCE
#define IMAGE_LIBRARY_SOURCE
#endif
#define LIBRARY_DEF
#define NO_OPEN_MACRO
#define FIX_RELEASE_COM_COLLISION
#include <stdhdrs.h>

#include <d3d9.h>
#include <imglib/imagestruct.h>
#include <image.h>

#include "local.h"
#include "blotproto.h"

/* void do_line(BITMAP *bmp, int x1, y1, x2, y2, int d, void (*proc)())
 *  Calculates all the points along a line between x1, y1 and x2, y2,
 *  calling the supplied function for each one. This will be passed a
 *  copy of the bmp parameter, the x and y position, and a copy of the
 *  d parameter (so do_line() can be used with putpixel()).
 */
#ifdef __cplusplus
namespace sack {
	namespace image {
extern "C" {
#endif

//unsigned long DOALPHA( unsigned long over, unsigned long in, unsigned long a );

#define FIX_SHIFT 18
#define ROUND_ERROR ( ( 1<< ( FIX_SHIFT - 1 ) ) - 1 )


void CPROC do_linec( ImageFile *pImage, int x1, int y1
						 , int x2, int y2, int d )
{
	if( pImage->flags & IF_FLAG_FINAL_RENDER )
	{
		int glDepth = 1;
		VECTOR v1[2], v2[2];
		VECTOR v3[2], v4[2];
		VECTOR slope;
		RCOORD tmp;
		VECTOR normal;
		int v = 0;

		TranslateCoord( pImage, (S_32*)&x1, (S_32*)&y1 );
		TranslateCoord( pImage, (S_32*)&x2, (S_32*)&y2 );
		v1[v][0] = x1;
		v1[v][1] = y1;
		v1[v][2] = 0.0;
		v2[v][0] = x2;
		v2[v][1] = y2;
		v2[v][2] = 0.0;

		sub( slope, v2[v], v1[v] );
		normalize( slope );
		tmp = slope[0];
		slope[0] = -slope[1];
		slope[1] = tmp;

		addscaled( v1[v], v1[v], slope, -0.5 );
		addscaled( v4[v], v1[v], slope, 1.0 );
		addscaled( v2[v], v2[v], slope, -0.5 );
		addscaled( v3[v], v2[v], slope, 1.0 );

		while( pImage && pImage->pParent )
		{
			glDepth = 0;
			if(pImage->transform )
			{
				Apply( pImage->transform, v1[1-v], v1[v] );
				Apply( pImage->transform, v2[1-v], v2[v] );
				Apply( pImage->transform, v3[1-v], v3[v] );
				Apply( pImage->transform, v4[1-v], v4[v] );
				v = 1-v;
			}
			pImage = pImage->pParent;
		}
		if( pImage->transform )
		{
			Apply( pImage->transform, v1[1-v], v1[v] );
			Apply( pImage->transform, v2[1-v], v2[v] );
			Apply( pImage->transform, v3[1-v], v3[v] );
			Apply( pImage->transform, v4[1-v], v4[v] );
			v = 1-v;
		}

			LPDIRECT3DVERTEXBUFFER9 pQuadVB;
			#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL)

			g_d3d_device->SetFVF( D3DFVF_CUSTOMVERTEX );
			g_d3d_device->SetRenderState(D3DRS_AMBIENT,RGB(255,255,255));
			g_d3d_device->SetRenderState( D3DRS_LIGHTING,false);

			g_d3d_device->CreateVertexBuffer(sizeof( D3DVERTEX )*4,
                                      D3DUSAGE_WRITEONLY,
                                      D3DFVF_CUSTOMVERTEX,
                                      D3DPOOL_MANAGED,
                                      &pQuadVB,
                                      NULL);
			D3DVERTEX* pData;
			//lock buffer (NEW)
			pQuadVB->Lock(0,sizeof(pData),(void**)&pData,0);
			//copy data to buffer (NEW)
			{
				pData[0].fX = v1[v][vRight];
				pData[0].fY = v1[v][vUp];
				pData[0].fZ = v1[v][vForward];
				pData[0].dwColor = d;
				pData[1].fX = v2[v][vRight];
				pData[1].fY = v2[v][vUp];
				pData[1].fZ = v2[v][vForward];
				pData[1].dwColor = d;
				pData[2].fX = v4[v][vRight];
				pData[2].fY = v4[v][vUp];
				pData[2].fZ = v4[v][vForward];
				pData[2].dwColor = d;
				pData[3].fX = v3[v][vRight];
				pData[3].fY = v3[v][vUp];
				pData[3].fZ = v3[v][vForward];
				pData[3].dwColor = d;
			}
			//unlock buffer (NEW)
			pQuadVB->Unlock();
			g_d3d_device->SetStreamSource(0,pQuadVB,0,sizeof(D3DVERTEX));
			//draw quad (NEW)
			g_d3d_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
			pQuadVB->Release();

#if 0
		glBegin( GL_TRIANGLE_STRIP );
		glColor4ub( RedVal(d), GreenVal(d),BlueVal(d), 255 );
		glVertex3dv(v1[v]);	// Bottom Left Of The Texture and Quad
		glVertex3dv(v2[v]);	// Bottom Right Of The Texture and Quad
		glVertex3dv(v4[v]);	// Bottom Left Of The Texture and Quad
		glVertex3dv(v3[v]);	// Bottom Right Of The Texture and Quad
		glEnd();
#endif
	}
   else
	{
   int err, delx, dely, len, inc;
   if( !pImage || !pImage->image ) return;
   delx = x2 - x1;
   if( delx < 0 )
      delx = -delx;

   dely = y2 - y1;
   if( dely < 0 )
      dely = -dely;

   if( dely > delx ) // length for y is more than length for x
   {
      len = dely;
      if( y1 > y2 )
      {
         int tmp = x1;
         x1 = x2;
         x2 = tmp;
         y1 = y2; // x1 is start...
      }
      if( x2 > x1 )
         inc = 1;
      else
         inc = -1;

      err = -(dely / 2);
      while( len >= 0 )
      {
         plot( pImage, x1, y1, d );
         y1++;
         err += delx;
         while( err >= 0 )
         {
            err -= dely;
            x1 += inc;
         }
         len--;
      }
   }
   else
   {
      if( !delx ) // 0 length line
         return;
      len = delx;
      if( x1 > x2 )
      {
         int tmp = y1;
         y1 = y2;
         y2 = tmp;
         x1 = x2; // x1 is start...
      }
      if( y2 > y1 )
         inc = 1;
      else
         inc = -1;

      err = -(delx / 2);
      while( len >= 0 )
      {
         plot( pImage, x1, y1, d );
         x1++;
         err += dely;
         while( err >= 0 )
         {
            err -= delx;
            y1 += inc;
         }
         len--;
      }
   }
   MarkImageUpdated( pImage );
	}
}

void CPROC do_lineAlphac( ImageFile *pImage, int x1, int y1
                            , int x2, int y2, int d )
{
	if( pImage->flags & IF_FLAG_FINAL_RENDER )
	{
      int glDepth = 1;
		VECTOR v1[2], v2[2];
		VECTOR v3[2], v4[2];
		VECTOR slope;
      RCOORD tmp;
      VECTOR normal;
		int v = 0;

      TranslateCoord( pImage, (S_32*)&x1, (S_32*)&y1 );
      TranslateCoord( pImage, (S_32*)&x2, (S_32*)&y2 );
		v1[v][0] = x1;
		v1[v][1] = y1;
		v1[v][2] = 0.0;
		v2[v][0] = x2;
		v2[v][1] = y2;
		v2[v][2] = 0.0;

		sub( slope, v2[v], v1[v] );
		normalize( slope );
		tmp = slope[0];
		slope[0] = -slope[1];
		slope[1] = tmp;

      addscaled( v1[v], v1[v], slope, -0.5 );
      addscaled( v4[v], v1[v], slope, 1.0 );
      addscaled( v2[v], v2[v], slope, -0.5 );
      addscaled( v3[v], v2[v], slope, 1.0 );

		while( pImage && pImage->pParent )
		{
         glDepth = 0;
         if(pImage->transform )
			{
				Apply( pImage->transform, v1[1-v], v1[v] );
				Apply( pImage->transform, v2[1-v], v2[v] );
				Apply( pImage->transform, v3[1-v], v3[v] );
				Apply( pImage->transform, v4[1-v], v4[v] );
				v = 1-v;
			}
			pImage = pImage->pParent;
		}
		if(pImage->transform )
		{
			Apply( pImage->transform, v1[1-v], v1[v] );
			Apply( pImage->transform, v2[1-v], v2[v] );
			Apply( pImage->transform, v3[1-v], v3[v] );
			Apply( pImage->transform, v4[1-v], v4[v] );
			v = 1-v;
		}
#if 0
		if( glDepth )
			glEnable( GL_DEPTH_TEST );
		else
			glDisable( GL_DEPTH_TEST );
#endif
			LPDIRECT3DVERTEXBUFFER9 pQuadVB;
			#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL)

			g_d3d_device->SetFVF( D3DFVF_CUSTOMVERTEX );
			g_d3d_device->SetRenderState(D3DRS_AMBIENT,RGB(255,255,255));
			g_d3d_device->SetRenderState( D3DRS_LIGHTING,false);

			g_d3d_device->CreateVertexBuffer(sizeof( D3DVERTEX )*4,
                                      D3DUSAGE_WRITEONLY,
                                      D3DFVF_CUSTOMVERTEX,
                                      D3DPOOL_MANAGED,
                                      &pQuadVB,
                                      NULL);
			D3DVERTEX* pData;
			//lock buffer (NEW)
			pQuadVB->Lock(0,sizeof(pData),(void**)&pData,0);
			//copy data to buffer (NEW)
			{
				pData[0].fX = v1[v][vRight];
				pData[0].fY = v1[v][vUp];
				pData[0].fZ = v1[v][vForward];
				pData[0].dwColor = d;
				pData[1].fX = v2[v][vRight];
				pData[1].fY = v2[v][vUp];
				pData[1].fZ = v2[v][vForward];
				pData[1].dwColor = d;
				pData[2].fX = v4[v][vRight];
				pData[2].fY = v4[v][vUp];
				pData[2].fZ = v4[v][vForward];
				pData[2].dwColor = d;
				pData[3].fX = v3[v][vRight];
				pData[3].fY = v3[v][vUp];
				pData[3].fZ = v3[v][vForward];
				pData[3].dwColor = d;
			}
			//unlock buffer (NEW)
			pQuadVB->Unlock();
			g_d3d_device->SetStreamSource(0,pQuadVB,0,sizeof(D3DVERTEX));
			//draw quad (NEW)
			g_d3d_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
			pQuadVB->Release();
#if 0
      glBegin( GL_TRIANGLE_STRIP );
		glColor4ub( RedVal(d), GreenVal(d),BlueVal(d), AlphaVal( d ) );
		glVertex3dv(v1[v]);	// Bottom Left Of The Texture and Quad
		glVertex3dv(v2[v]);	// Bottom Right Of The Texture and Quad
		glVertex3dv(v4[v]);	// Bottom Left Of The Texture and Quad
		glVertex3dv(v3[v]);	// Bottom Right Of The Texture and Quad
		glEnd();
#endif
	}
   else
	{
   int err, delx, dely, len, inc;
   if( !pImage || !pImage->image ) return;
   delx = x2 - x1;
   if( delx < 0 )
      delx = -delx;

   dely = y2 - y1;
   if( dely < 0 )
      dely = -dely;

   if( dely > delx ) // length for y is more than length for x
   {
      len = dely;
      if( y1 > y2 )
      {
         int tmp = x1;
         x1 = x2;
         x2 = tmp;
         y1 = y2; // x1 is start...
      }
      if( x2 > x1 )
         inc = 1;
      else
         inc = -1;

      err = -(dely / 2);
      while( len >= 0 )
      {
         plotalpha( pImage, x1, y1, d );
         y1++;
         err += delx;
         while( err >= 0 )
         {
            err -= dely;
            x1 += inc;
         }
         len--;
      }
   }
   else
   {
      if( !delx ) // 0 length line
         return;
      len = delx;
      if( x1 > x2 )
      {
         int tmp = y1;
         y1 = y2;
         y2 = tmp;
         x1 = x2; // x1 is start...
      }
      if( y2 > y1 )
         inc = 1;
		else
			inc = -1;

			err = -(delx / 2);
			while( len >= 0 )
			{
				plotalpha( pImage, x1, y1, d );
				x1++;
				err += dely;
				while( err >= 0 )
				{
					err -= delx;
					y1 += inc;
				}
				len--;
			}
		}
		MarkImageUpdated( pImage );
	}
}

void CPROC do_lineExVc( ImageFile *pImage, int x1, int y1
                            , int x2, int y2, int d
                            , void (*func)(ImageFile *pif, int x, int y, int d ) )
{
   int err, delx, dely, len, inc;
   //if( !pImage || !pImage->image ) return;
   delx = x2 - x1;
   if( delx < 0 )
      delx = -delx;

   dely = y2 - y1;
   if( dely < 0 )
      dely = -dely;

   if( dely > delx ) // length for y is more than length for x
   {
      len = dely;
      if( y1 > y2 )
      {
         int tmp = x1;
         x1 = x2;
         x2 = tmp;
         y1 = y2; // x1 is start...
      }
      if( x2 > x1 )
         inc = 1;
      else
         inc = -1;

      err = -(dely / 2);
      while( len >= 0 )
      {
         func( pImage, x1, y1, d );
         y1++;
         err += delx;
         while( err >= 0 )
         {
            err -= dely;
            x1 += inc;
         }
         len--;
      }
   }
   else
   {
      if( !delx ) // 0 length line
         return;
      len = delx;
      if( x1 > x2 )
      {
         int tmp = y1;
         y1 = y2;
         y2 = tmp;
         x1 = x2; // x1 is start...
      }
      if( y2 > y1 )
         inc = 1;
      else
         inc = -1;

      err = -(delx / 2);
      while( len >= 0 )
      {
         func( pImage, x1, y1, d );
         x1++;
         err += dely;
         while( err >= 0 )
         {
            err -= delx;
            y1 += inc;
         }
         len--;
      }
   // pImageFile is not nesseciarily an image; do not set updated.
	}
}

void CPROC do_hlinec( ImageFile *pImage, int y, int xfrom, int xto, CDATA color )
{
   BlatColor( pImage, xfrom, y, xto-xfrom, 1, color );
   //do_linec( pImage, xfrom, y, xto, y, color );
}

void CPROC do_vlinec( ImageFile *pImage, int x, int yfrom, int yto, CDATA color )
{
   BlatColor( pImage, x, yfrom, 1, yto-yfrom, color );
   //do_linec( pImage, x, yfrom, x, yto, color );
}

void CPROC do_hlineAlphac( ImageFile *pImage, int y, int xfrom, int xto, CDATA color )
{
	if( xfrom < xto )
		BlatColorAlpha( pImage, xfrom, y, xto-xfrom, 1, color );
	else
		BlatColorAlpha( pImage, xfrom, y, xfrom-xto, 1, color );
}

void CPROC do_vlineAlphac( ImageFile *pImage, int x, int yfrom, int yto, CDATA color )
{
	if( yto > yfrom )
		BlatColorAlpha( pImage, x, yfrom, 1, yto-yfrom, color );
	else
		BlatColorAlpha( pImage, x, yfrom, 1, yfrom-yto, color );
}

#ifdef __cplusplus
		}; //extern "C" {
	}; //	namespace image {
}; //namespace sack {
#endif

// $Log: line.c,v $
// Revision 1.14  2004/06/21 07:47:13  d3x0r
// Account for newly moved structure files.
//
// Revision 1.13  2003/07/24 16:56:41  panther
// Updates to expliclity define C procedure model for callbacks and assembly modules - incomplete
//
// Revision 1.12  2003/03/31 01:11:28  panther
// Tweaks to work better under service application
//
// Revision 1.11  2003/03/30 18:18:02  panther
// More clip fixes
//
// Revision 1.10  2003/03/30 06:24:56  panther
// Turns out I had badly implemented clipping...
//
// Revision 1.9  2003/03/27 10:50:59  panther
// Display - enable resize that works.  Image - remove hline failed message.  Display - Removed some logging messages.
//
// Revision 1.8  2003/03/25 23:35:59  panther
// Base INVERT_IMAGE off off real_height.  Also updated to use more base ComputeImageData
//
// Revision 1.7  2003/03/25 08:45:51  panther
// Added CVS logging tag
//
