/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#ifdef CONFIG_GL_FRAMEBUFFER_16BIT
  // NOTE: Assumes cfX1B5G5R5 (GBA/NDS)
  #define RASTER_COLOR_LOAD COLOR_LOAD_555
  #define RASTER_COLOR_SAVE COLOR_SAVE_555
  #define RASTER_PIXEL_TYPE uint16_t
#else
  // NOTE: Assumes cfA8R8G8B8
  #define RASTER_COLOR_LOAD COLOR_LOAD_8888
  #define RASTER_COLOR_SAVE COLOR_SAVE_8888
  #define RASTER_PIXEL_TYPE uint32_t
#endif
#if !defined(RASTER_ENABLE_TEXTURES) && \
    !defined(CONFIG_GL_ENABLE_ALPHA_TEST) && \
    !defined(RASTER_ENABLE_BLENDING) && \
    !defined(RASTER_ENABLE_SMOOTH_COLORS)
  #define RASTER_DIRECT
#endif
#if  defined(CONFIG_GL_SIMPLE_TEXTURES) && \
    !defined(CONFIG_GL_ENABLE_ALPHA_TEST) && \
    !defined(RASTER_ENABLE_BLENDING) && \
    !defined(RASTER_ENABLE_SMOOTH_COLORS)
  #define RASTER_DIRECT
#endif


  float fdx1 = (float)(vmi->x - vlo->x) * (1.0f / (float)(1<<SHIFT_XY));
  float fdy1 = (float)(vmi->y - vlo->y) * (1.0f / (float)(1<<SHIFT_XY));
  float fdx2 = (float)(vhi->x - vlo->x) * (1.0f / (float)(1<<SHIFT_XY));
  float fdy2 = (float)(vhi->y - vlo->y) * (1.0f / (float)(1<<SHIFT_XY));
  float fz   = fdx1 * fdy2 - fdx2 * fdy1;
  if(fz == 0.0f)
    return;
  fz = 1.0f / fz;

  fdx1 *= fz;
  fdy1 *= fz;
  fdx2 *= fz;
  fdy2 *= fz;

#ifdef RASTER_DIRECT
  // Single value for direct output
  RASTER_PIXEL_TYPE pixel;
#else
  // Color accumulator, used in inner loop
  SRasterColor caccu;
#endif

#ifndef RASTER_ENABLE_SMOOTH_COLORS
  #ifdef RASTER_DIRECT
  // Directly ouput flat color
  pixel = RASTER_COLOR_SAVE(vlo->c);
  #else
  // Setup a flat color
  SRasterColor cflat;
  cflat.r = vlo->c.r;
  cflat.g = vlo->c.g;
  cflat.b = vlo->c.b;
  cflat.a = vlo->c.a;
  #endif
#endif

#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  // Premultiply u and v with 1/w
  // FIXME: This is a dirty hack!
  const_cast<SRasterVertex *>(vlo)->t.u = vlo->t.u * vlo->t.w;
  const_cast<SRasterVertex *>(vlo)->t.v = vlo->t.v * vlo->t.w;
  const_cast<SRasterVertex *>(vmi)->t.u = vmi->t.u * vmi->t.w;
  const_cast<SRasterVertex *>(vmi)->t.v = vmi->t.v * vmi->t.w;
  const_cast<SRasterVertex *>(vhi)->t.u = vhi->t.u * vhi->t.w;
  const_cast<SRasterVertex *>(vhi)->t.v = vhi->t.v * vhi->t.w;
  #endif
#endif

  int32_t iYTop, iYBottom, iHeight;

#if defined(RASTER_ENABLE_DEPTH_TEST) || defined(RASTER_ENABLE_SMOOTH_COLORS) || defined(RASTER_ENABLE_TEXTURES)
  float d1;
  float d2;
  #define DDX() (fdy2 * d1 - fdy1 * d2)
  #define DDY() (fdx1 * d2 - fdx2 * d1)
#endif

  // X Interpolation
  int32_t left_x, right_x, dxdy_left, dxdy_right;

  int32_t dxdy_left_mod; // dx modulo dy
  int32_t dxdy_left_den; // denominator (dy)
  int32_t dxdy_left_err; // error (added modulo's)

  int32_t dxdy_right_mod; // dx modulo dy
  int32_t dxdy_right_den; // denominator (dy)
  int32_t dxdy_right_err; // error (added modulo's)
#ifdef RASTER_ENABLE_DEPTH_TEST
  // Z Interpolation
  int32_t z, left_z, /*right_z,*/ dzdy_left, /*dzdy_right,*/ dzdx, dzdy;
  d1 = vmi->z - vlo->z;
  d2 = vhi->z - vlo->z;
  dzdx = (int32_t)DDX();
  dzdy = (int32_t)DDY();
