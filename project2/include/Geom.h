#ifndef GEOM_H
#define GEOM_H

#include <vector>

#ifdef WIN32
typedef __int64 int64;
#else
typedef long long int64;
#endif
#include <math.h>


struct Rect
{
   Rect(int inW=0,int inH=0) : x(0), y(0), w(inW), h(inH) { } 
   Rect(int inX,int inY,int inW,int inH) : x(inX), y(inY), w(inW), h(inH) { } 
   Rect(int inX0,int inY0,int inX1,int inY1, bool inDummy) :
      x(inX0), y(inY0), w(inX1-inX0), h(inY1-inY0) { } 

	Rect Intersect(const Rect &inOther) const;
	Rect operator *(int inScale) const { return Rect(x*inScale,y*inScale,w*inScale,h*inScale); }
	Rect operator /(int inScale) const { return Rect(x/inScale,y/inScale,w/inScale,h/inScale); }
	int x1() const { return x+w; }
	int y1() const { return y+h; }

   int x,y;
   int w,h;
};



template<typename T>
struct Point2D
{
	inline Point2D() { }
	inline Point2D(T inX,T inY) : x(inX), y(inY) { }
	inline Point2D(T *inPtr) : x(inPtr[0]), y(inPtr[1]) { }

	inline bool operator==(const Point2D &inRHS) const
	   { return x==inRHS.x && y==inRHS.y; }
	inline bool operator!=(const Point2D &inRHS) const
	   { return x!=inRHS.x || y!=inRHS.y; }

	inline Point2D operator+(const Point2D &inRHS) const
	   { return Point2D(x+inRHS.x,y+inRHS.y); }
	inline Point2D operator-(const Point2D &inRHS) const
	   { return Point2D(x-inRHS.x,y-inRHS.y); }
	inline Point2D operator-() const
	   { return Point2D(-x,-y); }
	inline T Norm2() const
	   { return x*x + y*y; }
	inline double Norm() const
	   { return sqrt((double)(x*x + y*y)); }
	inline Point2D Perp() const
	   { return Point2D(-y,x); }
	inline Point2D Perp(double inLen) const
	{
		double norm = Norm();
		if (norm>0)
		{
			norm = inLen/norm;
			return Point2D(-y*norm,x*norm);
		}
		return Point2D(0,0);
	}

   T x;
   T y;
};


typedef Point2D<float> UserPoint;
typedef Point2D<int>   ImagePoint;


template<int FIXED>
struct FixedPoint
{
	enum { Bits = FIXED };

   inline FixedPoint() {}
   inline FixedPoint(int inX,int inY) :x(inX), y(inY) {}
   inline FixedPoint(double inX,double inY)
   {
      x =  (int)(inX * (double)(1<<Bits) );
      y =  (int)(inY * (double)(1<<Bits));
   }

   inline int X() const { return x>>Bits; };
   inline int Y() const { return y>>Bits; };
   inline int X(int inAABits) const { return x>>(Bits-inAABits); };
   inline int Y(int inAABits) const { return y>>(Bits-inAABits); };

   inline FixedPoint(const FixedPoint &inRHS) :x(inRHS.x), y(inRHS.y) {}
   inline FixedPoint(const ImagePoint &inRHS) :
                x(inRHS.x<<Fixed), y(inRHS.y<<Fixed) { }
   
   inline bool operator==(const FixedPoint inRHS) const
      { return x==inRHS.x && y==inRHS.y; }

   inline bool operator!=(const FixedPoint inRHS) const
      { return x!=inRHS.x && y!=inRHS.y; }


   inline FixedPoint operator-(const FixedPoint inRHS) const
      { return FixedPoint(x-inRHS.x,y-inRHS.y); }

   inline FixedPoint operator+(const FixedPoint inRHS) const
      { return FixedPoint(x+inRHS.x,y+inRHS.y); }

