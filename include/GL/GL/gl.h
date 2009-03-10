/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
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


#ifndef GL_GL_H
#define GL_GL_H


#ifndef GL_API
  #define GL_API extern
#endif
#ifndef GLAPI
  #define GLAPI GL_API
#endif
#ifndef GL_APIENTRY
  #define GL_APIENTRY
#endif
#ifndef GLAPIENTRY
  #define GLAPIENTRY GL_APIENTRY
#endif


#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned int    GLenum;
typedef unsigned char   GLboolean;
typedef unsigned int    GLbitfield;
typedef signed char     GLbyte;
typedef short           GLshort;
typedef int             GLint;
typedef int             GLsizei;
typedef unsigned char   GLubyte;
typedef unsigned short  GLushort;
typedef unsigned int    GLuint;
typedef float           GLfloat;
typedef float           GLclampf;
typedef void            GLvoid;
typedef int             GLfixed;
typedef int             GLclampx;

typedef int             GLintptr;
typedef int             GLsizeiptr;

typedef double          GLdouble;
typedef double          GLclampd;


// Boolean values
#define GL_FALSE                                0x0
#define GL_TRUE                                 0x1

// Data types
#define GL_BYTE                                 0x1400
#define GL_UNSIGNED_BYTE                        0x1401
#define GL_SHORT                                0x1402
#define GL_UNSIGNED_SHORT                       0x1403
#define GL_INT                                  0x1404
#define GL_UNSIGNED_INT                         0x1405
#define GL_FLOAT                                0x1406
#define GL_2_BYTES                              0x1407
#define GL_3_BYTES                              0x1408
#define GL_4_BYTES                              0x1409
#define GL_DOUBLE                               0x140a
#define GL_FIXED                                0x140c // OpenGL-ES ???

// Primitives
#define GL_POINTS                               0x0000
#define GL_LINES                                0x0001
#define GL_LINE_LOOP                            0x0002
#define GL_LINE_STRIP                           0x0003
#define GL_TRIANGLES                            0x0004
#define GL_TRIANGLE_STRIP                       0x0005
#define GL_TRIANGLE_FAN                         0x0006
#define GL_QUADS                                0x0007
#define GL_QUAD_STRIP                           0x0008
#define GL_POLYGON                              0x0009

// Vertex Arrays
#define GL_VERTEX_ARRAY                         0x8074
#define GL_NORMAL_ARRAY                         0x8075
#define GL_COLOR_ARRAY                          0x8076
#define GL_INDEX_ARRAY                          0x8077
#define GL_TEXTURE_COORD_ARRAY                  0x8078
#define GL_EDGE_FLAG_ARRAY                      0x8079
#define GL_VERTEX_ARRAY_SIZE                    0x807a
#define GL_VERTEX_ARRAY_TYPE                    0x807b
#define GL_VERTEX_ARRAY_STRIDE                  0x807c
#define GL_NORMAL_ARRAY_TYPE                    0x807e
#define GL_NORMAL_ARRAY_STRIDE                  0x807f
#define GL_COLOR_ARRAY_SIZE                     0x8081
#define GL_COLOR_ARRAY_TYPE                     0x8082
#define GL_COLOR_ARRAY_STRIDE                   0x8083
#define GL_INDEX_ARRAY_TYPE                     0x8085
#define GL_INDEX_ARRAY_STRIDE                   0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE             0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE             0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE           0x808a
#define GL_EDGE_FLAG_ARRAY_STRIDE               0x808c
#define GL_VERTEX_ARRAY_POINTER                 0x808e
#define GL_NORMAL_ARRAY_POINTER                 0x808f
#define GL_COLOR_ARRAY_POINTER                  0x8090
#define GL_INDEX_ARRAY_POINTER                  0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER          0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER              0x8093
#define GL_V2F                                  0x2a20
#define GL_V3F                                  0x2a21
#define GL_C4UB_V2F                             0x2a22
#define GL_C4UB_V3F                             0x2a23
#define GL_C3F_V3F                              0x2a24
#define GL_N3F_V3F                              0x2a25
#define GL_C4F_N3F_V3F                          0x2a26
#define GL_T2F_V3F                              0x2a27
#define GL_T4F_V4F                              0x2a28
#define GL_T2F_C4UB_V3F                         0x2a29
#define GL_T2F_C3F_V3F                          0x2a2a
#define GL_T2F_N3F_V3F                          0x2a2b
#define GL_T2F_C4F_N3F_V3F                      0x2a2c
#define GL_T4F_C4F_N3F_V4F                      0x2a2d

// Matrix Mode
#define GL_MATRIX_MODE                          0x0ba0
#define GL_MODELVIEW                            0x1700
#define GL_PROJECTION                           0x1701
#define GL_TEXTURE                              0x1702

// Points
#define GL_POINT_SMOOTH                         0x0b10
#define GL_POINT_SIZE                           0x0b11
#define GL_POINT_SIZE_GRANULARITY               0x0b13
#define GL_POINT_SIZE_RANGE                     0x0b12

// Lines
#define GL_LINE_SMOOTH                          0x0b20
#define GL_LINE_STIPPLE                         0x0b24
#define GL_LINE_STIPPLE_PATTERN                 0x0b25
#define GL_LINE_STIPPLE_REPEAT                  0x0b26
#define GL_LINE_WIDTH                           0x0b21
#define GL_LINE_WIDTH_GRANULARITY               0x0b23
#define GL_LINE_WIDTH_RANGE                     0x0b22

// Polygons
#define GL_POINT                                0x1b00
#define GL_LINE                                 0x1b01
#define GL_FILL                                 0x1b02
#define GL_CW                                   0x0900
#define GL_CCW                                  0x0901
#define GL_FRONT                                0x0404
#define GL_BACK                                 0x0405
#define GL_POLYGON_MODE                         0x0b40
#define GL_POLYGON_SMOOTH                       0x0b41
#define GL_POLYGON_STIPPLE                      0x0b42
#define GL_EDGE_FLAG                            0x0b43
#define GL_CULL_FACE                            0x0b44
#define GL_CULL_FACE_MODE                       0x0b45
#define GL_FRONT_FACE                           0x0b46
#define GL_POLYGON_OFFSET_FACTOR                0x8038
#define GL_POLYGON_OFFSET_UNITS                 0x2a00
#define GL_POLYGON_OFFSET_POINT                 0x2a01
#define GL_POLYGON_OFFSET_LINE                  0x2a02
#define GL_POLYGON_OFFSET_FILL                  0x8037

// Display Lists
#define GL_COMPILE                              0x1300
#define GL_COMPILE_AND_EXECUTE                  0x1301
#define GL_LIST_BASE                            0x0b32
#define GL_LIST_INDEX                           0x0b33
#define GL_LIST_MODE                            0x0b30

// Depth buffer
#define GL_NEVER                                0x0200
#define GL_LESS                                 0x0201
#define GL_EQUAL                                0x0202
#define GL_LEQUAL                               0x0203
#define GL_GREATER                              0x0204
#define GL_NOTEQUAL                             0x0205
#define GL_GEQUAL                               0x0206
#define GL_ALWAYS                               0x0207
#define GL_DEPTH_TEST                           0x0b71
#define GL_DEPTH_BITS                           0x0d56
#define GL_DEPTH_CLEAR_VALUE                    0x0b73
#define GL_DEPTH_FUNC                           0x0b74
#define GL_DEPTH_RANGE                          0x0b70
#define GL_DEPTH_WRITEMASK                      0x0b72
#define GL_DEPTH_COMPONENT                      0x1902

