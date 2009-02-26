/// This file is (C) 2004..2008 Dmytry Lavrov.

// This file is released under zLib license, which is provided below.
/*
 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

//convention: constantsinlowercaseandlikedeuch but_sometimes_with_aid_for_readability_when_really_needed, ClassesAndProcedures
//namespaces in lower case

#ifndef _MATH3_H_
#define _MATH3_H_
#include <math.h>
#include <iostream>
#ifdef __GNUC__
#define __int32 long
#define __int64 long long
#endif
//--
namespace math3
{
template<class T>
T square(T a)
{
    return a*a;
};
// Constants i like
static const double pi=3.1415926535897932384626433832795;

// speeds, masses, etc. in MeterKilogrammSecond system.
static const double lightspeed=3.0E8;
static const double year=3600.0 * 24.0 * 365.24;
static const double lightyear=(lightspeed*year);
//static const double lightyear=1.0;
// light year = 9.46702080000000E+0015 m
// Size of universe = approx. 2E+10 light years
// fits well into INT64
template <class T>
struct Vec2
{
public:
    T x,y;
    Vec2(T a,T b)
    {
        x=a;
        y=b;
    }
    Vec2()
    {}
    template<class A>
    inline Vec2(const Vec2<A> &a)
    {
        x=a.x;
        y=a.y;
    }

    inline Vec2<T> operator -() const
    {
        Vec2<T> result;
        result.x=-x;
        result.y=-y;
        return result;
    }

    template<class A>
    inline Vec2<T>& operator +=(const Vec2<A> &b)
    {
        x+=b.x;
        y+=b.y;
        return *this;
    }
    template<class A>
    inline Vec2<T>& operator -=(const Vec2<A> &b)
    {

        x-=b.x;
        y-=b.y;
        return *this;
    }
    template<class A>
    inline Vec2<T>& operator *=(const A &b)
    {
        x*=b;
        y*=b;
        return *this;
    }
    template<class A>
    inline Vec2<T>& operator /=(const A &b)
    {
        T a=1/b;
        x*=a;
        y*=a;
        return *this;
    }

    template<class A>
    inline bool operator ==(const A &b) const
    {
        return x==b.x && y==b.y;
    }

    template<class A>
        inline bool operator !=(const A &b) const
    {
      return x!=b.x || y!=b.y;
    }
};
template<class T>
inline Vec2<T> operator *(const Vec2<T> &a , const T &b)
{
    Vec2<T> result;
    result.x=a.x*b;
    result.y=a.y*b;
    return result;
}

template<class T>
inline Vec2<T> operator *(const T &b, const Vec2<T> &a)
{
    Vec2<T> result;
    result.x=a.x*b;
    result.y=a.y*b;
    return result;
}
template<class T>
inline Vec2<T> operator +(const Vec2<T> &a, const Vec2<T> &b)
{
    Vec2<T> result;
    result.x=a.x+b.x;
    result.y=a.y+b.y;
    return result;
}
template<class T>
inline Vec2<T> operator -(const Vec2<T> &a, const Vec2<T> &b)
{
    Vec2<T> result;
    result.x=a.x-b.x;
    result.y=a.y-b.y;
    return result;
}

template <class T>
inline Vec2<T> PerpRight(const Vec2<T> &a)
{
    return Vec2<T>(a.y,-a.x);
};
template <class T>
inline Vec2<T> PerpLeft(const Vec2<T> &a)
{
    return Vec2<T>(-a.y,a.x);
};

template <class T>
inline T DotProd(const Vec2<T> &a,const Vec2<T> &b)
{
    return a.x*b.x + a.y*b.y;
};

template <class T>
inline T Length2(const Vec2<T> &a)
{
    return a.x*a.x + a.y*a.y ;
};

template <class T>
inline T Length(const Vec2<T> &a)
{
    return sqrt( a.x*a.x + a.y*a.y);
};

template <class T>
inline void Normalize (Vec2<T> &a)
{
    T il=1.0/sqrt( a.x*a.x + a.y*a.y);
    a*=il;
};

template <class T>
inline Vec2<T> Normalized (const Vec2<T> &a)
{
    T il=1.0/sqrt( a.x*a.x + a.y*a.y);
    return (a*il);
};


template <class T>
struct Vec3
{
public:
    T x,y,z;
    Vec3(T a,T b,T c)
    {
        x=a;
        y=b;
        z=c;
    }
    Vec3()
    {}

    template<class A>
    inline Vec3(const Vec3<A> &a)
    {
        x=a.x;
        y=a.y;
        z=a.z;
    }

    inline Vec3<T> operator -()const
    {
        Vec3<T> result;
        result.x=-x;
        result.y=-y;
        result.z=-z;
        return result;
    }

    template<class A>
    inline Vec3<T> operator /(const A &b)const
    {
        Vec3<T> result;
        T a=1/b;
        result.x=x*a;
        result.y=y*a;
        result.z=z*a;
        return result;
    }
    template<class A>
    inline Vec3<T>& operator +=(const Vec3<A> &b)
    {
        x+=b.x;
        y+=b.y;
        z+=b.z;
        return *this;
    }
    template<class A>
    inline Vec3<T>& operator -=(const Vec3<A> &b)
    {

        x-=b.x;
        y-=b.y;
        z-=b.z;
        return *this;
    }
    template<class A>
    inline Vec3<T>& operator *=(const A &b)
    {
        x*=b;
        y*=b;
        z*=b;
        return *this;
    }
    template<class A>
    inline Vec3<T>& operator /=(const A &b)
    {
        T a=1/b;
        x*=a;
        y*=a;
        z*=a;
        return *this;
    }

    template<class A>
    inline bool operator ==(const A &b) const
    {
        return x==b.x && y==b.y && z==b.z;
    }

    template<class A>
        inline bool operator !=(const A &b) const
    {
      return x!=b.x || y!=b.y || z!=b.z;
    }

    inline T &operator[](int i){
      return *(&x+i);
    }
    inline const T &operator[](int i) const {
      return *(&x+i);
    }
};


/*template<class A> inline Vec3<T>& operator=(const Vec3<A> &b){
x=b.x;
y=b.y;
z=b.z;
return *this;
}
*/

