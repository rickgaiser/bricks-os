#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H


typedef long fxpoint_t;


//---------------------------------------------------------------------------
#define fpfromi(c,a) ((a)<<(c))
#define fpfromf(c,a) ((long)((a)*(1<<(c))))
#define fptoi(c,a)   ((a)>>(c))
#define fptof(c,a)   ((float)(a)/(1<<(c)))
#define fpmul(c,a,b) (((a)*(b))>>(c))
#define fpdiv(c,a,b) (((a)<<(c))/(b))

//---------------------------------------------------------------------------
// Precision: 4bits, 1/16
// Step size: 0.0625
// add/substract: use "long" for values     ranging from -128M to +128M (sign + 27bits)
// multiply:      use "long" for results    ranging from -512K to +512K (sign + 19bits)
// divide:        use "long" for top values ranging from -512K to +512K (sign + 19bits)
#define fpfromi_dot4(a) fpfromi(4,a)
#define fpfromf_dot4(a) fpfromf(4,a)
#define fptoi_dot4(a)   fptoi(4,a)
#define fptof_dot4(a)   fptof(4,a)
#define fpmul_dot4(a,b) fpmul(4,a,b)
#define fpdiv_dot4(a,b) fpdiv(4,a,b)

//---------------------------------------------------------------------------
// Precision: 8bits, 1/256
// Step size: 0.00390625
// add/substract: use "long" for values     ranging from   -8M to   +8M (sign + 23bits)
// multiply:      use "long" for results    ranging from  -32K to  +32K (sign + 15bits)
// divide:        use "long" for top values ranging from  -32K to  +32K (sign + 15bits)
#define fpfromi_dot8(a) fpfromi(8,a)
#define fpfromf_dot8(a) fpfromf(8,a)
#define fptoi_dot8(a)   fptoi(8,a)
#define fptof_dot8(a)   fptof(8,a)
#define fpmul_dot8(a,b) fpmul(8,a,b)
#define fpdiv_dot8(a,b) fpdiv(8,a,b)

//---------------------------------------------------------------------------
// Precision: 12bits, 1/4096
// Step size: 0.000244140625
// add/substract: use "long" for values     ranging from -512K to +512K (sign + 19bits)
// multiply:      use "long" for results    ranging from -128  to +128  (sign +  7bits)
// divide:        use "long" for top values ranging from -128  to +128  (sign +  7bits)
#define fpfromi_dot12(a) fpfromi(12,a)
#define fpfromf_dot12(a) fpfromf(12,a)
#define fptoi_dot12(a)   fptoi(12,a)
#define fptof_dot12(a)   fptof(12,a)
#define fpmul_dot12(a,b) fpmul(12,a,b)
#define fpdiv_dot12(a,b) fpdiv(12,a,b)

//---------------------------------------------------------------------------
// Precision: 16bits, 1/65536
// Step size: 0.0000152587890625
// add/substract: use "long" for values     ranging from  -32K to  +32K (sign + 15bits)
// multiply:      use "long" for results    ranging from   -8  to   +8  (sign +  3bits)
// divide:        use "long" for top values ranging from   -8  to   +8  (sign +  3bits)
#define fpfromi_dot16(a) fpfromi(16,a)
#define fpfromf_dot16(a) fpfromf(16,a)
#define fptoi_dot16(a)   fptoi(16,a)
#define fptof_dot16(a)   fptof(16,a)
#define fpmul_dot16(a,b) fpmul(16,a,b)
#define fpdiv_dot16(a,b) fpdiv(16,a,b)


#endif