// Lighting
#define GL_LIGHTING                             0x0b50
#define GL_LIGHT0                               0x4000
#define GL_LIGHT1                               0x4001
#define GL_LIGHT2                               0x4002
#define GL_LIGHT3                               0x4003
#define GL_LIGHT4                               0x4004
#define GL_LIGHT5                               0x4005
#define GL_LIGHT6                               0x4006
#define GL_LIGHT7                               0x4007
#define GL_SPOT_EXPONENT                        0x1205
#define GL_SPOT_CUTOFF                          0x1206
#define GL_CONSTANT_ATTENUATION                 0x1207
#define GL_LINEAR_ATTENUATION                   0x1208
#define GL_QUADRATIC_ATTENUATION                0x1209
#define GL_AMBIENT                              0x1200
#define GL_DIFFUSE                              0x1201
#define GL_SPECULAR                             0x1202
#define GL_SHININESS                            0x1601
#define GL_EMISSION                             0x1600
#define GL_POSITION                             0x1203
#define GL_SPOT_DIRECTION                       0x1204
#define GL_AMBIENT_AND_DIFFUSE                  0x1602
#define GL_COLOR_INDEXES                        0x1603
#define GL_LIGHT_MODEL_TWO_SIDE                 0x0b52
#define GL_LIGHT_MODEL_LOCAL_VIEWER             0x0b51
#define GL_LIGHT_MODEL_AMBIENT                  0x0b53
#define GL_FRONT_AND_BACK                       0x0408
#define GL_SHADE_MODEL                          0x0b54
#define GL_FLAT                                 0x1d00
#define GL_SMOOTH                               0x1d01
#define GL_COLOR_MATERIAL                       0x0b57
#define GL_COLOR_MATERIAL_FACE                  0x0b55
#define GL_COLOR_MATERIAL_PARAMETER             0x0b56
#define GL_NORMALIZE                            0x0ba1

// User clipping planes
#define GL_CLIP_PLANE0                          0x3000
#define GL_CLIP_PLANE1                          0x3001
#define GL_CLIP_PLANE2                          0x3002
#define GL_CLIP_PLANE3                          0x3003
#define GL_CLIP_PLANE4                          0x3004
#define GL_CLIP_PLANE5                          0x3005

// Accumulation buffer
#define GL_ACCUM_RED_BITS                       0x0d58
#define GL_ACCUM_GREEN_BITS                     0x0d59
#define GL_ACCUM_BLUE_BITS                      0x0d5a
#define GL_ACCUM_ALPHA_BITS                     0x0d5b
#define GL_ACCUM_CLEAR_VALUE                    0x0b80
#define GL_ACCUM                                0x0100
#define GL_ADD                                  0x0104
#define GL_LOAD                                 0x0101
#define GL_MULT                                 0x0103
#define GL_RETURN                               0x0102

// Alpha testing
#define GL_ALPHA_TEST                           0x0bc0
#define GL_ALPHA_TEST_REF                       0x0bc2
#define GL_ALPHA_TEST_FUNC                      0x0bc1

// Blending
#define GL_BLEND                                0x0be2
#define GL_BLEND_SRC                            0x0be1
#define GL_BLEND_DST                            0x0be0
#define GL_ZERO                                 0x0
#define GL_ONE                                  0x1
#define GL_SRC_COLOR                            0x0300
#define GL_ONE_MINUS_SRC_COLOR                  0x0301
#define GL_SRC_ALPHA                            0x0302
#define GL_ONE_MINUS_SRC_ALPHA                  0x0303
#define GL_DST_ALPHA                            0x0304
#define GL_ONE_MINUS_DST_ALPHA                  0x0305
#define GL_DST_COLOR                            0x0306
#define GL_ONE_MINUS_DST_COLOR                  0x0307
#define GL_SRC_ALPHA_SATURATE                   0x0308

// Render Mode
#define GL_FEEDBACK                             0x1c01
#define GL_RENDER                               0x1c00
#define GL_SELECT                               0x1c02

// Feedback
#define GL_2D                                   0x0600
#define GL_3D                                   0x0601
#define GL_3D_COLOR                             0x0602
#define GL_3D_COLOR_TEXTURE                     0x0603
#define GL_4D_COLOR_TEXTURE                     0x0604
#define GL_POINT_TOKEN                          0x0701
#define GL_LINE_TOKEN                           0x0702
#define GL_LINE_RESET_TOKEN                     0x0707
#define GL_POLYGON_TOKEN                        0x0703
#define GL_BITMAP_TOKEN                         0x0704
#define GL_DRAW_PIXEL_TOKEN                     0x0705
#define GL_COPY_PIXEL_TOKEN                     0x0706
#define GL_PASS_THROUGH_TOKEN                   0x0700
#define GL_FEEDBACK_BUFFER_POINTER              0x0df0
#define GL_FEEDBACK_BUFFER_SIZE                 0x0df1
#define GL_FEEDBACK_BUFFER_TYPE                 0x0df2

// Selection
#define GL_SELECTION_BUFFER_POINTER             0x0df3
#define GL_SELECTION_BUFFER_SIZE                0x0df4

// Fog
#define GL_FOG                                  0x0b60
#define GL_FOG_MODE                             0x0b65
#define GL_FOG_DENSITY                          0x0b62
#define GL_FOG_COLOR                            0x0b66
#define GL_FOG_INDEX                            0x0b61
#define GL_FOG_START                            0x0b63
#define GL_FOG_END                              0x0b64
#define GL_LINEAR                               0x2601
#define GL_EXP                                  0x0800
#define GL_EXP2                                 0x0801

// Logic Ops
#define GL_LOGIC_OP                             0x0bf1
#define GL_INDEX_LOGIC_OP                       0x0bf1
#define GL_COLOR_LOGIC_OP                       0x0bf2
#define GL_LOGIC_OP_MODE                        0x0bf0
#define GL_CLEAR                                0x1500
#define GL_SET                                  0x150f
#define GL_COPY                                 0x1503
#define GL_COPY_INVERTED                        0x150c
#define GL_NOOP                                 0x1505
#define GL_INVERT                               0x150a
#define GL_AND                                  0x1501
#define GL_NAND                                 0x150e
#define GL_OR                                   0x1507
#define GL_NOR                                  0x1508
#define GL_XOR                                  0x1506
#define GL_EQUIV                                0x1509
#define GL_AND_REVERSE                          0x1502
#define GL_AND_INVERTED                         0x1504
#define GL_OR_REVERSE                           0x150b
#define GL_OR_INVERTED                          0x150d

// Stencil
#define GL_STENCIL_BITS                         0x0d57
#define GL_STENCIL_TEST                         0x0b90
#define GL_STENCIL_CLEAR_VALUE                  0x0b91
#define GL_STENCIL_FUNC                         0x0b92
#define GL_STENCIL_VALUE_MASK                   0x0b93
#define GL_STENCIL_FAIL                         0x0b94
#define GL_STENCIL_PASS_DEPTH_FAIL              0x0b95
#define GL_STENCIL_PASS_DEPTH_PASS              0x0b96
#define GL_STENCIL_REF                          0x0b97
#define GL_STENCIL_WRITEMASK                    0x0b98
#define GL_STENCIL_INDEX                        0x1901
#define GL_KEEP                                 0x1e00
#define GL_REPLACE                              0x1e01
#define GL_INCR                                 0x1e02
#define GL_DECR                                 0x1e03