template<class T>
inline Vec3<T> operator *(const Vec3<T> &a , const T &b)
{
    Vec3<T> result;
    result.x=a.x*b;
    result.y=a.y*b;
    result.z=a.z*b;
    return result;
}

template<class T>
inline Vec3<T> operator *(const T &b, const Vec3<T> &a)
{
    Vec3<T> result;
    result.x=a.x*b;
    result.y=a.y*b;
    result.z=a.z*b;
    return result;
}
template<class T>
inline Vec3<T> operator +(const Vec3<T> &a, const Vec3<T> &b)
{
    Vec3<T> result;
    result.x=a.x+b.x;
    result.y=a.y+b.y;
    result.z=a.z+b.z;
    return result;
}
template<class T>
inline Vec3<T> operator -(const Vec3<T> &a, const Vec3<T> &b)
{
    Vec3<T> result;
    result.x=a.x-b.x;
    result.y=a.y-b.y;
    result.z=a.z-b.z;
    return result;
}


template <class T>
Vec3<T> CrossProd(const Vec3<T> &a,const Vec3<T> &b)
{
    Vec3<T> result;
    result.x=a.y*b.z - a.z*b.y;
    result.y=a.z*b.x - a.x*b.z;
    result.z=a.x*b.y - a.y*b.x;
    return result;
};

template <class T>
inline T DotProd(const Vec3<T> &a,const Vec3<T> &b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
};

template <class T>
inline T Length2(const Vec3<T> &a)
{
    return a.x*a.x + a.y*a.y + a.z*a.z ;
};

template <class T>
inline T Length(const Vec3<T> &a)
{
    return sqrt( a.x*a.x + a.y*a.y + a.z*a.z );
};

template <class T>
inline void Normalize (Vec3<T> &a)
{
    T il=1/sqrt( a.x*a.x + a.y*a.y + a.z*a.z );
    a*=il;
};

template <class T>
inline Vec3<T> Normalized (const Vec3<T> &a)
{
    T il=1/sqrt( a.x*a.x + a.y*a.y + a.z*a.z );
    return (a*il);
};

/** ----------------------------------------------------------- **/
template <class T>
struct Vec4{
	T x,y,z,w;
	  Vec4(){}
    Vec4(T x_, T y_, T z_, T w_):x(x_),y(y_),z(z_),w(w_){}
	  inline Vec4<T> operator -()const
    {
        return Vec4<T>(-x,-y,-z,-w);
    }
    template<class A>
    inline Vec4(const Vec4<A> &a)
    {
        x=a.x;
        y=a.y;
        z=a.z;
        w=a.w;
    }

    template<class A>
    inline Vec4<T>& operator +=(const Vec4<A> &b)
    {
        x+=b.x;
        y+=b.y;
        z+=b.z;
        w+=b.w;
        return *this;
    }
    template<class A>
    inline Vec4<T>& operator -=(const Vec4<A> &b)
    {

        x-=b.x;
        y-=b.y;
        z-=b.z;
        w-=b.w;
        return *this;
    }
    template<class A>
    inline Vec4<T>& operator *=(const A &b)
    {
        x*=b;
        y*=b;
        z*=b;
        w*=b;
        return *this;
    }
    template<class A>
    inline Vec4<T>& operator /=(const A &b)
    {
        T a=1/b;
        x*=a;
        y*=a;
        z*=a;
        w*=a;
        return *this;
    }
};

template<class T>
inline Vec4<T> operator *(const Vec4<T> &a , const T &b)
{
    Vec4<T> result;
    result.x=a.x*b;
    result.y=a.y*b;
    result.z=a.z*b;
    result.w=a.w*b;
    return result;
}

template<class T>
inline Vec4<T> operator *(const T &b, const Vec4<T> &a)
{
    Vec4<T> result;
    result.x=a.x*b;
    result.y=a.y*b;
    result.z=a.z*b;
    result.w=a.w*b;
    return result;
}
template<class T>
inline Vec4<T> operator +(const Vec4<T> &a, const Vec4<T> &b)
{
    Vec4<T> result;
    result.x=a.x+b.x;
    result.y=a.y+b.y;
    result.z=a.z+b.z;
    result.w=a.w+b.w;
    return result;
}
template<class T>
inline Vec4<T> operator -(const Vec4<T> &a, const Vec4<T> &b)
{
    Vec4<T> result;
    result.x=a.x-b.x;
    result.y=a.y-b.y;
    result.z=a.z-b.z;
    result.w=a.w-b.w;
    return result;
}
template<class T>
inline T DotProd(const Vec4<T> &a,const Vec4<T> &b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
};

