#include "stdafx.h"
#include "./StepExcuter.h"


namespace IlluminationImpl
{

DynamicLayerSession::DynamicLayerSession()
{
}

DynamicLayerSession::~DynamicLayerSession()
{
	CleanLayer();
}

void DynamicLayerSession::CleanLayer()
{
	LayerList& layerList = const_cast<LayerList&>(GetLayerList());
	LayerList::iterator iterLayer = layerList.begin();
	while (iterLayer != layerList.end())
	{
		delete *iterLayer;
		++iterLayer;
	}
	layerList.clear();
}

void DynamicLayerSession::SetClouseRect(const RectI& rect)
{
	m_clouseRect = rect;
}

IlluminationLayer& DynamicLayerSession::GetLayerOnHeight(int height, bool& isCreated, IlluminationLayer** ppUperLayer)
{
	assert(ppUperLayer != NULL);
	

	LayerList& layerList = const_cast<LayerList&>(GetLayerList());
	if (layerList.empty())
	{
		isCreated = true;
		*ppUperLayer = NULL;
	}
	else
	{
		IlluminationLayer* lowmostLayer = layerList.back();
		if (lowmostLayer->GetDirection().start2direction.start.z == height)
		{
			isCreated = false;
			assert(lowmostLayer != NULL);
			LayerList::reverse_iterator iterUperLayer = layerList.rbegin()++;
			if (iterUperLayer != layerList.rend())
			{
				*ppUperLayer = (*iterUperLayer);
			}
			else
			{
				*ppUperLayer = NULL;
			}
			return *lowmostLayer;
		}
		else
		{
			*ppUperLayer = layerList.back();
			isCreated = true;
		}
	}
	
	IlluminationLayer* newLayer = new IlluminationLayer;
	newLayer->SetIsUseAlphaAsProp(true);
	newLayer->SetClouseRect(m_clouseRect);
	Vector3I direction;
	direction.start2direction.start.x = 0;
	direction.start2direction.start.y = 0;
	direction.start2direction.start.z = height;
	direction.start2direction.direction.x = 0;
	direction.start2direction.direction.y = 0;
	direction.start2direction.direction.z = 1;
	newLayer->SetDirection(direction);
	layerList.push_back(newLayer);

	return *newLayer;
}


class LayerEqualHeight
{
public:
	LayerEqualHeight(int height)
		: m_height(height)
	{

	}
	bool operator()(const IlluminationLayer* pLayer)
	{
		return pLayer->GetDirection().start2direction.start.z == m_height;
	}
private:
	int m_height;
};

IlluminationLayer* DynamicLayerSession::GetLayerOnHeight(int height)
{
	const LayerList& layerList = GetLayerList();
	LayerEqualHeight layerEqualHeight(height);
	LayerList::const_iterator iterLayer = std::find_if<LayerList::const_iterator,LayerEqualHeight>(layerList.begin(), layerList.end(), layerEqualHeight);
	if (iterLayer != layerList.end())
	{
		return *iterLayer;
	}
	return NULL;
}
	

StepExcuter::StepExcuter()
	: m_lastExcuteHeight(0x7fffffff)
{
}

StepExcuter::~StepExcuter()
{

}




int StepExcuter::ExcuteOnHeight(int height, XL_BITMAP_HANDLE hTexture, const RectI& rect)
{
	if (height > m_lastExcuteHeight)
	{
		dynamic_cast<DynamicLayerSession*>(m_pSession)->CleanLayer();
	}
	m_lastExcuteHeight = height;
	bool isLayerCreated = false;
	IlluminationLayer* pPreLayer = NULL;
	IlluminationLayer& layer = dynamic_cast<DynamicLayerSession*>(m_pSession)->GetLayerOnHeight(height, isLayerCreated, &pPreLayer);
	if (pPreLayer == NULL && isLayerCreated)
	{
		Reset();
	}
	unsigned long index;
	layer.GetTextureContainer().AddRectValue(rect, hTexture, index);
	if (pPreLayer != NULL && isLayerCreated)
	{
		Swap(layer, pPreLayer);
	}
	ExcuteOnLayer(layer, pPreLayer, rect);
	//SaveOutput(rect);
	return IlluminationSession::RetSuc;
}

int StepExcuter::BlendOnHeight(XL_BITMAP_HANDLE hCanvas, int height, const RectI&rect)
{
	IlluminationLayer* pLayer = dynamic_cast<DynamicLayerSession*>(m_pSession)->GetLayerOnHeight(height);
	assert(pLayer != NULL);
	if (pLayer)
	{
		return GetBlender()->BlendLayer(*pLayer, rect, hCanvas);
	}
	return IlluminationSession::RetSuc;
}


int StepExcuter::CompositeShadowOnHeight(int height, const RectI&rect)
{
	IlluminationLayer* pLayer = dynamic_cast<DynamicLayerSession*>(m_pSession)->GetLayerOnHeight(height);
	assert(pLayer != NULL);
	if (pLayer)
	{
		return GetBlender()->CompositeLayerWithShadow(*pLayer, rect);
	}
	return IlluminationSession::RetSuc;
}
}


//#include "./IlluminationImplTest.h"
//namespace IlluminationImpl
//{
//
//
////////for debug
//void StepExcuter::SaveOutput(const RectI& rect)
//{
//	//bool isLayerCreated = false;
//	///IlluminationLayer* pPreLayer = NULL;
//	//IlluminationLayer& layer = dynamic_cast<DynamicLayerSession*>(m_pSession)->GetLayerOnHeight(m_lastExcuteHeight, isLayerCreated, &pPreLayer);
//
//	const IlluminantExcuterList excuterList = GetIlluminantExcuterList();
//	IlluminantExcuterList::const_iterator excuterIter = excuterList.begin();
//
//	while (excuterIter != excuterList.end())
//	{
//		XL_BITMAP_HANDLE hOutput = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, rect.right, rect.bottom);
//		XL_FillBitmap(hOutput, XLCOLOR_BGRA(0, 0, 0, 0));
//		(dynamic_cast<IlluminantAllPlaneExcuter*>(*excuterIter))->SaveOutput(hOutput, rect);
//		wchar_t szName[100];
//		wsprintf(szName, L"%d output", reinterpret_cast<long>(*excuterIter));
//		TestGraphic::SaveBitmap(hOutput, szName);
//		++ excuterIter;
//	}
//}
//}

