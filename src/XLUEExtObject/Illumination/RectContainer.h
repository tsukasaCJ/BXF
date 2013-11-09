#ifndef __ILLUMINATIONIMPL_RECTCONTAINER_H__
#define __ILLUMINATIONIMPL_RECTCONTAINER_H__

#include <list>
#include <queue>
#include <map>
#include "./IlluminationDefine.h"


#pragma warning(push)
#pragma warning(disable: 4512)


namespace IlluminationImpl
{



class RectContainerRectMap
{
public:
	virtual ~RectContainerRectMap(){}
	virtual RectI& Map(const RectI& thisRect, RectI& mapRect) = 0;
	virtual RectI& ReverseMap(const RectI& mapRect, RectI& thisRect) = 0;
};

class RectContainerRectMapEqual
	: public RectContainerRectMap
{
public:
	virtual RectI& Map(const RectI& thisRect, RectI& mapRect)
	{
		mapRect = thisRect;
		return mapRect;
	}
	virtual RectI& ReverseMap(const RectI& mapRect, RectI& thisRect) 
	{
		thisRect = mapRect;
		return thisRect;
	}
};

class RectContainerRectMapReverse
	: public RectContainerRectMap
{
public:
	RectContainerRectMapReverse(RectContainerRectMap& rectMap)
		: m_rectMap(rectMap)
	{

	}

	virtual RectI& Map(const RectI& thisRect, RectI& mapRect) 
	{
		return m_rectMap.ReverseMap(thisRect, mapRect);
	}
	virtual RectI& ReverseMap(const RectI& mapRect, RectI& thisRect) 
	{
		return m_rectMap.Map(mapRect, thisRect);
	}
private:
	RectContainerRectMap& m_rectMap;
};

template<typename T>
class RectContainer
{
public:
	typedef T ValueType;
	RectContainer();
	////为了能够支持浅拷贝， 析构函数中不会调用Clean， 要释放必须显示调用Clean
	~RectContainer();
	
	
	class ValueEnum
	{
	public:
		virtual ~ValueEnum(){}
		virtual int OnRectValue(const RectI& rect, const ValueType& value) = 0;
		virtual int OnPointValue(const RectI& rect, const ValueType* const pValueBitmap, const unsigned long& ulLineLength) = 0;
	};
	static const int RetSuc = 0;
	static const int RetNoIndex = 1;
	static const int RetInvalidParam = 2;
	
	static const unsigned long InvalidIndex = static_cast<unsigned long>(-1);
public:
	int SetDefaultValue(const ValueType& value);
	int AddRectValue(const RectI& rect, const ValueType& value, unsigned long& refIndex);
	int AddPointValue(const RectI& rect, unsigned long& refIndex, T** ppValueBitmap = NULL, unsigned long* pLineLength = NULL);
	int AddUnhandledPointValue(const RectI& rect, unsigned long& refIndex, T* pValueBitmap, const unsigned long lineLength);
	int GetPointValueBitmap(unsigned long& index, ValueType* & refValueBitmap, unsigned long& refLineLength);
	
	int Offset(Point2I& offset);
	int MoveRect(const unsigned long& index, const RectI& rect);
	void Clean();
	int Copy(RectContainer& rightContainer);
	
private:
	ValueType m_defaultValue;
	struct PointValueStub{
		RectI clouseRect;
		ValueType* valueBitmap;
		unsigned long lineLength;
		unsigned long index;
		bool handled;
		bool operator ==(const PointValueStub& right) const
		{
			return index == right.index;
		}
	};
	void DeleteValueBitmap(PointValueStub* pPointStub);
	typedef std::list<PointValueStub> PointValueList;
	PointValueList m_pointValuelist;
	struct RectValueStub{
		RectI rect;
		ValueType value;
		unsigned long index;

		bool operator ==(const RectValueStub& right) const
		{
			return index == right.index;
		}
	};
	typedef std::list<RectValueStub> RectValueList;
	RectValueList m_rectValueList;

