#include "stdafx.h"
#include "RayContainer.h"


namespace IlluminationImpl
{

template<class T>
ContainerRectImpl<T>::~ContainerRectImpl()
{
	Clean();
}

template<class T>
int ContainerRectImpl<T>::EnumOnPos(IContainerEnumtor<T>& refEnumtor) 
{
	RayList::iterator listIter = m_list.begin();
	while(listIter != m_list.end())
	{
		for(int iy = listIter->rect.top; iy < listIter->rect.bottom; ++ iy)
		{
			for(int ix = listIter->rect.left; ix < listIter->rect.right; ++ ix)
			{
				Point2I ptPos = {ix, iy};
				if(IContainer::RetStopEnum == refEnumtor.OnPos(ptPos,*(listIter->ray)))
				{
					return IContainer::RetStopEnum;
				}
			}
		}

		++ listIter;
	}
	return IContainer::RetSuc;
}

template<class T>
int ContainerRectImpl<T>::EnumInRect(IContainerEnumtor<T>& refEnumtor) 
{
	RayList::iterator listIter = m_list.begin();
	while(listIter != m_list.end())
	{
		if(IContainer::RetStopEnum == refEnumtor.InRect(listIter->rect,*(listIter->ray)))
		{
			return IContainer::RetStopEnum;
		}

		++ listIter;
	}
	return IContainer::RetSuc;
}

template<class T>
int ContainerRectImpl<T>::AddOnPos(const Point2I& ptPos, T& ray) 
{
	RayStub newStub = {&ray};
	newStub.rect.left = ptPos.x, newStub.rect.top = ptPos.y, 
		newStub.rect.right = ptPos.x + 1, newStub.rect.bottom = ptPos.y + 1;
	ray.AddRef();

	m_list.push_back(newStub);
	return IContainer::RetSuc; 
}

template<class T>
int ContainerRectImpl<T>::AddInRect(const RectI& rect, T& ray) 
{
	RayStub newStub = {&ray, rect};
	ray.AddRef();

	m_list.push_back(newStub);
	return IContainer::RetSuc;
}

template<class T>
int ContainerRectImpl<T>::Clean() 
{
	RayList::iterator listIter = m_list.begin();
	while(listIter != m_list.end())
	{
		/*listIter->ray->Release();*/
	}
	m_list.clear();

	return IContainer::RetSuc;
}




template<class T>
ContainerPointImpl<T>::ContainerPointImpl()
:m_curClouseRect(NULL)
{
}

template<class T>
ContainerPointImpl<T>::~ContainerPointImpl()
{
	Clean();
}

template<class T>
int ContainerPointImpl<T>::EnumOnPos(IContainerEnumtor<T>& refEnumtor) 
{
	ClouseRectList::iterator listIter = m_list.begin();
	while(listIter != m_list.end())
	{
		for(int iy = listIter->rect.top; iy < listIter->rect.bottom; ++ iy)
		{
			for(int ix = listIter->rect.left; ix < listIter->rect.right; ++ ix)
			{
				Point2I ptPos = {ix, iy};
				if(IContainer::RetStopEnum == refEnumtor.OnPos(ptPos,*GetOnPosFromClouse(ptPos, *listIter)))
				{
					return IContainer::RetStopEnum;
				}
			}
		}
		++ listIter;
	}
	return IContainer::RetSuc;
}	

template<class T>
int ContainerPointImpl<T>::EnumInRect(IContainerEnumtor<T>& refEnumtor) 
{
	ClouseRectList::iterator listIter = m_list.begin();
	while(listIter != m_list.end())
	{
		for(int iy = listIter->rect.top; iy < listIter->rect.bottom; ++ iy)
		{
			for(int ix = listIter->rect.left; ix < listIter->rect.right; ++ ix)
			{
				Point2I ptPos = {ix, iy};
				RectI rect = {ix, iy, ix + 1, iy + 1};
				if(IContainer::RetStopEnum == refEnumtor.InRect(rect,*GetOnPosFromClouse(ptPos, *listIter)))
				{
					return IContainer::RetStopEnum;
				}
			}
		}
		++ listIter;
	}
	return IContainer::RetSuc;
}

template<class T>
int ContainerPointImpl<T>::AddClouseRect(const RectI& rect)
{
	m_curClouseRect = NULL;
	int nBitmapWidth = rect.right - rect.left ;
	int nBitmapHeight = rect.bottom - rect.top;
	if(nBitmapHeight <= 0 || nBitmapWidth <= 0)
	{
		return IContainer::RetInvalidParam;
	}
	unsigned long nBitmapSize = nBitmapWidth * nBitmapHeight ;
	ClouseRectStub newStub = {rect, NULL, nBitmapWidth};
	newStub.bitmap = new T*[nBitmapSize];
	if(newStub.bitmap == NULL)
	{
		return IContainer::RetOutofMemory;
	}
	ZeroMemory(newStub.rayBitmap, bitmapSize * sizeof(T*));
	m_list.push_back(newStub);
	m_curClouseRect = &(m_list.back());
	return IContainer::RetSuc;
}

template<class T>
int ContainerPointImpl<T>::AddOnPos(const Point2I& ptPos, T& ray) 
{
	assert(m_curClouseRect != NULL);
	assert(m_curClouseRect->rayBitmap != NULL);
	assert(ptPos.x >= m_curClouseRect->rect.left && ptPos.x < m_curClouseRect->rect.right 
		&& ptPos.y >= m_curClouseRect->rect.top && ptPos.y < m_curClouseRect->rect.bottom);

	GetOnPosFromClouse(ptPos, *m_curClouseRect) = &ray;
	//ray.AddRef();
	return IContainer::RetSuc;
}

template<class T>
int ContainerPointImpl<T>::AddInRect(const RectI& rect, T& ray) 
{
	int nRet = IContainer::RetSuc;
	for(int iy = rect.top; iy < rect.bottom; ++ iy)
	{
		for(int ix = rect.left; ix < rect.right; ++ ix)
		{
			Point2I ptPos = {ix, iy};
			if(IContainer::RetSuc != (nRet = AddOnPos(ptPos, ray)))
			{
				return nRet;
			}
		}
	}
	return nRet;
}

template<class T>
int ContainerPointImpl<T>::Clean() 
{
	ClouseRectList::iterator listIter = m_list.begin();
	while(listIter != m_list.end())
	{
		unsigned long nBitmapSize = listIter->bitmapWidth * (listIter->rect.bottom - listIter->rect.top);
		for(unsigned long ix = 0; ix < nBitmapSize; ++ ix)
		{
			//((listIter->bitmap)[ix])->Release();
		}
		delete []listIter->bitmap;
		++ listIter;
	}
	m_list.clear();
	return IContainer::RetSuc;
}


template<class T>
T*& ContainerPointImpl<T>::GetOnPosFromClouse(const Point2I& ptPos, ClouseRectStub& clouseRect)
{
	unsigned int uIndex = (ptPos.x - clouseRect.rect.left) + (ptPos.y - clouseRect.rect.top) * clouseRect.bitmapWidth;
	return clouseRect.bitmap[uIndex];
}

	
template<class T>
ContainerCompositeImpl<T>::~ContainerCompositeImpl()
{

}

template<class T>
int ContainerCompositeImpl<T>::EnumOnPos(IContainerEnumtor<T>& refEnumtor) 
{
	int nRet = m_pointContainer.EnumOnPos(refEnumtor);
	if(nRet != IContainer::RetStopEnum)
	{
		return m_rectContainer.EnumOnPos(refEnumtor);
	}
	return nRet;
}

template<class T>
int ContainerCompositeImpl<T>::EnumInRect(IContainerEnumtor<T>& refEnumtor) 
{
	int nRet = m_rectContainer.EnumOnPos(refEnumtor);
	if(nRet != IContainer::RetStopEnum)
	{
		return m_pointContainer.EnumOnPos(refEnumtor);
	}
	return nRet;
}

template<class T>
int ContainerCompositeImpl<T>::AddClouseRect(const RectI& rect)
{
	return m_pointContainer.AddClouseRect(rect);
}

template<class T>
int ContainerCompositeImpl<T>::AddOnPos(const Point2I& ptPos, T& ray) 
{
	return m_pointContainer.AddOnPos(ptPos, ray);
}

template<class T>
int ContainerCompositeImpl<T>::AddInRect(const RectI& rect, T& ray) 
{
	return m_rectContainer.AddInRect(rect, ray);
}

template<class T>
int ContainerCompositeImpl<T>::Clean() 
{
	m_pointContainer.Clean();
	return m_rectContainer.Clean();
}




}