#endif
#ifdef RASTER_ENABLE_SMOOTH_COLORS
  // RGBA Interpolation
  SRasterColor c, left_c, /*right_c,*/ dcdy_left, /*dcdy_right,*/ dcdx, dcdy;
  d1 = vmi->c.r - vlo->c.r;
  d2 = vhi->c.r - vlo->c.r;
  dcdx.r = (int32_t)DDX();
  dcdy.r = (int32_t)DDY();
  d1 = vmi->c.g - vlo->c.g;
  d2 = vhi->c.g - vlo->c.g;
  dcdx.g = (int32_t)DDX();
  dcdy.g = (int32_t)DDY();
  d1 = vmi->c.b - vlo->c.b;
  d2 = vhi->c.b - vlo->c.b;
  dcdx.b = (int32_t)DDX();
  dcdy.b = (int32_t)DDY();
  d1 = vmi->c.a - vlo->c.a;
  d2 = vhi->c.a - vlo->c.a;
  dcdx.a = (int32_t)DDX();
  dcdy.a = (int32_t)DDY();
#endif
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  float u, left_u, /*right_u,*/ dudy_left, /*dudy_right,*/ dudx, dudy;
  float v, left_v, /*right_v,*/ dvdy_left, /*dvdy_right,*/ dvdx, dvdy;
  float w, left_w, /*right_w,*/ dwdy_left, /*dwdy_right,*/ dwdx, dwdy;
  d1 = vmi->t.u - vlo->t.u;
  d2 = vhi->t.u - vlo->t.u;
  dudx = DDX();
  dudy = DDY();
  d1 = vmi->t.v - vlo->t.v;
  d2 = vhi->t.v - vlo->t.v;
  dvdx = DDX();
  dvdy = DDY();
  d1 = vmi->t.w - vlo->t.w;
  d2 = vhi->t.w - vlo->t.w;
  dwdx = DDX();
  dwdy = DDY();
  #else
  int32_t u, left_u, /*right_u,*/ dudy_left, /*dudy_right,*/ dudx, dudy;
  int32_t v, left_v, /*right_v,*/ dvdy_left, /*dvdy_right,*/ dvdx, dvdy;
  d1 = vmi->t.u - vlo->t.u;
  d2 = vhi->t.u - vlo->t.u;
  dudx = (int32_t)DDX();
  dudy = (int32_t)DDY();
  d1 = vmi->t.v - vlo->t.v;
  d2 = vhi->t.v - vlo->t.v;
  dvdx = (int32_t)DDX();
  dvdy = (int32_t)DDY();
    #ifndef RASTER_DIRECT
  // The texture is near when advancing less than 1 texel per pixel
  // To speed things up, be near when advancing less than half a texel per pixel
  bool bNear = ((int_abs(dudx) < (1 << (SHIFT_TEX - 1))) || (int_abs(dvdx) < (1 << (SHIFT_TEX - 1))));
    #endif
  #endif