// Buffers, Pixel Drawing/Reading
#define GL_NONE                                 0x0
#define GL_LEFT                                 0x0406
#define GL_RIGHT                                0x0407
//GL_FRONT                                      0x0404
//GL_BACK                                       0x0405
//GL_FRONT_AND_BACK                             0x0408
#define GL_FRONT_LEFT                           0x0400
#define GL_FRONT_RIGHT                          0x0401
#define GL_BACK_LEFT                            0x0402
#define GL_BACK_RIGHT                           0x0403
#define GL_AUX0                                 0x0409
#define GL_AUX1                                 0x040a
#define GL_AUX2                                 0x040b
#define GL_AUX3                                 0x040C
#define GL_COLOR_INDEX                          0x1900
#define GL_RED                                  0x1903
#define GL_GREEN                                0x1904
#define GL_BLUE                                 0x1905
#define GL_ALPHA                                0x1906
#define GL_LUMINANCE                            0x1909
#define GL_LUMINANCE_ALPHA                      0x190a
#define GL_ALPHA_BITS                           0x0d55
#define GL_RED_BITS                             0x0d52
#define GL_GREEN_BITS                           0x0d53
#define GL_BLUE_BITS                            0x0d54
#define GL_INDEX_BITS                           0x0d51
#define GL_SUBPIXEL_BITS                        0x0d50
#define GL_AUX_BUFFERS                          0x0c00
#define GL_READ_BUFFER                          0x0c02
#define GL_DRAW_BUFFER                          0x0c01
#define GL_DOUBLEBUFFER                         0x0c32
#define GL_STEREO                               0x0c33
#define GL_BITMAP                               0x1a00
#define GL_COLOR                                0x1800
#define GL_DEPTH                                0x1801
#define GL_STENCIL                              0x1802
#define GL_DITHER                               0x0bd0
#define GL_RGB                                  0x1907
#define GL_RGBA                                 0x1908
#define GL_BGR                                  0x80e0 // OpenGL 1.2
#define GL_BGRA                                 0x80e1 // OpenGL 1.2

// Pixel Types
#define GL_UNSIGNED_BYTE_3_3_2                  0x8032 // OpenGL 1.2
#define GL_UNSIGNED_BYTE_2_3_3_REV              0x8362 // OpenGL 1.2
#define GL_UNSIGNED_SHORT_5_6_5                 0x8363 // OpenGL ???
#define GL_UNSIGNED_SHORT_5_6_5_REV             0x8364 // OpenGL 1.2
#define GL_UNSIGNED_SHORT_4_4_4_4               0x8033 // OpenGL ???
#define GL_UNSIGNED_SHORT_4_4_4_4_REV           0x8365 // OpenGL 1.2
#define GL_UNSIGNED_SHORT_5_5_5_1               0x8034 // OpenGL ???
#define GL_UNSIGNED_SHORT_1_5_5_5_REV           0x8366 // OpenGL 1.2
#define GL_UNSIGNED_INT_8_8_8_8                 0x8035 // OpenGL 1.2
#define GL_UNSIGNED_INT_8_8_8_8_REV             0x8367 // OpenGL 1.2
#define GL_UNSIGNED_INT_10_10_10_2              0x8036 // OpenGL 1.2
#define GL_UNSIGNED_INT_2_10_10_10_REV          0x8368 // OpenGL 1.2

// Implementation limits
#define GL_MAX_LIST_NESTING                     0x0b31
#define GL_MAX_EVAL_ORDER                       0x0d30
#define GL_MAX_LIGHTS                           0x0d31
#define GL_MAX_CLIP_PLANES                      0x0d32
#define GL_MAX_TEXTURE_SIZE                     0x0d33
#define GL_MAX_PIXEL_MAP_TABLE                  0x0d34
#define GL_MAX_ATTRIB_STACK_DEPTH               0x0d35
#define GL_MAX_MODELVIEW_STACK_DEPTH            0x0d36
#define GL_MAX_NAME_STACK_DEPTH                 0x0d37
#define GL_MAX_PROJECTION_STACK_DEPTH           0x0d38
#define GL_MAX_TEXTURE_STACK_DEPTH              0x0d39
#define GL_MAX_VIEWPORT_DIMS                    0x0d3a
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH        0x0d3b

// Gets
#define GL_ATTRIB_STACK_DEPTH                   0x0bb0
#define GL_CLIENT_ATTRIB_STACK_DEPTH            0x0bb1
#define GL_COLOR_CLEAR_VALUE                    0x0c22
#define GL_COLOR_WRITEMASK                      0x0c23
#define GL_CURRENT_INDEX                        0x0b01
#define GL_CURRENT_COLOR                        0x0b00
#define GL_CURRENT_NORMAL                       0x0b02
#define GL_CURRENT_RASTER_COLOR                 0x0b04
#define GL_CURRENT_RASTER_DISTANCE              0x0b09
#define GL_CURRENT_RASTER_INDEX                 0x0b05
#define GL_CURRENT_RASTER_POSITION              0x0b07
#define GL_CURRENT_RASTER_TEXTURE_COORDS        0x0b06
#define GL_CURRENT_RASTER_POSITION_VALID        0x0b08
#define GL_CURRENT_TEXTURE_COORDS               0x0b03
#define GL_INDEX_CLEAR_VALUE                    0x0c20
#define GL_INDEX_MODE                           0x0c30
#define GL_INDEX_WRITEMASK                      0x0c21
#define GL_MODELVIEW_MATRIX                     0x0ba6
#define GL_MODELVIEW_STACK_DEPTH                0x0ba3
#define GL_NAME_STACK_DEPTH                     0x0d70
#define GL_PROJECTION_MATRIX                    0x0ba7
#define GL_PROJECTION_STACK_DEPTH               0x0ba4
#define GL_RENDER_MODE                          0x0c40
#define GL_RGBA_MODE                            0x0c31
#define GL_TEXTURE_MATRIX                       0x0ba8
#define GL_TEXTURE_STACK_DEPTH                  0x0ba5
#define GL_VIEWPORT                             0x0ba2

// Evaluators
#define GL_AUTO_NORMAL                          0x0d80
#define GL_MAP1_COLOR_4                         0x0d90
#define GL_MAP1_INDEX                           0x0d91
#define GL_MAP1_NORMAL                          0x0d92
#define GL_MAP1_TEXTURE_COORD_1                 0x0d93
#define GL_MAP1_TEXTURE_COORD_2                 0x0d94
#define GL_MAP1_TEXTURE_COORD_3                 0x0d95
#define GL_MAP1_TEXTURE_COORD_4                 0x0d96
#define GL_MAP1_VERTEX_3                        0x0d97
#define GL_MAP1_VERTEX_4                        0x0d98
#define GL_MAP2_COLOR_4                         0x0db0
#define GL_MAP2_INDEX                           0x0db1
#define GL_MAP2_NORMAL                          0x0db2
#define GL_MAP2_TEXTURE_COORD_1                 0x0db3
#define GL_MAP2_TEXTURE_COORD_2                 0x0db4
#define GL_MAP2_TEXTURE_COORD_3                 0x0db5
#define GL_MAP2_TEXTURE_COORD_4                 0x0db6
#define GL_MAP2_VERTEX_3                        0x0db7
#define GL_MAP2_VERTEX_4                        0x0db8
#define GL_MAP1_GRID_DOMAIN                     0x0dd0
#define GL_MAP1_GRID_SEGMENTS                   0x0dd1
#define GL_MAP2_GRID_DOMAIN                     0x0dd2
#define GL_MAP2_GRID_SEGMENTS                   0x0dd3
#define GL_COEFF                                0x0a00
#define GL_ORDER                                0x0a01
#define GL_DOMAIN                               0x0a02

// Hints
#define GL_PERSPECTIVE_CORRECTION_HINT          0x0c50
#define GL_POINT_SMOOTH_HINT                    0x0c51
#define GL_LINE_SMOOTH_HINT                     0x0c52
#define GL_POLYGON_SMOOTH_HINT                  0x0c53
#define GL_FOG_HINT                             0x0c54
#define GL_DONT_CARE                            0x1100
#define GL_FASTEST                              0x1101
#define GL_NICEST                               0x1102

