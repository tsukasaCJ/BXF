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

	
	///这里想要设计成自适应的策略模式，从实现优化上来说一定需要对特定情境做特定优化，满足全是平行特性时用这个excuter
	///监听session上layer的变化，当不再符合情境时，自己演化为其他策略
	class AllPlaneExcuter
		: public IlluminationExcuter
	{
	public:
		AllPlaneExcuter();
		virtual ~AllPlaneExcuter();

		
		virtual int OnAddIlluminationLayer();
		virtual int Excute();
		///这里提供最小粒度的支持分步 分块处理的基本支持， 不把分步处理的完整状态逻辑放在这里的原因是Session提供的接口过于灵活
		///所以必须特化Session使之不能动态插入到已处理过的高度的layer， 否则处理逻辑会过于复杂， 具体的分步完整状态逻辑实现
		///在StepExcuter中对绑定到DynamicLayerSession上实现
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