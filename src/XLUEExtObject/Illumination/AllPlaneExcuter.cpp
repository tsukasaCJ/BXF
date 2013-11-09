#include "stdafx.h"
#include <XLMath.h>

#include "./AllPlaneExcuter.h"


#pragma warning(push)
#pragma warning(disable: 4127)

namespace IlluminationImpl
{
	
	

	AllPlaneExcuter::AllPlaneExcuter()
		: m_pBlender(NULL)
	{
	}

	AllPlaneExcuter::~AllPlaneExcuter()
	{
		if(m_pBlender != NULL)
		{
			delete m_pBlender;
			m_pBlender = NULL;
		}
	}

	bool AllPlaneExcuter::VerifyPlane(const IlluminationLayer& refLayer)
	{
		((void)refLayer);
		return true;
	}


	class IlluminationLayerGreater
	{
	public:
		bool operator()(IlluminationLayer*&leftLayer, IlluminationLayer*&rightLayer)
		{
			return leftLayer->GetDirection().start2direction.start.z > rightLayer->GetDirection().start2direction.start.z;
		}
	};


	IlluminantExcuter* AllPlaneExcuter::CreateIlluminantExcuter(const Illuminant& refIlluminant)
	{
		if (refIlluminant.GetType() == EnvIlluminant::IlluminantType)
		{
			return new EnvIlluminantAllPlaneExcuter();
		}
		return NULL;
	}


	int AllPlaneExcuter::PrepareSession()
	{
		IlluminationExcuter::PrepareSession();
		assert(m_sortedLayerList.size() == 0);
		const IlluminationSession::LayerList& layerList = m_pSession->GetLayerList();
		m_sortedLayerList = layerList;  
		return OnUpdateLayerList();
	}

	void AllPlaneExcuter::CleanSession()
	{
		m_sortedLayerList.clear();
		IlluminationExcuter::CleanSession();
	}


	int AllPlaneExcuter::OnAddIlluminationLayer()
	{
		
		const IlluminationSession::LayerList& layerList = m_pSession->GetLayerList();
		m_sortedLayerList.push_back(layerList.back());
		
		return OnUpdateLayerList();
	}

	int AllPlaneExcuter::OnUpdateLayerList()
	{
		////todo： 这里要判断是不是都是平行的平面
		IlluminationLayerGreater sortOpt;
		m_sortedLayerList.sort(sortOpt);
		IlluminantExcuterList::iterator iterExcuter = m_listIlluminantExcuters.begin();
		while (iterExcuter != m_listIlluminantExcuters.end())
		{
			(dynamic_cast<IlluminantAllPlaneExcuter*>(*iterExcuter))->OnUpdateLayerList();
			++iterExcuter;
		}
		(dynamic_cast<AllPlaneBlender*>(GetBlender()))->OnUpdateLayerList();
		return IlluminationSession::RetSuc;
	}

	int AllPlaneExcuter::ExcuteOnLayer(IlluminationLayer& layer, IlluminationLayer* pPreLayer, const RectI& rect)
	{
		const IlluminantExcuterList excuterList = GetIlluminantExcuterList();
		IlluminantExcuterList::const_iterator excuterIter = excuterList.begin();

		while (excuterIter != excuterList.end())
		{
			(dynamic_cast<IlluminantAllPlaneExcuter*>(*excuterIter))->ExcuteOnLayer(layer, pPreLayer, rect);
			++ excuterIter;
		}

		return IlluminationSession::RetSuc;
	}

	void AllPlaneExcuter::Reset()
	{
		const IlluminantExcuterList excuterList = GetIlluminantExcuterList();
		IlluminantExcuterList::const_iterator excuterIter = excuterList.begin();

		while (excuterIter != excuterList.end())
		{
			(dynamic_cast<IlluminantAllPlaneExcuter*>(*excuterIter))->Reset();
			++ excuterIter;
		}
	}
	void AllPlaneExcuter::Swap(IlluminationLayer& layer, IlluminationLayer* pPreLayer)
	{
		const IlluminantExcuterList excuterList = GetIlluminantExcuterList();
		IlluminantExcuterList::const_iterator excuterIter = excuterList.begin();

		while (excuterIter != excuterList.end())
		{
			(dynamic_cast<IlluminantAllPlaneExcuter*>(*excuterIter))->Swap(layer, pPreLayer);
			++ excuterIter;
		}
	}

