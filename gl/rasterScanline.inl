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
#if defined(RASTER_ENABLE_TEXTURES) || defined(RASTER_ENABLE_DEPTH_TEST) || defined(RASTER_INTERPOLATE_W)
  #define RASTER_INTERPOLATE_FLOATS
#endif


#ifndef RASTER_DIRECT
  // Color accumulator, used in inner loop
  CInt32_4 caccu;
#endif

#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  // Premultiply u and v with w (1/z)
  float top_u_inv    = vtop->t[0]    * vtop->w;
  float top_v_inv    = vtop->t[1]    * vtop->w;
  float middle_u_inv = vmiddle->t[0] * vmiddle->w;
  float middle_v_inv = vmiddle->t[1] * vmiddle->w;
  float bottom_u_inv = vbottom->t[0] * vbottom->w;
  float bottom_v_inv = vbottom->t[1] * vbottom->w;
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

  CDDA left_edge;
  CDDA right_edge;

#ifdef RASTER_ENABLE_SMOOTH_SHADING
  // Color interpolation
  d1 = vmiddle->c.r - vtop->c.r;
  d2 = vbottom->c.r - vtop->c.r;
  grad_c_ddx.r = fpfromf(SHIFT_COLOR_INTERP, DDX());
  grad_c_ddy.r = fpfromf(SHIFT_COLOR_INTERP, DDY());
  d1 = vmiddle->c.g - vtop->c.g;
  d2 = vbottom->c.g - vtop->c.g;
  grad_c_ddx.g = fpfromf(SHIFT_COLOR_INTERP, DDX());
  grad_c_ddy.g = fpfromf(SHIFT_COLOR_INTERP, DDY());
  d1 = vmiddle->c.b - vtop->c.b;
  d2 = vbottom->c.b - vtop->c.b;
  grad_c_ddx.b = fpfromf(SHIFT_COLOR_INTERP, DDX());
  grad_c_ddy.b = fpfromf(SHIFT_COLOR_INTERP, DDY());
  d1 = vmiddle->c.a - vtop->c.a;
  d2 = vbottom->c.a - vtop->c.a;
  grad_c_ddx.a = fpfromf(SHIFT_COLOR_INTERP, DDX());
  grad_c_ddy.a = fpfromf(SHIFT_COLOR_INTERP, DDY());

  scan_c_ddx = grad_c_ddx;
#else
  #ifdef RASTER_DIRECT
    #ifdef RASTER_DIRECT_FLAT_COLOR
  // Directly ouput flat color
  RASTER_PIXEL_TYPE flat_color = vtop->c; // FIXME
    #endif
  #else
  // Setup a flat color
  scan_c_current.r = fpfromf(SHIFT_COLOR_CALC, vtop->c.r);
  scan_c_current.g = fpfromf(SHIFT_COLOR_CALC, vtop->c.g);
  scan_c_current.b = fpfromf(SHIFT_COLOR_CALC, vtop->c.b);
  scan_c_current.a = fpfromf(SHIFT_COLOR_CALC, vtop->c.a);
  #endif
#endif

#ifdef RASTER_ENABLE_DEPTH_TEST
  // Depth interpolation
  d1 = vmiddle->z - vtop->z;
  d2 = vbottom->z - vtop->z;
  grad_z_ddx = DDX();
  grad_z_ddy = DDY();

  scan_z_ddx = grad_z_ddx;
#endif

#ifdef RASTER_INTERPOLATE_W
  // Linear depth interpolation (for perspective correct rendering)
  d1 = vmiddle->w - vtop->w;
  d2 = vbottom->w - vtop->w;
  grad_w_ddx = DDX();
  grad_w_ddy = DDY();

  scan_w_ddx = grad_w_ddx;
#endif

