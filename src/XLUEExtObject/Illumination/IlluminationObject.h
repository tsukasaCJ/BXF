#ifndef __ILLUMINATIONOBJECT_H__
#define __ILLUMINATIONOBJECT_H__

#include "./StepExcuter.h"

#define EXTCLASSNAME_ILLUMINATIONSESSIONOBJECT "IlluminationSessionObject"
class IlluminationLayerObject;
class IlluminationSessionObject
	: public ExtLayoutObjMethodsImpl
{
public:
	IlluminationSessionObject(XLUE_LAYOUTOBJ_HANDLE hObj);

	~IlluminationSessionObject();

public:
	IlluminationImpl::StepExcuter* GetIlluminationExcuter();
	//// height = zorder * value
	//void SetZorderHeightRate(float zorderHeightRate);
	//float GetZorderHeightRate();

	void AddIlluminationLayerObject(IlluminationLayerObject* iLayerObj);
	void RemoveIlluminationLayerObject(IlluminationLayerObject* iLayerObj);
	void InvalidSession();
private:
	virtual void OnInitControl();
	virtual void OnAbsPosChanged(const RECT* , const RECT* );
	
	
private:
	IlluminationImpl::DynamicLayerSession* m_pSession;
	IlluminationImpl::StepExcuter* m_pExcuter;
	float m_zorderHeightRate;

	std::list<IlluminationLayerObject*> m_layerObjList;
};

#define EXTCLASSNAME_ILLUMINATIONLAYEROBJECT "IlluminationLayerObject"
class IlluminationLayerObject
	: public ExtLayoutObjMethodsImpl
{
public:
	IlluminationLayerObject(XLUE_LAYOUTOBJ_HANDLE hObj);

	~IlluminationLayerObject();

	void SetLayerHeight(int height);
	int GetLayerHeight() const;

private:
	virtual void OnInitControl();
	virtual void OnDestroy();
	virtual void OnPaint(XL_BITMAP_HANDLE hCanvas, const RECT* pSrcRect, const RECT* pDstRect, unsigned char alpha);
	int m_layerHeight;
};

#define EXTCLASSNAME_ILLUMINANTOBJECT "IlluminantObject"
class IlluminantObject
	: public ExtLayoutObjMethodsImpl
{
public:
	IlluminantObject(XLUE_LAYOUTOBJ_HANDLE hObj);

	~IlluminantObject();

public:
	void SetDirectionX(int directionX);
	int GetDirectionX() const;
	void SetDirectionY(int directionY);
	int GetDirectionY() const;
	void SetColor(XL_Color color);
	XL_Color GetColor() const;
	void SetColorId(const char* colorId);


private:
	virtual void OnInitControl();
	virtual void OnDestroy();
	virtual void OnResProviderChange(XLUE_RESPROVIDER_HANDLE hResProvider);
private:
	IlluminationImpl::Illuminant* m_pIlluminat;
	ColorResHolder m_colorRes;

};



#endif //__ILLUMINATIONOBJECT_H__