// Scissor box
#define GL_SCISSOR_BOX                          0x0c10
#define GL_SCISSOR_TEST                         0x0c11

// Pixel Mode / Transfer
#define GL_MAP_COLOR                            0x0d10
#define GL_MAP_STENCIL                          0x0d11
#define GL_INDEX_SHIFT                          0x0d12
#define GL_INDEX_OFFSET                         0x0d13
#define GL_RED_SCALE                            0x0d14
#define GL_RED_BIAS                             0x0d15
#define GL_GREEN_SCALE                          0x0d18
#define GL_GREEN_BIAS                           0x0d19
#define GL_BLUE_SCALE                           0x0d1a
#define GL_BLUE_BIAS                            0x0d1b
#define GL_ALPHA_SCALE                          0x0d1c
#define GL_ALPHA_BIAS                           0x0d1d
#define GL_DEPTH_SCALE                          0x0d1e
#define GL_DEPTH_BIAS                           0x0d1f
#define GL_PIXEL_MAP_S_TO_S_SIZE                0x0cb1
#define GL_PIXEL_MAP_I_TO_I_SIZE                0x0cb0
#define GL_PIXEL_MAP_I_TO_R_SIZE                0x0cb2
#define GL_PIXEL_MAP_I_TO_G_SIZE                0x0cb3
#define GL_PIXEL_MAP_I_TO_B_SIZE                0x0cb4
#define GL_PIXEL_MAP_I_TO_A_SIZE                0x0cb5
#define GL_PIXEL_MAP_R_TO_R_SIZE                0x0cb6
#define GL_PIXEL_MAP_G_TO_G_SIZE                0x0cb7
#define GL_PIXEL_MAP_B_TO_B_SIZE                0x0cb8
#define GL_PIXEL_MAP_A_TO_A_SIZE                0x0cb9
#define GL_PIXEL_MAP_S_TO_S                     0x0c71
#define GL_PIXEL_MAP_I_TO_I                     0x0c70
#define GL_PIXEL_MAP_I_TO_R                     0x0c72
#define GL_PIXEL_MAP_I_TO_G                     0x0c73
#define GL_PIXEL_MAP_I_TO_B                     0x0c74
#define GL_PIXEL_MAP_I_TO_A                     0x0c75
#define GL_PIXEL_MAP_R_TO_R                     0x0c76
#define GL_PIXEL_MAP_G_TO_G                     0x0c77
#define GL_PIXEL_MAP_B_TO_B                     0x0c78
#define GL_PIXEL_MAP_A_TO_A                     0x0c79
#define GL_PACK_ALIGNMENT                       0x0d05
#define GL_PACK_LSB_FIRST                       0x0d01
#define GL_PACK_ROW_LENGTH                      0x0d02
#define GL_PACK_SKIP_PIXELS                     0x0d04
#define GL_PACK_SKIP_ROWS                       0x0d03
#define GL_PACK_SWAP_BYTES                      0x0d00
#define GL_UNPACK_ALIGNMENT                     0x0cF5
#define GL_UNPACK_LSB_FIRST                     0x0cF1
#define GL_UNPACK_ROW_LENGTH                    0x0cF2
#define GL_UNPACK_SKIP_PIXELS                   0x0cF4
#define GL_UNPACK_SKIP_ROWS                     0x0cF3
#define GL_UNPACK_SWAP_BYTES                    0x0cF0
#define GL_ZOOM_X                               0x0d16
#define GL_ZOOM_Y                               0x0d17

// Texture mapping
#define GL_TEXTURE_ENV                          0x2300
#define GL_TEXTURE_ENV_MODE                     0x2200
#define GL_TEXTURE_1D                           0x0de0
#define GL_TEXTURE_2D                           0x0de1
#define GL_TEXTURE_WRAP_S                       0x2802
#define GL_TEXTURE_WRAP_T                       0x2803
#define GL_TEXTURE_MAG_FILTER                   0x2800
#define GL_TEXTURE_MIN_FILTER                   0x2801
#define GL_TEXTURE_ENV_COLOR                    0x2201
#define GL_TEXTURE_GEN_S                        0x0c60
#define GL_TEXTURE_GEN_T                        0x0c61
#define GL_TEXTURE_GEN_MODE                     0x2500
#define GL_TEXTURE_BORDER_COLOR                 0x1004
#define GL_TEXTURE_WIDTH                        0x1000
#define GL_TEXTURE_HEIGHT                       0x1001
#define GL_TEXTURE_BORDER                       0x1005
#define GL_TEXTURE_COMPONENTS                   0x1003
#define GL_TEXTURE_RED_SIZE                     0x805c
#define GL_TEXTURE_GREEN_SIZE                   0x805d
#define GL_TEXTURE_BLUE_SIZE                    0x805e
#define GL_TEXTURE_ALPHA_SIZE                   0x805f
#define GL_TEXTURE_LUMINANCE_SIZE               0x8060
#define GL_TEXTURE_INTENSITY_SIZE               0x8061
#define GL_NEAREST_MIPMAP_NEAREST               0x2700
#define GL_NEAREST_MIPMAP_LINEAR                0x2702
#define GL_LINEAR_MIPMAP_NEAREST                0x2701
#define GL_LINEAR_MIPMAP_LINEAR                 0x2703
#define GL_OBJECT_LINEAR                        0x2401
#define GL_OBJECT_PLANE                         0x2501
#define GL_EYE_LINEAR                           0x2400
#define GL_EYE_PLANE                            0x2502
#define GL_SPHERE_MAP                           0x2402
#define GL_DECAL                                0x2101
#define GL_MODULATE                             0x2100
#define GL_NEAREST                              0x2600
#define GL_REPEAT                               0x2901
#define GL_CLAMP                                0x2900
#define GL_S                                    0x2000
#define GL_T                                    0x2001
#define GL_R                                    0x2002
#define GL_Q                                    0x2003
#define GL_TEXTURE_GEN_R                        0x0c62
#define GL_TEXTURE_GEN_Q                        0x0c63

// Utility
#define GL_VENDOR                               0x1f00
#define GL_RENDERER                             0x1f01
#define GL_VERSION                              0x1f02
#define GL_EXTENSIONS                           0x1f03

// Errors
#define GL_NO_ERROR                             0x0
#define GL_INVALID_ENUM                         0x0500
#define GL_INVALID_VALUE                        0x0501
#define GL_INVALID_OPERATION                    0x0502
#define GL_STACK_OVERFLOW                       0x0503
#define GL_STACK_UNDERFLOW                      0x0504
#define GL_OUT_OF_MEMORY                        0x0505

// glPush/PopAttrib bits
#define GL_CURRENT_BIT                          0x00000001
#define GL_POINT_BIT                            0x00000002
#define GL_LINE_BIT                             0x00000004
#define GL_POLYGON_BIT                          0x00000008
#define GL_POLYGON_STIPPLE_BIT                  0x00000010
#define GL_PIXEL_MODE_BIT                       0x00000020
#define GL_LIGHTING_BIT                         0x00000040
#define GL_FOG_BIT                              0x00000080
#define GL_DEPTH_BUFFER_BIT                     0x00000100
#define GL_ACCUM_BUFFER_BIT                     0x00000200
#define GL_STENCIL_BUFFER_BIT                   0x00000400
#define GL_VIEWPORT_BIT                         0x00000800
#define GL_TRANSFORM_BIT                        0x00001000
#define GL_ENABLE_BIT                           0x00002000
#define GL_COLOR_BUFFER_BIT                     0x00004000
#define GL_HINT_BIT                             0x00008000
#define GL_EVAL_BIT                             0x00010000
#define GL_LIST_BIT                             0x00020000
#define GL_TEXTURE_BIT                          0x00040000
#define GL_SCISSOR_BIT                          0x00080000
#define GL_ALL_ATTRIB_BITS                      0x000fffff


