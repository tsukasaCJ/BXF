#include "stdafx.h"
#include "./IlluminationObject.h"
using namespace IlluminationImpl;

class IllumiationObjectHelper
{
public:
	static IlluminationSessionObject* GetIlluminationSessionObject(LayoutObjectBaseWrapper& thisObj)
	{
		XLUE_LAYOUTOBJ_HANDLE hFather = thisObj.GetFather();
		while (hFather != NULL && 0 != ::strcmp(LayoutObjectWrapper(hFather).GetClass() , EXTCLASSNAME_ILLUMINATIONSESSIONOBJECT))
		{
			hFather = LayoutObjectWrapper(hFather).GetFather();
		}
		if (hFather == NULL)
		{
			return NULL;
		}
		return IlluminationSessionObject::ObjectFromExtHandle<IlluminationSessionObject>(XLUE_GetExtHandle(hFather));
	}

	static StepExcuter* GetIlluminationExcuter(LayoutObjectBaseWrapper& thisObj)
	{
		return GetIlluminationSessionObject(thisObj)->GetIlluminationExcuter();
	}

	static DynamicLayerSession* GetIlluminationSession(LayoutObjectBaseWrapper& thisObj)
	{
		return dynamic_cast<DynamicLayerSession*>(&(GetIlluminationExcuter(thisObj)->GetIlluminationSession()));
	}
};


IlluminationSessionObject::IlluminationSessionObject(XLUE_LAYOUTOBJ_HANDLE hObj)
	: ExtLayoutObjMethodsImpl(hObj)
	, m_pSession(NULL)
	, m_pExcuter(NULL)
	//, m_zorderHeightRate(1.0f)
{
	m_pSession = new DynamicLayerSession();
	m_pExcuter = new StepExcuter();
	m_pExcuter->SetIlluminationSession(*m_pSession);
}

IlluminationSessionObject::~IlluminationSessionObject()
{
	delete m_pExcuter;
	delete m_pSession;

	m_layerObjList.clear();
}

StepExcuter* IlluminationSessionObject::GetIlluminationExcuter()
{
	return m_pExcuter;
}

//void IlluminationSessionObject::SetZorderHeightRate(float zorderHeightRate)
//{
//	m_zorderHeightRate = zorderHeightRate;
//}
//
//float IlluminationSessionObject::GetZorderHeightRate()
//{
//	return m_zorderHeightRate;
//}

void IlluminationSessionObject::OnInitControl()
{
	
}

void IlluminationSessionObject::OnAbsPosChanged(const RECT* pOldRect, const RECT* pNewRect)
{
	((void)pOldRect);
	RectI clouseRect = {pNewRect->left, pNewRect->top, pNewRect->right, pNewRect->bottom};
	m_pSession->SetClouseRect(clouseRect);
}

void IlluminationSessionObject::AddIlluminationLayerObject(IlluminationLayerObject* iLayerObj)
{
	m_layerObjList.push_back(iLayerObj);
	InvalidSession();
}

void IlluminationSessionObject::RemoveIlluminationLayerObject(IlluminationLayerObject* iLayerObj)
{
	m_layerObjList.remove(iLayerObj);
	InvalidSession();
}

void IlluminationSessionObject::InvalidSession()
{
	std::list<IlluminationLayerObject*>::iterator iter = m_layerObjList.begin();
	while (iter != m_layerObjList.end())
	{
		(*iter)->PushDirtyRect(NULL);
		++iter;
	}
}



IlluminationLayerObject::IlluminationLayerObject(XLUE_LAYOUTOBJ_HANDLE hObj)
	: ExtLayoutObjMethodsImpl(hObj)
	, m_layerHeight(0)
{
}

IlluminationLayerObject::~IlluminationLayerObject()
{
}


void IlluminationLayerObject::SetLayerHeight(int height)
{
	m_layerHeight = height;
}

int IlluminationLayerObject::GetLayerHeight() const
{
	return m_layerHeight;
}

void IlluminationLayerObject::OnInitControl()
{
	IlluminationSessionObject* pSessionObj = IllumiationObjectHelper::GetIlluminationSessionObject(*this);
	assert(pSessionObj != NULL);
	pSessionObj->AddIlluminationLayerObject(this);
}
void IlluminationLayerObject::OnDestroy()
{
	IlluminationSessionObject* pSessionObj = IllumiationObjectHelper::GetIlluminationSessionObject(*this);
	assert(pSessionObj != NULL);
	pSessionObj->RemoveIlluminationLayerObject(this);
}

