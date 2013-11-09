#include "stdafx.h"
#include "./Illumination.h"

#pragma warning(push)
#pragma warning(disable: 4512)

namespace IlluminationImpl
{
	IlluminationLayer::IlluminationLayer()
		: m_isUseAlphaAsProp(false)
	{
		ZeroMemory(&m_clouseRect, sizeof(m_clouseRect));
		ZeroMemory(&m_direction, sizeof(m_direction));

		m_textureContainer.SetDefaultValue(NULL);
		m_intensityContainer.SetDefaultValue(0);

		m_textureBitmapContainer.SetDefaultValue(0);
	}

	IlluminationLayer::~IlluminationLayer()
	{
		m_propContainer.Clean();
		m_intensityContainer.Clean();
		m_textureContainer.Clean();

		m_textureBitmapContainer.Clean();
	}

	ChanelIntensityContainer& IlluminationLayer::GetRayIntensityContainer()
	{
		return m_intensityContainer;
	}

	const ChanelIntensityContainer& IlluminationLayer::GetRayIntensityContainer() const
	{
		return m_intensityContainer;
	}

	const TextureContainer& IlluminationLayer::GetTextureContainer() const
	{
		return m_textureContainer;
	}

	TextureContainer& IlluminationLayer::GetTextureContainer()
	{
		return m_textureContainer;
	}

	const IlluminatPropContainer& IlluminationLayer::GetPropContainer() const
	{
		return m_propContainer;
	}

	IlluminatPropContainer& IlluminationLayer::GetPropContainer()
	{
		return m_propContainer;
	}

	void IlluminationLayer::SetClouseRect(const RectI& rect)
	{
		m_clouseRect = rect;
	}
	
	const RectI& IlluminationLayer::GetClouseRect() const
	{
		return m_clouseRect;
	}

	void IlluminationLayer::SetDirection(const Vector3I& direction)
	{
		m_direction = direction;
	}

	const Vector3I& IlluminationLayer::GetDirection() const
	{
		return m_direction;
	}

	void IlluminationLayer::SetIsUseAlphaAsProp(bool isUseAlphaAsProp)
	{
		m_isUseAlphaAsProp = isUseAlphaAsProp;
	}

	bool IlluminationLayer::GetIsUseAlphaAsProp() const
	{
		return m_isUseAlphaAsProp;
	}



	IlluminationLayer::TextureEnumtor::TextureEnumtor(TextureBitmapContainer& textureBitmapContainer)
		: m_textureBitmapContainer(textureBitmapContainer)
	{
		
	}

	int IlluminationLayer::TextureEnumtor::OnRectValue(const RectI& rect, const BitmapHandleWrap& textureHandle, const unsigned long &index, const RectI& clouseRect)
	{
		((void)index);
		XL_BITMAP_HANDLE hTexture = textureHandle;
		if (hTexture != NULL)
		{
			XLBitmapInfo textureBitmapInfo;
			XL_GetBitmapInfo(hTexture, &textureBitmapInfo);
			unsigned long index;
			unsigned char* pTextureBitmap = XL_GetBitmapBuffer(hTexture, rect.left - clouseRect.left, rect.top - clouseRect.top);
			m_textureBitmapContainer.AddUnhandledPointValue(rect, index, reinterpret_cast<XL_Color*>(pTextureBitmap), textureBitmapInfo.ScanLineLength);
		}
		return TextureContainer::RetSuc;
	}

	int IlluminationLayer::TextureEnumtor::OnPointValue(const RectI& rect, const BitmapHandleWrap* const pValueBitmap, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect)
	{
		assert(0);
		((void)rect);
		((void)pValueBitmap);
		((void)ulLineLength);
		((void)clouseRect);
		((void)index);

		return TextureContainer::RetSuc;
	}

	TextureBitmapContainer& IlluminationLayer::GetTextureBitmapContainer()
	{
		m_textureBitmapContainer.Clean();
		TextureContainer& textureContainer = GetTextureContainer();
		TextureEnumtor textureEnumtor(m_textureBitmapContainer);
		textureContainer.EnumInRectEx(GetClouseRect(), textureEnumtor);
		return m_textureBitmapContainer;
	}


	IlluminationSession::IlluminationSession()
		: m_ulNewEventCookie(0)
	{
	}

	IlluminationSession::~IlluminationSession()
	{
		m_listIlluminants.clear();
		m_listLayers.clear();
	}

