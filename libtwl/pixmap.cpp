#include "pixmap.h"
//#include <stdio.h>
#include <string.h>
#include <iostream>
//#include <png.h>


namespace twl
{


//---------------------------------------------------------------------------
CPixmap::CPixmap()
 : pData_(0)
 , iWidth_(0)
 , iHeight_(0)
{
}

//---------------------------------------------------------------------------
CPixmap::CPixmap(int width, int height)
 : pData_(new color_t[width * height])
 , iWidth_(width)
 , iHeight_(height)
{
}

//---------------------------------------------------------------------------
CPixmap::CPixmap(const char * filename)
 : pData_(0)
 , iWidth_(0)
 , iHeight_(0)
{
  load(filename);
}

//---------------------------------------------------------------------------
CPixmap::~CPixmap()
{
  if(pData_ != 0)
    delete pData_;
}

//---------------------------------------------------------------------------
int
CPixmap::width()
{
  return iWidth_;
}

//---------------------------------------------------------------------------
int
CPixmap::height()
{
  return iHeight_;
}

//---------------------------------------------------------------------------
void
CPixmap::fill(color_t color)
{
  int iSize(iWidth_ * iHeight_);
  for(int i(0); i < iSize; i++)
    pData_[i] = color;
}

//---------------------------------------------------------------------------
void
CPixmap::fillRect(CRect & rect, color_t color)
{
  int iBottom(rect.bottom());
  int iRight(rect.right());
  int iIndex(rect.top() * iWidth_);

  for(int y(rect.top()); y <= iBottom; y++)
  {
    for(int x(rect.left()); x <= iRight; x++)
      pData_[iIndex + x] = color;
    iIndex += iWidth_;
  }
}

//---------------------------------------------------------------------------
void
CPixmap::drawHLine(int y, int left, int right, color_t color)
{
  right += y * iWidth_;
  for(int x(left + (y * iWidth_)); x <= right; x++)
    pData_[x] = color;
}

//---------------------------------------------------------------------------
void
CPixmap::drawVLine(int x, int top, int bottom, color_t color)
{
  bottom = (bottom * iWidth_) + x;
  for(int y((top * iWidth_) + x); y <= bottom; y += iWidth_)
    pData_[y] = color;
}

//---------------------------------------------------------------------------
void
CPixmap::bitBlt(int xoff, int yoff, CPixmap & pixmap, EBltType type)
{
  int iPWidth(pixmap.iWidth_);
  int iPHeight(pixmap.iHeight_);
  int iSrcXOff(0);
  int iSrcYOff(0);

  // Clip left
  if(xoff < 0)
  {
    iPWidth += xoff;
    iSrcXOff -= xoff;
    xoff = 0;
  }
  // Clip top
  if(yoff < 0)
  {
    iPHeight += yoff;
    iSrcYOff -= yoff;
    yoff = 0;
  }
  // Clip right
  if((pixmap.iWidth_ + xoff) > iWidth_)
  {
    iPWidth = iWidth_ - xoff;
  }
  // Clip left
  if((pixmap.iHeight_ + yoff) > iHeight_)
  {
    iPHeight = iHeight_ - yoff;
  }

  switch(type)
  {
    case eBT_COPY:
    {
      int iSrcIndex(iSrcXOff + (iSrcYOff * pixmap.iWidth_));
      int iDstIndex(xoff + (yoff * iWidth_));
      for(int y(0); y < iPHeight; y++)
      {
        memcpy(&(pData_[iDstIndex]), &(pixmap.pData_[iSrcIndex]), iPWidth * sizeof(color_t));
        iSrcIndex += pixmap.iWidth_;
        iDstIndex += iWidth_;
      }
      break;
    }
    case eBT_MERGE:
    {
      int iSrcIndex(iSrcXOff + (iSrcYOff * pixmap.iWidth_));
      int iDstIndex(xoff + (yoff * iWidth_));
      for(int y(0); y < iPHeight; y++)
      {
        for(int x(0); x < iPWidth; x++)
        {
          color_t & dst = pData_[iDstIndex + x];
          color_t & src = pixmap.pData_[iSrcIndex + x];
          uint8_t    sa  = BxColorFormat_GetA(cfA8R8G8B8, src);
          if(sa == 0)
          {
            // Do nothing
          }
          else if(sa == 255)
          {
            dst = src;
          }
          else
          {
            dst = BxColorFormat_FromRGBA(cfA8R8G8B8, 255 - ((255 - BxColorFormat_GetA(cfA8R8G8B8, dst)) * (255 - BxColorFormat_GetA(cfA8R8G8B8, src)) / 255),
                         (((BxColorFormat_GetR(cfA8R8G8B8, dst) * (255 - sa)) + (BxColorFormat_GetR(cfA8R8G8B8, src) * sa)) / 255),
                         (((BxColorFormat_GetG(cfA8R8G8B8, dst) * (255 - sa)) + (BxColorFormat_GetG(cfA8R8G8B8, src) * sa)) / 255),
                         (((BxColorFormat_GetB(cfA8R8G8B8, dst) * (255 - sa)) + (BxColorFormat_GetB(cfA8R8G8B8, src) * sa)) / 255));
          }
        }
        iSrcIndex += pixmap.iWidth_;
        iDstIndex += iWidth_;
      }
      break;
    }
  };
}

//---------------------------------------------------------------------------
#define PNG_SIGNATURE_SIZE 8
void
CPixmap::load(const char * filename)
{
  /*
  uint8_t signature[PNG_SIGNATURE_SIZE];
  FILE * fp = 0;
  png_struct * pPngStruct = 0;
//  png_info * pPngInfo = 0;

  fp = fopen(filename, "rb");
  if(fp != 0)
  {
    if(fread(signature, 1, PNG_SIGNATURE_SIZE, fp) == PNG_SIGNATURE_SIZE)
    {
      if(png_check_sig(signature, PNG_SIGNATURE_SIZE) == true)
      {
        png_struct * pPngStruct(png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0));
        if(pPngStruct != 0)
        {
          png_info * pPngInfo(png_create_info_struct(pPngStruct));
          if(pPngInfo != 0)
          {
            png_init_io(pPngStruct, fp);
            png_set_sig_bytes(pPngStruct, PNG_SIGNATURE_SIZE);
            png_read_png(pPngStruct, pPngInfo, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_BGR, 0);

            png_uint_32 width = pPngInfo->width;
            png_uint_32 height = pPngInfo->height;
            int color_type = pPngInfo->color_type;

            png_byte **pRowTable = pPngInfo->row_pointers;
            unsigned char r=0, g=0, b=0, a=0;

            if(pData_ != 0)
              delete pData_;
            pData_ = new pixel_t[width * height];
            iWidth_ = width;
            iHeight_ = height;

            pixel_t * p_dest = pData_;

            for(uint32_t y(0); y < height; ++y)
            {
              const png_byte *	pRow( pRowTable[ y ] );

              for(uint32_t x(0); x < width; x++)
              {
                switch(color_type)
                {
                case PNG_COLOR_TYPE_GRAY:
                  r = g = b = *pRow++;
                  if ( r == 0 && g == 0 && b == 0 )	a = 0x00;
                  else								a = 0xff;
                  break;
                case PNG_COLOR_TYPE_GRAY_ALPHA:
                  r = g = b = *pRow++;
                  if ( r == 0 && g == 0 && b == 0 )	a = 0x00;
                  else								a = 0xff;
                  pRow++;
                  break;
                case PNG_COLOR_TYPE_RGB:
                  b = *pRow++;
                  g = *pRow++;
                  r = *pRow++;
                  if ( r == 0 && g == 0 && b == 0 )	a = 0x00;
                  else								a = 0xff;
                  break;
                case PNG_COLOR_TYPE_RGB_ALPHA:
                  b = *pRow++;
                  g = *pRow++;
                  r = *pRow++;
                  a = *pRow++;
                  break;
                }
                p_dest[x] = BxARGB(a, r, g, b);
              }
              p_dest += width;
            }
            std::cout<<"CPixmap::load: PNG file "<<filename<<" loaded ("<<width<<"x"<<height<<")"<<std::endl;
          }
        }
      }
      else
      {
        std::cout<<"CPixmap::load: Invalid PNG signature for file "<<filename<<std::endl;
      }
    }
    else
    {
      std::cout<<"CPixmap::load: Cannot read from file "<<filename<<std::endl;
    }
  }
  else
  {
    std::cout<<"CPixmap::load: Cannot open file "<<filename<<std::endl;
  }

  if(pPngStruct != 0)
    png_destroy_read_struct(&pPngStruct, 0, 0);

  if(fp != 0)
    fclose(fp);
  */
}


};