// OpenGL 1.1
#define GL_PROXY_TEXTURE_1D                     0x8063
#define GL_PROXY_TEXTURE_2D                     0x8064
#define GL_TEXTURE_PRIORITY                     0x8066
#define GL_TEXTURE_RESIDENT                     0x8067
#define GL_TEXTURE_BINDING_1D                   0x8068
#define GL_TEXTURE_BINDING_2D                   0x8069
#define GL_TEXTURE_INTERNAL_FORMAT              0x1003
#define GL_ALPHA4                               0x803b
#define GL_ALPHA8                               0x803c
#define GL_ALPHA12                              0x803d
#define GL_ALPHA16                              0x803e
#define GL_LUMINANCE4                           0x803f
#define GL_LUMINANCE8                           0x8040
#define GL_LUMINANCE12                          0x8041
#define GL_LUMINANCE16                          0x8042
#define GL_LUMINANCE4_ALPHA4                    0x8043
#define GL_LUMINANCE6_ALPHA2                    0x8044
#define GL_LUMINANCE8_ALPHA8                    0x8045
#define GL_LUMINANCE12_ALPHA4                   0x8046
#define GL_LUMINANCE12_ALPHA12                  0x8047
#define GL_LUMINANCE16_ALPHA16                  0x8048
#define GL_INTENSITY                            0x8049
#define GL_INTENSITY4                           0x804a
#define GL_INTENSITY8                           0x804b
#define GL_INTENSITY12                          0x804c
#define GL_INTENSITY16                          0x804d
#define GL_R3_G3_B2                             0x2a10
#define GL_RGB4                                 0x804f
#define GL_RGB5                                 0x8050
#define GL_RGB8                                 0x8051
#define GL_RGB10                                0x8052
#define GL_RGB12                                0x8053
#define GL_RGB16                                0x8054
#define GL_RGBA2                                0x8055
#define GL_RGBA4                                0x8056
#define GL_RGB5_A1                              0x8057
#define GL_RGBA8                                0x8058
#define GL_RGB10_A2                             0x8059
#define GL_RGBA12                               0x805a
#define GL_RGBA16                               0x805b
#define GL_CLIENT_PIXEL_STORE_BIT               0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT              0x00000002
#define GL_ALL_CLIENT_ATTRIB_BITS               0xffffffff
#define GL_CLIENT_ALL_ATTRIB_BITS               0xffffffff


// Miscellaneous
//GLAPI void GLAPIENTRY glClearIndex( GLfloat c );
GLAPI void GLAPIENTRY glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
GLAPI void GLAPIENTRY glClear( GLbitfield mask );
//GLAPI void GLAPIENTRY glIndexMask( GLuint mask );
//GLAPI void GLAPIENTRY glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
GLAPI void GLAPIENTRY glAlphaFunc( GLenum func, GLclampf ref );
GLAPI void GLAPIENTRY glBlendFunc( GLenum sfactor, GLenum dfactor );
//GLAPI void GLAPIENTRY glLogicOp( GLenum opcode );
GLAPI void GLAPIENTRY glCullFace( GLenum mode );
GLAPI void GLAPIENTRY glFrontFace( GLenum mode );
//GLAPI void GLAPIENTRY glPointSize( GLfloat size );
//GLAPI void GLAPIENTRY glLineWidth( GLfloat width );
//GLAPI void GLAPIENTRY glLineStipple( GLint factor, GLushort pattern );
//GLAPI void GLAPIENTRY glPolygonMode( GLenum face, GLenum mode );
//GLAPI void GLAPIENTRY glPolygonOffset( GLfloat factor, GLfloat units );
//GLAPI void GLAPIENTRY glPolygonStipple( const GLubyte *mask );
//GLAPI void GLAPIENTRY glGetPolygonStipple( GLubyte *mask );
//GLAPI void GLAPIENTRY glEdgeFlag( GLboolean flag );
//GLAPI void GLAPIENTRY glEdgeFlagv( const GLboolean *flag );
//GLAPI void GLAPIENTRY glScissor( GLint x, GLint y, GLsizei width, GLsizei height);
//#define glClipPlanef glClipPlane
//GLAPI void GLAPIENTRY glClipPlane( GLenum plane, const GLdouble *equation );
//GLAPI void GLAPIENTRY glGetClipPlane( GLenum plane, GLdouble *equation );
GLAPI void GLAPIENTRY glDrawBuffer( GLenum mode );
GLAPI void GLAPIENTRY glReadBuffer( GLenum mode );
GLAPI void GLAPIENTRY glEnable( GLenum cap );
GLAPI void GLAPIENTRY glDisable( GLenum cap );
//GLAPI GLboolean GLAPIENTRY glIsEnabled( GLenum cap );
GLAPI void GLAPIENTRY glEnableClientState( GLenum cap ); // OpenGL 1.1
GLAPI void GLAPIENTRY glDisableClientState( GLenum cap ); // OpenGL 1.1
//GLAPI void GLAPIENTRY glGetBooleanv( GLenum pname, GLboolean *params );
//GLAPI void GLAPIENTRY glGetDoublev( GLenum pname, GLdouble *params );
GLAPI void GLAPIENTRY glGetFloatv( GLenum pname, GLfloat *params );
//GLAPI void GLAPIENTRY glGetIntegerv( GLenum pname, GLint *params );
//GLAPI void GLAPIENTRY glPushAttrib( GLbitfield mask );
//GLAPI void GLAPIENTRY glPopAttrib( void );
//GLAPI void GLAPIENTRY glPushClientAttrib( GLbitfield mask ); // OpenGL 1.1
//GLAPI void GLAPIENTRY glPopClientAttrib( void ); // OpenGL 1.1
//GLAPI GLint GLAPIENTRY glRenderMode( GLenum mode );
GLAPI GLenum GLAPIENTRY glGetError( void );
//GLAPI const GLubyte * GLAPIENTRY glGetString( GLenum name );
GLAPI void GLAPIENTRY glFinish( void );
GLAPI void GLAPIENTRY glFlush( void );
GLAPI void GLAPIENTRY glHint( GLenum target, GLenum mode );

// Depth Buffer
GLAPI void GLAPIENTRY glClearDepthf( GLclampf depth );
GLAPI void GLAPIENTRY glClearDepth ( GLclampd depth );
GLAPI void GLAPIENTRY glDepthFunc( GLenum func );
GLAPI void GLAPIENTRY glDepthMask( GLboolean flag );
GLAPI void GLAPIENTRY glDepthRangef( GLclampf near_val, GLclampf far_val );
GLAPI void GLAPIENTRY glDepthRange ( GLclampd near_val, GLclampd far_val );

// Accumulation Buffer
//GLAPI void GLAPIENTRY glClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
//GLAPI void GLAPIENTRY glAccum( GLenum op, GLfloat value );

// Transformation
GLAPI void GLAPIENTRY glMatrixMode( GLenum mode );
GLAPI void GLAPIENTRY glOrthof( GLfloat  left, GLfloat  right, GLfloat  bottom, GLfloat  top, GLfloat  near_val, GLfloat  far_val );
GLAPI void GLAPIENTRY glOrtho ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val );
GLAPI void GLAPIENTRY glFrustumf( GLfloat  left, GLfloat  right, GLfloat  bottom, GLfloat  top, GLfloat  near_val, GLfloat  far_val );
GLAPI void GLAPIENTRY glFrustum ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val );
GLAPI void GLAPIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height );
GLAPI void GLAPIENTRY glPushMatrix( void );
GLAPI void GLAPIENTRY glPopMatrix( void );
GLAPI void GLAPIENTRY glLoadIdentity( void );
//GLAPI void GLAPIENTRY glLoadMatrixd( const GLdouble *m );
GLAPI void GLAPIENTRY glLoadMatrixf( const GLfloat *m );
//GLAPI void GLAPIENTRY glMultMatrixd( const GLdouble *m );
GLAPI void GLAPIENTRY glMultMatrixf( const GLfloat *m );
//GLAPI void GLAPIENTRY glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
//GLAPI void GLAPIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z );
//GLAPI void GLAPIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z );

