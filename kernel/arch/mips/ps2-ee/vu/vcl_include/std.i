;write-protected

.macro   _CrossProduct  vectoroutput,vector1,vector2
   opmula.xyz     ACC,           \vector1, \vector2
   opmsub.xyz     \vectoroutput, \vector2, \vector1
.endm

.macro _GenericArrayCopy dst, src, num
count .assigna 0
  .awhile \&count lt \num
    move  \dst[\&count],\src[\&count]
count .assigna \&count+1
  .aendw
.endm

.macro _GenericArrayLoad array, mem, offset, num
count .assigna 0
  .awhile \&count lt \num
    lq  \array[\&count], \offset+\&count(\mem)
count .assigna \&count+1
  .aendw
.endm

.macro _GenericArraySave array, mem, offset, num
count .assigna 0
  .awhile \&count lt \num
    sq  \array[\&count], \offset+\&count(\mem)
count .assigna \&count+1
  .aendw
.endm

.macro _imul retVal, var, constval
  .aif \constval EQ 1
    iaddiu \retVal, \var, 0
  .aendi
  .aif \constval EQ 2
    iadd \retVal, \var, \var
  .aendi
  .aif \constval EQ 3
    iadd \retVal, \var, \var
    iadd \retVal, \retVal, \var
  .aendi
  .aif \constval EQ 4
    iadd \retVal, \var, \var
    iadd \retVal, \retVal, \retVal
  .aendi
  .aif \constval EQ 5
    iadd \retVal, \var, \var
    iadd \retVal, \retVal, \retVal
    iadd \retVal, \retVal, \var
  .aendi
  .aif \constval EQ 6
    iadd \var2, \var, \var
    iadd \retVal, \var2, \var2
    iadd \retVal, \retVal, \var2
  .aendi
  .aif \constval EQ 7
    iadd \var2, \var, \var
    iadd \retVal, \var2, \var2
    iadd \retVal, \retVal, \var2
    iadd \retVal, \retVal, \var
  .aendi
  .aif \constval EQ 8
    iadd \retVal, \var, \var
    iadd \retVal, \retVal, \retVal
    iadd \retVal, \retVal, \retVal
  .aendi
.endm

;const bool support macros

.macro _const_bool_and ret, a, b
  .aif \a NE 0
\ret .equ \b
  .aelse
\ret .equ 0
  .aendi
.endm

.macro _const_bool_or ret, a, b
  .aif \a NE 0
\ret .equ 1
  .aelse
\ret .equ \b
  .aendi
.endm

.macro _const_bool_rel op, ret, a, b
  .aif \a \op \b
\ret .equ 1
  .aelse
\ret .equ 0
  .aendi
.endm

;float rel op support macros
;assumes a subtract of left and right has taken place, with register mask X

; Branch if neither sign nor zero bit is set
.macro _float_BGTZ trueLabel
  iaddiu vi01, vi00, (128 + 8)
  fmand  vi01, vi01
  ibeq   vi01, vi00, \trueLabel
.endm

; Branch if sign bit is not set 
.macro _float_BGEZ trueLabel
  iaddiu vi01, vi00, (128)
  fmand  vi01, vi01
  ibeq   vi01, vi00, \trueLabel
.endm

; Branch if sign bit is set 
.macro _float_BLTZ trueLabel
  iaddiu vi01, vi00, (128)
  fmand  vi01, vi01
  ibne   vi01, vi00, \trueLabel
.endm

; Branch if either sign or zero bit is set
.macro _float_BLEZ trueLabel
  iaddiu vi01, vi00, (128 + 8)
  fmand  vi01, vi01
  ibne   vi01, vi00, \trueLabel
.endm

; Branch if zero bit is 0
.macro _float_BEQZ trueLabel
  iaddiu vi01, vi00, (8)
  fmand  vi01, vi01
  ibne   vi01, vi00, \trueLabel
.endm

; Branch if zero bit is 0
.macro _float_BNEZ trueLabel
  iaddiu vi01, vi00, (8)
  fmand  vi01, vi01
  ibeq   vi01, vi00, \trueLabel
.endm

; Intrinsics


.macro _xgkick address
  xgkick  \address
.endm

.macro _xitop address
  xitop   \address
.endm

.macro _xtop address
  xtop    \address
.endm

.macro _ftoi0 retVal, param
  ftoi0    \retVal, \param
.endm

.macro _ftoi4 retVal, param
  ftoi4    \retVal, \param
.endm

.macro _ftoi12 retVal, param
  ftoi12    \retVal, \param
.endm

.macro _ftoi15 retVal, param
  ftoi15    \retVal, \param
.endm

.macro _itof0 retVal, param
  itof0    \retVal, \param
.endm

.macro _itof4 retVal, param
  itof4    \retVal, \param
.endm

.macro _itof12 retVal, param
  itof12    \retVal, \param
.endm

.macro _itof15 retVal, param
  itof15    \retVal, \param
.endm

.macro _abs retVal, val
  abs   \retVal, \val