#ifdef RASTER_ENABLE_TEXTURES
  // Texture coordinate interpolation
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
  d1 = middle_u_inv - top_u_inv;
  d2 = bottom_u_inv - top_u_inv;
  grad_tz_ddx[0] = DDX();
  grad_tz_ddy[0] = DDY();
  d1 = middle_v_inv - top_v_inv;
  d2 = bottom_v_inv - top_v_inv;
  grad_tz_ddx[1] = DDX();
  grad_tz_ddy[1] = DDY();

  scan_tz_ddx = grad_tz_ddx;
  #else
  d1 = vmiddle->t[0] - vtop->t[0];
  d2 = vbottom->t[0] - vtop->t[0];
  grad_t_ddx[0] = DDX();
  grad_t_ddy[0] = DDY();
  d1 = vmiddle->t[1] - vtop->t[1];
  d2 = vbottom->t[1] - vtop->t[1];
  grad_t_ddx[1] = DDX();
  grad_t_ddy[1] = DDY();

  scan_t_ddx = grad_t_ddx;

  // Get the level of detail (lambda)
  float lodbias = 0.0f;
  float lod = pCurrentTex_->lambda(grad_t_ddx.u, grad_t_ddy.u, grad_t_ddx.v, grad_t_ddy.v) + lodbias;
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
      iYTop    = fpfloor(SHIFT_XY, vtop->y    + fxPixelFloorOffset_);
      iYBottom = fpfloor(SHIFT_XY, vmiddle->y + fxPixelFloorOffset_);
      // Y Offset from vertex to pixel center
      fxYOff   = (iYTop << SHIFT_XY) + fxPixelCenterOffset_ - vtop->y;
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
      iYTop    = iYBottom;
      iYBottom = fpfloor(SHIFT_XY, vbottom->y + fxPixelFloorOffset_);
      // Y Offset from vertex to pixel center
      fxYOff   = (iYTop << SHIFT_XY) + fxPixelCenterOffset_ - vmiddle->y;
    }

    // Need to draw at least one line
    if((iYBottom - iYTop) <= 0)
      continue;

    if(bUpdateLeft == true)
    {
      bUpdateLeft = false;
      left_edge.init(vtop_l->x, vtop_l->y, vbottom_l->x, vbottom_l->y);

#if defined(RASTER_ENABLE_DEPTH_TEST) || defined(RASTER_ENABLE_SMOOTH_SHADING) || defined(RASTER_ENABLE_TEXTURES) || defined(RASTER_INTERPOLATE_W)
      int32_t fxXOff = (left_edge.x() << SHIFT_XY) + fxPixelCenterOffset_ - vtop_l->x;
#endif

#ifdef RASTER_INTERPOLATE_FLOATS
      float f_dxdy_left = (float)left_edge.dxdy();
      float fXOff = fptof(SHIFT_XY, fxXOff);
      float fYOff = fptof(SHIFT_XY, fxYOff);
#endif

#ifdef RASTER_ENABLE_SMOOTH_SHADING
      edge_c_increment = (grad_c_ddx * left_edge.dxdy()) + grad_c_ddy;
      edge_c_current.r = fpfromf(SHIFT_COLOR_INTERP, vtop_l->c.r);
      edge_c_current.g = fpfromf(SHIFT_COLOR_INTERP, vtop_l->c.g);
      edge_c_current.b = fpfromf(SHIFT_COLOR_INTERP, vtop_l->c.b);
      edge_c_current.a = fpfromf(SHIFT_COLOR_INTERP, vtop_l->c.a);
      edge_c_current += (((grad_c_ddx * fxXOff) + (grad_c_ddy * fxYOff)) >> SHIFT_XY);
#endif

#ifdef RASTER_ENABLE_DEPTH_TEST
      edge_z_increment = (f_dxdy_left * grad_z_ddx) + grad_z_ddy;
      edge_z_current = vtop_l->z + (grad_z_ddx * fXOff) + (grad_z_ddy * fYOff);
#endif

#ifdef RASTER_INTERPOLATE_W
      edge_w_increment = (f_dxdy_left * grad_w_ddx) + grad_w_ddy;
      edge_w_current = vtop_l->w + (grad_w_ddx * fXOff) + (grad_w_ddy * fYOff);
#endif

#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
      edge_tz_increment[0] = (f_dxdy_left * grad_tz_ddx[0]) + grad_tz_ddy[0];
      edge_tz_increment[1] = (f_dxdy_left * grad_tz_ddx[1]) + grad_tz_ddy[1];
      edge_tz_current[0] = (vtop_l->t[0] * vtop_l->w)+ (grad_tz_ddx[0] * fXOff) + (grad_tz_ddy[0] * fYOff);
      edge_tz_current[1] = (vtop_l->t[1] * vtop_l->w)+ (grad_tz_ddx[1] * fXOff) + (grad_tz_ddy[1] * fYOff);
  #else
      edge_t_increment[0] = (f_dxdy_left * grad_t_ddx[0]) + grad_t_ddy[0];
      edge_t_increment[1] = (f_dxdy_left * grad_t_ddx[1]) + grad_t_ddy[1];
      edge_t_current[0] = vtop_l->t[0] + (grad_t_ddx[0] * fXOff) + (grad_t_ddy[0] * fYOff);
      edge_t_current[1] = vtop_l->t[1] + (grad_t_ddx[1] * fXOff) + (grad_t_ddy[1] * fYOff);
  #endif
#endif

    }
    if(bUpdateRight == true)
    {
      bUpdateRight = false;
      right_edge.init(vtop_r->x, vtop_r->y, vbottom_r->x, vbottom_r->y);
    }

    for(int32_t iY = iYTop; iY < iYBottom; iY++)
    {
      int32_t iLineWidth = right_edge.x() - left_edge.x();

      if(iLineWidth > 0)
      {
#ifdef ENABLE_PROFILING_FILLRATE
        fillRate += iLineWidth;
#endif
#ifdef RASTER_ENABLE_SMOOTH_SHADING
        scan_c_current = edge_c_current;
#endif
#ifdef RASTER_ENABLE_DEPTH_TEST
        scan_z_current = edge_z_current;
#endif
#ifdef RASTER_INTERPOLATE_W
        scan_w_current = edge_w_current;
#endif
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
        scan_tz_current = edge_tz_current;
  #else
        scan_t_current = edge_t_current;
  #endif
#endif

        uint32_t pixelIndex = iY * renderSurface->width() + left_edge.x();
        RASTER_PIXEL_TYPE * pDestPixel = ((RASTER_PIXEL_TYPE *)renderSurface->p) + pixelIndex;
#ifdef RASTER_ENABLE_DEPTH_TEST
        GLfloat * pDepthPixel = pZBuffer_ + pixelIndex;
#endif
        while(iLineWidth--)
        {
          // Depth Test:
          //   First thing we do is the depth test, becouse
          //   if this fails, we don't have to calculate the
          //   color of the pixel. (the most time consuming part of the loop)
#ifdef RASTER_ENABLE_DEPTH_TEST
          if(rasterTest(depthFunction_, scan_z_current, *pDepthPixel) == true)
#endif
          {
#ifndef RASTER_DIRECT
            // Get the fragment color
  #ifdef RASTER_ENABLE_SMOOTH_SHADING
            // Get smoothly interpolated color
            caccu = scan_c_current >> (SHIFT_COLOR_INTERP - SHIFT_COLOR_CALC);
  #else
            // Set flat filled color
            caccu = scan_c_current;
  #endif

            // Get the fragment texel
  #ifdef RASTER_ENABLE_TEXTURES
            CInt32_4 ctexture;
    #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
            float z = 1.0f / scan_w_current;

            // Get the level of detail (lambda)
            //float lodbias = 0.0f;
            //float lod = pCurrentTex_->lambda(grad_tz_ddx[0] * z, grad_tz_ddy[0] * z, grad_tz_ddx[1] * z, grad_tz_ddy[1] * z) + lodbias;

            pCurrentTex_->getTexel(ctexture, scan_tz_current[0] * z, scan_tz_current[1] * z/*, lod*/);
    #else
            pCurrentTex_->getTexel(ctexture, scan_t_current[0], scan_t_current[1]/*, lod*/);
    #endif
            rasterTexture(caccu, caccu, ctexture);
  #endif

            // Alpha Test (RGBA only)
  #ifdef CONFIG_GL_ENABLE_ALPHA_TEST
            if((bAlphaTestEnabled_ == false) || (rasterTest(alphaFunc_, caccu.a, alphaValueFX_) == true))
  #endif
            {
              // Blending (RGBA only): Blend the fragment color with the framebuffer color
  #ifdef RASTER_ENABLE_BLENDING
              CInt32_4 screen_pix;
              RASTER_COLOR_LOAD(screen_pix, *pDestPixel, SHIFT_COLOR_CALC);
              rasterBlend(caccu, caccu, screen_pix);
  #endif

  #ifdef RASTER_ENABLE_DEPTH_TEST
              if(bDepthMask_ == true)
                *pDepthPixel = scan_z_current;
  #endif
              // Write the resulting fragment color to the framebuffer
              *pDestPixel = RASTER_COLOR_SAVE(caccu, SHIFT_COLOR_CALC);
            }
#else
  #ifdef RASTER_ENABLE_DEPTH_TEST
              if(bDepthMask_ == true)
                *pDepthPixel = scan_z_current;
  #endif
  #ifdef RASTER_DIRECT_FLAT_COLOR
            *pDestPixel = flat_color;
  #endif
  #ifdef RASTER_DIRECT_TEXTURES
            CInt32_4 ctexture;
            pCurrentTex_->getTexel(ctexture, scan_t_current.u, scan_t_current.v, lod);
            *pDestPixel = RASTER_COLOR_SAVE(ctexture, SHIFT_COLOR_CALC);
  #endif
#endif
          }
#ifdef RASTER_ENABLE_SMOOTH_SHADING
          scan_c_current += scan_c_ddx;
#endif
#ifdef RASTER_ENABLE_DEPTH_TEST
          scan_z_current += scan_z_ddx;
#endif
#ifdef RASTER_INTERPOLATE_W
          scan_w_current += scan_w_ddx;
#endif
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
          scan_tz_current += scan_tz_ddx;
  #else
          scan_t_current += scan_t_ddx;
  #endif
#endif
          pDestPixel++;
#ifdef RASTER_ENABLE_DEPTH_TEST
          pDepthPixel++;
#endif
        }
      }

#ifdef RASTER_ENABLE_SMOOTH_SHADING
      edge_c_current += edge_c_increment;
#endif
#ifdef RASTER_ENABLE_DEPTH_TEST
      edge_z_current += edge_z_increment;
#endif
#ifdef RASTER_INTERPOLATE_W
      edge_w_current += edge_w_increment;
#endif
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
      edge_tz_current += edge_tz_increment;
  #else
      edge_t_current += edge_t_increment;
  #endif
#endif

      // Advance left edge to the next line
      // If x makes an extra step, all others need to make an extra step as well
      if(left_edge.advance())
      {
#ifdef RASTER_ENABLE_SMOOTH_SHADING
        edge_c_current += grad_c_ddx;
#endif
#ifdef RASTER_ENABLE_DEPTH_TEST
        edge_z_current += grad_z_ddx;
#endif
#ifdef RASTER_INTERPOLATE_W
        edge_w_current += grad_w_ddx;
#endif
#ifdef RASTER_ENABLE_TEXTURES
  #ifdef CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
        edge_tz_current += grad_tz_ddx;
  #else
        edge_t_current += grad_t_ddx;
  #endif
#endif
      }

      // Advance right edge to the next line
      right_edge.advance();
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
