#ifndef __ILLUMINATION_ALLPLANEEXCUTER_H__
#define __ILLUMINATION_ALLPLANEEXCUTER_H__

#include "./IlluminationDefine.h"
#include "./Illumination.h"


#pragma warning(push)
#pragma warning(disable: 4512)

namespace IlluminationImpl
{
	class IlluminantAllPlaneExcuter
		: public IlluminantExcuter
	{
	public:
		IlluminantAllPlaneExcuter();
		virtual ~IlluminantAllPlaneExcuter();


	public:
		virtual int SetIlluminant(const Illuminant& refIllunminant);
		virtual int OnUpdateLayerList();

	public:
		virtual int Excute();
		virtual int ExcuteOnLayer(IlluminationLayer& layer, IlluminationLayer* pPreLayer, const RectI& rect);
		virtual void Reset();
		virtual void Swap(IlluminationLayer& layer, IlluminationLayer* pPreLayer);

	protected:
		IlluminationLayer m_illuminantLayer;
		RayIntensityContainer m_inputLayerIntensityContainer;
		RayIntensityContainer m_outputLayerIntensityContainer;

	/*public:
		///for debug
		void SaveOutput(XL_BITMAP_HANDLE hOutput, const RectI& rect);
	private:
		class OutputIntensityEnumtor
			: public RayIntensityContainer::ValueEnum
		{
		public:	
			OutputIntensityEnumtor(XL_BITMAP_HANDLE hOutput);
			~OutputIntensityEnumtor();

			virtual int OnPointValue(const RectI& rect, const unsigned char* const pValueBitmap, const unsigned long& ulLineLength);
			virtual int OnRectValue(const RectI& rect, const  unsigned char& value);

		private:
			XL_BITMAP_HANDLE m_hOutput;
		};*/
	};

	class EnvIlluminantAllPlaneExcuter
		: public IlluminantAllPlaneExcuter
	{
	public:
		EnvIlluminantAllPlaneExcuter();
		virtual ~EnvIlluminantAllPlaneExcuter();

	public:
		virtual int SetIlluminant(const Illuminant& refIllunminant);
		virtual int OnUpdateLayerList();
		virtual int ExcuteOnLayer(IlluminationLayer& layer, IlluminationLayer* pPreLayer, const RectI& rect);
		virtual void Reset();
		virtual void Swap(IlluminationLayer& layer, IlluminationLayer* pPreLayer);

	protected:
		class MapLayerRect
			: public RectContainerRectMap
		{
		public:
			MapLayerRect(int deltaX, int deltaY);
			virtual RectI& Map(const RectI& thisRect, RectI& mapRect);
			virtual RectI& ReverseMap(const RectI& mapRect, RectI& thisRect);

		private:
			int m_deltaX;
			int m_deltaY;
		};

		class SetIntensityWithProp
			: public RayIntensityContainer::AddValue<IlluminatProp>
		{
		public:
			virtual RayIntensity& operator()(RayIntensity& intensity, const IlluminatProp& prop);
		};

		class SetIntensityWithAlpha
			: public RayIntensityContainer::AddValue<XL_Color>
		{
		public:
			virtual RayIntensity& operator()(RayIntensity& intensity, const XL_Color& color);
		};

		class AssignIntensity
			: public ChanelIntensityContainer::AddValue<RayIntensity>
		{
		public:
			AssignIntensity(XL_Color color);
			virtual ChanelIntensity& operator()(ChanelIntensity& leftIntensity, const RayIntensity& rightIntensity);

		private:
			XL_Color m_color;
		};

		class TextureContainerEnumtor
			: public TextureContainer::ValueEnumEx
		{
		public:
			TextureContainerEnumtor(ChanelIntensityContainer& intensityContainer, RayIntensityContainer& layerInputIntensityContainer
								, MapLayerRect& rectMap, AssignIntensity& addValue);
			virtual int OnRectValue(const RectI& rect, const TextureContainer::ValueType& value, const unsigned long &index, const RectI& clouseRect) ;
			virtual int OnPointValue(const RectI& rect, const TextureContainer::ValueType* const pValueBitmap, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect) ;

		private:
			ChanelIntensityContainer& m_intensityContainer;
			RayIntensityContainer& m_layerInputIntensityContainer;
			MapLayerRect& m_rectMap;
			AssignIntensity& m_addValue;
		};
		
		double m_deltaX;
		double m_deltaY;
	};

	
	///������Ҫ��Ƴ�����Ӧ�Ĳ���ģʽ����ʵ���Ż�����˵һ����Ҫ���ض��龳���ض��Ż�������ȫ��ƽ������ʱ�����excuter
	///����session��layer�ı仯�������ٷ����龳ʱ���Լ��ݻ�Ϊ��������
	class AllPlaneExcuter
		: public IlluminationExcuter
	{
	public:
		AllPlaneExcuter();
		virtual ~AllPlaneExcuter();

		
		virtual int OnAddIlluminationLayer();
		virtual int Excute();
		///�����ṩ��С���ȵ�֧�ֲַ� �ֿ鴦��Ļ���֧�֣� ���ѷֲ����������״̬�߼����������ԭ����Session�ṩ�Ľӿڹ������
		///���Ա����ػ�Sessionʹ֮���ܶ�̬���뵽�Ѵ�����ĸ߶ȵ�layer�� �������߼�����ڸ��ӣ� ����ķֲ�����״̬�߼�ʵ��
		///��StepExcuter�ж԰󶨵�DynamicLayerSession��ʵ��
		int ExcuteOnLayer(IlluminationLayer& layer, IlluminationLayer* pPreLayer, const RectI& rect);
		void Reset();
		void Swap(IlluminationLayer& layer, IlluminationLayer* pPreLayer);
		virtual IlluminationBlender* GetBlender();

		const IlluminationSession::LayerList& GetSortedLayerList();
		
	protected:
		virtual int PrepareSession();
		virtual void CleanSession(); 
		int OnUpdateLayerList();
		virtual IlluminantExcuter* CreateIlluminantExcuter(const Illuminant& refIlluminant);
		
	protected:
		bool VerifyPlane(const IlluminationLayer& refLayer);
		IlluminationSession::LayerList m_sortedLayerList;
		IlluminationBlender* m_pBlender;	
	};