.endm

.macro _max retVal, a, b
  max     \retVal, \a, \b
.endm

.macro _mini retVal, a, b
  mini     \retVal, \a, \b
.endm

.macro _mr32 retVal, param
  mr32    \retVal, \param
.endm

; random nr gen, intrinsics

.macro _rget retVal
  rget  \retVal, R
.endm

.macro _rinit seed
  rinit  R, \seed[x]
.endm

.macro _rnext retVal
  rnext \retVal, R
.endm

.macro _rxor val
  rxor  R, \val[x]
.endm

;perform clipping judgement with param.XYZ against param.W
;void clipW(float4 param);
.macro _clipW param
  clipw.xyz \param, \param
.endm

; flag operation intrisics

;int fsand(const int mask);
.macro _fsand retVal, param
  fsand    \retVal, \param
.endm

;bool fseq(const int mask);
.macro _fseq retVal, param
  fseq    \retVal, \param
.endm

;int fsor(const int mask);
.macro _fsor retVal, param
  fsor    \retVal, \param
.endm

;void fsset(const int val);
;.macro _fsset param
;  fsset    \param
;.endm

;int fmand(int mask);
.macro _fmand retVal, param
  fmand    \retVal, \param
.endm

;bool fmeq(int mask);
.macro _fmeq retVal, param
  fmeq    \retVal, \param
.endm

;int fmor(int mask);
.macro _fmor retVal, param
  fmor    \retVal, \param
.endm

;int fcand(const int mask);
.macro _fcand retVal, param
  fcand    vi01, \param
  iaddiu   \retVal, vi01, 0
.endm

;bool fceq(const int mask);
.macro _fceq retVal, param
  fceq    vi01, \param
  iaddiu   \retVal, vi01, 0
.endm

;int fcor(const int mask);
.macro _fcor retVal, param
  fcor    vi01, \param
  iaddiu   \retVal, vi01, 0
.endm

;void fcset(const int val);
.macro _fcset val
  fcset    \val
.endm

;int fcget();
.macro _fcget ret_val
  fcget    \ret_val
.endm


; elementary function unit intrinsics

;float4 eatan(float val);
.macro _eatan retVal, val
  eatan      p, \val[x]
  mfp.xyzw  \retVal, p
.endm

;float4 eatanXY(float4 val);
.macro _eatanXY retVal, val
  eatanXY      p, \val
  mfp.xyzw  \retVal, p
.endm

;float4 eatanXZ(float4 val);
.macro _eatanXZ retVal, val
  eatanXZ      p, \val
  mfp.xyzw  \retVal, p
.endm

;float4 eexp(float val);
.macro _eexp retVal, val
  eexp      p, \val[x]
  mfp.xyzw  \retVal, p
.endm

;float4 eleng(float4 val);
.macro _eleng retVal, val
  eleng      p, \val
  mfp.xyzw  \retVal, p
.endm

;float4 ercpr(float val);
.macro _ercpr retVal, val
  ercpr      p, \val[x]
  mfp.xyzw  \retVal, p
.endm

;float4 erleng(float4 val);
.macro _erleng retVal, val
  erleng      p, \val
  mfp.xyzw  \retVal, p
.endm

;float4 ersadd(float4 val);
.macro _ersadd retVal, val
  ersadd      p, \val
  mfp.xyzw  \retVal, p
.endm

;float4 ersqrt(float val);
.macro _ersqrt retVal, val
  ersqrt     p, \val[x]
  mfp.xyzw  \retVal, p
.endm

;float4 esadd(float4 val);
.macro _esadd retVal, val
  esadd      p, \val
  mfp.xyzw  \retVal, p
.endm

;float4 esin(float val);
.macro _esin retVal, val
  esin      p, \val[x]
  mfp.xyzw  \retVal, p
.endm

;float4 esqrt(float val);
.macro _esqrt retVal, val
  esqrt     p, \val[x]
  mfp.xyzw  \retVal, p
.endm

;float4 esum(float4 val);
.macro _esum retVal, val
  esum      p, \val
  mfp.xyzw  \retVal, p
.endm


;float rsqrt(float numerator, float denominator);
.macro _rsqrt retVal, n, d
  rsqrt     q, \n[x], \d[x]
  add.x     \retVal, vf00, q
.endm

;float sqrt(float val);
.macro _sqrt retVal, val
  sqrt      q, \val[x]
  add.x     \retVal, vf00, q
.endm


; macros implemented in vcl_sml.i, declarations here is to decorate macro names properly


;void MatrixIdentity(float4 matrix[4]);
.macro   _MatrixIdentity matrix
  MatrixIdentity \matrix
.endm

;void MatrixSwap(float4 a[4], float4 b[4]);
.macro _MatrixSwap a,b
  MatrixSwap \a, \b
.endm

;void MatrixTranspose(float4 result[4], float4 src[4]);
.macro   _MatrixTranspose matresult,matrixsrc
  MatrixTranspose \matresult, \matrixsrc