	int AllPlaneExcuter::Excute()
	{
		const IlluminantExcuterList excuterList = GetIlluminantExcuterList();
		IlluminantExcuterList::const_iterator excuterIter = excuterList.begin();
		
		IlluminationSession::LayerList::iterator layerIter = m_sortedLayerList.begin();
		while (layerIter != m_sortedLayerList.end())
		{
			((*layerIter)->GetRayIntensityContainer()).Clean();
			++layerIter;
		}
		
		while (excuterIter != excuterList.end())
		{
			(*excuterIter)->Excute();
			++ excuterIter;
		}
		
		return IlluminationSession::RetSuc;
	}

	IlluminationBlender* AllPlaneExcuter::GetBlender()
	{
		if(m_pBlender == NULL)
		{
			m_pBlender = new AllPlaneBlender();
			m_pBlender->SetExcuter(*this);
		}
		return m_pBlender;
	}

	const IlluminationSession::LayerList& AllPlaneExcuter::GetSortedLayerList()
	{
		return m_sortedLayerList;
	}


	IlluminantAllPlaneExcuter::IlluminantAllPlaneExcuter()
	{
		
	}

	IlluminantAllPlaneExcuter::~IlluminantAllPlaneExcuter()
	{
		
	}

	void IlluminantAllPlaneExcuter::Reset()
	{

	}

	int IlluminantAllPlaneExcuter::ExcuteOnLayer(IlluminationLayer& layer, IlluminationLayer* pPreLayer, const RectI& rect)
	{
		((void)layer);
		((void)pPreLayer);
		((void)rect);
		return IlluminationSession::RetSuc;
	}

	



	int IlluminantAllPlaneExcuter::Excute()
	{
		const IlluminationSession::LayerList sortedLayerList = dynamic_cast<AllPlaneExcuter&>(GetSessionExcuter()).GetSortedLayerList();
		IlluminationSession::LayerList::const_iterator layerIter = sortedLayerList.begin();
		IlluminationLayer* pLayer = NULL;
		IlluminationLayer* pPreLayer = &m_illuminantLayer;

		while (true)
		{
			pLayer = *layerIter;

			Swap(*pLayer, pPreLayer);

			const RectI& layerRect = pLayer->GetClouseRect();

			ExcuteOnLayer(*pLayer, pPreLayer, layerRect);

			++layerIter;

			if (layerIter == sortedLayerList.end())
			{ 
				break;
			}
			
			pPreLayer = pLayer;
		}

		Reset();
		return IlluminationSession::RetSuc;
	}


	int IlluminantAllPlaneExcuter::SetIlluminant(const Illuminant& refIllunminant)
	{
		IlluminantExcuter::SetIlluminant(refIllunminant);
		OnUpdateLayerList();
		return IlluminationSession::RetSuc;
	}


	int IlluminantAllPlaneExcuter::OnUpdateLayerList()
	{
		return IlluminationSession::RetSuc;
	}

	void IlluminantAllPlaneExcuter::Swap(IlluminationLayer& layer, IlluminationLayer* pPreLayer)
	{
		((void)layer);
		((void)pPreLayer);
	}

	


	EnvIlluminantAllPlaneExcuter::EnvIlluminantAllPlaneExcuter()
		: m_deltaY(0)
		, m_deltaX(0)
	{
		Vector3I direction;
		direction.start2direction.start.x = 0;
		direction.start2direction.start.y = 0;
		//////这里先设置一个应该不会超出的值
		direction.start2direction.start.z = 100000000;
		direction.start2direction.direction.x = 0;
		direction.start2direction.direction.y = 0;
		direction.start2direction.direction.z = -1;
		m_illuminantLayer.SetDirection(direction);

		//Reset();
	}

	EnvIlluminantAllPlaneExcuter::~EnvIlluminantAllPlaneExcuter()
	{
		Reset();
	}

	int EnvIlluminantAllPlaneExcuter::SetIlluminant(const Illuminant& refIllunminant)
	{
		IlluminantAllPlaneExcuter::SetIlluminant(refIllunminant);
		const Direction3I&illuminantDirection = (dynamic_cast<const EnvIlluminant*>(m_pIlluminant))->GetDirection();
		////todo:这里也要应用光源与平面的最小角度来规避溢出问题， 先忽略
		m_deltaX = static_cast<double>(illuminantDirection.x) / illuminantDirection.z;
		m_deltaY = static_cast<double>(illuminantDirection.y) / illuminantDirection.z;
		Reset();
		return IlluminationSession::RetSuc;
	}

	
	
