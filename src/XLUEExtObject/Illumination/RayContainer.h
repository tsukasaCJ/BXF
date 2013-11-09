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
	////���ܱ�֤��ɨ���߱�������������ν
	virtual int EnumOnPos(IContainerEnumtor<T>& refEnumtor) = 0;
	////���ܷ���ֻ��Χһ�����ص�Rect��ʾ�����ĵ�
	virtual int EnumInRect(IContainerEnumtor<T>& refEnumtor) = 0;

	////��ʵ�ʵĴ洢�ṹ�������������ڵ�Ԫ�أ�����ǰ���洢���ǵ���OnPos�ص�������ǰ����δ洢�ĵ���InRect�ص�
	virtual int EnumRectRegion(IContainerEnumtor<T>& refEnumtor, const RectI& rect) = 0;

	//��AddClouseRect ����һ�ε���AddClouseRect֮ǰ AddRayOnPos�ĵ�����Ҫ��֤��ClouseRect֮�У� ���ﲻ��������
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