	unsigned long m_ulNextIndex;


public:
	enum RectEnumType
	{
		RectEnumType_RectValue = 0,
		RectEnumType_PointValue = 1,
		RectEnumType_DefaultValue = 2,
	};
	int EnumInRect(const RectI& rect, ValueEnum& enumtor);
private:
	struct RectEnumStub
	{
		RectI rect;
		RectEnumType enumType;
		union
		{
			RectValueStub* pRectValue;
			PointValueStub* pPointValue;
		};
	};
	typedef std::queue<RectEnumStub> RectEnumStubQueue;
	typedef std::queue<RectI> RectEnumInputQueue;

	int EnumInRectImpl(RectEnumInputQueue& inputQueue, RectEnumStubQueue& outputQueue);
	bool DivideRect(RectEnumInputQueue& inputQueue, RectEnumStubQueue& outputQueue, RectI& resultRect, const RectI& valueRect);


public:
	/// class AddValue_
	//{
	//public:
	//	T& operator()(const LeftValueType& leftValue, const RightValueType& rightValue);
	//}

	template <class LeftValueType,  class RightValueType, class AddValue_>
	static int Cat(RectContainer<LeftValueType>&leftContainer, RectContainer<RightValueType>& rightContainer
		, const RectI&clouseRect, RectContainer& resultContainer, RectContainerRectMap& rectMap);
	///把clouseRect用rectMap映射到catContainer之上，范围内的catContainer的值用AddValue做变换后覆写原来的值
	///不保留clouseRect之外的值， 是因为合并逻辑太过复杂了， 不如给每个clouseRect单独维护一个container
	template <class CatValueType, class AddValue_>
	int Cat(RectContainer<CatValueType>& catContainer, const RectI&clouseRect, RectContainerRectMap& rectMap);


private:
	template <class LeftValueType, class RightValueType, class AddValue_>
	class EnumtorForCat
		: public RectContainer<LeftValueType>::ValueEnum
	{
	public:
		EnumtorForCat(RectContainer<LeftValueType>&leftContainer, RectContainer<RightValueType>& rightContainer
			, const RectI&clouseRect, RectContainer& resultContainer, RectContainerRectMap& rectMap);
		virtual int OnRectValue(const RectI& rect, const LeftValueType& value);
		virtual int OnPointValue(const RectI& rect, const LeftValueType* const pValueBitmap, const unsigned long& ulLineLength);
	
		////懒得前置声明友元了， 反正私用， 直接公开
	public:
		RectContainerRectMap& m_rectMap;
		const RectI& m_clouseRect;
		RectContainer& m_resultContainer;
		struct RectEnumStub
		{
			RectEnumType enumType;
			RectI rect;
			const LeftValueType* pValue;
			unsigned long ulLineLength;
		}m_curRectValue;
		RectContainer<LeftValueType>& m_leftContainer;
		RectContainer<RightValueType>& m_rightContainer;
	};

	template <class LeftValueType, class RightValueType, class AddValue_>
	class CatEnumtorForCat
		: public RectContainer<RightValueType>::ValueEnum
	{
	public:
		CatEnumtorForCat(EnumtorForCat<LeftValueType, RightValueType, AddValue_>& enumtor);

		virtual int OnRectValue(const RectI& rect, const RightValueType& value);
		virtual int OnPointValue(const RectI& rect, const RightValueType* const pValueBitmap, const unsigned long& ulLineLength);

	private:
		EnumtorForCat<LeftValueType, RightValueType, AddValue_>& m_enumtor;
	};

public:
	template<class AddValueType>
	class AddValue
	{
	public:
		virtual ~AddValue(){}

		virtual T& operator()(T& leftValue, const AddValueType& rightValue) = 0;
	};
	////Add方法的实现类似于Cat， 比Cat的区别在于clouseRect之外的部分保留不受影响，而且是一个原地的，不会额外创建中间结果的container
	///但是相较Cat， 矩形分裂会更快, 劣化为点阵的速度更高~~
	template<class AddValueType>
	int Add(RectContainer<AddValueType>& addContainer, const RectI&clouseRect, RectContainerRectMap& rectMap, AddValue<AddValueType>& addValue);

private:
	template <class AddValueType>
	class InitAndAddValue
		: public AddValue<AddValueType>
	{
	public:
		InitAndAddValue(const T& initValue, AddValue<AddValueType>& addValue);

		virtual T& operator()(T& leftValue, const AddValueType& rightValue);

	private:
		const T& m_initValue;
		AddValue<AddValueType>& m_addValue;
	};