	EnvIlluminantAllPlaneExcuter::MapLayerRect::MapLayerRect(int deltaX, int deltaY)
		: m_deltaX(deltaX)
		, m_deltaY(deltaY)
	{

	}

	RectI& EnvIlluminantAllPlaneExcuter::MapLayerRect::Map(const RectI& thisRect, RectI& mapRect)
	{
		mapRect = thisRect;
		RectOperation::OffsetRect(mapRect, m_deltaX, m_deltaY);
		return mapRect;
	}

	RectI& EnvIlluminantAllPlaneExcuter::MapLayerRect::ReverseMap(const RectI& mapRect, RectI& thisRect)
	{
		thisRect = mapRect;
		RectOperation::OffsetRect(thisRect, -m_deltaX, -m_deltaY);
		return thisRect;
	}

	RayIntensity& EnvIlluminantAllPlaneExcuter::SetIntensityWithProp::operator()(RayIntensity& intensity, const IlluminatProp& prop)
	{
		RayIntensityOperator::GetRayIntensity(intensity, prop, intensity);
		return intensity;
	}

	RayIntensity& EnvIlluminantAllPlaneExcuter::SetIntensityWithAlpha::operator()(RayIntensity& intensity, const XL_Color& color)
	{
		IlluminatProp prop;
		prop.byTrans = *(reinterpret_cast<const unsigned char*>(&color) + 3);
		RayIntensityOperator::GetRayIntensity(intensity, prop, intensity);
		return intensity;
	}
	

	EnvIlluminantAllPlaneExcuter::AssignIntensity::AssignIntensity(XL_Color color)
		: m_color(color)
	{
		
	}

	ChanelIntensity& EnvIlluminantAllPlaneExcuter::AssignIntensity::operator()(ChanelIntensity& leftIntensity, const RayIntensity& rightIntensity)
	{
		unsigned char* pChanel = reinterpret_cast<unsigned char*>(&leftIntensity);
		const unsigned char* pColorChanel = reinterpret_cast<const unsigned char*>(&m_color);

		int temp = static_cast<int>(pColorChanel[0]) * rightIntensity / 255 + pChanel[0];
		if (temp > 255)
		{
			temp = 255;
		}
		pChanel[0] = static_cast<unsigned char>(temp);
		temp = static_cast<int>(pColorChanel[1]) * rightIntensity / 255 + pChanel[1];
		if (temp > 255)
		{
			temp = 255;
		}
		pChanel[1] = static_cast<unsigned char>(temp);
		temp = static_cast<int>(pColorChanel[2]) * rightIntensity / 255 + pChanel[2];
		if (temp > 255)
		{
			temp = 255;
		}
		pChanel[2] = static_cast<unsigned char>(temp);
		return leftIntensity;
	}
	

	
	int EnvIlluminantAllPlaneExcuter::OnUpdateLayerList()
	{
		if (m_pIlluminant == NULL)
		{
			return IlluminationSession::RetSuc;
		}

		const IlluminationSession::LayerList&sortedLayerList = (dynamic_cast<AllPlaneExcuter*>(m_pSessionExcuter))->GetSortedLayerList();
		if (sortedLayerList.empty())
		{
			return IlluminationSession::RetSuc;
		}

		//IlluminationLayer& topmostLayer = *(sortedLayerList.front());
		////todo:这里也要应用光源与平面的最小角度来规避溢出问题， 先忽略
		
		
		return IlluminationSession::RetSuc;
	}


	EnvIlluminantAllPlaneExcuter::TextureContainerEnumtor::TextureContainerEnumtor(ChanelIntensityContainer& intensityContainer
		, RayIntensityContainer& layerInputIntensityContainer, MapLayerRect& rectMap, AssignIntensity& addValue)
		: m_intensityContainer(intensityContainer)
		, m_layerInputIntensityContainer(layerInputIntensityContainer)
		, m_rectMap(rectMap)
		, m_addValue(addValue)
	{
	}


	int EnvIlluminantAllPlaneExcuter::TextureContainerEnumtor::OnRectValue(const RectI& rect, const TextureContainer::ValueType& value, const unsigned long &index, const RectI& clouseRect) 
	{
		((void)index);
		((void)clouseRect);

		if(value != NULL)
		{
			m_intensityContainer.Add<RayIntensity>(m_layerInputIntensityContainer, rect, m_rectMap, m_addValue);
		}
		return TextureContainer::RetSuc;
	}

