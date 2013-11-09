#ifndef __ILLUMINATIONIMPL_STEPEXCUTER_H__
#define __ILLUMINATIONIMPL_STEPEXCUTER_H__
#include "./AllPlaneExcuter.h"


/////为了嵌入到对象树层次使用，并且不打破对象树以zorder纵向组织对象的规则， 要求光照场景的excute和 blend都能够分层分块单步处理
////为此还必须特化Session使之动态生成Layer，并且生成layer的顺序是自上而下的，否则静态扫描layer的逻辑会比较复杂
namespace IlluminationImpl
{
class DynamicLayerSession
	: public IlluminationSession
{
public:
	DynamicLayerSession();
	~DynamicLayerSession();

public:
	void SetClouseRect(const RectI& rect);
	const RectI& GetClouseRect() const;
	void CleanLayer();
	IlluminationLayer& GetLayerOnHeight(int height, bool& isCreated, IlluminationLayer** ppUperLayer);
	IlluminationLayer* GetLayerOnHeight(int height);
	virtual int AddIlluminationLayer(IlluminationLayer& refLayer)
	{
		((void)refLayer);
		assert(0);
		return RetNoImpl;
	}
	virtual int RemoveIlluminationLayer(IlluminationLayer& refLayer)
	{
		((void)refLayer);
		assert(0);
		return RetNoImpl;
	}
private:
	RectI m_clouseRect;
	
};

class StepExcuter
	: public AllPlaneExcuter
{
public:
	StepExcuter();
	~StepExcuter();

public:
	int ExcuteOnHeight(int height, XL_BITMAP_HANDLE hTexture, const RectI& rect);
	int BlendOnHeight(XL_BITMAP_HANDLE hCanvas, int height, const RectI&rect);
	int CompositeShadowOnHeight(int height, const RectI&rect);

private:
	int m_lastExcuteHeight;

//public:
//	//////for debug
//	void SaveOutput(const RectI& rect);
};

}




#endif //__ILLUMINATIONIMPL_STEPEXCUTER_H__