	const IlluminationSession::IlluminantList& IlluminationSession::GetIlluminantList() const
	{
		return m_listIlluminants;
	}

	const IlluminationSession::LayerList& IlluminationSession::GetLayerList() const
	{
		return m_listLayers;
	}


	int IlluminationSession::AddIlluminant(Illuminant& refIlluminant)
	{
		m_listIlluminants.push_back(&refIlluminant);
		ListenerList::iterator listIter = m_listListeners.begin();
		while(listIter != m_listListeners.end())
		{
			ISessionEvent* pListener = listIter->pListener;
			++listIter;
			pListener->OnAddIlluminant(*this);
		}
		return IlluminationSession::RetSuc;
	}

	int IlluminationSession::AddIlluminationLayer(IlluminationLayer& refLayer)
	{
		m_listLayers.push_back(&refLayer);
		ListenerList::iterator listIter = m_listListeners.begin();
		while(listIter != m_listListeners.end())
		{
			ISessionEvent* pListener = listIter->pListener;
			++ listIter;
			pListener->OnAddIlluminationLayer(*this);
		}
		return IlluminationSession::RetSuc;
	}


	int IlluminationSession::RemoveIllumnant(Illuminant& refIlluminant)
	{
		IlluminantList::iterator iter = std::find(m_listIlluminants.begin(), m_listIlluminants.end(), &refIlluminant);
		if (iter != m_listIlluminants.end())
		{
			m_listIlluminants.erase(iter);
			ListenerList::iterator listIter = m_listListeners.begin();
			while(listIter != m_listListeners.end())
			{
				ISessionEvent* pListener = listIter->pListener;
				++ listIter;
				pListener->OnRemoveIlluminant(*this, refIlluminant);
			}
		}
		return RetSuc;
	}

	int IlluminationSession::RemoveIlluminationLayer(IlluminationLayer& refLayer)
	{
		LayerList::iterator iter = std::find(m_listLayers.begin(), m_listLayers.end(), &refLayer);
		if (iter != m_listLayers.end())
		{
			m_listLayers.erase(iter);
			ListenerList::iterator listIter = m_listListeners.begin();
			while(listIter != m_listListeners.end())
			{
				ISessionEvent* pListener = listIter->pListener;
				++ listIter;
				pListener->OnRemoveIlluminationLayer(*this, refLayer);
			}
		}
		return RetSuc;
	}

	int IlluminationSession::AttachListener(ISessionEvent& refEvent, unsigned long *pCookie)
	{
		assert(pCookie != NULL);
		ListenerStub newStub = {&refEvent, m_ulNewEventCookie};
		*pCookie = m_ulNewEventCookie;
		++ m_ulNewEventCookie;
		m_listListeners.push_back(newStub);
		return IlluminationSession::RetSuc;
	}

	int IlluminationSession::DetachListener(unsigned long ulCookie)
	{
		ListenerList::iterator listIter = m_listListeners.begin();
		while(listIter != m_listListeners.end())
		{
			if(listIter->ulCookie == ulCookie)
			{
				m_listListeners.erase(listIter);
				break;
			}
			++ listIter;
		}
		return IlluminationSession::RetSuc;
	}


	Illuminant::Illuminant()
	{
		ZeroMemory(&m_clr, sizeof(m_clr));
	}

	Illuminant::~Illuminant()
	{

	}

	XL_Color Illuminant::GetColor() const
	{
		return m_clr;
	}

	void Illuminant::SetColor(XL_Color clr)
	{
		m_clr = clr;
	}

	RayIntensity Illuminant::GetIntensity() const
	{
		return XLCOLOR_BGRA_A(m_clr);
	}



	IlluminationExcuter::IlluminationExcuter()
		: m_ulSessionEventCookie(static_cast<unsigned long>(-1))
		, m_pSession(NULL)
	{
	}

	IlluminationExcuter::~IlluminationExcuter()
	{
		CleanSession();
	}

	int IlluminationExcuter::SetIlluminationSession(IlluminationSession& refSession)
	{
		CleanSession();
		m_pSession = &refSession;
		PrepareSession();
		return IlluminationSession::RetSuc;
	}

	IlluminationSession& IlluminationExcuter::GetIlluminationSession()
	{
		return *m_pSession;
	}