	int EnvIlluminantAllPlaneExcuter::TextureContainerEnumtor::OnPointValue(const RectI& rect, const TextureContainer::ValueType* const pValueBitmap
		, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect) 
	{
		assert(0);
		((void)rect);
		((void)pValueBitmap);
		((void)ulLineLength);
		((void)index);
		((void)clouseRect);
		
		return TextureContainer::RetSuc;
	}

	void EnvIlluminantAllPlaneExcuter::Swap(IlluminationLayer& layer, IlluminationLayer* pPreLayer)
	{
		if (pPreLayer == NULL)
		{
			pPreLayer = &m_illuminantLayer;
		}
		int deltaZ = pPreLayer->GetDirection().start2direction.start.z - layer.GetDirection().start2direction.start.z; 
		int deltaX = static_cast<int>(m_deltaX * deltaZ);
		int deltaY = static_cast<int>(m_deltaY * deltaZ);

		m_inputLayerIntensityContainer.Copy(m_outputLayerIntensityContainer);
		Point2I ptOffset = {-deltaX, -deltaY};
		m_outputLayerIntensityContainer.Offset(ptOffset);
	}

	int EnvIlluminantAllPlaneExcuter::ExcuteOnLayer(IlluminationLayer& layer, IlluminationLayer* pPreLayer, const RectI& rect)
	{
		if (pPreLayer == NULL)
		{
			pPreLayer = &m_illuminantLayer;
		}

		ChanelIntensityContainer& layerRayIntensityContainer = layer.GetRayIntensityContainer();

		const RectI& layerRect = rect;

		int deltaZ = pPreLayer->GetDirection().start2direction.start.z - layer.GetDirection().start2direction.start.z; 
		int deltaX = static_cast<int>(m_deltaX * deltaZ);
		int deltaY = static_cast<int>(m_deltaY * deltaZ);

		IlluminatPropContainer& layerPropContainer = layer.GetPropContainer();

		MapLayerRect rectMap(deltaX, deltaY);

		
		AssignIntensity assignIntensity(m_pIlluminant->GetColor());
		TextureContainerEnumtor textureEnumtor(layerRayIntensityContainer, m_inputLayerIntensityContainer, rectMap, assignIntensity);
		/////因为只关心有纹理部分的结果， 所以这里没必要计算整个clouseRect
		layer.GetTextureContainer().EnumInRectEx(layerRect, textureEnumtor);

		
		RectContainerRectMapEqual rectMapEqual;
		if (layer.GetIsUseAlphaAsProp())
		{
			SetIntensityWithAlpha setIntensityWithAlpha;
			m_outputLayerIntensityContainer.Add<XL_Color>(layer.GetTextureBitmapContainer(), layerRect, rectMapEqual, setIntensityWithAlpha);
		}
		else
		{
			SetIntensityWithProp setIntensityWithProp;
			m_outputLayerIntensityContainer.Add<IlluminatProp>(layerPropContainer, layerRect, rectMapEqual, setIntensityWithProp);
		}

		return IlluminationSession::RetSuc;
	}

	void EnvIlluminantAllPlaneExcuter::Reset()
	{
		m_inputLayerIntensityContainer.Clean();
		m_outputLayerIntensityContainer.Clean();
		if (m_pIlluminant != NULL)
		{
			m_inputLayerIntensityContainer.SetDefaultValue(m_pIlluminant->GetIntensity());
			m_outputLayerIntensityContainer.SetDefaultValue(m_pIlluminant->GetIntensity());
		}
		
	}


	


	AllPlaneBlender::AllPlaneBlender()
	{
		
	}

	AllPlaneBlender::~AllPlaneBlender()
	{
	}

	int AllPlaneBlender::Blend(XL_BITMAP_HANDLE hCanvas)
	{
		assert(m_pExcuter);
		const IlluminationSession::LayerList& sortedLayerList = (dynamic_cast<AllPlaneExcuter*>(m_pExcuter))->GetSortedLayerList();
		IlluminationSession::LayerList::const_reverse_iterator iterLayer = sortedLayerList.rbegin();
		while (iterLayer != sortedLayerList.rend())
		{
			BlendLayer(**iterLayer, (*iterLayer)->GetClouseRect(), hCanvas);
			++iterLayer;
		}
		return IlluminationSession::RetSuc;
	}