#endif


  bool bUpdateLeft  = false;
  bool bUpdateRight = false;
  const SRasterVertex *pl1, *pl2, *pr1, *pr2;
  for(int i(0); i < 2; i++)
  {
    if(i == 0)
    {
      if(fz > 0.0f)
      {
        bUpdateLeft  = true;
        bUpdateRight = true;
        pl1 = vlo;
        pl2 = vhi;
        pr1 = vlo;
        pr2 = vmi;
      }
      else
      {
        bUpdateLeft  = true;
        bUpdateRight = true;
        pl1 = vlo;
        pl2 = vmi;
        pr1 = vlo;
        pr2 = vhi;
      }
      iYTop    = fx_ceil(vlo->y, SHIFT_XY);
      iYBottom = fx_ceil(vmi->y, SHIFT_XY);
    }
    else
    {
      if(fz > 0)
      {
        bUpdateRight = true;
        pr1 = vmi;
        pr2 = vhi;
      }
      else
      {
        bUpdateLeft  = true;
        pl1 = vmi;
        pl2 = vhi;
      }
      iYTop    = fx_ceil(vmi->y, SHIFT_XY);
      iYBottom = fx_ceil(vhi->y, SHIFT_XY);
    }

    iHeight = iYBottom - iYTop;
    if(iHeight <= 0)
      continue;

    if(bUpdateLeft == true)
    {
      bUpdateLeft = false;

      // Get the real width and height
      int32_t fxDX = pl2->x - pl1->x;
      int32_t fxDY = pl2->y - pl1->y;

      // Prestep y, so we are at the vertical center of the pixel
      int32_t fxYOff = (iYTop << SHIFT_XY) - pl1->y;

      // Calculate the slope
      fxFloorDivMod(fxDX, fxDY, SHIFT_XY, dxdy_left, dxdy_left_mod);
      dxdy_left_den = fxDY;
      dxdy_left_err = 0; // FIXME

      // Get the inital x position
      left_x = pl1->x + i_mul_shr_64(dxdy_left, fxYOff, SHIFT_XY);

#ifdef RASTER_ENABLE_DEPTH_TEST
      dzdy_left = i_mul_shr_64(dxdy_left, dzdx, SHIFT_XY) + dzdy;
      left_z = pl1->z + i_mul_shr_64(dzdy_left, fxYOff, SHIFT_XY);
#endif

#ifdef RASTER_ENABLE_SMOOTH_COLORS
      dcdy_left.r = i_mul_shr_64(dxdy_left, dcdx.r, SHIFT_XY) + dcdy.r;
      dcdy_left.g = i_mul_shr_64(dxdy_left, dcdx.g, SHIFT_XY) + dcdy.g;
      dcdy_left.b = i_mul_shr_64(dxdy_left, dcdx.b, SHIFT_XY) + dcdy.b;
      dcdy_left.a = i_mul_shr_64(dxdy_left, dcdx.a, SHIFT_XY) + dcdy.a;
      left_c.r = pl1->c.r + i_mul_shr_64(dcdy_left.r, fxYOff, SHIFT_XY);
      left_c.g = pl1->c.g + i_mul_shr_64(dcdy_left.g, fxYOff, SHIFT_XY);
      left_c.b = pl1->c.b + i_mul_shr_64(dcdy_left.b, fxYOff, SHIFT_XY);
      left_c.a = pl1->c.a + i_mul_shr_64(dcdy_left.a, fxYOff, SHIFT_XY);
#endif

#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
      float f_dxdy_left = (float)dxdy_left * (1.0f / (float)(1<<SHIFT_XY));
      dwdy_left = f_dxdy_left * dwdx + dwdy;
      dudy_left = f_dxdy_left * dudx + dudy;
      dvdy_left = f_dxdy_left * dvdx + dvdy;

      float f_y_off = (float)fxYOff * (1.0f / (float)(1<<SHIFT_XY));
      left_w = pl1->t.w + (dwdy_left * f_y_off);
      left_u = pl1->t.u + (dudy_left * f_y_off);
      left_v = pl1->t.v + (dvdy_left * f_y_off);
  #else
      dudy_left = i_mul_shr_64(dxdy_left, dudx, SHIFT_XY) + dudy;
      dvdy_left = i_mul_shr_64(dxdy_left, dvdx, SHIFT_XY) + dvdy;
      left_u = pl1->t.u + i_mul_shr_64(dudy_left, fxYOff, SHIFT_XY);
      left_v = pl1->t.v + i_mul_shr_64(dvdy_left, fxYOff, SHIFT_XY);
  #endif
#endif
    }
    if(bUpdateRight == true)
    {
      bUpdateRight = false;

      int32_t fxDX = pr2->x - pr1->x;
      int32_t fxDY = pr2->y - pr1->y;

      // Prestep y, so we are at the vertical center of the pixel
      int32_t fxYOff = (iYTop << SHIFT_XY) - pr1->y;

      // Calculate the slope
      fxFloorDivMod(fxDX, fxDY, SHIFT_XY, dxdy_right, dxdy_right_mod);
      dxdy_right_den = fxDY;
      dxdy_right_err = 0; // FIXME

      // Get the inital x position
      right_x = pr1->x + i_mul_shr_64(dxdy_right, fxYOff, SHIFT_XY);
    }

    for(int32_t iY = iYTop; iY < iYBottom; iY++)
    {
      int32_t iXStart = fx_ceil(left_x,  SHIFT_XY);
      int32_t iXEnd   = fx_ceil(right_x, SHIFT_XY);
      int32_t iXWidth = iXEnd - iXStart;

      if(iXWidth > 0)
      {
#ifdef RASTER_ENABLE_DEPTH_TEST
        z = left_z;
#endif
#ifdef RASTER_ENABLE_SMOOTH_COLORS
        c.r = left_c.r;
        c.g = left_c.g;
        c.b = left_c.b;
        c.a = left_c.a;
#endif
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
        w = left_w;
        u = left_u;
        v = left_v;
  #else
        u = left_u;
        v = left_v;
  #endif
#endif

        uint32_t pixelIndex = iY * renderSurface->width() + iXStart;
        RASTER_PIXEL_TYPE * pDestPixel = ((RASTER_PIXEL_TYPE *)renderSurface->p) + pixelIndex;
#ifdef RASTER_ENABLE_DEPTH_TEST
        uint16_t * pDepthPixel = pZBuffer_ + pixelIndex;
#endif
#if defined(RASTER_ENABLE_TEXTURES) && defined(CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES)
        int32_t iBlockWidth;
        float sub_w_left,  sub_u_left,  sub_v_left;
        float sub_w_right, sub_u_right, sub_v_right;
        // Subdevided u and v values
        TInterpolation<int32_t> sub_u;
        TInterpolation<int32_t> sub_v;

        // Calculate left values of affine block
        sub_w_left = 1.0f / w;
        sub_u_left = u * sub_w_left;
        sub_v_left = v * sub_w_left;

        while(iXWidth > 0)
        {
          iBlockWidth = (iXWidth > MIN_AFFINE_LENGTH) ? AFFINE_LENGTH : iXWidth;
          iXWidth -= iBlockWidth;

          // Advance to the right of the block
          w += dwdx * iBlockWidth;
          u += dudx * iBlockWidth;
          v += dvdx * iBlockWidth;

          // Calculate right values of affine block
          sub_w_right = 1.0f / w;
          sub_u_right = u * sub_w_right;
          sub_v_right = v * sub_w_right;

          // Create affine block interpolation
          sub_u.current = (int32_t)(sub_u_left * (1 << SHIFT_TEX));
          sub_v.current = (int32_t)(sub_v_left * (1 << SHIFT_TEX));
          float fMultiplier = (iBlockWidth == AFFINE_LENGTH) ? (1 << (SHIFT_TEX - AFFINE_SHIFT)) : ((1.0f / (float)iBlockWidth) * (1 << SHIFT_TEX));
          sub_u.increment = (int32_t)((sub_u_right - sub_u_left) * fMultiplier);
          sub_v.increment = (int32_t)((sub_v_right - sub_v_left) * fMultiplier);

#ifndef RASTER_DIRECT
          // The texture is near when advancing less than 1 texel per pixel
          // To speed things up, be near when advancing less than half a texel per pixel
          bool bNear = ((int_abs(sub_u.increment) < (1 << (SHIFT_TEX - 1))) || (int_abs(sub_v.increment) < (1 << (SHIFT_TEX - 1))));
#endif

          while(iBlockWidth--)
          {
#else
        while(iXWidth--)
        {
#endif
          // Depth Test:
          //   First thing we do is the depth test, becouse
          //   if this fails, we don't have to calculate the
          //   color of the pixel. (the most time consuming part of the loop)
#ifdef RASTER_ENABLE_DEPTH_TEST
          uint16_t zz = z >> SHIFT_Z;
          if(rasterDepth(zz, *pDepthPixel) == true)
#endif
          {
#ifndef RASTER_DIRECT
            // Get the fragment color
  #ifdef RASTER_ENABLE_SMOOTH_COLORS
            // Get smoothly interpolated color
            //caccu.r = c.r;
            //caccu.g = c.g;
            //caccu.b = c.b;
            //caccu.a = c.a;
            COLOR_CLAMP(caccu, c);
  #else
            // Set flat filled color
            caccu.r = cflat.r;
            caccu.g = cflat.g;
            caccu.b = cflat.b;
            caccu.a = cflat.a;
  #endif
#endif
            // Get the fragment texel
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
    #ifdef RASTER_DIRECT
            pixel = ((uint16_t *)pCurrentTex_->data)[(sub_u.current >> SHIFT_TEX) + ((sub_v.current >> SHIFT_TEX) << pCurrentTex_->bitWidth_)];
    #else
            //pCurrentTex_->getTexel(caccu, sub_u.current, sub_v.current, bNear);
            rasterTexture(caccu, caccu, sub_u.current, sub_v.current, bNear);
    #endif
  #else
    #ifdef RASTER_DIRECT
            pixel = ((uint16_t *)pCurrentTex_->data)[(u >> SHIFT_TEX) + ((v >> SHIFT_TEX) << pCurrentTex_->bitWidth_)];
    #else
            //pCurrentTex_->getTexel(caccu, u, v, bNear);
            rasterTexture(caccu, caccu, u, v, bNear);
    #endif
  #endif
#endif

            // Alpha Test (RGBA only)
#ifdef CONFIG_GL_ENABLE_ALPHA_TEST
            bool bAlphaPasses = true;
            if(alphaTestEnabled_ == true)
            {
              switch(alphaFunc_)
              {
                case GL_NEVER:    bAlphaPasses = false;                    break;
                case GL_LESS:     bAlphaPasses = caccu.a <  alphaValueFX_; break;
                case GL_EQUAL:    bAlphaPasses = caccu.a == alphaValueFX_; break;
                case GL_LEQUAL:   bAlphaPasses = caccu.a <= alphaValueFX_; break;
                case GL_GREATER:  bAlphaPasses = caccu.a >  alphaValueFX_; break;
                case GL_NOTEQUAL: bAlphaPasses = caccu.a != alphaValueFX_; break;
                case GL_GEQUAL:   bAlphaPasses = caccu.a >= alphaValueFX_; break;
                case GL_ALWAYS:   bAlphaPasses = true;                     break;
              };
            }
            if(bAlphaPasses == true)
#endif
            {
              // Blending (RGBA only): Blend the fragment color with the framebuffer color
#ifdef RASTER_ENABLE_BLENDING
              SRasterColor screen_pix;
              RASTER_COLOR_LOAD(screen_pix, *pDestPixel);
              rasterBlend(caccu, caccu, screen_pix);
#endif

#ifdef RASTER_ENABLE_DEPTH_TEST
              if(depthMask_ == true)
                *pDepthPixel = zz;
#endif
              // Write the resulting fragment color to the framebuffer
#ifdef RASTER_DIRECT
              *pDestPixel = pixel;
#else
              *pDestPixel = RASTER_COLOR_SAVE(caccu);
#endif
            }
          }
#ifdef RASTER_ENABLE_DEPTH_TEST
          z += dzdx;
#endif
#ifdef RASTER_ENABLE_SMOOTH_COLORS
          c.r += dcdx.r;
          c.g += dcdx.g;
          c.b += dcdx.b;
          c.a += dcdx.a;
#endif
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
          sub_u.current += sub_u.increment;
          sub_v.current += sub_v.increment;
  #else
          u += dudx;
          v += dvdx;
  #endif
#endif
          pDestPixel++;
#ifdef RASTER_ENABLE_DEPTH_TEST
          pDepthPixel++;
#endif
#ifdef ENABLE_PROFILING
          fillRate++;
#endif
#if defined(RASTER_ENABLE_TEXTURES) && defined(CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES)
          }
          sub_w_left = sub_w_right;
          sub_u_left = sub_u_right;
          sub_v_left = sub_v_right;
        }
#else
        }
#endif
      }
      left_x   += dxdy_left;
#ifdef RASTER_ENABLE_DEPTH_TEST
      left_z   += dzdy_left;
#endif
#ifdef RASTER_ENABLE_SMOOTH_COLORS
      left_c.r += dcdy_left.r;
      left_c.g += dcdy_left.g;
      left_c.b += dcdy_left.b;
      left_c.a += dcdy_left.a;
#endif
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
      left_w   += dwdy_left;
  #endif
      left_u   += dudy_left;
      left_v   += dvdy_left;
#endif

      dxdy_left_err += dxdy_left_mod;
      if(dxdy_left_err >= dxdy_left_den)
      {
        dxdy_left_err -= dxdy_left_den;

        left_x   += 1;
#ifdef RASTER_ENABLE_DEPTH_TEST
        left_z   += dzdx   >> SHIFT_XY;
#endif
#ifdef RASTER_ENABLE_SMOOTH_COLORS
        left_c.r += dcdx.r >> SHIFT_XY;
        left_c.g += dcdx.g >> SHIFT_XY;
        left_c.b += dcdx.b >> SHIFT_XY;
        left_c.a += dcdx.a >> SHIFT_XY;
#endif
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
        left_w   += dwdx * (1.0f / (1 << SHIFT_XY));
        left_u   += dudx * (1.0f / (1 << SHIFT_XY));
        left_v   += dvdx * (1.0f / (1 << SHIFT_XY));
  #else
        left_u   += dudx   >> SHIFT_XY;
        left_v   += dvdx   >> SHIFT_XY;
  #endif
#endif
      }

      right_x += dxdy_right;

      dxdy_right_err += dxdy_right_mod;
      if(dxdy_right_err >= dxdy_right_den)
      {
        dxdy_right_err -= dxdy_right_den;
        right_x += 1;
      }
    }
  }


#undef RASTER_COLOR_LOAD
#undef RASTER_COLOR_SAVE
#undef RASTER_PIXEL_TYPE
#undef RASTER_DIRECT

#undef RASTER_ENABLE_DEPTH_TEST
#undef RASTER_ENABLE_SMOOTH_COLORS
#undef RASTER_ENABLE_TEXTURES
#undef RASTER_ENABLE_BLENDING