	template<class AddValueType>
	int AddValueOnPoint(T* leftBitmap, unsigned long leftColOffset, unsigned long leftLineOffset, const AddValueType* rightBitmap
		, unsigned long rightColOffset, unsigned long rightLineOffset, AddValue<AddValueType>& addValue,const RectI& rect);
	int EraseRect(const unsigned long& index);

public:
	class ValueEnumEx
	{
	public:
		virtual ~ValueEnumEx(){};

		virtual int OnRectValue(const RectI& rect, const ValueType& value, const unsigned long &index, const RectI& clouseRect) = 0;
		virtual int OnPointValue(const RectI& rect, const ValueType* const pValueBitmap, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect) = 0;
	};

	int EnumInRectEx(const RectI& rect, ValueEnumEx& enumtor);
	
private:
	class ValueEnumWrap
		: public ValueEnumEx
	{
	public:
		ValueEnumWrap(ValueEnum& enumtor)
			: m_enumtor(enumtor)
		{

		}

		virtual int OnRectValue(const RectI& rect, const ValueType& value, const unsigned long &index, const RectI& clouseRect)
		{
			((void)clouseRect);
			((void)index);
			return m_enumtor.OnRectValue(rect, value);
		}
		virtual int OnPointValue(const RectI& rect, const ValueType* const pValueBitmap, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect)
		{
			((void)index);
			((void)clouseRect);
			return m_enumtor.OnPointValue(rect, pValueBitmap, ulLineLength);
		}

	private:
		ValueEnum& m_enumtor;
	};


	template<class AddValueType>
	class ValueEnumForAdd
		: public ValueEnumEx
	{
	public:
		ValueEnumForAdd(RectContainer&leftContainer, RectContainer<AddValueType>& rightContainer, const RectI& clouseRect, RectContainerRectMap& rectMap, AddValue<AddValueType>& addValue);

		virtual int OnRectValue(const RectI& rect, const ValueType& value, const unsigned long &index, const RectI& clouseRect);
		virtual int OnPointValue(const RectI& rect, const ValueType* const pValueBitmap, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect);
		


	public:
		struct RightRectStub
		{
			RectI leftRect;
		
			RectEnumType rightEnumType;
			const AddValueType* pRightValue;
			unsigned long rightLineLength;
		};
		typedef std::list<RightRectStub> RightRectList;
		struct DivideRectStub
		{
			unsigned long leftIndex;
			RectEnumType leftEnumType;
			////因为原来的RectValue可能被释放掉， 这里以值引用了
			union{
				T leftRectValue;
				struct{
				T* pValueBitmap;
				unsigned long lineLength;
				}leftPointValue;
			}leftValue;
			RectI leftClouseRect;
			RectI leftEnumRect;

			RightRectList rightRectList;
		};
		typedef std::map<unsigned long, DivideRectStub> DivideRectListMap;
		RightRectList* m_pCurDivideRectList;
		DivideRectListMap m_divideRectListMap;

		RectContainer& m_leftContainer;
		RectContainer<AddValueType>& m_rightContainer;
		const RectI& m_clouseRect;
		RectContainerRectMap& m_rectMap; 
		AddValue<AddValueType>& m_addValue;

	};

	template<class AddValueType>
	class RightValueEnumForAdd
		: public RectContainer<AddValueType>::ValueEnumEx
	{
	public:
		RightValueEnumForAdd(ValueEnumForAdd<AddValueType>& enumtor);
		virtual int OnRectValue(const RectI& rect, const AddValueType& value, const unsigned long &index, const RectI& clouseRect);
		virtual int OnPointValue(const RectI& rect, const AddValueType* const pValueBitmap, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect);

	private:
		ValueEnumForAdd<AddValueType>& m_enumtor;
	};
	
};


}

#pragma warning(pop)

#include "RectContainer.cpp"

#endif //__ILLUMINATIONIMPL_RECTCONTAINER_H__