	AllPlaneBlender::ChanelIntensityEnumtor::ChanelIntensityEnumtor(XL_BITMAP_HANDLE hCanvas, XL_BITMAP_HANDLE hTexture, const RectI& textureRect)
		: m_textureRect(textureRect)
		, m_hTexture(hTexture)
		, m_hCanvas(hCanvas)
	{
	}
	
	AllPlaneBlender::ChanelIntensityEnumtor::~ChanelIntensityEnumtor()
	{

	}

	int AllPlaneBlender::ChanelIntensityEnumtor::OnRectValue(const RectI& rect, const ChanelIntensityContainer::ValueType& value)
	{
		RECT clipRect = {rect.left - m_textureRect.left, rect.top - m_textureRect.top, rect.right - m_textureRect.left, rect.bottom - m_textureRect.top};
		XL_BITMAP_HANDLE hClipTexture = XL_ClipSubBindBitmap(m_hTexture, &clipRect);
		Point2I ptPos = {rect.left, rect.top};
		XLBlendHint blendHint;
		blendHint.BaseHint = XL_DefaultGraphicHint();
		blendHint.BlendType = XLGRAPHIC_BLEND_SRC_ALPHA;
		blendHint.ConstAlpha = 255;
		if (m_hCanvas != NULL)
		{
			IlluminationBlender::BlendWithShadow(m_hCanvas, ptPos, hClipTexture, &value, 0, 0, &blendHint);
		}
		else
		{
			IlluminationBlender::CompositeShadow(hClipTexture, &value, 0, 0);
		}
		XL_ReleaseBitmap(hClipTexture);

		return ChanelIntensityContainer::RetSuc;
	}

	int AllPlaneBlender::ChanelIntensityEnumtor::OnPointValue(const RectI& rect, const ChanelIntensityContainer::ValueType* const pValueBitmap, const unsigned long& ulLineLength)
	{
		RECT clipRect = {rect.left - m_textureRect.left, rect.top - m_textureRect.top, rect.right - m_textureRect.left, rect.bottom - m_textureRect.top};
		XL_BITMAP_HANDLE hClipTexture = XL_ClipSubBindBitmap(m_hTexture, &clipRect);
		Point2I ptPos = {rect.left, rect.top};
		XLBlendHint blendHint;
		blendHint.BaseHint = XL_DefaultGraphicHint();
		blendHint.BlendType = XLGRAPHIC_BLEND_SRC_ALPHA;
		blendHint.ConstAlpha = 255;
		if (m_hCanvas != NULL)
		{
			IlluminationBlender::BlendWithShadow(m_hCanvas, ptPos, hClipTexture, pValueBitmap, sizeof(XL_Color), ulLineLength, &blendHint);
		}
		else
		{
			IlluminationBlender::CompositeShadow(hClipTexture, pValueBitmap,  sizeof(XL_Color), ulLineLength);
		}
		
		XL_ReleaseBitmap(hClipTexture);


		return ChanelIntensityContainer::RetSuc;
	}


	AllPlaneBlender::TextureContainerEnumtor::TextureContainerEnumtor(XL_BITMAP_HANDLE hCanvas, ChanelIntensityContainer& intensityContainer)
		: m_hCanvas(hCanvas)
		, m_intensityContainer(intensityContainer)
	{
	}

	AllPlaneBlender::TextureContainerEnumtor::~TextureContainerEnumtor()
	{

	}


	int AllPlaneBlender::TextureContainerEnumtor::OnRectValue(const RectI& rect, const BitmapHandleWrap& textureHandle, const unsigned long &index, const RectI& clouseRect) 
	{
		((void)index);
		XL_BITMAP_HANDLE hTexture = textureHandle;
		if (hTexture == NULL)
		{
			return TextureContainer::RetSuc;
		}
		ChanelIntensityEnumtor intensityEnumtor(m_hCanvas, hTexture, clouseRect);
		m_intensityContainer.EnumInRect(rect, intensityEnumtor);
		
		return TextureContainer::RetSuc;
	}

	int AllPlaneBlender::TextureContainerEnumtor::OnPointValue(const RectI& rect, const BitmapHandleWrap* const phTexture, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect) 
	{
		assert(0);
		((void)rect);
		((void)phTexture);
		((void)ulLineLength);
		((void)index);
		((void)clouseRect);
		return TextureContainer::RetSuc;
	}	
	