void IlluminationLayerObject::OnPaint(XL_BITMAP_HANDLE hCanvas, const RECT* pSrcRect, const RECT* pDstRect, unsigned char alpha)
{
	((void)pSrcRect);
	((void)pDstRect);
	((void)alpha);
	XLUE_LAYOUTOBJ_HANDLE hFather = GetFather();
	while (0 != ::strcmp(LayoutObjectWrapper(hFather).GetClass() , "LayerObject"))
	{
		hFather = LayoutObjectWrapper(hFather).GetFather();
	}
	LayoutObjectWrapper parentObj(hFather);
	//const char* szId = parentObj.GetID();
	if (0 == strcmp(parentObj.GetClass(), "LayerObject"))
	{
		IlluminationSessionObject* pSessionObj = IllumiationObjectHelper::GetIlluminationSessionObject(*this);
		assert(pSessionObj != NULL);
		//pSessionObj->SetSessionInvalid();
		StepExcuter*pExcuter = pSessionObj->GetIlluminationExcuter();
		assert(pExcuter != NULL);
		if (pExcuter != NULL)
		{
			//int parentAbsZorder = parentObj.GetAbsZorder();
			//int height = (parentObj.GetAbsZorder()  - pSessionObj->GetAbsZorder()) * pSessionObj->GetZorderHeightRate();
			const RECT* absPos = parentObj.GetAbsPos();
			RectI rect = {absPos->left, absPos->top, absPos->right, absPos->bottom};
			pExcuter->ExcuteOnHeight(m_layerHeight, hCanvas, rect);
			pExcuter->CompositeShadowOnHeight(m_layerHeight, rect);
		}
	}
}

IlluminantObject::IlluminantObject(XLUE_LAYOUTOBJ_HANDLE hObj)
	: ExtLayoutObjMethodsImpl(hObj)
	, m_pIlluminat(NULL)
{
	m_pIlluminat = new IlluminationImpl::EnvIlluminant();
	Direction3I direction = {0, 0, -1};
	dynamic_cast<EnvIlluminant*>(m_pIlluminat)->SetDirection(direction);

	////ÏÈºöÂÔÑÕÉ« for test
	m_pIlluminat->SetColor(0);
}

IlluminantObject::~IlluminantObject()
{
	delete m_pIlluminat;
}

void IlluminantObject::SetDirectionX(int directionX)
{
	assert(m_pIlluminat != NULL);
	Direction3I direction = dynamic_cast<EnvIlluminant*>(m_pIlluminat)->GetDirection();
	direction.x = directionX;
	dynamic_cast<EnvIlluminant*>(m_pIlluminat)->SetDirection(direction);
	IlluminationSessionObject* pSessionObj = IllumiationObjectHelper::GetIlluminationSessionObject(*this);
	if (pSessionObj != NULL)
	{
		pSessionObj->InvalidSession();
	}
	
}

int IlluminantObject::GetDirectionX() const
{
	assert(m_pIlluminat != NULL);
	return dynamic_cast<EnvIlluminant*>(m_pIlluminat)->GetDirection().x;
}



void IlluminantObject::SetDirectionY(int directionY)
{
	assert(m_pIlluminat != NULL);
	Direction3I direction = dynamic_cast<EnvIlluminant*>(m_pIlluminat)->GetDirection();
	direction.y = directionY;
	dynamic_cast<EnvIlluminant*>(m_pIlluminat)->SetDirection(direction);
	IlluminationSessionObject* pSessionObj = IllumiationObjectHelper::GetIlluminationSessionObject(*this);
	if (pSessionObj != NULL)
	{
		pSessionObj->InvalidSession();
	}
}

int IlluminantObject::GetDirectionY() const
{
	assert(m_pIlluminat != NULL);
	return dynamic_cast<EnvIlluminant*>(m_pIlluminat)->GetDirection().y;
}


void IlluminantObject::SetColor(XL_Color color)
{
	assert(m_pIlluminat);
	m_pIlluminat->SetColor(color);
	IlluminationSessionObject* pSessionObj = IllumiationObjectHelper::GetIlluminationSessionObject(*this);
	if (pSessionObj != NULL)
	{
		pSessionObj->InvalidSession();
	}
}

XL_Color IlluminantObject::GetColor() const
{
	assert(m_pIlluminat);
	return m_pIlluminat->GetColor();
}


void IlluminantObject::SetColorId(const char* colorId)
{
	m_colorRes.SetResID(colorId);
	assert(m_pIlluminat);
	m_pIlluminat->SetColor(m_colorRes.GetHandle());
	IlluminationSessionObject* pSessionObj = IllumiationObjectHelper::GetIlluminationSessionObject(*this);
	if (pSessionObj != NULL)
	{
		pSessionObj->InvalidSession();
	}
}

void IlluminantObject::OnInitControl()
{
	IlluminationImpl::IlluminationSession* pSession = IllumiationObjectHelper::GetIlluminationSession(*this);
	assert(pSession != NULL);
	if(pSession != NULL)
	{
		pSession->AddIlluminant(*m_pIlluminat);
	}
	IllumiationObjectHelper::GetIlluminationSessionObject(*this)->InvalidSession();
}

void IlluminantObject::OnDestroy()
{
	IlluminationImpl::IlluminationSession* pSession = IllumiationObjectHelper::GetIlluminationSession(*this);
	assert(pSession != NULL);
	if(pSession != NULL)
	{
		pSession->RemoveIllumnant(*m_pIlluminat);
	}
	IllumiationObjectHelper::GetIlluminationSessionObject(*this)->InvalidSession();
}

void IlluminantObject::OnResProviderChange(XLUE_RESPROVIDER_HANDLE hResProvider)
{
	m_colorRes.SetResProvider(hResProvider);
}
