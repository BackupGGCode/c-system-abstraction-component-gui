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


#define IMAGE_LIBRARY_SOURCE
#define LIBRARY_DEF
#include <imglib/imagestruct.h>
#include <image.h>

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
}

void CPROC do_lineAlphac( ImageFile *pImage, int x1, int y1
                            , int x2, int y2, int d )
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
   }
}

void CPROC do_hlinec( ImageFile *pImage, int y, int xfrom, int xto, CDATA color )
{
   PCDATA po;
   int len;
   if( !pImage || !pImage->image ) return;

   if( y < pImage->y || y >= (pImage->y + pImage->height ))
   {
      //Log4( WIDE("hline failed: %d<%d or %d>%d"), y, pImage->y, y, pImage->y+pImage->height );
      return;
   }

   if( xfrom > xto )
   {
      int tmp = xto;
      xto = xfrom;
      xfrom = tmp;
   }
   if( xto < pImage->x || xfrom >= (pImage->x + pImage->width))
   {
      //Log4( WIDE("hline(2) failed: %d<%d or %d>%d"), xto, pImage->x, xfrom, pImage->x + pImage->width );
      return;
   }

   if( xfrom < pImage->x )
      xfrom = pImage->x;
   if( xto >= pImage->x + pImage->width )
      xto = pImage->x + pImage->width - 1;

   len = (xto - xfrom) + 1;
   po = IMG_ADDRESS(pImage,xfrom,y);
   while( len )
   {
      *po = color;
      po++;
      len--;
   }
}

void CPROC do_vlinec( ImageFile *pImage, int x, int yfrom, int yto, CDATA color )
{
   PCDATA po;
   int oo;
   int len;
	//cpg26dec2006 Warning! W124: Comparison result always 0
	if( !pImage || !pImage->image /*|| pImage->height < 0*/ ) return;

   if( x < pImage->x || x >= (pImage->x + pImage->width ))
      return;

   if( yfrom > yto )
   {
      int tmp = yto;
      yto = yfrom;
      yfrom = tmp;
   }
   if( yto < pImage->y || yfrom >= (pImage->y + pImage->height))
      return;

   if( yfrom < pImage->y )
      yfrom = pImage->y;
   if( yto >= pImage->y + pImage->height )
      yto = pImage->y + pImage->height-1;

   len = (yto - yfrom) + 1;
   oo = pImage->pwidth;
	if( pImage->flags & IF_FLAG_INVERTED )
		po = (CDATA*)IMG_ADDRESS(pImage,x,yto);
   else
		po = (CDATA*)IMG_ADDRESS(pImage,x,yfrom);
   while( len )
   {
      *po = color;
      po += oo;
      len--;
   }
}

void CPROC do_hlineAlphac( ImageFile *pImage, int y, int xfrom, int xto, CDATA color )
{
   PCDATA po;
   int len;
   int alpha = AlphaVal( color );
   if( !pImage || !pImage->image ) return;
   if( y < pImage->y || y >= (pImage->y + pImage->height) )
      return;
   if( xfrom > xto )
   {
      int tmp = xto;
      xto = xfrom;
      xfrom = tmp;
   }
   if( xto < pImage->x || xfrom >= (pImage->x + pImage->width))
      return;

   if( xfrom < pImage->x )
      xfrom = pImage->x;
   if( xto >= pImage->x + pImage->width )
      xto = pImage->x + pImage->width - 1;
   len = (xto - xfrom) + 1;
   po = (CDATA*)IMG_ADDRESS(pImage,xfrom,y);
   while( len )
   {
      *po = DOALPHA( *po, color, alpha );
      po++;
      len--;
   }
}

void CPROC do_vlineAlphac( ImageFile *pImage, int x, int yfrom, int yto, CDATA color )
{
   PCDATA po;
   int oo;
   int len;
   int alpha = AlphaVal( color );
   if( !pImage || !pImage->image ) return;

   if( x < pImage->x || x >= (pImage->x + pImage->width) )
      return;

   if( yfrom > yto )
   {
      int tmp = yto;
      yto = yfrom;
      yfrom = tmp;
   }
   if( yto < pImage->y || yfrom >= (pImage->y + pImage->height))
      return;
   if( yfrom < pImage->y )
      yfrom = pImage->y;
   if( yto >= pImage->y + pImage->height )
      yto = pImage->y + pImage->height-1;
   len = (yto - yfrom) + 1;
   oo = pImage->pwidth;
	if( pImage->flags & IF_FLAG_INVERTED )
		po = (CDATA*)IMG_ADDRESS(pImage,x,yto);
   else
		po = (CDATA*)IMG_ADDRESS(pImage,x,yfrom);
   while( len )
   {
      *po = DOALPHA( *po, color, alpha );
      po += oo;
      len--;
   }
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