   inline FixedPoint operator*(int inScalar) const
      { return FixedPoint(x*inScalar,y*inScalar); }

   inline FixedPoint operator/(int inDivisor) const
      { return FixedPoint(x/inDivisor,y/inDivisor); }

   inline FixedPoint operator>>(int inShift) const
      { return FixedPoint(x>>inShift,y>>inShift); }

   inline FixedPoint operator<<(int inShift) const
      { return FixedPoint(x<<inShift,y<<inShift); }

   inline void operator+=(const FixedPoint &inRHS)
      { x+=inRHS.x, y+=inRHS.y; }

   int x;
   int y;
};

typedef FixedPoint<10> Fixed10;


template<typename T_>
struct Extent2D
{
   Extent2D() : mValidX(false), mValidY(false)
   {
      mMinX = mMinY = mMaxX = mMaxY = 0;
   }


   template<typename P_>
   inline void AddX(P_ inX)
   {
      if (mValidX)
      {
         if (inX<mMinX) mMinX = (T_)inX;
         else if (inX>mMaxX) mMaxX = (T_)inX;
      }
      else
      {
         mMinX = mMaxX = (T_)inX;
         mValidX = true;
      }

   }

   template<typename P_>
   inline void AddY(P_ inY)
   {
      if (mValidY)
      {
         if (inY<mMinY) mMinY = (T_)inY;
         else if (inY>mMaxY) mMaxY = (T_)inY;
      }
      else
      {
         mMinY = mMaxY = (T_)inY;
         mValidY = true;
      }
   }

   template<typename P_>
   inline void Add(P_ inX, P_ inY)
   {
      AddX(inX);
      AddY(inY);
   }


   template<typename P_>
   inline void Add(const P_ &inPoint)
   {
      AddX(inPoint.x);
      AddY(inPoint.y);
   }

   inline void Add(const Extent2D<T_> &inExtent)
   {
      if (inExtent.mValidX)
      {
         AddX(inExtent.mMinX);
         AddX(inExtent.mMaxX);
      }
      if (inExtent.mValidY)
      {
         AddY(inExtent.mMinY);
         AddY(inExtent.mMaxY);
      }
   }
   bool Intersect(T_ inX0,T_ inY0, T_ inX1, T_ inY1)
   {
      if (!mValidX)
      {
         mMinX = inX0;
         mMaxX = inX1;
         mValidX = true;
      }
      else
      {
         if (inX0 > mMinX) mMinX = inX0;
         if (inX1 < mMaxX) mMaxX = inX1;
      }
      if (!mValidY)
      {
         mMinY = inY0;
         mMaxY = inY1;
         mValidY = true;
      }
      else
      {
         if (inY0 > mMinY) mMinY = inY0;
         if (inY1 < mMaxY) mMaxY = inY1;
      }
      return mMinX<mMaxX && mMinY<mMaxY;
   }
   void Translate(int inTX,int inTY)
   {
      mMinX += inTX;
      mMaxX += inTX;
      mMinY += inTY;
      mMaxY += inTY;
   }

	void Transform(double inSX, double inSY, double inTX, double inTY)
   {
      mMinX = inTX + inSX*(mMinX);
      mMaxX = inTX + inSX*(mMaxX);
      mMinY = inTY + inSY*(mMinY);
      mMaxY = inTY + inSY*(mMaxY);
   }



   inline bool Valid() const { return mValidX && mValidY; }

   T_ Width() const { return mMaxX-mMinX; }
   T_ Height() const { return mMaxY-mMinY; }

   T_ mMinX,mMaxX;
   T_ mMinY,mMaxY;
   bool mValidX,mValidY;
};

typedef Extent2D<int> Extent2DI;
typedef Extent2D<float> Extent2DF;


struct Tri
{
   Tri(int i0=0, int i1=0, int i2=0) { mIndex[0] = i0; mIndex[1] = i1; mIndex[2]=i2; }
   int mIndex[3];
};




#endif
