#ifndef __ILLUMINATIONIMPL_STEPEXCUTER_H__
#define __ILLUMINATIONIMPL_STEPEXCUTER_H__
#include "./AllPlaneExcuter.h"


/////Ϊ��Ƕ�뵽���������ʹ�ã����Ҳ����ƶ�������zorder������֯����Ĺ��� Ҫ����ճ�����excute�� blend���ܹ��ֲ�ֿ鵥������
////Ϊ�˻������ػ�Sessionʹ֮��̬����Layer����������layer��˳�������϶��µģ�����̬ɨ��layer���߼���Ƚϸ���
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