	int IlluminationExcuter::Excute()
	{
		return IlluminationSession::RetSuc;
	}

	int IlluminationExcuter::OnAddIlluminant(IlluminationSession& refSession)
	{
		const IlluminationSession::IlluminantList& illuminantList = refSession.GetIlluminantList();
		const Illuminant& refIlluminant = *(illuminantList.back());
		IlluminantExcuter* pNewExcuter = CreateIlluminantExcuter(refIlluminant);
		pNewExcuter->SetSessionExcuter(*this);
		pNewExcuter->SetIlluminant(refIlluminant);
		m_listIlluminantExcuters.push_back(pNewExcuter);
		return IlluminationSession::RetSuc;
	}

	int IlluminationExcuter::OnAddIlluminationLayer(IlluminationSession& refSession)
	{
		((void)refSession);
		return IlluminationSession::RetSuc;
	}


	class IfIlluminantExcuter
	{
	public:
		IfIlluminantExcuter(const Illuminant&refIlluminant)
			: m_refIlluminant(refIlluminant)
		{

		}

		bool operator()(const IlluminantExcuter* pExcuter)
		{
			return (&(const_cast<IlluminantExcuter*>(pExcuter)->GetIlluminant()) == &m_refIlluminant);
		}

	private:
		const Illuminant& m_refIlluminant;
	};
	int IlluminationExcuter::OnRemoveIlluminant(IlluminationSession& refSession, Illuminant& refIlluminant) 
	{
		((void)refSession);
		IfIlluminantExcuter ifExcuter(refIlluminant);
		IlluminantExcuterList::iterator iter = std::find_if<IlluminantExcuterList::iterator, IfIlluminantExcuter>(m_listIlluminantExcuters.begin(), m_listIlluminantExcuters.end(), ifExcuter);
		if (iter != m_listIlluminantExcuters.end())
		{
			DestroyIlluminantExcuter(*iter);
			m_listIlluminantExcuters.erase(iter);
		}
		return IlluminationSession::RetSuc;
	}

	int IlluminationExcuter::OnRemoveIlluminationLayer(IlluminationSession& refIlluminationLayer, IlluminationLayer& refLayer) 
	{
		((void)refIlluminationLayer);
		((void)refLayer);
		return IlluminationSession::RetSuc;
	}	

	IlluminantExcuter* IlluminationExcuter::CreateIlluminantExcuter(const Illuminant& refIlluminant)
	{
		((void)refIlluminant);
		return NULL;
	}

	

	void IlluminationExcuter::DestroyIlluminantExcuter(IlluminantExcuter* pExcuter)
	{
		delete pExcuter;
	}
	
	int IlluminationExcuter::PrepareSession()
	{
		const IlluminationSession::IlluminantList& illuminantList = m_pSession->GetIlluminantList();
		IlluminationSession::IlluminantList::const_iterator iter = illuminantList.begin();
		while(iter != illuminantList.end())
		{
			IlluminantExcuter* pNewExcuter = CreateIlluminantExcuter(**iter);
			pNewExcuter->SetSessionExcuter(*this);
			pNewExcuter->SetIlluminant(**iter);
			m_listIlluminantExcuters.push_back(pNewExcuter);
			++iter;
		}
		m_pSession->AttachListener(*this,&m_ulSessionEventCookie);
		return IlluminationSession::RetSuc;
	}

	void IlluminationExcuter::CleanSession()
	{
		IlluminantExcuterList::iterator iter = m_listIlluminantExcuters.begin();
		while(iter != m_listIlluminantExcuters.end())
		{
			DestroyIlluminantExcuter(*iter);
			++ iter;
		}
		m_listIlluminantExcuters.clear();
		if(m_pSession != NULL)
		{
			m_pSession->DetachListener(m_ulSessionEventCookie);
		}
		m_pSession = NULL;
		m_ulSessionEventCookie = static_cast<unsigned long>(-1);
	}

	const IlluminationExcuter::IlluminantExcuterList& IlluminationExcuter::GetIlluminantExcuterList()
	{
		return m_listIlluminantExcuters;
	}


	IlluminantExcuter::IlluminantExcuter()
		: m_pSessionExcuter(NULL)
		, m_pIlluminant(NULL)
	{

	}

	IlluminantExcuter::~IlluminantExcuter()
	{
		
	}