.endm

;void MatrixMultiply(float4 result[4], float4 left[4], float4 right[4]);
.macro   _MatrixMultiply   matresult,matleft,matright
  MatrixMultiply   \matresult, \matleft, \matright
.endm

;void LocalizeLightMatrix(float4 locallightmatrix[4], float4 matrix[4], float lightmatrix[4]);
.macro   _LocalizeLightMatrix locallightmatrix,matrix,lightmatrix
  LocalizeLightMatrix \locallightmatrix, \matrix, \lightmatrix
.endm

;float4 MatrixMultiplyVertex(float4 matrix[4], float4 vert);
.macro   _MatrixMultiplyVertex vertexresult, matrix, vertex
  MatrixMultiplyVertex \vertexresult, \matrix, \vertex
.endm

;float4 MatrixMultiplyVector(float4 matrix[4], float4 vert);
.macro   _MatrixMultiplyVector vertexresult, matrix, vertex
  MatrixMultiplyVector \vertexresult, \matrix, \vertex
.endm

;float4 VertexPersCorr(float4 vertex);
.macro _VertexPersCorr vertexoutput,vertex
  VertexPersCorr \vertexoutput, \vertex
.endm

;void VertexPersCorrST(float4 outVert, float4 outST, float4 vert, float4 st);
.macro   _VertexPersCorrST vertexoutput,stoutput,vertex,st
  VertexPersCorrST \vertexoutput, \stoutput, \vertex, \st
.endm

;float4 VertexFpToGsXYZ2(float4 vert);
.macro   _VertexFpToGsXYZ2  outputxyz,vertex
  VertexFpToGsXYZ2  \outputxyz, \vertex
.endm

;float4 VertexFpToGsXYZ2Adc(float4 vert);
.macro  _VertexFpToGsXYZ2Adc  outputxyz,vertex
  VertexFpToGsXYZ2Adc  \outputxyz, \vertex
.endm

;float VectorDotProduct(float4 v1, float4 v2);
.macro   _VectorDotProduct  dotproduct,vector1,vector2
  VectorDotProduct  \dotproduct, \vector1, \vector2
.endm

;float4 VectorCrossProduct(float4 v1, float4 v2);
.macro   _VectorCrossProduct  vectoroutput,vector1,vector2
  VectorCrossProduct  \vectoroutput, \vector1, \vector2
.endm

;float4 VectorNormalize(float4 v);
.macro   _VectorNormalize   vecoutput,vector
  VectorNormalize   \vecoutput, \vector
.endm

;float4 VectorNormalizeXYZ(float4 v);
.macro   _VectorNormalizeXYZ   vecoutput,vector
  VectorNormalizeXYZ   \vecoutput, \vector
.endm

;float4 VertexLightAmb(float4 vertRGBA, float4 ambRGBA);
.macro   _VertexLightAmb rgbaout,vertexrgba,ambientrgba
  VertexLightAmb \rgbaout,\vertexrgba,\ambientrgba
.endm

;float4 VertexLightDir3(float4 vertRGBA, float4 vertNormal, float4 lightColors[3], float4 lightNormals[4]);
.macro   _VertexLightDir3   rgbaout,vertexrgba,vertexnormal,lightcolors,lightnormals
  VertexLightDir3   \rgbaout, \vertexrgba, \vertexnormal, \lightcolors, \lightnormals
.endm

;float4 VertexLightDir3Amb(float4 vertRGBA, float4 vertNormal, float4 lightColors[4], float4 lightNormals[4]);
.macro   _VertexLightDir3Amb   rgbaout,vertexrgba,vertexnormal,lightcolors,lightnormals
  VertexLightDir3Amb   \rgbaout, \vertexrgba, \vertexnormal, \lightcolors, \lightnormals
.endm

.macro   _FogSetup fogparams,nearfogz,farfogz
  FogSetup \fogparams, \nearfogz, \farfogz
.endm

;float4 VertexFogLinear(float4 xyzw, float4 fogParams);
.macro   _VertexFogLinear   xyzfoutput,xyzw,fogparams
  VertexFogLinear   \xyzfoutput, \xyzw, \fogparams
.endm

;void VertexFogRemove(float4 xyzf, float4 fogParams);
.macro   _VertexFogRemove   xyzf,fogparams
  VertexFogRemove   \xyzf, \fogparams
.endm

;void AngleSinCos(float4 angles, float4 sincos);
.macro   _AngleSinCos    angle,sincos
  AngleSinCos    \angle, \sincos
.endm


.macro   _QuaternionToMatrix   matresult,quaternion
  QuaternionToMatrix   \matresult, \quaternion
.endm

.macro   _QuaternionMultiply   quatresult,quaternion1,quaternion2
  QuaternionMultiply   \quatresult, \quaternion1, \quaternion2
.endm

.macro  _TriangleWinding   result, vert1, vert2, vert3, eyepos
  TriangleWinding   \result,  \vert1, \vert2, \vert3, \eyepos
.endm