// Display Lists
//GLAPI GLboolean GLAPIENTRY glIsList( GLuint list );
//GLAPI void GLAPIENTRY glDeleteLists( GLuint list, GLsizei range );
//GLAPI GLuint GLAPIENTRY glGenLists( GLsizei range );
//GLAPI void GLAPIENTRY glNewList( GLuint list, GLenum mode );
//GLAPI void GLAPIENTRY glEndList( void );
//GLAPI void GLAPIENTRY glCallList( GLuint list );
//GLAPI void GLAPIENTRY glCallLists( GLsizei n, GLenum type, const GLvoid *lists );
//GLAPI void GLAPIENTRY glListBase( GLuint base );

// Drawing Functions
GLAPI void GLAPIENTRY glBegin( GLenum mode );
GLAPI void GLAPIENTRY glEnd( void );
GLAPI void GLAPIENTRY glVertex2d ( GLdouble x, GLdouble y );
GLAPI void GLAPIENTRY glVertex2f ( GLfloat  x, GLfloat  y );
GLAPI void GLAPIENTRY glVertex2i ( GLint    x, GLint    y );
GLAPI void GLAPIENTRY glVertex2s ( GLshort  x, GLshort  y );
GLAPI void GLAPIENTRY glVertex3d ( GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glVertex3f ( GLfloat  x, GLfloat  y, GLfloat  z );
GLAPI void GLAPIENTRY glVertex3i ( GLint    x, GLint    y, GLint    z );
GLAPI void GLAPIENTRY glVertex3s ( GLshort  x, GLshort  y, GLshort  z );
GLAPI void GLAPIENTRY glVertex4d ( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
GLAPI void GLAPIENTRY glVertex4f ( GLfloat  x, GLfloat  y, GLfloat  z, GLfloat  w );
GLAPI void GLAPIENTRY glVertex4i ( GLint    x, GLint    y, GLint    z, GLint    w );
GLAPI void GLAPIENTRY glVertex4s ( GLshort  x, GLshort  y, GLshort  z, GLshort  w );
GLAPI void GLAPIENTRY glVertex2dv( const GLdouble *v );
GLAPI void GLAPIENTRY glVertex2fv( const GLfloat  *v );
GLAPI void GLAPIENTRY glVertex2iv( const GLint    *v );
GLAPI void GLAPIENTRY glVertex2sv( const GLshort  *v );
GLAPI void GLAPIENTRY glVertex3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glVertex3fv( const GLfloat  *v );
GLAPI void GLAPIENTRY glVertex3iv( const GLint    *v );
GLAPI void GLAPIENTRY glVertex3sv( const GLshort  *v );
GLAPI void GLAPIENTRY glVertex4dv( const GLdouble *v );
GLAPI void GLAPIENTRY glVertex4fv( const GLfloat  *v );
GLAPI void GLAPIENTRY glVertex4iv( const GLint    *v );
GLAPI void GLAPIENTRY glVertex4sv( const GLshort  *v );
GLAPI void GLAPIENTRY glNormal3b ( GLbyte   nx, GLbyte   ny, GLbyte   nz );
GLAPI void GLAPIENTRY glNormal3d ( GLdouble nx, GLdouble ny, GLdouble nz );
GLAPI void GLAPIENTRY glNormal3f ( GLfloat  nx, GLfloat  ny, GLfloat  nz );
GLAPI void GLAPIENTRY glNormal3i ( GLint    nx, GLint    ny, GLint    nz );
GLAPI void GLAPIENTRY glNormal3s ( GLshort  nx, GLshort  ny, GLshort  nz );
GLAPI void GLAPIENTRY glNormal3bv( const GLbyte   *v );
GLAPI void GLAPIENTRY glNormal3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glNormal3fv( const GLfloat  *v );
GLAPI void GLAPIENTRY glNormal3iv( const GLint    *v );
GLAPI void GLAPIENTRY glNormal3sv( const GLshort  *v );
//GLAPI void GLAPIENTRY glIndexd( GLdouble c );
//GLAPI void GLAPIENTRY glIndexf( GLfloat c );
//GLAPI void GLAPIENTRY glIndexi( GLint c );
//GLAPI void GLAPIENTRY glIndexs( GLshort c );
//GLAPI void GLAPIENTRY glIndexub( GLubyte c ); // OpenGL 1.1
//GLAPI void GLAPIENTRY glIndexdv( const GLdouble *c );
//GLAPI void GLAPIENTRY glIndexfv( const GLfloat *c );
//GLAPI void GLAPIENTRY glIndexiv( const GLint *c );
//GLAPI void GLAPIENTRY glIndexsv( const GLshort *c );
//GLAPI void GLAPIENTRY glIndexubv( const GLubyte *c ); // OpenGL 1.1
GLAPI void GLAPIENTRY glColor3b  ( GLbyte   red, GLbyte   green, GLbyte   blue );
GLAPI void GLAPIENTRY glColor3d  ( GLdouble red, GLdouble green, GLdouble blue );
GLAPI void GLAPIENTRY glColor3f  ( GLfloat  red, GLfloat  green, GLfloat  blue );
GLAPI void GLAPIENTRY glColor3i  ( GLint    red, GLint    green, GLint    blue );
GLAPI void GLAPIENTRY glColor3s  ( GLshort  red, GLshort  green, GLshort  blue );
GLAPI void GLAPIENTRY glColor3ub ( GLubyte  red, GLubyte  green, GLubyte  blue );
GLAPI void GLAPIENTRY glColor3ui ( GLuint   red, GLuint   green, GLuint   blue );
GLAPI void GLAPIENTRY glColor3us ( GLushort red, GLushort green, GLushort blue );
GLAPI void GLAPIENTRY glColor4b  ( GLbyte   red, GLbyte   green, GLbyte   blue, GLbyte   alpha );
GLAPI void GLAPIENTRY glColor4d  ( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
GLAPI void GLAPIENTRY glColor4f  ( GLfloat  red, GLfloat  green, GLfloat  blue, GLfloat  alpha );
GLAPI void GLAPIENTRY glColor4i  ( GLint    red, GLint    green, GLint    blue, GLint    alpha );
GLAPI void GLAPIENTRY glColor4s  ( GLshort  red, GLshort  green, GLshort  blue, GLshort  alpha );
GLAPI void GLAPIENTRY glColor4ub ( GLubyte  red, GLubyte  green, GLubyte  blue, GLubyte  alpha );
GLAPI void GLAPIENTRY glColor4ui ( GLuint   red, GLuint   green, GLuint   blue, GLuint   alpha );
GLAPI void GLAPIENTRY glColor4us ( GLushort red, GLushort green, GLushort blue, GLushort alpha );
GLAPI void GLAPIENTRY glColor3bv ( const GLbyte   *v );
GLAPI void GLAPIENTRY glColor3dv ( const GLdouble *v );
GLAPI void GLAPIENTRY glColor3fv ( const GLfloat  *v );
GLAPI void GLAPIENTRY glColor3iv ( const GLint    *v );
GLAPI void GLAPIENTRY glColor3sv ( const GLshort  *v );
GLAPI void GLAPIENTRY glColor3ubv( const GLubyte  *v );
GLAPI void GLAPIENTRY glColor3uiv( const GLuint   *v );
GLAPI void GLAPIENTRY glColor3usv( const GLushort *v );
GLAPI void GLAPIENTRY glColor4bv ( const GLbyte   *v );
GLAPI void GLAPIENTRY glColor4dv ( const GLdouble *v );
GLAPI void GLAPIENTRY glColor4fv ( const GLfloat  *v );
GLAPI void GLAPIENTRY glColor4iv ( const GLint    *v );
GLAPI void GLAPIENTRY glColor4sv ( const GLshort  *v );
GLAPI void GLAPIENTRY glColor4ubv( const GLubyte  *v );
GLAPI void GLAPIENTRY glColor4uiv( const GLuint   *v );
GLAPI void GLAPIENTRY glColor4usv( const GLushort *v );
GLAPI void GLAPIENTRY glTexCoord1d ( GLdouble s );
GLAPI void GLAPIENTRY glTexCoord1f ( GLfloat  s );
GLAPI void GLAPIENTRY glTexCoord1i ( GLint    s );
GLAPI void GLAPIENTRY glTexCoord1s ( GLshort  s );
GLAPI void GLAPIENTRY glTexCoord2d ( GLdouble s, GLdouble t );
GLAPI void GLAPIENTRY glTexCoord2f ( GLfloat  s, GLfloat  t );
GLAPI void GLAPIENTRY glTexCoord2i ( GLint    s, GLint    t );
GLAPI void GLAPIENTRY glTexCoord2s ( GLshort  s, GLshort  t );
GLAPI void GLAPIENTRY glTexCoord3d ( GLdouble s, GLdouble t, GLdouble r );
GLAPI void GLAPIENTRY glTexCoord3f ( GLfloat  s, GLfloat  t, GLfloat  r );
GLAPI void GLAPIENTRY glTexCoord3i ( GLint    s, GLint    t, GLint    r );
GLAPI void GLAPIENTRY glTexCoord3s ( GLshort  s, GLshort  t, GLshort  r );
GLAPI void GLAPIENTRY glTexCoord4d ( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
GLAPI void GLAPIENTRY glTexCoord4f ( GLfloat  s, GLfloat  t, GLfloat  r, GLfloat  q );
GLAPI void GLAPIENTRY glTexCoord4i ( GLint    s, GLint    t, GLint    r, GLint    q );
GLAPI void GLAPIENTRY glTexCoord4s ( GLshort  s, GLshort  t, GLshort  r, GLshort  q );
GLAPI void GLAPIENTRY glTexCoord1dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord1fv( const GLfloat  *v );
GLAPI void GLAPIENTRY glTexCoord1iv( const GLint    *v );
GLAPI void GLAPIENTRY glTexCoord1sv( const GLshort  *v );
GLAPI void GLAPIENTRY glTexCoord2dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord2fv( const GLfloat  *v );
GLAPI void GLAPIENTRY glTexCoord2iv( const GLint    *v );
GLAPI void GLAPIENTRY glTexCoord2sv( const GLshort  *v );
GLAPI void GLAPIENTRY glTexCoord3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord3fv( const GLfloat  *v );
GLAPI void GLAPIENTRY glTexCoord3iv( const GLint    *v );
GLAPI void GLAPIENTRY glTexCoord3sv( const GLshort  *v );
GLAPI void GLAPIENTRY glTexCoord4dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord4fv( const GLfloat  *v );
GLAPI void GLAPIENTRY glTexCoord4iv( const GLint    *v );
GLAPI void GLAPIENTRY glTexCoord4sv( const GLshort  *v );
//GLAPI void GLAPIENTRY glRasterPos2d( GLdouble x, GLdouble y );
//GLAPI void GLAPIENTRY glRasterPos2f( GLfloat x, GLfloat y );
//GLAPI void GLAPIENTRY glRasterPos2i( GLint x, GLint y );
//GLAPI void GLAPIENTRY glRasterPos2s( GLshort x, GLshort y );
//GLAPI void GLAPIENTRY glRasterPos3d( GLdouble x, GLdouble y, GLdouble z );
//GLAPI void GLAPIENTRY glRasterPos3f( GLfloat x, GLfloat y, GLfloat z );
//GLAPI void GLAPIENTRY glRasterPos3i( GLint x, GLint y, GLint z );
//GLAPI void GLAPIENTRY glRasterPos3s( GLshort x, GLshort y, GLshort z );
//GLAPI void GLAPIENTRY glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
//GLAPI void GLAPIENTRY glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
//GLAPI void GLAPIENTRY glRasterPos4i( GLint x, GLint y, GLint z, GLint w );
//GLAPI void GLAPIENTRY glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w );
//GLAPI void GLAPIENTRY glRasterPos2dv( const GLdouble *v );
//GLAPI void GLAPIENTRY glRasterPos2fv( const GLfloat *v );
//GLAPI void GLAPIENTRY glRasterPos2iv( const GLint *v );
//GLAPI void GLAPIENTRY glRasterPos2sv( const GLshort *v );
//GLAPI void GLAPIENTRY glRasterPos3dv( const GLdouble *v );
//GLAPI void GLAPIENTRY glRasterPos3fv( const GLfloat *v );
//GLAPI void GLAPIENTRY glRasterPos3iv( const GLint *v );
//GLAPI void GLAPIENTRY glRasterPos3sv( const GLshort *v );
//GLAPI void GLAPIENTRY glRasterPos4dv( const GLdouble *v );
//GLAPI void GLAPIENTRY glRasterPos4fv( const GLfloat *v );
//GLAPI void GLAPIENTRY glRasterPos4iv( const GLint *v );
//GLAPI void GLAPIENTRY glRasterPos4sv( const GLshort *v );
//GLAPI void GLAPIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
//GLAPI void GLAPIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
//GLAPI void GLAPIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 );
//GLAPI void GLAPIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );
//GLAPI void GLAPIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 );
//GLAPI void GLAPIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 );
//GLAPI void GLAPIENTRY glRectiv( const GLint *v1, const GLint *v2 );
//GLAPI void GLAPIENTRY glRectsv( const GLshort *v1, const GLshort *v2 );

// Vertex Arrays (OpenGL 1.1)
GLAPI void GLAPIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );
GLAPI void GLAPIENTRY glNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr );
GLAPI void GLAPIENTRY glColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );
//GLAPI void GLAPIENTRY glIndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr );
GLAPI void GLAPIENTRY glTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );
//GLAPI void GLAPIENTRY glEdgeFlagPointer( GLsizei stride, const GLvoid *ptr );
//GLAPI void GLAPIENTRY glGetPointerv( GLenum pname, GLvoid **params );
//GLAPI void GLAPIENTRY glArrayElement( GLint i );
GLAPI void GLAPIENTRY glDrawArrays( GLenum mode, GLint first, GLsizei count );
//GLAPI void GLAPIENTRY glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices );
//GLAPI void GLAPIENTRY glInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer );

// Lighting
GLAPI void GLAPIENTRY glShadeModel( GLenum mode );
GLAPI void GLAPIENTRY glLightf( GLenum light, GLenum pname, GLfloat param );
//GLAPI void GLAPIENTRY glLighti( GLenum light, GLenum pname, GLint param );
GLAPI void GLAPIENTRY glLightfv( GLenum light, GLenum pname, const GLfloat *params );
//GLAPI void GLAPIENTRY glLightiv( GLenum light, GLenum pname, const GLint *params );
//GLAPI void GLAPIENTRY glGetLightfv( GLenum light, GLenum pname, GLfloat *params );
//GLAPI void GLAPIENTRY glGetLightiv( GLenum light, GLenum pname, GLint *params );
//GLAPI void GLAPIENTRY glLightModelf( GLenum pname, GLfloat param );
//GLAPI void GLAPIENTRY glLightModeli( GLenum pname, GLint param );
//GLAPI void GLAPIENTRY glLightModelfv( GLenum pname, const GLfloat *params );
//GLAPI void GLAPIENTRY glLightModeliv( GLenum pname, const GLint *params );
GLAPI void GLAPIENTRY glMaterialf( GLenum face, GLenum pname, GLfloat param );
//GLAPI void GLAPIENTRY glMateriali( GLenum face, GLenum pname, GLint param );
GLAPI void GLAPIENTRY glMaterialfv( GLenum face, GLenum pname, const GLfloat *params );
//GLAPI void GLAPIENTRY glMaterialiv( GLenum face, GLenum pname, const GLint *params );
//GLAPI void GLAPIENTRY glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params );
//GLAPI void GLAPIENTRY glGetMaterialiv( GLenum face, GLenum pname, GLint *params );
//GLAPI void GLAPIENTRY glColorMaterial( GLenum face, GLenum mode );