	int IlluminantExcuter::SetSessionExcuter(IlluminationExcuter& refSessionExcuter)
	{
		m_pSessionExcuter = &refSessionExcuter;
		return IlluminationSession::RetSuc;
	}

	int IlluminantExcuter::SetIlluminant(const Illuminant& refIllunminant)
	{
		m_pIlluminant = &refIllunminant;
		return IlluminationSession::RetSuc;
	}

	IlluminationExcuter& IlluminantExcuter::GetSessionExcuter()
	{
		return *m_pSessionExcuter;
	}

	const Illuminant& IlluminantExcuter::GetIlluminant()
	{
		return *m_pIlluminant;
	}


	int IlluminantExcuter::Excute()
	{
		return IlluminationSession::RetSuc;
	}

	


	IlluminationBlender::IlluminationBlender()
		: m_pExcuter(NULL)
	{
	}

	IlluminationBlender::~IlluminationBlender()
	{

	}

	int IlluminationBlender::SetExcuter(IlluminationExcuter& refExcuter)
	{
		m_pExcuter = &refExcuter;
		return IlluminationSession::RetSuc;
	}

	int IlluminationBlender::Blend(XL_BITMAP_HANDLE hCanvas)
	{
		((void)hCanvas);
		return IlluminationSession::RetSuc;
	}
	

	int IlluminationBlender::BlendLayer(IlluminationLayer& layer,  const RectI& rect, XL_BITMAP_HANDLE hCanvas)
	{
		((void)layer);
		((void)hCanvas);
		((void)rect);
		return IlluminationSession::RetSuc; 
	}

	int IlluminationBlender::CompositeLayerWithShadow(IlluminationLayer& layer,  const RectI& rect)
	{
		((void)layer);
		((void)rect);
		return IlluminationSession::RetSuc;
	}



	XL_BITMAP_HANDLE IlluminationBlender::BlendWithShadow(XL_BITMAP_HANDLE hDstBmp, Point2I ptPos
		, XL_BITMAP_HANDLE hSrcBmp, const XL_Color* pChanelIntensity, unsigned long chanelColOffset
		, unsigned long chanelLineOffset,  XLBlendHint* pBlendHint)
	{
		assert(hDstBmp != XLGRAPHIC_INVALID_HANDLE);
		assert(hSrcBmp != XLGRAPHIC_INVALID_HANDLE);
		assert(pBlendHint != NULL);
		XLBitmapInfo dstBmpInfo;
		XL_GetBitmapInfo(hDstBmp, &dstBmpInfo);

		XLBitmapInfo srcBmpInfo;
		XL_GetBitmapInfo(hSrcBmp, &srcBmpInfo);

		
		RectI blendRect = {ptPos.x, ptPos.y, ptPos.x + srcBmpInfo.Width, ptPos.y + srcBmpInfo.Height};
		RectI dstRect = {0, 0, dstBmpInfo.Width, dstBmpInfo.Height};
		RectOperation::IntersectRect(blendRect, blendRect, dstRect);
		if (RectOperation::IsRectEmpty(blendRect))
		{
			return hDstBmp;
		}
		
		unsigned char* iterDstLine = XL_GetBitmapBuffer(hDstBmp, blendRect.left, blendRect.top);
		unsigned char* iterDstValue = NULL;

		const unsigned char* iterShadowLine = reinterpret_cast<const unsigned char*>(pChanelIntensity) 
			+ (blendRect.left - ptPos.x) * chanelColOffset + (blendRect.top - ptPos.y) * chanelLineOffset;
		const unsigned char* iterShadowValue = NULL;

		unsigned char* iterSrcLine = XL_GetBitmapBuffer(hSrcBmp, blendRect.left - ptPos.x, blendRect.top - ptPos.y);
		unsigned char* iterSrcValue = NULL;

		for (int iy = 0; iy < blendRect.bottom - blendRect.top; ++iy)
		{
			iterDstValue = iterDstLine;
			iterSrcValue = iterSrcLine;
			iterShadowValue = iterShadowLine;

			for (int ix = 0; ix < blendRect.right - blendRect.left; ++ix)
			{
				int alpha = iterSrcValue[3] * pBlendHint->ConstAlpha / 255;
				int beta = 255 - alpha;
				int visibleColor = (((int)iterSrcValue[0]) * iterShadowValue[0] * alpha / 255 + iterDstValue[0] * beta) / 255; 
				if(visibleColor > 255)
				{
					visibleColor = 255;
				}
				iterDstValue[0] = (unsigned char)visibleColor;
				visibleColor = (((int)iterSrcValue[1]) * iterShadowValue[1] * alpha / 255 + iterDstValue[1] * beta) / 255; 
				if(visibleColor > 255)
				{
					visibleColor = 255;
				}
				iterDstValue[1] = (unsigned char)visibleColor;
				visibleColor = (((int)iterSrcValue[2]) * iterShadowValue[2] * alpha / 255 + iterDstValue[2] * beta) / 255; 
				if(visibleColor > 255)
				{
					visibleColor = 255;
				}
				iterDstValue[2] = (unsigned char)visibleColor;
				visibleColor = (((int)iterSrcValue[3])  * alpha  + iterDstValue[3] * beta) / 255; 
				if(visibleColor > 255)
				{
					visibleColor = 255;
				}
				iterDstValue[3] = (unsigned char)visibleColor;

				iterDstValue += sizeof(XL_Color);
				iterSrcValue += sizeof(XL_Color);
				iterShadowValue += chanelColOffset;
			}

			iterDstLine += dstBmpInfo.ScanLineLength;
			iterSrcLine += srcBmpInfo.ScanLineLength;
			iterShadowLine += chanelLineOffset;
		}
		return hDstBmp;
	}
	