template <class T>
inline T Length(const Vec4<T> &a)
{
    return sqrt( a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
};

template <class T>
inline void Normalize (Vec4<T> &a)
{
    T il=1/sqrt( a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
    a*=il;
};

template <class T>
inline Vec4<T> Normalized (const Vec4<T> &a)
{
    T il=1/sqrt( a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
    return (a*il);
};

/** ----------------------------------------------------- **/





template <class T>
struct Quaternion
{
    T a,b,c,d;

    inline Quaternion(const Vec3<T> &axis, T angle)
    {
        Vec3<T> axisn=Normalized(axis);
        a=cos(angle/2);
        T s=sin(angle/2);
        b=axisn.x*s;
        c=axisn.y*s;
        d=axisn.z*s;
    }
    template <class A>
    inline Quaternion(const Quaternion<A> &q)
    {
        a=q.a;
        b=q.b;
        c=q.c;
        d=q.d;
    }
    inline Quaternion(T _a,T _b,T _c,T _d)
    {
        a=_a;
        b=_b;
        c=_c;
        d=_d;
    }
    inline Quaternion()
    {}
    inline Quaternion<T> operator *(const Quaternion<T> &q2) const
    {
        Quaternion<T> result;

        result.a = a*q2.a - b*q2.b - c*q2.c - d*q2.d;
        result.b = a*q2.b + b*q2.a + c*q2.d - d*q2.c;
        result.c = a*q2.c - b*q2.d + c*q2.a + d*q2.b;
        result.d = a*q2.d + b*q2.c - c*q2.b + d*q2.a;
        return result;
    };
    inline Quaternion<T> operator *(T s) const
    {
        Quaternion<T> result;

        result.a = a*s;
        result.b = b*s;
        result.c = c*s;
        result.d = d*s;
        return result;
    };
    inline Quaternion<T>& operator *=(T s)
    {

        a *= s;
        b *= s;
        c *= s;
        d *= s;
        return *this;
    };
};

template <class T>
inline T Length2(const Quaternion<T> &q)
{
    return	q.a*q.a + q.b*q.b + q.c*q.c + q.d*q.d ;
};

template <class T>
inline T Length(const Quaternion<T> &q)
{
    return sqrt( q.a*q.a + q.b*q.b + q.c*q.c + q.d*q.d );
};
template <class T>
inline void Normalize(Quaternion<T> &q)
{
    T il=1/Length(q);
    q*=il;
};

template <class T>
inline Quaternion<T> Normalized(const Quaternion<T> &q)
{
    T il=1/Length(q);
    return q*il;
};

//--- does inverse transform
template <class T>
inline Quaternion<T> Inverse(const Quaternion<T> &q)
{
    return Quaternion<T>(-q.a , q.b , q.c , q.d);
};
/************************************************************************************************************************************************/

template <class T>
struct Matrix2x2
{
    T a[2][2];
    inline Matrix2x2()
    {}
    ;
    template<class A>
    inline Matrix2x2(const A &m)
    {
        a[0][0]=m.a[0][0];
        a[0][1]=m.a[0][1];

        a[1][0]=m.a[1][0];
        a[1][1]=m.a[1][1];


    };

    inline Matrix2x2(T m00, T m01, T m10, T m11)
    {
        a[0][0]=m00;
        a[0][1]=m01;

        a[1][0]=m10;
        a[1][1]=m11;
    };


    inline Matrix2x2<T> operator*(const Matrix2x2<T> &m2)const
    {
        Matrix2x2<T> result;

        result.a[0][0] = a[0][0]*m2.a[0][0] + a[0][1]*m2.a[1][0]  ;
        result.a[0][1] = a[0][0]*m2.a[0][1] + a[0][1]*m2.a[1][1]  ;
        result.a[1][0] = a[1][0]*m2.a[0][0] + a[1][1]*m2.a[1][0]  ;
        result.a[1][1] = a[1][0]*m2.a[0][1] + a[1][1]*m2.a[1][1]  ;

        return result ;
    };
    inline Vec2<T> operator*(const Vec2<T> &v) const
    {
        Vec2<T> result;
        result.x = a[0][0]*v.x+a[0][1]*v.y;
        result.y = a[1][0]*v.x+a[1][1]*v.y;
        return result;
    }
    inline T& c(int i,int j)
    {
        return (a[i&1][j&1]);
    };
    inline const T& c(int i,int j) const
    {
        return (a[i&1][j&1]);
    };
    inline T& cf(int i,int j)
    {
        return (a[i][j]);
    };
    inline const T& cf(int i,int j) const
    {
        return (a[i][j]);
    };

    inline T& at(int i,int j)
    {
        return (a[i][j]);
    };

    inline const T& at(int i,int j) const
    {
        return (a[i][j]);
    };

    Vec2<T> GetColumn(int i){
      return Vec2<T>(a[0][i], a[1][i]);
    }

    void SetColumn(int i, const Vec2<T> &v){
      a[0][i]=v.x;
      a[1][i]=v.y;
    }
};

template <class T>
Matrix2x2<T> operator*(const Matrix2x2<T> &m, T v){
  return Matrix2x2<T>(
  m.at(0,0)*v, m.at(0,1)*v,
  m.at(1,0)*v, m.at(1,1)*v
  );
};

template <class T>
Matrix2x2<T> operator*(T v, const Matrix2x2<T> &m){
  return Matrix2x2<T>(
  m.at(0,0)*v, m.at(0,1)*v,
  m.at(1,0)*v, m.at(1,1)*v
  );
};

template <class T>
T Det(const Matrix2x2<T> &m)
{
    return m.a[0][0]*m.a[1][1]-m.a[0][1]*m.a[1][0];
}
template <class T>
Matrix2x2<T> Transpose(const Matrix2x2<T> &m)
{
    return Matrix2x2<T>(
               m.a[0][0],m.a[1][0],
               m.a[0][1],m.a[1][1]
           );
}

template <class T>
Matrix2x2<T> Inverse(const Matrix2x2<T> &m)
{
    T d=Det(m);
    if(d!=0)
    {
        d=1/d;
    }
    else
    {
        std::cout<<"Matrix inverse failed"<<std::endl;
    };
    Matrix2x2<T> result;
    result.a[0][0]=m.a[1][1]*d;
    result.a[0][1]=-m.a[0][1]*d;
    result.a[1][0]=-m.a[1][0]*d;
    result.a[1][1]=m.a[0][0]*d;
    return result;
};
template <class T>
Matrix2x2<T> AngleToMatrix(T alpha)
{
  return Matrix2x2<T>(
  cos(alpha),-sin(alpha),
  sin(alpha), cos(alpha)
  );
};

/***********************************************************************/

template <class T>
struct Matrix3x3
{
    T a[3][3];
    inline Matrix3x3()
    {}
    ;
    template<class A>
    inline Matrix3x3(const A &m)
    {
        a[0][0]=m.a[0][0];
        a[0][1]=m.a[0][1];
        a[0][2]=m.a[0][2];

        a[1][0]=m.a[1][0];
        a[1][1]=m.a[1][1];
        a[1][2]=m.a[1][2];

        a[2][0]=m.a[2][0];
        a[2][1]=m.a[2][1];
        a[2][2]=m.a[2][2];

    };

    inline Matrix3x3(T m00,T m01,T m02,T m10,T m11,T m12,T m20,T m21,T m22)
    {
        a[0][0]=m00;
        a[0][1]=m01;
        a[0][2]=m02;
        a[1][0]=m10;
        a[1][1]=m11;
        a[1][2]=m12;
        a[2][0]=m20;
        a[2][1]=m21;
        a[2][2]=m22;
    };


    inline Matrix3x3<T> operator*(const Matrix3x3<T> &m2)const
    {
        Matrix3x3<T> result;

        result.a[0][0] = a[0][0]*m2.a[0][0] + a[0][1]*m2.a[1][0] + a[0][2]*m2.a[2][0] ;
        result.a[0][1] = a[0][0]*m2.a[0][1] + a[0][1]*m2.a[1][1] + a[0][2]*m2.a[2][1] ;
        result.a[0][2] = a[0][0]*m2.a[0][2] + a[0][1]*m2.a[1][2] + a[0][2]*m2.a[2][2] ;
        result.a[1][0] = a[1][0]*m2.a[0][0] + a[1][1]*m2.a[1][0] + a[1][2]*m2.a[2][0] ;
        result.a[1][1] = a[1][0]*m2.a[0][1] + a[1][1]*m2.a[1][1] + a[1][2]*m2.a[2][1] ;
        result.a[1][2] = a[1][0]*m2.a[0][2] + a[1][1]*m2.a[1][2] + a[1][2]*m2.a[2][2] ;
        result.a[2][0] = a[2][0]*m2.a[0][0] + a[2][1]*m2.a[1][0] + a[2][2]*m2.a[2][0] ;
        result.a[2][1] = a[2][0]*m2.a[0][1] + a[2][1]*m2.a[1][1] + a[2][2]*m2.a[2][1] ;
        result.a[2][2] = a[2][0]*m2.a[0][2] + a[2][1]*m2.a[1][2] + a[2][2]*m2.a[2][2] ;

        return result ;
    };
    inline Vec3<T> operator*(const Vec3<T> &v) const
    {
        Vec3<T> result;
        result.x = a[0][0]*v.x+a[0][1]*v.y+a[0][2]*v.z;
        result.y = a[1][0]*v.x+a[1][1]*v.y+a[1][2]*v.z;
        result.z = a[2][0]*v.x+a[2][1]*v.y+a[2][2]*v.z;
        return result;
    }
    inline T& c(int i,int j)
    {
        return (a[((i+3000000) % 3)][((j+3000000) % 3)]);
    };
    inline const T& c(int i,int j) const
    {
        return (a[((i+3000000) % 3)][((j+3000000) % 3)]);
    };
    inline T& cf(int i,int j)
    {
        return (a[i][j]);
    };
    inline const T& cf(int i,int j) const
    {
        return (a[i][j]);
    };

    inline T& at(int i,int j)
    {
        return (a[i][j]);
    };

    inline const T& at(int i,int j) const
    {
        return (a[i][j]);
    };

    Vec3<T> GetColumn(int i){
      return Vec3<T>(a[0][i], a[1][i], a[2][i]);
    }

    void SetColumn(int i, const Vec3<T> &v){
      a[0][i]=v.x;
      a[1][i]=v.y;
      a[2][i]=v.z;
    }
};

template <class T>
Matrix3x3<T> operator*(const Matrix3x3<T> &m, T v){
  return Matrix3x3<T>(
  m.at(0,0)*v, m.at(0,1)*v, m.at(0,2)*v,
  m.at(1,0)*v, m.at(1,1)*v, m.at(1,2)*v,
  m.at(2,0)*v, m.at(2,1)*v, m.at(2,2)*v
  );
};

template <class T>
Matrix3x3<T> operator*(T v, const Matrix3x3<T> &m){
  return Matrix3x3<T>(
  m.at(0,0)*v, m.at(0,1)*v, m.at(0,2)*v,
  m.at(1,0)*v, m.at(1,1)*v, m.at(1,2)*v,
  m.at(2,0)*v, m.at(2,1)*v, m.at(2,2)*v
  );
};

template <class T>
T Det(const Matrix3x3<T> &m)
{
    int i,j;
    T b,c,r=0;
    for(j=0;j<3;++j)
    {
        b=m.c(0,j);
        c=b;
        for(i=1;i<3;++i)
        {
            b*=m.cf(i,(j+i)%3);
            c*=m.cf(i,(j-i+3)%3);
        };
        r+=b-c;
    };
    return r;
}
template <class T>
Matrix3x3<T> Transpose(const Matrix3x3<T> &m)
{
    return Matrix3x3<T>(
               m.a[0][0],m.a[1][0],m.a[2][0],
               m.a[0][1],m.a[1][1],m.a[2][1],
               m.a[0][2],m.a[1][2],m.a[2][2]
           );
}

template <class T>
inline T Adj(const Matrix3x3<T> &m,int i,int j)
{
    return m.c(i+1,j+1)*m.c(i+2,j+2)-m.c(i+1,j-1)*m.c(i+2,j-2);
}

template <class T>
Matrix3x3<T> Inverse(const Matrix3x3<T> &m)
{
    int i,j;
    T d=Det(m);
    if(d!=0)
    {
        d=1/d;
    }
    else
    {
        std::cout<<"error"<<std::endl;
    };
    Matrix3x3<T> result;
    for(i=0;i<3;++i)
    {
        for(j=0;j<3;j++)
        {
            result.cf(i,j)=Adj(m,j,i)*d;
        }
    };
    return result;
};
template <class T>
Matrix3x3<T> QuaternionToMatrix(const Quaternion<T> &q)
{

    T sq[4]={q.a*q.a , q.b*q.b , q.c*q.c , q.d*q.d};
    /*    Matrix3x3<T> result;
    result.c(0,0)=	sq[0]+sq[1]-sq[2]-sq[3];	result.c(0,1)=	2*(q.b*q.c-q.a*q.d);		result.c(0,2)=2*(q.a*q.c+q.b*q.d);
    result.c(1,0)=2*(q.a*q.d+q.b*q.c);	    result.c(1,1)=sq[0]+sq[2]-sq[1]-sq[3];  result.c(1,2)=2*(q.c*q.d-q.a*q.b);
    result.c(2,0)=2*(q.b*q.d-q.a*q.c);		    result.c(2,1)=2*(q.a*q.b+q.c*q.d);		    result.c(2,2)=sq[0]+sq[3]-sq[1]-sq[2];
    */
    Matrix3x3<T> result(

        sq[0]+sq[1]-sq[2]-sq[3],	2*(q.b*q.c-q.a*q.d),		2*(q.a*q.c+q.b*q.d),
        2*(q.a*q.d+q.b*q.c),		sq[0]+sq[2]-sq[1]-sq[3],	2*(q.c*q.d-q.a*q.b),
        2*(q.b*q.d-q.a*q.c),		2*(q.a*q.b+q.c*q.d),		sq[0]+sq[3]-sq[1]-sq[2]

    );

    return result;
};

/***********************************************************************/
template <class T>
struct Matrix4x4
{
    T a[4][4];
    inline Matrix4x4()
    {}
    ;
    inline Matrix4x4(Matrix3x3<T> m)
    {
        a[0][0]=m.a[0][0];
        a[0][1]=m.a[0][1];
        a[0][2]=m.a[0][2];
        a[0][3]=0;

        a[1][0]=m.a[1][0];
        a[1][1]=m.a[1][1];
        a[1][2]=m.a[1][2];
        a[1][3]=0;

        a[2][0]=m.a[2][0];
        a[2][1]=m.a[2][1];
        a[2][2]=m.a[2][2];
        a[2][3]=0;

        a[3][0]=0;
        a[3][1]=0;
        a[3][2]=0;
        a[3][3]=1;
    };
    inline Matrix4x4(
        T a00, T a01, T a02, T a03,
        T a10, T a11, T a12, T a13,
        T a20, T a21, T a22, T a23,
        T a30, T a31, T a32, T a33
    )
    {
        a[0][0]=a00;
        a[0][1]=a01;
        a[0][2]=a02;
        a[0][3]=a03;

        a[1][0]=a10;
        a[1][1]=a11;
        a[1][2]=a12;
        a[1][3]=a13;

        a[2][0]=a20;
        a[2][1]=a21;
        a[2][2]=a22;
        a[2][3]=a23;

        a[3][0]=a30;
        a[3][1]=a31;
        a[3][2]=a32;
        a[3][3]=a33;
    };
    template<class A>
    inline Matrix4x4<T> operator*(const Matrix4x4<A> &m2)const
    {
        Matrix4x4<T> result;
        for(int i=0;i<4;++i)
        {
            for(int j=0;j<4;++j)
            {
                float tmp=0;
                for(int n=0;n<4;n++)
                {
                    tmp+=a[i][n]*m2.a[n][j];
                };
                result.a[i][j]=tmp;
            };
        };
        return result;
    };

    inline Matrix4x4<T> operator*(double v)const
    {
      Matrix4x4<T> result;
      for(int i=0;i<4;++i)
        {
            for(int j=0;j<4;++j)
            {
                result.a[i][j]=a[i][j]*v;
            };
        };
        return result;
    };

    template<class A>
    inline Vec4<A> operator*(const Vec4<A> &v)const
    {
       return Vec4<A>(
        v.x*a[0][0]+v.y*a[0][1]+v.z*a[0][2]+v.w*a[0][3],
        v.x*a[1][0]+v.y*a[1][1]+v.z*a[1][2]+v.w*a[1][3],
        v.x*a[2][0]+v.y*a[2][1]+v.z*a[2][2]+v.w*a[2][3],
        v.x*a[3][0]+v.y*a[3][1]+v.z*a[3][2]+v.w*a[3][3]
       );
    };
    inline T& At(int row,int column){
        return a[row][column];
    }
    inline T& at(int row,int column){
        return a[row][column];
    }
};

template <class T>
void Orthonormalize(Matrix4x4<T> &m){
	int i,j;
	/*
	T l=1.0/sqrt(m[0][0]*m[0][0] + m[0][1]*m[0][1] + m[0][2]*m[0][2] + m[0][3]*m[0][3]);
	m[0][0]*=l;
	m[0][1]*=l;
	m[0][2]*=l;
	m[0][3]*=l;*/
	for(i=3;i>=0;--i){
		// I use 4th row as base row because it makes most sense to preserve 4D rotation the best(i.e. i don't want it to drift as result of orthonormalize).
		// ortnogonalize row i to all prev. orthonormalized rows
    for(j=i+1;j<4;++j){
    	// make row i be orthogonal to j
    	T d = m.a[i][0]*m.a[j][0] + m.a[i][1]*m.a[j][1] + m.a[i][2]*m.a[j][2] + m.a[i][3]*m.a[j][3];
    	m.a[i][0]-=d*m.a[j][0];
    	m.a[i][1]-=d*m.a[j][1];
    	m.a[i][2]-=d*m.a[j][2];
    	m.a[i][3]-=d*m.a[j][3];
    }

    // normalize row i
    T l=1.0/sqrt(m.a[i][0]*m.a[i][0] + m.a[i][1]*m.a[i][1] + m.a[i][2]*m.a[i][2] + m.a[i][3]*m.a[i][3]);
    m.a[i][0]*=l;
	  m.a[i][1]*=l;
	  m.a[i][2]*=l;
	  m.a[i][3]*=l;
	}
}

template <class T>
inline Vec3<T> Mulw1(const Matrix4x4<T> &m,const Vec3<T> &v)
{
    return Vec3<T>(
               v.x*m.a[0][0]+v.y*m.a[0][1]+v.z*m.a[0][2]+m.a[0][3],
               v.x*m.a[1][0]+v.y*m.a[1][1]+v.z*m.a[1][2]+m.a[1][3],
               v.x*m.a[2][0]+v.y*m.a[2][1]+v.z*m.a[2][2]+m.a[2][3]
           );
};
template <class T>
inline Vec3<T> Mulw0(const Matrix4x4<T> &m,const Vec3<T> &v)
{
    return Vec3<T>(
               v.x*m.a[0][0]+v.y*m.a[0][1]+v.z*m.a[0][2],
               v.x*m.a[1][0]+v.y*m.a[1][1]+v.z*m.a[1][2],
               v.x*m.a[2][0]+v.y*m.a[2][1]+v.z*m.a[2][2]
           );
};

template <class T>
Matrix4x4<T> Transpose(const Matrix4x4<T> &m)
{
    return Matrix4x4<T>(
               m.a[0][0],m.a[1][0],m.a[2][0],m.a[3][0],
               m.a[0][1],m.a[1][1],m.a[2][1],m.a[3][1],
               m.a[0][2],m.a[1][2],m.a[2][2],m.a[3][2],
               m.a[0][3],m.a[1][3],m.a[2][3],m.a[3][3]
           );
}

//borrowed from intel docs.
template <class T>
Matrix4x4<T> Inverse(const Matrix4x4<T> &mat)
{
    Matrix4x4<T> result;
    T tmp[12]; /* temp array for pairs */
    T m[16]; /* array of transpose source mat.arix */
    T det; /* determinant */
    /* transpose mat.arix */
    for ( int i = 0; i < 4; i++)
    {
        m[i] = mat.a[i][0];
        m[i + 4] = mat.a[i][1];
        m[i + 8] = mat.a[i][2];
        m[i + 12] = mat.a[i][3];
    }
    /* calculate pairs for first 8 elements (cofactors) */
    tmp[0] = (m[10]) * (m[15]);
    tmp[1] = (m[11]) * (m[14]);
    tmp[2] = (m[9]) * (m[15]);
    tmp[3] = m[11] * m[13];
    tmp[4] = m[9] * m[14];
    tmp[5] = m[10] * m[13];
    tmp[6] = m[8] * m[15];
    tmp[7] = m[11] * m[12];
    tmp[8] = m[8] * m[14];
    tmp[9] = m[10] * m[12];
    tmp[10] = m[8] * m[13];
    tmp[11] = m[9] * m[12];
    /* calculate first 8 elements (cofactors) */
    result.a[0][0] = tmp[0]*m[5] + tmp[3]*m[6] + tmp[4]*m[7];
    result.a[0][0] -= tmp[1]*m[5] + tmp[2]*m[6] + tmp[5]*m[7];
    result.a[0][1] = tmp[1]*m[4] + tmp[6]*m[6] + tmp[9]*m[7];
    result.a[0][1] -= tmp[0]*m[4] + tmp[7]*m[6] + tmp[8]*m[7];
    result.a[0][2] = tmp[2]*m[4] + tmp[7]*m[5] + tmp[10]*m[7];
    result.a[0][2] -= tmp[3]*m[4] + tmp[6]*m[5] + tmp[11]*m[7];
    result.a[0][3] = tmp[5]*m[4] + tmp[8]*m[5] + tmp[11]*m[6];
    result.a[0][3] -= tmp[4]*m[4] + tmp[9]*m[5] + tmp[10]*m[6];
    result.a[1][0] = tmp[1]*m[1] + tmp[2]*m[2] + tmp[5]*m[3];
    result.a[1][0] -= tmp[0]*m[1] + tmp[3]*m[2] + tmp[4]*m[3];
    result.a[1][1] = tmp[0]*m[0] + tmp[7]*m[2] + tmp[8]*m[3];
    result.a[1][1] -= tmp[1]*m[0] + tmp[6]*m[2] + tmp[9]*m[3];
    result.a[1][2] = tmp[3]*m[0] + tmp[6]*m[1] + tmp[11]*m[3];
    result.a[1][2] -= tmp[2]*m[0] + tmp[7]*m[1] + tmp[10]*m[3];
    result.a[1][3] = tmp[4]*m[0] + tmp[9]*m[1] + tmp[10]*m[2];
    result.a[1][3] -= tmp[5]*m[0] + tmp[8]*m[1] + tmp[11]*m[2];
    /* calculate pairs for second 8 elements (cofactors) */
    tmp[0] = m[2]*m[7];
    tmp[1] = m[3]*m[6];
    tmp[2] = m[1]*m[7];
    tmp[3] = m[3]*m[5];
    tmp[4] = m[1]*m[6];
    tmp[5] = m[2]*m[5];
    tmp[6] = m[0]*m[7];
    tmp[7] = m[3]*m[4];
    tmp[8] = m[0]*m[6];
    tmp[9] = m[2]*m[4];
    tmp[10] = m[0]*m[5];
    tmp[11] = m[1]*m[4];
    /* calculate second 8 elements (cofactors) */
    result.a[2][0] = tmp[0]*m[13] + tmp[3]*m[14] + tmp[4]*m[15];
    result.a[2][0] -= tmp[1]*m[13] + tmp[2]*m[14] + tmp[5]*m[15];
    result.a[2][1] = tmp[1]*m[12] + tmp[6]*m[14] + tmp[9]*m[15];
    result.a[2][1] -= tmp[0]*m[12] + tmp[7]*m[14] + tmp[8]*m[15];
    result.a[2][2] = tmp[2]*m[12] + tmp[7]*m[13] + tmp[10]*m[15];
    result.a[2][2]-= tmp[3]*m[12] + tmp[6]*m[13] + tmp[11]*m[15];
    result.a[2][3] = tmp[5]*m[12] + tmp[8]*m[13] + tmp[11]*m[14];
    result.a[2][3]-= tmp[4]*m[12] + tmp[9]*m[13] + tmp[10]*m[14];
    result.a[3][0] = tmp[2]*m[10] + tmp[5]*m[11] + tmp[1]*m[9];
    result.a[3][0]-= tmp[4]*m[11] + tmp[0]*m[9] + tmp[3]*m[10];
    result.a[3][1] = tmp[8]*m[11] + tmp[0]*m[8] + tmp[7]*m[10];
    result.a[3][1]-= tmp[6]*m[10] + tmp[9]*m[11] + tmp[1]*m[8];
    result.a[3][2] = tmp[6]*m[9] + tmp[11]*m[11] + tmp[3]*m[8];
    result.a[3][2]-= tmp[10]*m[11] + tmp[2]*m[8] + tmp[7]*m[9];
    result.a[3][3] = tmp[10]*m[10] + tmp[4]*m[8] + tmp[9]*m[9];
    result.a[3][3]-= tmp[8]*m[9] + tmp[11]*m[10] + tmp[5]*m[8];
    /* calculate determinant */
    det=m[0]*result.a[0][0]+m[1]*result.a[0][1]+m[2]*result.a[0][2]+m[3]*result.a[0][3];
    /* calculate mat.arix inverse */
    if(det!=0.0)
    {
        det = 1/det;
    }
    else
    {
        std::cout<<"can't get inverse"<<std::endl;
    }
    for ( int j = 0; j < 16; j++)
        result.a[j/4][j&3] *= det;

    return result;
};
/***********************************************************************/

template <class T>
struct CoordSys
{
    Quaternion<T> orientation;
    // contains quaternion for transforming
    // from this system to global.
    Vec3<T> position;
    CoordSys()
    {}
    ;
    CoordSys(Quaternion<T> o,Vec3<T>pos)
    {
        position=pos;
        orientation=o;
    };
    //helpser
    void TranslateByLocal(const Vec3<T> &by)
    {
        position+=QuaternionToMatrix(orientation)*by;
    };
    void TranslateByGlobal(const Vec3<T> &by)
    {
        position+=by;
    };

    void Rotate(const Quaternion<T> &by)
    {
        orientation=
#ifndef dont_normalize
            Normalized
#endif
            (orientation*by);
    };
    inline void Rotate(const Vec3<T> &axis,T angle)
    {
        orientation=
#ifndef dont_normalize
            Normalized
#endif
            (orientation*(Quaternion<T>(axis,angle)));
    }

    template<class A>
    inline Vec3<A> TransformFrom(const Vec3<A> &p)
    {
      //Matrix4x4<T> result(QuaternionToMatrix(c1.orientation));
      //result.a[0][3]=c1.position.x;
      //result.a[1][3]=c1.position.y;
      //result.a[2][3]=c1.position.z;
    return position + QuaternionToMatrix(orientation)*p;
    };
    template<class A>
    inline Vec3<A> TransformTo(const Vec3<A> &p)
    {
      return QuaternionToMatrix(Inverse(orientation))*(p-position);
    };
};

template<class T>
CoordSys<T> operator*(const CoordSys<T> &c1,const CoordSys<T> &c2)
{
    CoordSys<T> result;
    result.orientation=
#ifndef dont_normalize
        Normalized
#endif
        (c1.orientation*c2.orientation)
        ;
    result.position=QuaternionToMatrix(c1.orientation)*c2.position+c1.position;
};


template<class T>
CoordSys<T> Inverse(const CoordSys<T> &c1)
{
    CoordSys<T> result;
    result.orientation=Inverse(c1.orientation);
    result.position= -((QuaternionToMatrix(result.orientation))*c1.position);
    return result;
};
template<class T>
inline Matrix4x4<T> TransformFrom(const CoordSys<T> &c1)
{
    Matrix4x4<T> result(QuaternionToMatrix(c1.orientation));
    result.a[0][3]=c1.position.x;
    result.a[1][3]=c1.position.y;
    result.a[2][3]=c1.position.z;
    return result;
};
template<class T>
inline Matrix4x4<T> TransformTo(const CoordSys<T> &c1)
{
    return TransformFrom(Inverse(c1));
};




template<class T>
std::ostream& operator<<(std::ostream&s,const Vec3<T> &v)
{
    s<<"vector:"<<std::endl;
    s<<"["<<v.x<<",\t"<<v.y<<",\t"<<v.z<<"]"<<std::endl;
    return s;
};
template<class T>
std::ostream& operator<<(std::ostream&s,const Quaternion<T> &q)
{
    s<<"Quaternion:"<<std::endl;
    s<<"["<<q.a<<",\t"<<q.b<<",\t"<<q.c<<",\t"<<q.d<<"]"<<std::endl;
    return s;
};
template<class T>
std::ostream& operator<<(std::ostream&s,const Matrix3x3<T> &m)
{
    s<<"matrix:"<<std::endl;
    for(int i=0;i<3;i++)
    {
        s<<"[";
        for(int j=0;j<3;j++)
        {
            s<<m.a[i][j];
            if(j<2)
                s<<",\t";
        };
        s<<"]"<<std::endl;
    };
    return s;
};
template<class T>
std::ostream& operator<<(std::ostream&s,const Matrix4x4<T> &m)
{
    s<<"matrix:"<<std::endl;
    for(int i=0;i<4;i++)
    {
        s<<"[";
        for(int j=0;j<4;j++)
        {
            s<<m.a[i][j];
            if(j<3)
                s<<",\t";
        };
        s<<"]"<<std::endl;
    };
    return s;
};
//typedef unsigned __int64 UINT64;
struct fixed128
{
    unsigned __int64 l,h;
};
fixed128 inline operator+(const fixed128 &a, const fixed128 &b)
{
    fixed128 result;
    result.l=a.l+b.l;
    result.h=a.h+b.h;
    if(result.l<a.l)
        result.h+=1;
    return result;
};
fixed128 inline operator-(const fixed128 &a, const fixed128 &b)
{
    fixed128 result;
    result.l=a.l+b.l;
    result.h=a.h+b.h;
    if(result.l<a.l)
        result.h+=1;
    return result;
};
template<class T>
struct Color{
 T r,g,b;
 Color(){}
template<class A> Color(const Color<A> &c){
  r=c.r;
  g=c.g;
  b=c.b;
 }
 Color(T r_, T g_, T b_){
  r=r_;
  g=g_;
  b=b_;
 }


    template<class A>
    inline Color<T>& operator +=(const Color<A> &v)
    {
        r+=v.r;
        g+=v.g;
        b+=v.b;
        return *this;
    }
    template<class A>
    inline Color<T>& operator -=(const Color<A> &v)
    {

        r-=v.r;
        g-=v.g;
        b-=v.b;
        return *this;
    }

    template<class A>
    inline Color<T>& operator *=(const Color<A> &v)
    {
        r*=v.r;
        g*=v.g;
        b*=v.b;
        return *this;
    }

    template<class A>
    inline Color<T>& operator *=(const A &c)
    {
        r*=c;
        g*=c;
        b*=c;
        return *this;
    }
    inline bool IsBlack() const{
      return (r<=0) &&  (g<=0) &&  (b<=0);
    }
    inline T Brightness() const{
      return (1.0/3.0)*(r+g+b);
    }
};
template<class T>
inline Color<T> operator *(const Color<T> &a , const T &c)
{
    Color<T> result;
    result.r=a.r*c;
    result.g=a.g*c;
    result.b=a.b*c;
    return result;
}

template<class T>
inline Color<T> operator *(const T &c, const Color<T> &a)
{
    Color<T> result;
    result.r=c*a.r;
    result.g=c*a.g;
    result.b=c*a.b;
    return result;
}

template<class T>
inline Color<T> operator *(const Color<T> &a , const Color<T> &b)
{
    Color<T> result;
    result.r=a.r*b.r;
    result.g=a.g*b.g;
    result.b=a.b*b.b;
    return result;
}

template<class T>
inline Color<T> operator +(const Color<T> &a , const Color<T> &b)
{
    Color<T> result;
    result.r=a.r+b.r;
    result.g=a.g+b.g;
    result.b=a.b+b.b;
    return result;
}

template<class T>
inline Color<T> operator -(const Color<T> &a , const Color<T> &b)
{
    Color<T> result;
    result.r=a.r-b.r;
    result.g=a.g-b.g;
    result.b=a.b-b.b;
    return result;
}

template<class T>
inline Color<T> operator -(const Color<T> &b)
{
    Color<T> result;
    result.r=-b.r;
    result.g=-b.g;
    result.b=-b.b;
    return result;
}

template<class T, class U> inline bool AllLessThan(const Color<T> &a , const Color<U> &b){
  return (a.r<b.r)&&(a.g<b.g)&&(a.b<b.b);
}

template<class T, class U> inline bool AllLessThan(const Color<T> &a , U b){
  return (a.r<b)&&(a.g<b)&&(a.b<b);
}

template<class T, class U>
inline T blend(T a, T b, U s){
	return a+(b-a)*s;
}


template <class T>
inline Matrix4x4<T> LorentzMatrix(const Vec3<T> &V){
	// assumes that distance and time have same unit, e.g. second and light second
	// so c=1
  T v2=DotProd(V,V);
  // sanity checks:
  if(v2<1E-20 || v2>=1.0){// too slow, effects can be ignored, or too fast, must be ignored too.
  	return Matrix4x4<T>(
  	1,0,0,-V.x,
  	0,1,0,-V.y,
  	0,0,1,-V.z,
  	0,0,0,1
  	);
  }
	T gamma=1.0/sqrt(1-v2);
	T q=(gamma-1)/v2;
  return Matrix4x4<T>(
  	1+V.x*V.x*q ,   V.x*V.y*q ,   V.x*V.z*q , -V.x*gamma ,
      V.y*V.x*q , 1+V.y*V.y*q ,   V.y*V.z*q , -V.y*gamma ,
  	  V.z*V.x*q ,   V.z*V.y*q , 1+V.z*V.z*q , -V.z*gamma ,
  	 -V.x*gamma , -V.y*gamma , -V.z*gamma , gamma
  	);
}

template <class T, class A>
inline T square(A a){
  return a*a;
}
typedef double real64;
typedef float real32;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec4<float> Vec4f;
typedef Vec4<double> Vec4d;
typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniond;
typedef Matrix3x3<float> Matrix3x3f;
typedef Matrix3x3<double> Matrix3x3d;
typedef Matrix4x4<float> Matrix4x4f;
typedef Matrix4x4<double> Matrix4x4d;
typedef CoordSys<float> CoordSysf;
typedef CoordSys<double> CoordSysd;
typedef Color<float> Colorf;
typedef Color<double> Colord;

}//namespace

#endif //header
