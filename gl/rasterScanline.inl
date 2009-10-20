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

/*
 * Pipeline selection:
 *  - RASTER_ENABLE_DEPTH_TEST:               Enable depth testing (z-buffering)
 *  - RASTER_ENABLE_SMOOTH_SHADING:           Enable smooth shading (disabled == flat shading)
 *  - RASTER_ENABLE_TEXTURES:                 Enable texture mapping
 *  - RASTER_ENABLE_BLENDING:                 Enable blending
 *
 * Pipeline configuration:
 *  - CONFIG_GL_ENABLE_ALPHA_TEST:            Enable alpha testing
 *  - CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES: Use perspective correct texture mapping
 *  - CONFIG_GL_SIMPLE_TEXTURES:              Texture uses same format as frame-buffer
 *  - CONFIG_GL_FRAMEBUFFER_16BIT:            Output to 16bit frame-buffer (disabled == 32bit)
 *
 * Internal defines, NOT to be used!
 *  - RASTER_DIRECT:                          Colors are not calculated on, directly copy value into frame-buffer
 *  - RASTER_DIRECT_FLAT_COLOR:               Flat color is directly copied to frame-buffer
 *  - RASTER_DIRECT_TEXTURES:                 Texel color is directly copied to frame-buffer
 *  - RASTER_INTERPOLATE_W:                   Linear depth is interpolated
 *  - RASTER_INTERPOLATE_FLOATS:              Interpolate floating point values
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

// Directly raster flat colors
#if !defined(RASTER_ENABLE_SMOOTH_SHADING) && \
    !defined(RASTER_ENABLE_TEXTURES) && \
    !defined(RASTER_ENABLE_BLENDING) && \
    !defined(CONFIG_GL_ENABLE_ALPHA_TEST)
  #define RASTER_DIRECT_FLAT_COLOR
#endif
// Directly raster textures
#if  defined(RASTER_ENABLE_TEXTURES) && \
     defined(CONFIG_GL_SIMPLE_TEXTURES) && \
    !defined(CONFIG_GL_ENABLE_ALPHA_TEST) && \
    !defined(RASTER_ENABLE_BLENDING) && \
    !defined(RASTER_ENABLE_SMOOTH_SHADING)
  #define RASTER_DIRECT_TEXTURES
#endif
// Directly raster
#if defined(RASTER_DIRECT_FLAT_COLOR) || defined(RASTER_DIRECT_TEXTURES)
  #define RASTER_DIRECT
#endif
// Validate direct rendering
#if defined(RASTER_DIRECT_FLAT_COLOR) && defined(RASTER_DIRECT_TEXTURES)
  #error "multiple direct raster modes selected"
#endif

// Do we need linear depth?
#ifndef RASTER_INTERPOLATE_W
  #if defined(RASTER_ENABLE_TEXTURES) && defined(CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES)
    #define RASTER_INTERPOLATE_W
  #endif
#endif

// Do we need floating point interpolation
#if defined(RASTER_ENABLE_TEXTURES) || defined(RASTER_INTERPOLATE_W)
  #define RASTER_INTERPOLATE_FLOATS
#endif


#ifndef RASTER_DIRECT
  // Color accumulator, used in inner loop
  SColor caccu;
#endif

#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  // Premultiply u and v with 1/w
  float top_u_inv    = vtop->t.u    * vtop->w;
  float top_v_inv    = vtop->t.v    * vtop->w;
  float middle_u_inv = vmiddle->t.u * vmiddle->w;
  float middle_v_inv = vmiddle->t.v * vmiddle->w;
  float bottom_u_inv = vbottom->t.u * vbottom->w;
  float bottom_v_inv = vbottom->t.v * vbottom->w;
  #endif
#endif

  float fdx1  = fptof(SHIFT_XY, vmiddle->x - vtop->x);
  float fdy1  = fptof(SHIFT_XY, vmiddle->y - vtop->y);
  float fdx2  = fptof(SHIFT_XY, vbottom->x - vtop->x);
  float fdy2  = fptof(SHIFT_XY, vbottom->y - vtop->y);
  float farea = fdx1 * fdy2 - fdx2 * fdy1;
  if(farea == 0.0f)
    return;
#if defined(RASTER_ENABLE_DEPTH_TEST) || defined(RASTER_ENABLE_SMOOTH_SHADING) || defined(RASTER_ENABLE_TEXTURES) || defined(RASTER_INTERPOLATE_W)
  float finv_area = 1.0f / farea;
  fdx1 *= finv_area;
  fdy1 *= finv_area;
  fdx2 *= finv_area;
  fdy2 *= finv_area;

  // Calculate gradients:
  //  - DDX: Delta Delta X
  //  - DDY: Delta Delta Y
  #define DDX() (fdy2 * d1 - fdy1 * d2)
  #define DDY() (fdx1 * d2 - fdx2 * d1)
  float d1;
  float d2;
#endif

  // X Interpolation
  //  - mod: dx modulo dy
  //  - den: denominator (dy)
  //  - err: error (added modulo's)
  int32_t left_x  = 0, dxdy_left  = 0, dxdy_left_mod  = 0, dxdy_left_den  = 0, dxdy_left_err  = 0;
  int32_t right_x = 0, dxdy_right = 0, dxdy_right_mod = 0, dxdy_right_den = 0, dxdy_right_err = 0;

#ifdef RASTER_ENABLE_DEPTH_TEST
  // Depth interpolation
  d1 = vmiddle->z - vtop->z;
  d2 = vbottom->z - vtop->z;
  grad_z.ddx = (int32_t)DDX();
  grad_z.ddy = (int32_t)DDY();
#endif

#ifdef RASTER_INTERPOLATE_W
  // Linear depth interpolation (for perspective correct rendering)
  d1 = vmiddle->w - vtop->w;
  d2 = vbottom->w - vtop->w;
  grad_w.ddx = DDX();
  grad_w.ddy = DDY();
#endif

#ifdef RASTER_ENABLE_SMOOTH_SHADING
  // Color interpolation
  d1 = INTERFACE_TO_INTERP_COLOR(vmiddle->c.r - vtop->c.r);
  d2 = INTERFACE_TO_INTERP_COLOR(vbottom->c.r - vtop->c.r);
  grad_c.ddx.r = (int32_t)DDX();
  grad_c.ddy.r = (int32_t)DDY();
  d1 = INTERFACE_TO_INTERP_COLOR(vmiddle->c.g - vtop->c.g);
  d2 = INTERFACE_TO_INTERP_COLOR(vbottom->c.g - vtop->c.g);
  grad_c.ddx.g = (int32_t)DDX();
  grad_c.ddy.g = (int32_t)DDY();
  d1 = INTERFACE_TO_INTERP_COLOR(vmiddle->c.b - vtop->c.b);
  d2 = INTERFACE_TO_INTERP_COLOR(vbottom->c.b - vtop->c.b);
  grad_c.ddx.b = (int32_t)DDX();
  grad_c.ddy.b = (int32_t)DDY();
  d1 = INTERFACE_TO_INTERP_COLOR(vmiddle->c.a - vtop->c.a);
  d2 = INTERFACE_TO_INTERP_COLOR(vbottom->c.a - vtop->c.a);
  grad_c.ddx.a = (int32_t)DDX();
  grad_c.ddy.a = (int32_t)DDY();
#else
  #ifdef RASTER_DIRECT
    #ifdef RASTER_DIRECT_FLAT_COLOR
  // Directly ouput flat color
  RASTER_PIXEL_TYPE flat_color = RASTER_COLOR_SAVE(vtop->c, SHIFT_COLOR);
    #endif
  #else
  // Setup a flat color
  grad_c.current.r = INTERFACE_TO_CALC_COLOR(vtop->c.r);
  grad_c.current.g = INTERFACE_TO_CALC_COLOR(vtop->c.g);
  grad_c.current.b = INTERFACE_TO_CALC_COLOR(vtop->c.b);
  grad_c.current.a = INTERFACE_TO_CALC_COLOR(vtop->c.a);
  #endif
#endif

#ifdef RASTER_ENABLE_TEXTURES
  // Texture coordinate interpolation
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  d1 = middle_u_inv - top_u_inv;
  d2 = bottom_u_inv - top_u_inv;
  grad_t.ddx.u = DDX();
  grad_t.ddy.u = DDY();
  d1 = middle_v_inv - top_v_inv;
  d2 = bottom_v_inv - top_v_inv;
  grad_t.ddx.v = DDX();
  grad_t.ddy.v = DDY();
  #else
  d1 = vmiddle->t.u - vtop->t.u;
  d2 = vbottom->t.u - vtop->t.u;
  grad_t.ddx.u = DDX();
  grad_t.ddy.u = DDY();
  d1 = vmiddle->t.v - vtop->t.v;
  d2 = vbottom->t.v - vtop->t.v;
  grad_t.ddx.v = DDX();
  grad_t.ddy.v = DDY();

  // Get the level of detail (lambda)
  float lodbias = 0.0f;
  float lod = pCurrentTex_->lambda(grad_t.ddx.u, grad_t.ddy.u, grad_t.ddx.v, grad_t.ddy.v) + lodbias;
  #endif
#endif

  int32_t iYTop, iYBottom, fxYOff;
  bool bUpdateLeft  = true;
  bool bUpdateRight = true;
  const SVertex *vtop_l = NULL, *vbottom_l = NULL, *vtop_r = NULL, *vbottom_r = NULL;
  for(int i(0); i < 2; i++)
  {
    if(i == 0)
    {
      if(farea > 0.0f)
      {
        vtop_l    = vtop;
        vbottom_l = vbottom;
        vtop_r    = vtop;
        vbottom_r = vmiddle;
      }
      else
      {
        vtop_l    = vtop;
        vbottom_l = vmiddle;
        vtop_r    = vtop;
        vbottom_r = vbottom;
      }
      // height = bottom - top;
      iYTop    = fpfloor(SHIFT_XY, vtop->y    + pixelFloorOffset_);
      iYBottom = fpfloor(SHIFT_XY, vmiddle->y + pixelFloorOffset_);
      // Y Offset from vertex to pixel center
      fxYOff   = (iYTop << SHIFT_XY) + pixelCenterOffset_ - vtop->y;
    }
    else
    {
      if(farea > 0.0f)
      {
        bUpdateRight = true;
        vtop_r    = vmiddle;
        vbottom_r = vbottom;
      }
      else
      {
        bUpdateLeft  = true;
        vtop_l    = vmiddle;
        vbottom_l = vbottom;
      }
      // height = bottom - top;
      iYTop    = fpfloor(SHIFT_XY, vmiddle->y + pixelFloorOffset_);
      iYBottom = fpfloor(SHIFT_XY, vbottom->y + pixelFloorOffset_);
      // Y Offset from vertex to pixel center
      fxYOff   = (iYTop << SHIFT_XY) + pixelCenterOffset_ - vmiddle->y;
    }

    // Need to draw at least one line
    if((iYBottom - iYTop) <= 0)
      continue;

    if(bUpdateLeft == true)
    {
      bUpdateLeft = false;

      // Get the real width and height
      int32_t fxDX = vbottom_l->x - vtop_l->x;
      int32_t fxDY = vbottom_l->y - vtop_l->y;

      int32_t initial_numerator = 0;
      initial_numerator += fxDY * vtop_l->x;                  // Add x
      initial_numerator += fxDX * fxYOff;                     // Move down to pixel center
      initial_numerator += fxDY * oneMinusPixelCenterOffset_; // Add 0.5 or 1 to x
      FloorDivMod(initial_numerator,  (fxDY << SHIFT_XY), left_x,    dxdy_left_err);
      FloorDivMod((fxDX << SHIFT_XY), (fxDY << SHIFT_XY), dxdy_left, dxdy_left_mod);
      dxdy_left_den = fxDY << SHIFT_XY;
#if defined(RASTER_ENABLE_DEPTH_TEST) || defined(RASTER_ENABLE_SMOOTH_SHADING) || defined(RASTER_ENABLE_TEXTURES) || defined(RASTER_INTERPOLATE_W)
      int32_t fxXOff = (left_x << SHIFT_XY) + pixelCenterOffset_ - vtop_l->x;
#endif

#ifdef RASTER_INTERPOLATE_FLOATS
      float f_dxdy_left = (float)dxdy_left;
      float fXOff = fptof(SHIFT_XY, fxXOff);
      float fYOff = fptof(SHIFT_XY, fxYOff);
#endif

#ifdef RASTER_ENABLE_DEPTH_TEST
      grad_z.ddy_left = (dxdy_left * grad_z.ddx) + grad_z.ddy;
      grad_z.current_left = vtop_l->z + i_mul_shr(grad_z.ddx, fxXOff, SHIFT_XY) + i_mul_shr(grad_z.ddy, fxYOff, SHIFT_XY);
#endif

#ifdef RASTER_INTERPOLATE_W
      grad_w.ddy_left = (f_dxdy_left * grad_w.ddx) + grad_w.ddy;
      grad_w.current_left = vtop_l->w + (grad_w.ddx * fXOff) + (grad_w.ddy * fYOff);
#endif

#ifdef RASTER_ENABLE_SMOOTH_SHADING
      grad_c.ddy_left.r = (dxdy_left * grad_c.ddx.r) + grad_c.ddy.r;
      grad_c.ddy_left.g = (dxdy_left * grad_c.ddx.g) + grad_c.ddy.g;
      grad_c.ddy_left.b = (dxdy_left * grad_c.ddx.b) + grad_c.ddy.b;
      grad_c.ddy_left.a = (dxdy_left * grad_c.ddx.a) + grad_c.ddy.a;
      grad_c.current_left.r = INTERFACE_TO_INTERP_COLOR(vtop_l->c.r) + i_mul_shr(grad_c.ddx.r, fxXOff, SHIFT_XY) + i_mul_shr(grad_c.ddy.r, fxYOff, SHIFT_XY);
      grad_c.current_left.g = INTERFACE_TO_INTERP_COLOR(vtop_l->c.g) + i_mul_shr(grad_c.ddx.g, fxXOff, SHIFT_XY) + i_mul_shr(grad_c.ddy.g, fxYOff, SHIFT_XY);
      grad_c.current_left.b = INTERFACE_TO_INTERP_COLOR(vtop_l->c.b) + i_mul_shr(grad_c.ddx.b, fxXOff, SHIFT_XY) + i_mul_shr(grad_c.ddy.b, fxYOff, SHIFT_XY);
      grad_c.current_left.a = INTERFACE_TO_INTERP_COLOR(vtop_l->c.a) + i_mul_shr(grad_c.ddx.a, fxXOff, SHIFT_XY) + i_mul_shr(grad_c.ddy.a, fxYOff, SHIFT_XY);
#endif

#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
      grad_t.ddy_left.u = (f_dxdy_left * grad_t.ddx.u) + grad_t.ddy.u;
      grad_t.ddy_left.v = (f_dxdy_left * grad_t.ddx.v) + grad_t.ddy.v;
      grad_t.current_left.u = (vtop_l->t.u * vtop_l->w)+ (grad_t.ddx.u * fXOff) + (grad_t.ddy.u * fYOff);
      grad_t.current_left.v = (vtop_l->t.v * vtop_l->w)+ (grad_t.ddx.v * fXOff) + (grad_t.ddy.v * fYOff);
  #else
      grad_t.ddy_left.u = (f_dxdy_left * grad_t.ddx.u) + grad_t.ddy.u;
      grad_t.ddy_left.v = (f_dxdy_left * grad_t.ddx.v) + grad_t.ddy.v;
      grad_t.current_left.u = vtop_l->t.u + (grad_t.ddx.u * fXOff) + (grad_t.ddy.u * fYOff);
      grad_t.current_left.v = vtop_l->t.v + (grad_t.ddx.v * fXOff) + (grad_t.ddy.v * fYOff);
  #endif
#endif

    }
    if(bUpdateRight == true)
    {
      bUpdateRight = false;

      // Get the real width and height
      int32_t fxDX = vbottom_r->x - vtop_r->x;
      int32_t fxDY = vbottom_r->y - vtop_r->y;

      int32_t initial_numerator = 0;
      initial_numerator += fxDY * vtop_r->x;                  // Add x
      initial_numerator += fxDX * fxYOff;                     // Move down to pixel center
      initial_numerator += fxDY * oneMinusPixelCenterOffset_; // Add 0.5 or 1 to x
      FloorDivMod(initial_numerator,  (fxDY << SHIFT_XY), right_x,    dxdy_right_err);
      FloorDivMod((fxDX << SHIFT_XY), (fxDY << SHIFT_XY), dxdy_right, dxdy_right_mod);
      dxdy_right_den = fxDY << SHIFT_XY;
    }

    for(int32_t iY = iYTop; iY < iYBottom; iY++)
    {
      int32_t iWidth = right_x - left_x;

      if(iWidth > 0)
      {
#ifdef RASTER_ENABLE_DEPTH_TEST
        grad_z.current = grad_z.current_left;
#endif
#ifdef RASTER_INTERPOLATE_W
        grad_w.current = grad_w.current_left;
#endif
#ifdef RASTER_ENABLE_SMOOTH_SHADING
        grad_c.current = grad_c.current_left;
#endif
#ifdef RASTER_ENABLE_TEXTURES
        grad_t.current = grad_t.current_left;
#endif

        uint32_t pixelIndex = iY * renderSurface->width() + left_x;
        RASTER_PIXEL_TYPE * pDestPixel = ((RASTER_PIXEL_TYPE *)renderSurface->p) + pixelIndex;
#ifdef RASTER_ENABLE_DEPTH_TEST
        int32_t * pDepthPixel = pZBuffer_ + pixelIndex;
#endif
        while(iWidth--)
        {
          // Depth Test:
          //   First thing we do is the depth test, becouse
          //   if this fails, we don't have to calculate the
          //   color of the pixel. (the most time consuming part of the loop)
#ifdef RASTER_ENABLE_DEPTH_TEST
          uint32_t zz = grad_z.current >> SHIFT_Z;
          if(rasterDepth(zz, *pDepthPixel) == true)
#endif
          {
#ifndef RASTER_DIRECT
            // Get the fragment color
  #ifdef RASTER_ENABLE_SMOOTH_SHADING
            // Get smoothly interpolated color
            caccu.r = INTERP_TO_CALC_COLOR(grad_c.current.r);
            caccu.g = INTERP_TO_CALC_COLOR(grad_c.current.g);
            caccu.b = INTERP_TO_CALC_COLOR(grad_c.current.b);
            caccu.a = INTERP_TO_CALC_COLOR(grad_c.current.a);
  #else
            // Set flat filled color
            caccu = grad_c.current;
  #endif

            // Get the fragment texel
  #ifdef RASTER_ENABLE_TEXTURES
            SColor ctexture;
    #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
            float z = 1.0f / grad_w.current;

            // Get the level of detail (lambda)
            float lodbias = 0.0f;
            float lod = pCurrentTex_->lambda(grad_t.ddx.u * z, grad_t.ddy.u * z, grad_t.ddx.v * z, grad_t.ddy.v * z) + lodbias;

            pCurrentTex_->getTexel(ctexture, grad_t.current.u * z, grad_t.current.v * z, lod);
    #else
            pCurrentTex_->getTexel(ctexture, grad_t.current.u, grad_t.current.v, lod);
    #endif
            rasterTexture(caccu, caccu, ctexture);
  #endif

            // Alpha Test (RGBA only)
  #ifdef CONFIG_GL_ENABLE_ALPHA_TEST
            bool bAlphaPasses = true;
            if(bAlphaTestEnabled_ == true)
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
              SColor screen_pix;
              RASTER_COLOR_LOAD(screen_pix, *pDestPixel, SHIFT_COLOR_CALC);
              rasterBlend(caccu, caccu, screen_pix);
  #endif

  #ifdef RASTER_ENABLE_DEPTH_TEST
//              if(depthMask_ == true)
                *pDepthPixel = zz;
  #endif
              // Write the resulting fragment color to the framebuffer
              *pDestPixel = RASTER_COLOR_SAVE(caccu, SHIFT_COLOR_CALC);
            }
#else
  #ifdef RASTER_ENABLE_DEPTH_TEST
//              if(depthMask_ == true)
                *pDepthPixel = zz;
  #endif
  #ifdef RASTER_DIRECT_FLAT_COLOR
            *pDestPixel = flat_color;
  #endif
  #ifdef RASTER_DIRECT_TEXTURES
            SColor ctexture;
            pCurrentTex_->getTexel(ctexture, grad_t.current.u, grad_t.current.v, lod);
            *pDestPixel = RASTER_COLOR_SAVE(ctexture, SHIFT_COLOR_CALC);
  #endif
#endif
          }
#ifdef RASTER_ENABLE_DEPTH_TEST
          grad_z.current += grad_z.ddx;
#endif
#ifdef RASTER_INTERPOLATE_W
          grad_w.current += grad_w.ddx;
#endif
#ifdef RASTER_ENABLE_SMOOTH_SHADING
          grad_c.current += grad_c.ddx;
#endif
#ifdef RASTER_ENABLE_TEXTURES
          grad_t.current += grad_t.ddx;
#endif
          pDestPixel++;
#ifdef RASTER_ENABLE_DEPTH_TEST
          pDepthPixel++;
#endif
#ifdef ENABLE_PROFILING
          fillRate++;
#endif
        }
      }

      // Advance to the next line
      left_x += dxdy_left;
      dxdy_left_err += dxdy_left_mod;
      if(dxdy_left_err >= dxdy_left_den)
      {
        dxdy_left_err -= dxdy_left_den;

        left_x++;
#ifdef RASTER_ENABLE_DEPTH_TEST
        grad_z.current_left += grad_z.ddy_left + grad_z.ddx;
#endif
#ifdef RASTER_INTERPOLATE_W
        grad_w.current_left += grad_w.ddy_left + grad_w.ddx;
#endif
#ifdef RASTER_ENABLE_SMOOTH_SHADING
        grad_c.current_left += grad_c.ddy_left;
        grad_c.current_left += grad_c.ddx;
#endif
#ifdef RASTER_ENABLE_TEXTURES
        grad_t.current_left += grad_t.ddy_left;
        grad_t.current_left += grad_t.ddx;
#endif
      }
      else
      {
#ifdef RASTER_ENABLE_DEPTH_TEST
        grad_z.current_left += grad_z.ddy_left;
#endif
#ifdef RASTER_INTERPOLATE_W
        grad_w.current_left += grad_w.ddy_left;
#endif
#ifdef RASTER_ENABLE_SMOOTH_SHADING
        grad_c.current_left += grad_c.ddy_left;
#endif
#ifdef RASTER_ENABLE_TEXTURES
        grad_t.current_left += grad_t.ddy_left;
#endif
      }

      // Advance to the next line
      right_x += dxdy_right;
      dxdy_right_err += dxdy_right_mod;
      if(dxdy_right_err >= dxdy_right_den)
      {
        dxdy_right_err -= dxdy_right_den;
        right_x++;
      }
    }
  }


#undef RASTER_COLOR_LOAD
#undef RASTER_COLOR_SAVE
#undef RASTER_PIXEL_TYPE
#undef RASTER_DIRECT
#undef RASTER_DIRECT_FLAT_COLOR
#undef RASTER_DIRECT_TEXTURES

#undef RASTER_ENABLE_DEPTH_TEST
#undef RASTER_ENABLE_SMOOTH_SHADING
#undef RASTER_ENABLE_TEXTURES
#undef RASTER_ENABLE_BLENDING
#undef RASTER_INTERPOLATE_W
#undef RASTER_INTERPOLATE_FLOATS