	XL_BITMAP_HANDLE IlluminationBlender::CompositeShadow(XL_BITMAP_HANDLE hBmp
		, const XL_Color* pChanelIntensity, unsigned long chanelColOffset, unsigned long chanelLineOffset)
	{
		assert(hBmp != XLGRAPHIC_INVALID_HANDLE);
		XLBitmapInfo bmpInfo;
		XL_GetBitmapInfo(hBmp, &bmpInfo);

		RectI blendRect = {0, 0, bmpInfo.Width, bmpInfo.Height};;

		unsigned char* iterDstLine = XL_GetBitmapBuffer(hBmp, blendRect.left, blendRect.top);
		unsigned char* iterDstValue = NULL;

		const unsigned char* iterShadowLine = reinterpret_cast<const unsigned char*>(pChanelIntensity) 
			+ (blendRect.left) * chanelColOffset + (blendRect.top) * chanelLineOffset;
		const unsigned char* iterShadowValue = NULL;

		for (int iy = 0; iy < blendRect.bottom - blendRect.top; ++iy)
		{
			iterDstValue = iterDstLine;
			iterShadowValue = iterShadowLine;

			for (int ix = 0; ix < blendRect.right - blendRect.left; ++ix)
			{
				int visibleColor = ((int)iterDstValue[0]) * iterShadowValue[0] / 255; 
				if(visibleColor > 255)
				{
					visibleColor = 255;
				}
				iterDstValue[0] = (unsigned char)visibleColor;
				visibleColor = ((int)iterDstValue[1]) * iterShadowValue[0] / 255;; 
				if(visibleColor > 255)
				{
					visibleColor = 255;
				}
				iterDstValue[1] = (unsigned char)visibleColor;
				visibleColor = ((int)iterDstValue[2]) * iterShadowValue[0] / 255;  
				if(visibleColor > 255)
				{
					visibleColor = 255;
				}
				iterDstValue[2] = (unsigned char)visibleColor;

				iterDstValue += sizeof(XL_Color);
				iterShadowValue += chanelColOffset;
			}

			iterDstLine += bmpInfo.ScanLineLength;
			iterShadowLine += chanelLineOffset;
		}
		return hBmp;
	}

	EnvIlluminant::EnvIlluminant()
	{
		ZeroMemory(&m_vecDirection, sizeof(m_vecDirection));
	}

	EnvIlluminant::~EnvIlluminant()
	{

	}

	Ray EnvIlluminant::GetRayOnPos(Point3I& ptPos) const
	{
		Ray rayRet;
		rayRet.color = m_clr;
		rayRet.dirction.end2direction.direction = m_vecDirection;
		rayRet.dirction.end2direction.end = ptPos;
		return rayRet;
	}

	void EnvIlluminant::SetDirection(const Direction3I& vecDirection)
	{
		m_vecDirection = vecDirection;
	}

	const Direction3I& EnvIlluminant::GetDirection() const
	{
		return m_vecDirection;
	}
}

#pragma warning(pop)