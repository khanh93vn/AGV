// File: fixed_point_math.cpp
// Hỗ trợ tính toán kiểu fixed-point.
// Tham khảo:
// https://www.embedded.com/fixed-point-math-in-c/
// http://www.olliw.eu/2014/fast-functions/
#include "agv.h"

// Bảng góc từ 0 đến pi/2, kiểu Q3_28
static const Q3_28 angle_table[] = {
           0L,   6692975L,  13385950L,  20078925L,  26771900L,  33464875L,
    40157850L,  46850825L,  53543800L,  60236775L,  66929751L,  73622726L,
    80315701L,  87008676L,  93701651L, 100394626L, 107087601L, 113780576L,
   120473551L, 127166526L, 133859501L, 140552476L, 147245451L, 153938426L,
   160631401L, 167324376L, 174017351L, 180710326L, 187403301L, 194096277L,
   200789252L, 207482227L, 214175202L, 220868177L, 227561152L, 234254127L,
   240947102L, 247640077L, 254333052L, 261026027L, 267719002L, 274411977L,
   281104952L, 287797927L, 294490902L, 301183877L, 307876852L, 314569827L,
   321262802L, 327955778L, 334648753L, 341341728L, 348034703L, 354727678L,
   361420653L, 368113628L, 374806603L, 381499578L, 388192553L, 394885528L,
   401578503L, 408271478L, 414964453L, 421657428L,
};

// Bảng cosin của các góc tương ứng với bảng trên
static const Q3_28 cosine_table[] = {
   268435456L, 268352021L, 268101770L, 267684856L, 267101540L, 266352184L,
   265437254L, 264357318L, 263113049L, 261705219L, 260134703L, 258402479L,
   256509622L, 254457309L, 252246817L, 249879519L, 247356887L, 244680489L,
   241851989L, 238873145L, 235745809L, 232471924L, 229053527L, 225492742L,
   221791782L, 217952949L, 213978628L, 209871291L, 205633489L, 201267859L,
   196777113L, 192164044L, 187431519L, 182582479L, 177619940L, 172546985L,
   167366769L, 162082512L, 156697499L, 151215077L, 145638654L, 139971697L,
   134217728L, 128380325L, 122463116L, 116469779L, 110404041L, 104269672L,
    98070484L,  91810333L,  85493109L,  79122740L,  72703185L,  66238435L,
    59732508L,  53189450L,  46613328L,  40008229L,  33378259L,  26727540L,
    20060207L,  13380403L,   6692282L,         0L,
};

/**
 * Tìm góc khi biết cos của góc đó.
 * Dùng phương pháp tra bảng để tiết
 * kiệm thời gian.
 */
Q3_28 Q3_28acos_lookup(Q3_28 c)
{
  uint8_t i, j, k, s = 0;
  Q3_28 angle;
  uint64_t interpolated;

  // Đảm bảo c nằm trong đoạn [0, 1]
  if (c < 0) { c = -c; s = 1; }

  // Bắt đầu tìm
  i = 0; j = 63;
  while (i+1 < j)
  {
    k = (i+j)>>1;
    if (c > cosine_table[k]) j = k;
    else i = k;
  }

  // Nội suy giá trị góc
  angle = angle_table[i];
  interpolated = (int64_t)(c - cosine_table[i])*(angle_table[j] - angle_table[i]);
  interpolated /= (cosine_table[j] - cosine_table[i]);
  angle += interpolated;
  if (s) angle = Q3_28PI - angle;
  return angle;
}

/** 
 * Hàm tìm arctan xấp xỉ bằng đa thức
 * bậc 7 
 */
Q3_28 Q3_28atan2(Q3_28 y, Q3_28 x)
{
  Q3_28 a;
  if(x > 0) {
    if(y >= 0) { // đảm bảo |y+x| >= |y-x| => |a| <= 1
      a = Q3_28DIV((y>>1) - (x>>1), y + x);
      return Q3_28atan(a);
    } else { // đảm bảo |y-x| >= |y+x| => |a| <= 1
      a = Q3_28DIV((y>>1) + (x>>1), y - x);
      return -Q3_28atan(a);
    }
  }
  if(x < 0) {
    if(y >= 0){
      a = Q3_28DIV((y>>1) + (x>>1), y - x);
      return Q3_28HALF_PI - Q3_28atan(a);
    }else{
      a = Q3_28DIV( (y>>1) - (x>>1) , y + x );
      return Q3_28atan(a) - Q3_28HALF_PI;
    }
  }
  if(y > 0) return Q3_28HALF_PI;
  if(y < 0) return -Q3_28HALF_PI;
  return 0;
}

/**
 * Tính atan trong góc phần tư thứ nhất.
 */
Q3_28 Q3_28atan(Q3_28 a)
{
  static const Q3_28 a0 = 210828714,
                     a1 = 536447127,
                     a3 = -689594581,
                     a5 = 1253983892,
                     a7 = -1328963016;
  Q3_28 at, k, a_sqr;
  a_sqr = Q3_28MUL(a, a);
  at = a0; k = a;
  at += Q3_28MUL(a1, k);
  k = Q3_28MUL(k, a_sqr);
  at += Q3_28MUL(a3, k);
  k = Q3_28MUL(k, a_sqr);
  at += Q3_28MUL(a5, k);
  k = Q3_28MUL(k, a_sqr);
  at += Q3_28MUL(a7, k);
  return at;
}
