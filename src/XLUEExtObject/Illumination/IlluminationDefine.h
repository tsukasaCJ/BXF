#ifndef __ILLUMINATIONDEFINE_H__
#define __ILLUMINATIONDEFINE_H__

#include <XLGraphic.h>

namespace IlluminationImpl
{
	struct RectF
	{
		float left;
		float top;
		float right;
		float bottom;
	};

	struct RectI
	{
		int left;
		int top;
		int right;
		int bottom;
	};

	struct Point2I
	{
		int x;
		int y;
	};

	struct Point3I
	{
		int x;
		int y;
		int z;
	};

	typedef Point3I Direction3I ;

	struct Vector3I
	{
		union{
			struct{
				Point3I start;
				Point3I end;
			}start2end;

			struct{
				Point3I start;
				Direction3I direction;
			}start2direction;

			struct{
				Direction3I direction;
				Point3I end;
			}end2direction;
		};
	};


	struct Ray
	{
		Vector3I dirction;
		XL_Color color;
	};



	class RectOperation
	{
	public:
		/////windows定义的RECT 是 LONG形的， 这里就直接当RECT用了， 不安全， 反正x86平台，先忽略了
		inline static bool IntersectRect(RectI& resultRect, const RectI& firstRect, const RectI& secondRect)
		{
			return !!(::IntersectRect(reinterpret_cast<RECT*>(&resultRect), reinterpret_cast<CONST RECT*>(&firstRect), reinterpret_cast<CONST RECT*>(&secondRect)));
		}
		
		inline static bool OffsetRect(RectI& rect, int deltaX, int deltaY)
		{
			return !!(::OffsetRect(reinterpret_cast<RECT*>(&rect), deltaX, deltaY));
		}

		inline static bool IsRectEmpty(const RectI& rect)
		{
			return !!(::IsRectEmpty(reinterpret_cast<const RECT*>(&rect)));
		}
	};
}

#endif //__ILLUMINATIONDEFINE_H__