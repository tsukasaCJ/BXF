#pragma once
#include "IlluminationDefine.h"
#include <list>


namespace IlluminationImpl
{

template <class T>
class IContainerEnumtor
{
public:
	virtual ~IContainerEnumtor(){}
	virtual int OnPos(const Point2I& ptPos, T& ray) = 0;
	virtual int InRect(const RectI& rect, T& ray) = 0;
};

template <class T>
class IContainer
{
public:
	virtual ~IContainer(){}
	enum EnumType{
		EnumInput = 0,
		EnumOutput,
		EnumEnd,
	};
	static const int RetSuc = 0;
	static const int RetInvalidParam = 1;
	static const int RetNoImpl = 2;
	static const int RetOutofMemory = 3;

	static const int RetStopEnum = 1;
	static const int RetContinueEnum = 2;

	typedef IContainerEnumtor<T> Enumtor ;
	////不能保证沿扫描线遍历，不过无所谓
	virtual int EnumOnPos(IContainerEnumtor<T>& refEnumtor) = 0;
	////可能返回只包围一个像素的Rect表示孤立的点
	virtual int EnumInRect(IContainerEnumtor<T>& refEnumtor) = 0;

	////以实际的存储结构遍历矩形区域内的元素，如果是按点存储的是调用OnPos回调，如果是按矩形存储的调用InRect回调
	virtual int EnumRectRegion(IContainerEnumtor<T>& refEnumtor, const RectI& rect) = 0;

	//先AddClouseRect 在下一次调用AddClouseRect之前 AddRayOnPos的调用需要保证在ClouseRect之中， 这里不做超界检查
	virtual int AddClouseRect(const RectI& rect) = 0;
	virtual int AddOnPos(const Point2I& ptPos, T& ray) = 0;

	virtual int AddInRect(const RectI& rect, T& ray) = 0;
	virtual int Clean() = 0;
};


template <class T>
class ContainerPointImpl
	: public IContainer<T>
{
public:
	ContainerPointImpl();
	virtual ~ContainerPointImpl();
	virtual int EnumOnPos(IContainerEnumtor<T>& refEnumtor) ;
	virtual int EnumInRect(IContainerEnumtor<T>& refEnumtor) ;

	virtual int AddClouseRect(const RectI& rect);
	virtual int AddOnPos(const Point2I& ptPos, T& ray) ;
	virtual int AddInRect(const RectI& rect, T& ray) ;
	virtual int Clean() ;

protected:
	struct ClouseRectStub{
		RectI rect;
		T* bitmap;
		int bitmapWidth;
	};
	typedef std::list<ClouseRectStub> ClouseRectList;
	ClouseRectList m_list;
	ClouseRectStub* m_curClouseRect;

	T*& GetOnPosFromClouse(const Point2I& ptPos, ClouseRectStub& clouseRect);
};


template <class T>
class ContainerRectImpl
	: public IContainer<T>
{
public:
	virtual ~ContainerRectImpl();
	virtual int EnumOnPos(IContainerEnumtor<T>& refEnumtor) ;
	virtual int EnumInRect(IContainerEnumtor<T>& refEnumtor) ;

	virtual int AddClouseRect(const RectI& rect)
	{
		((void)rect);
		return IContainer::RetNoImpl;
	}
	virtual int AddOnPos(const Point2I& ptPos, T& ray) ;
	virtual int AddInRect(const RectI& rect, T& ray) ;
	virtual int Clean() ;

protected:
	struct RayStub
	{
		T ray;
		RectI rect;
	};
	typedef std::list<RayStub> RayList;
	RayList m_list;

};


template <class T>
class ContainerCompositeImpl
	: public IContainer<T>
{
public:
	virtual ~ContainerCompositeImpl();

	virtual int EnumOnPos(IContainerEnumtor<T>& refEnumtor) ;
	virtual int EnumInRect(IContainerEnumtor<T>& refEnumtor) ;

	virtual int AddClouseRect(const RectI& rect);
	virtual int AddOnPos(const Point2I& ptPos, T& ray) ;
	virtual int AddInRect(const RectI& rect, T& ray) ;
	virtual int Clean() ;

protected:
	ContainerRectImpl<T> m_rectContainer;
	ContainerPointImpl<T> m_pointContainer;
};

//typedef ContainerRectImpl<int> RayContainer; 
//typedef ContainerPointImpl<int> RayContainer2;
//typedef ContainerCompositeImpl<int> RayContainer3;

}