// Raster functions
//GLAPI void GLAPIENTRY glPixelZoom( GLfloat xfactor, GLfloat yfactor );
//GLAPI void GLAPIENTRY glPixelStoref( GLenum pname, GLfloat param );
//GLAPI void GLAPIENTRY glPixelStorei( GLenum pname, GLint param );
//GLAPI void GLAPIENTRY glPixelTransferf( GLenum pname, GLfloat param );
//GLAPI void GLAPIENTRY glPixelTransferi( GLenum pname, GLint param );
//GLAPI void GLAPIENTRY glPixelMapfv( GLenum map, GLsizei mapsize, const GLfloat *values );
//GLAPI void GLAPIENTRY glPixelMapuiv( GLenum map, GLsizei mapsize, const GLuint *values );
//GLAPI void GLAPIENTRY glPixelMapusv( GLenum map, GLsizei mapsize, const GLushort *values );
//GLAPI void GLAPIENTRY glGetPixelMapfv( GLenum map, GLfloat *values );
//GLAPI void GLAPIENTRY glGetPixelMapuiv( GLenum map, GLuint *values );
//GLAPI void GLAPIENTRY glGetPixelMapusv( GLenum map, GLushort *values );
//GLAPI void GLAPIENTRY glBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap );
GLAPI void GLAPIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );
//GLAPI void GLAPIENTRY glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
//GLAPI void GLAPIENTRY glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );

// Stenciling
//GLAPI void GLAPIENTRY glStencilFunc( GLenum func, GLint ref, GLuint mask );
//GLAPI void GLAPIENTRY glStencilMask( GLuint mask );
//GLAPI void GLAPIENTRY glStencilOp( GLenum fail, GLenum zfail, GLenum zpass );
//GLAPI void GLAPIENTRY glClearStencil( GLint s );

// Texture mapping
//GLAPI void GLAPIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param );
//GLAPI void GLAPIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param );
//GLAPI void GLAPIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param );
//GLAPI void GLAPIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble *params );
//GLAPI void GLAPIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params );
//GLAPI void GLAPIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint *params );
//GLAPI void GLAPIENTRY glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params );
//GLAPI void GLAPIENTRY glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params );
//GLAPI void GLAPIENTRY glGetTexGeniv( GLenum coord, GLenum pname, GLint *params );
GLAPI void GLAPIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param );
//GLAPI void GLAPIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param );
GLAPI void GLAPIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params );
//GLAPI void GLAPIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint *params );
//GLAPI void GLAPIENTRY glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params );
//GLAPI void GLAPIENTRY glGetTexEnviv( GLenum target, GLenum pname, GLint *params );
GLAPI void GLAPIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param );
//GLAPI void GLAPIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param );
//GLAPI void GLAPIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params );
//GLAPI void GLAPIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint *params );
//GLAPI void GLAPIENTRY glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params);
//GLAPI void GLAPIENTRY glGetTexParameteriv( GLenum target, GLenum pname, GLint *params );
//GLAPI void GLAPIENTRY glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params );
//GLAPI void GLAPIENTRY glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params );
//GLAPI void GLAPIENTRY glTexImage1D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
GLAPI void GLAPIENTRY glTexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
//GLAPI void GLAPIENTRY glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels );

// OpenGL 1.1 functions
GLAPI void GLAPIENTRY glGenTextures( GLsizei n, GLuint *textures );
GLAPI void GLAPIENTRY glDeleteTextures( GLsizei n, const GLuint *textures);
GLAPI void GLAPIENTRY glBindTexture( GLenum target, GLuint texture );
//GLAPI void GLAPIENTRY glPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities );
//GLAPI GLboolean GLAPIENTRY glAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences );
//GLAPI GLboolean GLAPIENTRY glIsTexture( GLuint texture );
//GLAPI void GLAPIENTRY glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels );
GLAPI void GLAPIENTRY glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
//GLAPI void GLAPIENTRY glCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border );
//GLAPI void GLAPIENTRY glCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
//GLAPI void GLAPIENTRY glCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
//GLAPI void GLAPIENTRY glCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );

// Evaluators
//GLAPI void GLAPIENTRY glMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points );
//GLAPI void GLAPIENTRY glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points );
//GLAPI void GLAPIENTRY glMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points );
//GLAPI void GLAPIENTRY glMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points );
//GLAPI void GLAPIENTRY glGetMapdv( GLenum target, GLenum query, GLdouble *v );
//GLAPI void GLAPIENTRY glGetMapfv( GLenum target, GLenum query, GLfloat *v );
//GLAPI void GLAPIENTRY glGetMapiv( GLenum target, GLenum query, GLint *v );
//GLAPI void GLAPIENTRY glEvalCoord1d( GLdouble u );
//GLAPI void GLAPIENTRY glEvalCoord1f( GLfloat u );
//GLAPI void GLAPIENTRY glEvalCoord1dv( const GLdouble *u );
//GLAPI void GLAPIENTRY glEvalCoord1fv( const GLfloat *u );
//GLAPI void GLAPIENTRY glEvalCoord2d( GLdouble u, GLdouble v );
//GLAPI void GLAPIENTRY glEvalCoord2f( GLfloat u, GLfloat v );
//GLAPI void GLAPIENTRY glEvalCoord2dv( const GLdouble *u );
//GLAPI void GLAPIENTRY glEvalCoord2fv( const GLfloat *u );
//GLAPI void GLAPIENTRY glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 );
//GLAPI void GLAPIENTRY glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 );
//GLAPI void GLAPIENTRY glMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
//GLAPI void GLAPIENTRY glMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );
//GLAPI void GLAPIENTRY glEvalPoint1( GLint i );
//GLAPI void GLAPIENTRY glEvalPoint2( GLint i, GLint j );
//GLAPI void GLAPIENTRY glEvalMesh1( GLenum mode, GLint i1, GLint i2 );
//GLAPI void GLAPIENTRY glEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );

// Fog
GLAPI void GLAPIENTRY glFogf( GLenum pname, GLfloat param );
//GLAPI void GLAPIENTRY glFogi( GLenum pname, GLint param );
GLAPI void GLAPIENTRY glFogfv( GLenum pname, const GLfloat *params );
//GLAPI void GLAPIENTRY glFogiv( GLenum pname, const GLint *params );

// Selection and Feedback
//GLAPI void GLAPIENTRY glFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer );
//GLAPI void GLAPIENTRY glPassThrough( GLfloat token );
//GLAPI void GLAPIENTRY glSelectBuffer( GLsizei size, GLuint *buffer );
//GLAPI void GLAPIENTRY glInitNames( void );
//GLAPI void GLAPIENTRY glLoadName( GLuint name );
//GLAPI void GLAPIENTRY glPushName( GLuint name );
//GLAPI void GLAPIENTRY glPopName( void );


#ifdef __cplusplus
}
#endif


#endif