	class AllPlaneBlender
		: public IlluminationBlender
	{
	public:
		AllPlaneBlender();
		~AllPlaneBlender();
		
		int OnUpdateLayerList();
		virtual int Blend(XL_BITMAP_HANDLE hCanvas);
		virtual int BlendLayer(IlluminationLayer& layer, const RectI& rect, XL_BITMAP_HANDLE hCanvas);
		virtual int CompositeLayerWithShadow(IlluminationLayer& layer,  const RectI& rect);

	private:
		class ChanelIntensityEnumtor
			: public ChanelIntensityContainer::ValueEnum
		{
		public:
			ChanelIntensityEnumtor(XL_BITMAP_HANDLE hCanvas, XL_BITMAP_HANDLE hTexture, const RectI& clouseRect);
			~ChanelIntensityEnumtor();

			virtual int OnRectValue(const RectI& rect, const ChanelIntensityContainer::ValueType& value);
			virtual int OnPointValue(const RectI& rect, const ChanelIntensityContainer::ValueType* const pValueBitmap, const unsigned long& ulLineLength);

		private:
			RectI m_textureRect;
			XL_BITMAP_HANDLE m_hTexture;
			XL_BITMAP_HANDLE m_hCanvas;
		};

		class TextureContainerEnumtor
			: public TextureContainer::ValueEnumEx
		{
		public:
			TextureContainerEnumtor(XL_BITMAP_HANDLE hCanvas, ChanelIntensityContainer& intensityContainer);
			~TextureContainerEnumtor();
			
			virtual int OnRectValue(const RectI& rect, const BitmapHandleWrap& textureHandle, const unsigned long &index, const RectI& clouseRect) ;
			virtual int OnPointValue(const RectI& rect, const BitmapHandleWrap* const phTexture, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect) ;

		private:
			XL_BITMAP_HANDLE m_hCanvas;
			ChanelIntensityContainer& m_intensityContainer;
		};
	};

}

#pragma warning(pop)

#endif //__ILLUMINATION_ALLPLANEEXCUTER_H__