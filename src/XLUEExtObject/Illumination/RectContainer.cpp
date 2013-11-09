#include "./RectContainer.h"

#pragma warning(push)
#pragma warning(disable: 4127)

namespace IlluminationImpl
{

template<typename T>
RectContainer<T>::RectContainer()
	: m_ulNextIndex(0) 
{
	::memset(&m_defaultValue, 0, sizeof(ValueType));
}

template<typename T>
RectContainer<T>::~RectContainer()
{
}

template<typename T>
void RectContainer<T>::Clean()
{
	PointValueList::iterator iterPointValue = m_pointValuelist.begin();
	while (iterPointValue != m_pointValuelist.end())
	{
		DeleteValueBitmap(&(*iterPointValue));
		++iterPointValue;
	}
	m_pointValuelist.clear();
	m_rectValueList.clear();
}
	
template<typename T>
int RectContainer<T>::SetDefaultValue(const ValueType& value)
{
	m_defaultValue = value;
	return RetSuc;
}

template<typename T>
int RectContainer<T>::AddRectValue(const RectI& rect, const ValueType& value, unsigned long& refIndex)
{
	RectValueStub newRectValue;
	newRectValue.rect = rect;
	newRectValue.value = value;
	newRectValue.index = m_ulNextIndex;
	refIndex = m_ulNextIndex;
	++ m_ulNextIndex;
	m_rectValueList.push_back(newRectValue);
	return RetSuc;
}


template<typename T>
int RectContainer<T>::AddPointValue(const RectI& rect, unsigned long& refIndex, T** ppValueBitmap, unsigned long* pLineLength)
{
	PointValueStub newPointValue;
	newPointValue.clouseRect = rect;
	int bitmapWidth = rect.right - rect.left;
	int bitmapHeight = rect.bottom - rect.top;
	newPointValue.valueBitmap = new ValueType[bitmapWidth * bitmapHeight];
	newPointValue.handled = true;
	newPointValue.lineLength = bitmapWidth * sizeof(ValueType);
	newPointValue.index = m_ulNextIndex;
	refIndex = m_ulNextIndex;
	m_pointValuelist.push_back(newPointValue);
	++ m_ulNextIndex; 

	if (ppValueBitmap != NULL)
	{
		*ppValueBitmap = newPointValue.valueBitmap;
	}
	if (pLineLength != NULL)
	{
		*pLineLength = newPointValue.lineLength;
	}
	return RetSuc;
}


template<typename T>
int RectContainer<T>::AddUnhandledPointValue(const RectI& rect, unsigned long& refIndex, T* pValueBitmap, const unsigned long lineLength)
{
	PointValueStub newPointValue;
	newPointValue.clouseRect = rect;
	//int bitmapWidth = rect.right - rect.left;
	//int bitmapHeight = rect.bottom - rect.top;
	newPointValue.valueBitmap = pValueBitmap;
	newPointValue.handled = false;
	newPointValue.lineLength = lineLength;
	newPointValue.index = m_ulNextIndex;
	refIndex = m_ulNextIndex;
	m_pointValuelist.push_back(newPointValue);
	++ m_ulNextIndex; 

	return RetSuc;
}


template<typename T>
int RectContainer<T>::GetPointValueBitmap(unsigned long& index, ValueType* & refValueBitmap, unsigned long& refLineLength)
{
	PointValueStub pointValue;
	pointValue.index = index;
	PointValueList::iterator iterPointValue = std::find(m_pointValuelist.begin(), m_pointValuelist.end(), pointValue);
	if (iterPointValue == m_pointValuelist.end())
	{
		return RetNoIndex;
	}
	refValueBitmap = iterPointValue->valueBitmap;
	refLineLength = iterPointValue->lineLength;
	return RetSuc;
}

template<typename T>
void RectContainer<T>::DeleteValueBitmap(PointValueStub* pPointStub)
{
	assert(pPointStub);
	if (pPointStub->handled)
	{
		delete []pPointStub->valueBitmap;
	}
	else
	{
		pPointStub->valueBitmap = NULL;
	}
}

template<typename T>
int RectContainer<T>::Offset(Point2I& offset)
{
	RectValueList::iterator iterRect = m_rectValueList.begin();
	while (iterRect != m_rectValueList.end())
	{
		RectOperation::OffsetRect((iterRect->rect), offset.x, offset.y);
		++iterRect;
	}

	PointValueList::iterator iterPoint = m_pointValuelist.begin();
	while (iterPoint != m_pointValuelist.end())
	{
		RectOperation::OffsetRect((iterPoint->clouseRect), offset.x, offset.y);
		++iterPoint;
	}
	return RetSuc;
}


template<typename T>
int RectContainer<T>::MoveRect(const unsigned long& index, const RectI& rect)
{
	RectValueStub rectValue;
	rectValue.index = index;
	RectValueList::iterator iterRectValue = std::find(m_rectValueList.begin(), m_rectValueList.end(), rectValue);
	if (iterRectValue != m_rectValueList.end())
	{
		iterRectValue->rect = rect;
		return RetSuc;
	}
	PointValueStub pointValue;
	pointValue.index = index;
	PointValueList::iterator iterPointValue = std::find(m_pointValuelist.begin(), m_pointValuelist.end(), pointValue);
	if (iterPointValue == m_pointValuelist.end())
	{
		return RetNoIndex;
	}
	int bitmapWidth = rect.right - rect.left;
	int bitmapHeight = rect.bottom - rect.top;
	int oldBitmapWidth = iterPointValue->clouseRect.right - iterPointValue->clouseRect.left;
	int oldBitmapHeight = iterPointValue->clouseRect.bottom - iterPointValue->clouseRect.top;
	if ((bitmapWidth == oldBitmapWidth) && (bitmapHeight == oldBitmapHeight))
	{
		iterPointValue->clouseRect = rect;
	}
	else
	{
		/////引入非持有的内存之后， 这里就的默认行为就奇怪了
		unsigned long lineLength = bitmapWidth * sizeof(ValueType);
		ValueType* valueBitmap = new ValueType[bitmapWidth * bitmapHeight];
		int setWidth = min(bitmapWidth, oldBitmapWidth);
		int setHeight = min(bitmapHeight, oldBitmapHeight);
		unsigned char* ptrValueBitmapLine = valueBitmap;
		unsigned char* ptrOldValueBitmapLine = iterPointValue->valueBitmap;
		ValueType* ptrValueBitmap = NULL;
		ValueType* ptrOldValueBitmap = NULL;
		for (int iy = 0; iy < setHeight; ++iy)
		{
			ptrValueBitmap = ptrValueBitmapLine;
			ptrOldValueBitmap = ptrOldValueBitmapLine;
			for (int ix = 0; ix < setWidth; ++ix)
			{
				*ptrValueBitmap = *ptrOldValueBitmap;
				++ptrValueBitmap;
				++ptrOldValueBitmap;
			}
			ptrValueBitmapLine += lineLength;
			ptrOldValueBitmapLine += iterPointValue->lineLength;
		}
		ptrValueBitmapLine = valueBitmap + oldBitmapHeight * lineLength + oldBitmapWidth * sizeof(ValueType);  
		for (int iy = oldBitmapHeight; iy < bitmapHeight; ++iy)
		{
			ptrValueBitmap = ptrValueBitmapLine;
			for(int ix = oldBitmapWidth; ix < bitmapWidth; ++ix)
			{
				*ptrValueBitmap = m_defaultValue;
				++ptrValueBitmap;
			}
			ptrValueBitmapLine += lineLength; 
		}
		DeleteValueBitmap(&(*iterPointValue));
		iterPointValue->handled = true;
		iterPointValue->valueBitmap = valueBitmap;
		iterPointValue->lineLength = lineLength;
		iterPointValue->clouseRect = rect;
	}
	return RetSuc;
}

template<typename T>
int RectContainer<T>::EraseRect(const unsigned long& index)
{
	PointValueStub pointValue;
	pointValue.index = index;
	PointValueList::iterator iterPointValue = std::find(m_pointValuelist.begin(), m_pointValuelist.end(), pointValue);
	if (iterPointValue != m_pointValuelist.end())
	{
		DeleteValueBitmap(&(*iterPointValue));
		m_pointValuelist.erase(iterPointValue);
		return RetSuc;
	}	
	RectValueStub rectValue;
	rectValue.index = index;
	RectValueList::iterator iterRectValue = std::find(m_rectValueList.begin(), m_rectValueList.end(), rectValue);
	if (iterRectValue == m_rectValueList.end())
	{
		return RetNoIndex;
	}
	m_rectValueList.erase(iterRectValue);
	return RetSuc;
}



template<typename T>
int RectContainer<T>::EnumInRect(const RectI& rect, ValueEnum& enumtor)
{
	ValueEnumWrap enumtorEx(enumtor);
	return EnumInRectEx(rect, enumtorEx);
}


/////这里的规则会比较复杂， 优先搜索point value， 因为有可能出现重叠区域， 
/// 所以命中有相交时， 把原始的输入矩形排除掉相交区域后分裂， 迭代直到没有任何相交为止， 剩下的矩形直接返回default value
template<typename T>
int RectContainer<T>::EnumInRectImpl(RectEnumInputQueue& inputQueue, RectEnumStubQueue& outputQueue)
{
	while (!inputQueue.empty())
	{
		bool ifReloop = false;
		PointValueList::iterator iterPointValue = m_pointValuelist.begin();
		while (iterPointValue != m_pointValuelist.end())
		{
			RectI resultRect;
			if (DivideRect(inputQueue, outputQueue, resultRect, iterPointValue->clouseRect))
			{
				RectEnumStub newStub;
				newStub.rect = resultRect;
				newStub.enumType = RectEnumType_PointValue;
				newStub.pPointValue = &(*iterPointValue);
				outputQueue.push(newStub);
				ifReloop = true;
				break;
			}
			++iterPointValue;
		}
		if (ifReloop)
		{
			continue;
		}
		RectValueList::iterator iterRectValue = m_rectValueList.begin();
		while (iterRectValue != m_rectValueList.end())
		{
			RectI resultRect;
			if (DivideRect(inputQueue, outputQueue, resultRect, iterRectValue->rect))
			{
				RectEnumStub newStub;
				newStub.rect = resultRect;
				newStub.enumType = RectEnumType_RectValue;
				newStub.pRectValue = &(*iterRectValue);
				outputQueue.push(newStub);
				ifReloop = true;
				break;
			}
			++iterRectValue;
		}
		if (ifReloop)
		{
			continue;
		}
		RectEnumStub newStub;
		newStub.rect = inputQueue.front();
		newStub.enumType = RectEnumType_DefaultValue;
		inputQueue.pop();
		outputQueue.push(newStub);
	}

	return RetSuc;
}

template<typename T>
bool RectContainer<T>::DivideRect(RectEnumInputQueue& inputQueue, RectEnumStubQueue& outputQueue, RectI& retRect, const RectI& valueRect)
{
	((void)outputQueue);
	RectI& inputRect = inputQueue.front();
	RectI resultRect;
	
	if (RectOperation::IntersectRect(resultRect, inputRect, valueRect))
	{
		RectI firstRect = {inputRect.left, inputRect.top, inputRect.right, resultRect.top};
		RectI secondRect = {inputRect.left, resultRect.top, resultRect.left, resultRect.bottom};
		RectI thirdRect = {resultRect.right, resultRect.top, inputRect.right, resultRect.bottom};
		RectI fourthRect = {inputRect.left, resultRect.bottom, inputRect.right, inputRect.bottom};
		if (!RectOperation::IsRectEmpty(firstRect))
		{
			inputQueue.push(firstRect);
		}
		if (!RectOperation::IsRectEmpty(secondRect))
		{
			inputQueue.push(secondRect);
		}
		if (!RectOperation::IsRectEmpty(thirdRect))
		{
			inputQueue.push(thirdRect);
		}
		if (!RectOperation::IsRectEmpty(fourthRect))
		{
			inputQueue.push(fourthRect);
		}
		retRect = resultRect;
		inputQueue.pop();
		return true;
	}
	else
	{
		return false;
	}
}





template <class T>
template <class LeftValueType, class RightValueType, class AddValue_>
RectContainer<T>::EnumtorForCat<LeftValueType, RightValueType, AddValue_>::EnumtorForCat(
	RectContainer<LeftValueType>&leftContainer, RectContainer<RightValueType>& rightContainer
	, const RectI&clouseRect, RectContainer& resultContainer, RectContainerRectMap& rectMap)
		: m_rectMap(rectMap)
		, m_clouseRect(clouseRect)
		, m_resultContainer(resultContainer)
		, m_leftContainer(leftContainer)
		, m_rightContainer(rightContainer)
{

}

template <class T>
template <class LeftValueType, class RightValueType, class AddValue_>
int RectContainer<T>::EnumtorForCat<LeftValueType, RightValueType, AddValue_>::OnRectValue(const RectI& rect, const LeftValueType& value)
{
	RectI mapRect;
	m_rectMap.Map(rect, mapRect);

	m_curRectValue.enumType = RectEnumType_RectValue;
	m_curRectValue.pValue = &value;
	m_curRectValue.rect = mapRect;
	m_curRectValue.ulLineLength = 0;

	CatEnumtorForCat<LeftValueType, RightValueType, AddValue_> catEnumtor(*this);
	m_rightContainer.EnumInRect(mapRect, catEnumtor);

	return RetSuc;
}


template <class T>
template <class LeftValueType, class RightValueType, class AddValue_>
int RectContainer<T>::EnumtorForCat<LeftValueType, RightValueType, AddValue_>::OnPointValue(const RectI& rect, const LeftValueType* const pValueBitmap, const unsigned long& ulLineLength)
{
	RectI mapRect;
	m_rectMap.Map(rect, mapRect);

	m_curRectValue.enumType = RectEnumType_PointValue;
	m_curRectValue.pValue = pValueBitmap;
	m_curRectValue.rect = mapRect;
	m_curRectValue.ulLineLength = ulLineLength;

	CatEnumtorForCat<LeftValueType, RightValueType, AddValue_> catEnumtor(*this);
	m_rightContainer.EnumInRect(mapRect, catEnumtor);


	return RetSuc;
}



template <class T>
template <class LeftValueType, class RightValueType, class AddValue_>
RectContainer<T>::CatEnumtorForCat<LeftValueType, RightValueType, AddValue_>::CatEnumtorForCat(EnumtorForCat<LeftValueType, RightValueType, AddValue_>& enumtor)
	: m_enumtor(enumtor)
{
}

template <class T>
template <class LeftValueType, class RightValueType, class AddValue_>
int RectContainer<T>::CatEnumtorForCat<LeftValueType, RightValueType, AddValue_>::OnRectValue(const RectI& rect, const RightValueType& value)
{
	RectI resultRect;
	m_enumtor.m_rectMap.ReverseMap(rect,resultRect);
	if (m_enumtor.m_curRectValue.enumType == RectEnumType_RectValue)
	{
		unsigned long index;
		AddValue_ addValue;
		m_enumtor.m_resultContainer.AddRectValue(resultRect, addValue(*(m_enumtor.m_curRectValue.pValue), value), index);
	}
	else if (m_enumtor.m_curRectValue.enumType == RectEnumType_PointValue)
	{
		unsigned long index;
		AddValue_ addValue;

		m_enumtor.m_resultContainer.AddPointValue(resultRect, index);

		unsigned long resultLineLength;
		T* pResultBitmap = NULL;
		m_enumtor.m_resultContainer.GetPointValueBitmap(index, pResultBitmap, resultLineLength);
		unsigned char* iterResultLine = pResultBitmap;
		T* iterResultValue = NULL;

		const LeftValueType* pLeftBitmap = m_enumtor.m_curRectValue.pValue;
		unsigned long leftLineLength = m_enumtor.m_curRectValue.ulLineLength;
		const unsigned char* iterLeftLine = reinterpret_cast<const unsigned char*>(pLeftBitmap);
		const LeftValueType* iterLeftValue = NULL;
		for (int iy = 0; iy < resultRect.bottom - resultRect.top; ++iy)
		{
			iterResultValue = reinterpret_cast<T*>(iterResultLine);
			iterLeftValue = reinterpret_cast<const LeftValueType*>(iterLeftLine);

			for (int ix = 0; ix < resultRect.right - resultRect.left; ++ix)
			{
				*iterResultValue = addValue(*iterLeftValue, value);
				++iterResultValue;
				++iterLeftValue;
			}
			iterResultLine += resultLineLength;
			iterLeftLine += leftLineLength;
		}


	}
	return RetSuc;
}

template <class T>
template <class LeftValueType, class RightValueType, class AddValue_>
int RectContainer<T>::CatEnumtorForCat<LeftValueType, RightValueType, AddValue_>::OnPointValue(const RectI& rect, const RightValueType* const pValueBitmap, const unsigned long& ulLineLength)
{
	RectI resultRect;
	m_enumtor.m_rectMap.ReverseMap(rect,resultRect);
	unsigned long index;
	AddValue_ addValue;

	m_enumtor.m_resultContainer.AddPointValue(resultRect, index);

	unsigned long resultLineLength;
	T* pResultBitmap = NULL;
	m_enumtor.m_resultContainer.GetPointValueBitmap(index, pResultBitmap, resultLineLength);
	unsigned char* iterResultLine = pResultBitmap;
	T* iterResultValue = NULL;

	const RightValueType* pRightBitmap = pValueBitmap;
	unsigned long rightLineLength = ulLineLength;
	const unsigned char* iterRightLine = reinterpret_cast<const unsigned char*>(pRightBitmap);
	const RightValueType* iterRightValue = NULL;


	if (m_enumtor.m_curRectValue.enumType == RectEnumType_RectValue)
	{
		for (int iy = 0; iy < resultRect.bottom - resultRect.top; ++iy)
		{
			iterResultValue = reinterpret_cast<T*>(iterResultLine);
			iterRightValue = reinterpret_cast<const RightValueType*>(iterRightLine);

			for (int ix = 0; ix < resultRect.right - resultRect.left; ++ix)
			{
				*iterResultValue = addValue(*(m_enumtor.m_curRectValue.pValue), *iterRightValue);
				++iterResultValue;
				++iterRightValue;
			}
			iterResultLine += resultLineLength;
			iterRightLine += rightLineLength;
		}
	}
	else if (m_enumtor.m_curRectValue.enumType == RectEnumType_PointValue)
	{
		const LeftValueType* pLeftBitmap = m_enumtor.m_curRectValue.pValue;
		unsigned long leftLineLength = m_enumtor.m_curRectValue.ulLineLength;
		const unsigned char* iterLeftLine = reinterpret_cast<const unsigned char*>(pLeftBitmap);
		const LeftValueType* iterLeftValue = NULL;

		for (int iy = 0; iy < resultRect.bottom - resultRect.top; ++iy)
		{
			iterResultValue = reinterpret_cast<T*>(iterResultLine);
			iterLeftValue = reinterpret_cast<const LeftValueType*>(iterLeftLine);
			iterRightValue = reinterpret_cast<const RightValueType*>(iterRightLine);

			for (int ix = 0; ix < resultRect.right - resultRect.left; ++ix)
			{
				*iterResultValue = addValue(*iterLeftValue, *iterRightValue);
				++iterResultValue;
				++iterLeftValue;
				++iterRightValue;
			}
			iterResultLine += resultLineLength;
			iterLeftLine += leftLineLength;
			iterRightLine += rightLineLength;
		}
	}
	return RetSuc;
}


template <class T>
template <class LeftValueType, class RightValueType, class AddValue_>
int RectContainer<T>::Cat(RectContainer<LeftValueType>&leftContainer, RectContainer<RightValueType>& rightContainer
			   , const RectI&clouseRect, RectContainer& resultContainer, RectContainerRectMap& rectMap)
{
	resultContainer.Clean();
	EnumtorForCat<LeftValueType, RightValueType, AddValue_> enumtor(leftContainer, rightContainer, clouseRect, resultContainer, rectMap);
	leftContainer.EnumInRect(clouseRect, enumtor);

	return RetSuc;
}

template <class T>
template <class CatValueType, class AddValue_>
int RectContainer<T>::Cat(RectContainer<CatValueType>& catContainer, const RectI&clouseRect, RectContainerRectMap& rectMap)
{
	RectContainer tempContainer;
	int result = RectContainer::Cat<T, CatValueType, AddValue_>(*this, catContainer, clouseRect, tempContainer, rectMap);
	Clean();
	*this = tempContainer;
	return result;
}

template <class T>
int RectContainer<T>::EnumInRectEx(const RectI& rect, ValueEnumEx& enumtor)
{
	RectEnumInputQueue rectEnumInput;
	RectEnumStubQueue rectEnumOutput;
	rectEnumInput.push(rect);
	if (RetSuc == EnumInRectImpl(rectEnumInput, rectEnumOutput))
	{
		while (!rectEnumOutput.empty())
		{
			RectEnumStub& curStub = rectEnumOutput.front();
			if (curStub.enumType == RectEnumType_PointValue)
			{
				const T * const pValueBitmap = reinterpret_cast<const T*>(reinterpret_cast<unsigned char*>
					(curStub.pPointValue->valueBitmap + (curStub.rect.left - curStub.pPointValue->clouseRect.left)) 
					+ curStub.pPointValue->lineLength * (curStub.rect.top - curStub.pPointValue->clouseRect.top));
				enumtor.OnPointValue(curStub.rect, pValueBitmap, curStub.pPointValue->lineLength, curStub.pPointValue->index, curStub.pPointValue->clouseRect);
			}
			else if (curStub.enumType == RectEnumType_RectValue)
			{
				enumtor.OnRectValue(curStub.rect, curStub.pRectValue->value, curStub.pRectValue->index, curStub.pRectValue->rect);
			}
			else if (curStub.enumType == RectEnumType_DefaultValue)
			{
				enumtor.OnRectValue(curStub.rect, m_defaultValue, InvalidIndex, curStub.rect);
			}
			rectEnumOutput.pop();
		}
	}
	return RetSuc;
}


template <class T>
template <class AddValueType>
RectContainer<T>::InitAndAddValue<AddValueType>::InitAndAddValue(const T& initValue, AddValue<AddValueType>& addValue)
	: m_initValue(initValue)
	, m_addValue(addValue)
{

}


template <class T>
template <class AddValueType>
T& RectContainer<T>::InitAndAddValue<AddValueType>::operator()(T& leftValue, const AddValueType& rightValue)
{
	leftValue = m_initValue;
	return m_addValue(leftValue, rightValue);
}



template <class T>
template<class AddValueType>
int RectContainer<T>::AddValueOnPoint(T* leftBitmap, unsigned long leftColOffset, unsigned long leftLineOffset, const AddValueType* rightBitmap
				  , unsigned long rightColOffset, unsigned long rightLineOffset, AddValue<AddValueType>& addValue, const RectI& rect)
{
	
	const unsigned char* iterRightLine = reinterpret_cast<const unsigned char*>(rightBitmap);
	const AddValueType* iterRightValue = NULL;

	unsigned char* iterLeftLine = reinterpret_cast<unsigned char*>(leftBitmap);
	T* iterLeftValue = NULL;

	for (int iy = 0; iy < rect.bottom - rect.top; ++iy)
	{
		iterLeftValue = reinterpret_cast<T*>(iterLeftLine);
		iterRightValue = reinterpret_cast<const AddValueType*>(iterRightLine);

		for (int ix = 0; ix < rect.right - rect.left; ++ix)
		{
			*iterLeftValue = addValue(*(iterLeftValue), *iterRightValue);
			iterLeftValue += leftColOffset;
			iterRightValue += rightColOffset;
		}
		iterLeftLine += leftLineOffset;
		iterRightLine += rightLineOffset;
	}
	
	return RetSuc;
}

template <class T>
template<class AddValueType>
int RectContainer<T>::Add(RectContainer<AddValueType>& addContainer, const RectI&clouseRect
						  , RectContainerRectMap& rectMap, AddValue<AddValueType>& addValue)
{
	ValueEnumForAdd<AddValueType> enumtor(*this, addContainer, clouseRect, rectMap, addValue);
	EnumInRectEx(clouseRect, enumtor);
	
	ValueEnumForAdd<AddValueType>::DivideRectListMap::iterator iterMap = enumtor.m_divideRectListMap.begin();
	ValueEnumForAdd<AddValueType>::RightRectList::iterator iterList;
	while (iterMap != enumtor.m_divideRectListMap.end())
	{
		ValueEnumForAdd<AddValueType>::DivideRectStub& divideRectStub = iterMap->second;
		if (divideRectStub.leftEnumType == RectEnumType_RectValue)
		{
			if (divideRectStub.leftIndex != InvalidIndex)
			{
				std::list<RectI> outputRect;
				outputRect.push_back(divideRectStub.leftClouseRect);
				std::list<RectI>::iterator iterRectList;
				iterList = divideRectStub.rightRectList.begin();
				while (iterList != divideRectStub.rightRectList.end())
				{
					size_t rectLoopCount = outputRect.size();
					size_t rectLoopIndex = 0;
					iterRectList = outputRect.begin();
					while(rectLoopIndex < rectLoopCount)
					{
						RectI intersectRect;
						if (RectOperation::IntersectRect(intersectRect, *iterRectList, iterList->leftRect))
						{
							RectI firstRect = {iterRectList->left, iterRectList->top, iterRectList->right, intersectRect.top};
							RectI secondRect = {iterRectList->left, intersectRect.top, intersectRect.left, intersectRect.bottom};
							RectI thirdRect = {intersectRect.right, intersectRect.top, iterRectList->right, intersectRect.bottom};
							RectI fourthRect = {iterRectList->left, intersectRect.bottom, iterRectList->right, iterRectList->bottom};
							++iterRectList;
							outputRect.pop_front();
							if (!RectOperation::IsRectEmpty(firstRect))
							{
								outputRect.push_back(firstRect);
							}
							if (!RectOperation::IsRectEmpty(secondRect))
							{
								outputRect.push_back(secondRect);
							}
							if (!RectOperation::IsRectEmpty(thirdRect))
							{
								outputRect.push_back(thirdRect);
							}
							if (!RectOperation::IsRectEmpty(fourthRect))
							{
								outputRect.push_back(fourthRect);
							}
						}
						else
						{
							++iterRectList;
						}
						
						++rectLoopIndex;
					}
					++iterList;
				}
				EraseRect(divideRectStub.leftIndex);
				iterRectList = outputRect.begin();
				unsigned long index;
				while (iterRectList != outputRect.end())
				{
					AddRectValue(*iterRectList, divideRectStub.leftValue.leftRectValue, index);
					++iterRectList;
				}
			}

			iterList = divideRectStub.rightRectList.begin();
			while (iterList != divideRectStub.rightRectList.end())
			{
				if (iterList->rightEnumType == RectEnumType_RectValue)
				{
					unsigned long index;
					T leftValue = divideRectStub.leftValue.leftRectValue;
					AddRectValue(iterList->leftRect, addValue(leftValue, *(iterList->pRightValue)), index);
				}
				else if(iterList->rightEnumType == RectEnumType_PointValue)
				{
					unsigned long index;
					T* leftValueBitmap = NULL;
					unsigned long leftLineLength = 0;
					AddPointValue(iterList->leftRect, index, &leftValueBitmap, &leftLineLength);
					T leftValue = divideRectStub.leftValue.leftRectValue;
					////新加的点值还要初始化为之前的矩形值
					InitAndAddValue<AddValueType> initAndAddValue(leftValue, addValue);
					AddValueOnPoint(leftValueBitmap, 1, leftLineLength, iterList->pRightValue, 1
						, iterList->rightLineLength, initAndAddValue, iterList->leftRect);
				}	
				++iterList;
			}
		}
		else if(divideRectStub.leftEnumType == RectEnumType_PointValue)
		{
			iterList = divideRectStub.rightRectList.begin();
			while (iterList != divideRectStub.rightRectList.end())
			{
				T* pLeftValueBitmap = reinterpret_cast<T*>(reinterpret_cast<unsigned char*>(
								divideRectStub.leftValue.leftPointValue.pValueBitmap + (iterList->leftRect.left - divideRectStub.leftEnumRect.left))
								+ (iterList->leftRect.top - divideRectStub.leftEnumRect.top) * divideRectStub.leftValue.leftPointValue.lineLength);
				if (iterList->rightEnumType == RectEnumType_RectValue)
				{
					AddValueOnPoint(pLeftValueBitmap, 1, divideRectStub.leftValue.leftPointValue.lineLength
						, iterList->pRightValue, 0, 0, addValue, iterList->leftRect);
				}
				else if(iterList->rightEnumType == RectEnumType_PointValue)
				{
					AddValueOnPoint(pLeftValueBitmap, 1, divideRectStub.leftValue.leftPointValue.lineLength
						, iterList->pRightValue, 1, iterList->rightLineLength, addValue, iterList->leftRect);
				}	
				++iterList;
			}
		}

		++iterMap;
	}

	return RetSuc;
}


template <class T>
template <class AddValueType>
RectContainer<T>::ValueEnumForAdd<AddValueType>::ValueEnumForAdd(RectContainer&leftContainer
																 , RectContainer<AddValueType>& rightContainer
																 , const RectI& clouseRect, RectContainerRectMap& rectMap
																 , AddValue<AddValueType>& addValue)
	: m_leftContainer(leftContainer)
	, m_rightContainer(rightContainer)
	, m_clouseRect(clouseRect)
	, m_rectMap(rectMap)
	, m_addValue(addValue)
	, m_pCurDivideRectList(NULL)
{

}

template <class T>
template <class AddValueType>
int RectContainer<T>::ValueEnumForAdd<AddValueType>::OnRectValue(const RectI& rect
																 , const ValueType& value
																 , const unsigned long &index
																 , const RectI& clouseRect)
{
	RectI mapRect;
	m_rectMap.Map(rect, mapRect);


	DivideRectListMap::iterator iterMap = m_divideRectListMap.find(index);
	if (iterMap == m_divideRectListMap.end())
	{
		DivideRectStub newRectStub;

		newRectStub.leftIndex = index;
		newRectStub.leftEnumType = RectEnumType_RectValue;
		newRectStub.leftValue.leftRectValue = value;
		newRectStub.leftClouseRect = clouseRect;
		newRectStub.leftEnumRect = rect;

		iterMap = (m_divideRectListMap.insert(std::pair<DivideRectListMap::key_type, DivideRectListMap::mapped_type>(index, newRectStub))).first;

	}
	m_pCurDivideRectList = &(iterMap->second.rightRectList);

	
	RightValueEnumForAdd<AddValueType> rightEnumtor(*this);
	m_rightContainer.EnumInRectEx(mapRect, rightEnumtor);
	return RetSuc;
}

template <class T>
template <class AddValueType>
int RectContainer<T>::ValueEnumForAdd<AddValueType>::OnPointValue(const RectI& rect
																  , const ValueType* const pValueBitmap
																  , const unsigned long& ulLineLength
																  , const unsigned long &index
																  , const RectI& clouseRect)
{


	RectI mapRect;
	m_rectMap.Map(rect, mapRect);


	DivideRectListMap::iterator iterMap = m_divideRectListMap.find(index);
	if (iterMap == m_divideRectListMap.end())
	{
		DivideRectStub newRectStub;

		newRectStub.leftIndex = index;
		newRectStub.leftEnumType = RectEnumType_PointValue;
		newRectStub.leftValue.leftPointValue.pValueBitmap = const_cast<ValueType*>(pValueBitmap);
		newRectStub.leftValue.leftPointValue.lineLength = ulLineLength;
		newRectStub.leftClouseRect = clouseRect;
		newRectStub.leftEnumRect = rect;

		iterMap = (m_divideRectListMap.insert(std::pair<DivideRectListMap::key_type, DivideRectListMap::mapped_type>(index, newRectStub))).first;

	}
	m_pCurDivideRectList = &(iterMap->second.rightRectList);


	RightValueEnumForAdd<AddValueType> rightEnumtor(*this);
	m_rightContainer.EnumInRectEx(mapRect, rightEnumtor);

	return RetSuc;
}


template <class T>
template<class AddValueType>
RectContainer<T>::RightValueEnumForAdd<AddValueType>::RightValueEnumForAdd(ValueEnumForAdd<AddValueType>& enumtor)
	: m_enumtor(enumtor)
{
}


template <class T>
template<class AddValueType>
int RectContainer<T>::RightValueEnumForAdd<AddValueType>::OnRectValue(const RectI& rect
																	  , const AddValueType& value
																	  , const unsigned long &index
																	  , const RectI& clouseRect)
{
	((void)index);
	((void)clouseRect);
	ValueEnumForAdd<AddValueType>::RightRectStub newRightRectStub;
	m_enumtor.m_rectMap.ReverseMap(rect, newRightRectStub.leftRect);
	newRightRectStub.rightEnumType = RectEnumType_RectValue;
	newRightRectStub.pRightValue = &value;
	newRightRectStub.rightLineLength = 0;

	m_enumtor.m_pCurDivideRectList->push_back(newRightRectStub);
	return RetSuc;
}


template <class T>
template<class AddValueType>
int RectContainer<T>::RightValueEnumForAdd<AddValueType>::OnPointValue(const RectI& rect
																	   , const AddValueType* const pValueBitmap
																	   , const unsigned long& ulLineLength
																	   , const unsigned long &index
																	   , const RectI& clouseRect)
{
	((void)clouseRect);
	((void)index);
	ValueEnumForAdd<AddValueType>::RightRectStub newRightRectStub;
	m_enumtor.m_rectMap.ReverseMap(rect, newRightRectStub.leftRect);
	newRightRectStub.rightEnumType = RectEnumType_PointValue;
	newRightRectStub.pRightValue = pValueBitmap;
	newRightRectStub.rightLineLength = ulLineLength;

	m_enumtor.m_pCurDivideRectList->push_back(newRightRectStub);
	return RetSuc;
}

template <class T>
int RectContainer<T>::Copy(RectContainer& rightContainer)
{
	Clean();
	m_defaultValue = rightContainer.m_defaultValue;
	m_ulNextIndex = rightContainer.m_ulNextIndex;
	RectValueList::iterator iterRect = rightContainer.m_rectValueList.begin();
	while (iterRect != rightContainer.m_rectValueList.end())
	{
		m_rectValueList.push_back(*iterRect);
		++iterRect;
	}

	PointValueList::iterator iterPoint = rightContainer.m_pointValuelist.begin();
	while (iterPoint != rightContainer.m_pointValuelist.end())
	{
		PointValueStub newStub = *iterPoint;
		if (newStub.handled)
		{
			int valueBitmapHeight = newStub.clouseRect.bottom - newStub.clouseRect.top;

			unsigned long valueBitmapSize = valueBitmapHeight * newStub.lineLength;
			unsigned char* valueBitmap = new unsigned char[valueBitmapSize];
			memcpy(valueBitmap, newStub.valueBitmap, valueBitmapSize);
			newStub.valueBitmap = valueBitmap;

			
		}
		m_pointValuelist.push_back(newStub);
		++iterPoint;
	}

	return RetSuc;
}

}

#pragma warning(pop)