	int AllPlaneBlender::BlendLayer(IlluminationLayer& layer, const RectI& rect, XL_BITMAP_HANDLE hCanvas)
	{
		ChanelIntensityContainer& intensityContainer = layer.GetRayIntensityContainer();

		TextureContainer& textureContainer = layer.GetTextureContainer();
		TextureContainerEnumtor textureEnumtor(hCanvas, intensityContainer);
		textureContainer.EnumInRectEx(rect, textureEnumtor);
		
		return IlluminationSession::RetSuc;
	}

	int AllPlaneBlender::CompositeLayerWithShadow(IlluminationLayer& layer,  const RectI& rect)
	{
		ChanelIntensityContainer& intensityContainer = layer.GetRayIntensityContainer();
		
		TextureContainer& textureContainer = layer.GetTextureContainer();
		TextureContainerEnumtor textureEnumtor(NULL, intensityContainer);
		textureContainer.EnumInRectEx(rect, textureEnumtor);

		return IlluminationSession::RetSuc;
	}

	int AllPlaneBlender::OnUpdateLayerList()
	{
		assert(m_pExcuter != NULL);
		return IlluminationSession::RetSuc;
	}

}

//#include "IlluminationImplTest.h"
//namespace IlluminationImpl
//{
//	
//	IlluminantAllPlaneExcuter::OutputIntensityEnumtor::OutputIntensityEnumtor(XL_BITMAP_HANDLE hOutput)
//		: m_hOutput(hOutput)
//	{
//	}
//	IlluminantAllPlaneExcuter::OutputIntensityEnumtor::~OutputIntensityEnumtor()
//	{
//
//	}
//
//	int IlluminantAllPlaneExcuter::OutputIntensityEnumtor::OnPointValue(const RectI& rect, const  unsigned char* const pValueBitmap, const unsigned long& ulLineLength)
//	{
//		XLBitmapInfo bmpInfo ;
//		XL_GetBitmapInfo(m_hOutput, &bmpInfo);
//		unsigned char* iterOutLine = XL_GetBitmapBuffer(m_hOutput, rect.left, rect.top);
//		unsigned char* iterOut = iterOutLine;
//
//		const unsigned char* iterInLine = pValueBitmap;
//		const unsigned char* iterIn = pValueBitmap;
//		for (int iy = 0; iy < rect.bottom - rect.top; ++iy)
//		{
//			iterOut = iterOutLine;
//			iterIn = iterInLine;
//			for (int ix = 0; ix < rect.right - rect.left; ++ix)
//			{
//				iterOut[0] = *iterIn;
//				iterOut[1] = *iterIn;
//				iterOut[2] = *iterIn;
//				iterOut[3] = 255;
//				iterOut += 4;
//				++iterIn; 
//			}
//			iterOutLine += bmpInfo.ScanLineLength;
//			iterInLine += ulLineLength;
//		}
//
//		return RayIntensityContainer::RetSuc;
//	}
//	int IlluminantAllPlaneExcuter::OutputIntensityEnumtor::OnRectValue(const RectI& rect, const  unsigned char& value)
//	{
//		XLBitmapInfo bmpInfo ;
//		XL_GetBitmapInfo(m_hOutput, &bmpInfo);
//		unsigned char* iterOutLine = XL_GetBitmapBuffer(m_hOutput, rect.left, rect.top);
//		unsigned char* iterOut = iterOutLine;
//
//		for (int iy = 0; iy < rect.bottom - rect.top; ++iy)
//		{
//			iterOut = iterOutLine;
//			for (int ix = 0; ix < rect.right - rect.left; ++ix)
//			{
//				iterOut[0] = value;
//				iterOut[1] = value;
//				iterOut[2] = value;
//				iterOut[3] = 255;
//				iterOut += 4; 
//			}
//			iterOutLine += bmpInfo.ScanLineLength;
//		}
//
//		return RayIntensityContainer::RetSuc;
//	}
//	void IlluminantAllPlaneExcuter::SaveOutput(XL_BITMAP_HANDLE hOutput, const RectI& rect)
//	{
//		OutputIntensityEnumtor enumtor(hOutput);
//		m_outputLayerIntensityContainer.EnumInRect(rect, enumtor);
//	}
//}

#pragma warning(pop)