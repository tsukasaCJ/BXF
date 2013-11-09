#ifndef __ILLUMINATION_H__
#define __ILLUMINATION_H__

#include <XLGraphic.h>
#include "./IlluminationDefine.h"
#include "./RectContainer.h"

#pragma warning(push)
#pragma warning(disable: 4512)
namespace IlluminationImpl
{

	////ģ�����治�������䣬��Ϊ������û����������ߵ�ģ��~
	struct IlluminatProp{
		unsigned char byTrans;
	};
	typedef RectContainer<IlluminatProp> IlluminatPropContainer;

	typedef unsigned char RayIntensity;
	typedef RectContainer<RayIntensity> RayIntensityContainer;
	class RayIntensityOperator
	{
	public:
		static const RayIntensity MaxRayIntensity = 255;
		inline static RayIntensity GetRayIntensity(RayIntensity inputIntensity, IlluminatProp inputProp, RayIntensity& outputRayIntensity)
		{
			int transIntensity = ((int)inputIntensity * inputProp.byTrans) / 255;
			outputRayIntensity = static_cast<RayIntensity>(inputIntensity - transIntensity);
			return static_cast<unsigned char>(transIntensity);
		}
	};

	////��������Ϳ��ǹ�Դ����ɫ�����ǵ����Ĺ�ǿ
	typedef XL_Color ChanelIntensity;
	typedef RectContainer<ChanelIntensity> ChanelIntensityContainer;
	
	/////�򲻵��� ֻ������ ��������
	class BitmapHandleWrap
	{
	public:
		BitmapHandleWrap(XL_BITMAP_HANDLE hBitmap = XLGRAPHIC_INVALID_HANDLE)
			: m_hBitmap(hBitmap)
		{
		/*	if (m_hBitmap != XLGRAPHIC_INVALID_HANDLE)
			{
				XL_AddRefBitmap(m_hBitmap);
			}*/
		}	
		BitmapHandleWrap(const BitmapHandleWrap& right)
			: m_hBitmap(right.m_hBitmap)
		{
			/*if (m_hBitmap != XLGRAPHIC_INVALID_HANDLE)
			{
				XL_AddRefBitmap(m_hBitmap);
			}*/
		}
		BitmapHandleWrap& operator=(const BitmapHandleWrap& right)
		{
			/*if(m_hBitmap != XLGRAPHIC_INVALID_HANDLE)
			{
				XL_ReleaseBitmap(m_hBitmap);
			}*/
			m_hBitmap = right.m_hBitmap;
			/*
			if (m_hBitmap != XLGRAPHIC_INVALID_HANDLE)
			{
				XL_AddRefBitmap(m_hBitmap);
			}*/
			return *this;
		}
		~BitmapHandleWrap()
		{
			/*if (m_hBitmap != XLGRAPHIC_INVALID_HANDLE)
			{
				XL_ReleaseBitmap(m_hBitmap);
			}*/
		}
		
		operator XL_BITMAP_HANDLE() const
		{
			return m_hBitmap;
		}
	private:
		XL_BITMAP_HANDLE m_hBitmap;
	};
	typedef RectContainer<BitmapHandleWrap> TextureContainer;
	typedef RectContainer<XL_Color> TextureBitmapContainer; 


	class Illuminant
	{
	public:
		Illuminant();
		virtual ~Illuminant();

	public:
		virtual int GetType() const = 0;

		//////Aͨ��ֵ�����Դ�Ĺ�ǿ
		XL_Color GetColor() const;
		void SetColor(XL_Color clr) ;
		RayIntensity GetIntensity() const;

		virtual Ray GetRayOnPos(Point3I& ptPos) const = 0;
	protected:
		XL_Color m_clr;

	};

	class IlluminationLayer
	{
	public:
		IlluminationLayer();
		~IlluminationLayer();

	public:
		////����������ӿڵĿɼ����򣬳��������������ݲ��ᱻblender�����������б����������RectΪ׼
		void SetClouseRect(const RectI& rect);
		const RectI& GetClouseRect() const;
		void SetDirection(const Vector3I& direction);
		////���ƽ����ĳ����
		const Vector3I& GetDirection() const;
		
		const IlluminatPropContainer& GetPropContainer() const;
		IlluminatPropContainer& GetPropContainer();
		const ChanelIntensityContainer& GetRayIntensityContainer() const;
		ChanelIntensityContainer& GetRayIntensityContainer();
		const TextureContainer& GetTextureContainer() const; 
		TextureContainer& GetTextureContainer();
	private:
		IlluminatPropContainer m_propContainer;
		ChanelIntensityContainer m_intensityContainer;
		TextureContainer m_textureContainer;
		Vector3I m_direction;
		RectI m_clouseRect;/////����հ���������layer��ƽ������ϵ������
		
	public:
		void SetIsUseAlphaAsProp(bool isUseAlphaAsProp);
		bool GetIsUseAlphaAsProp() const;
		TextureBitmapContainer& GetTextureBitmapContainer();

	private:
		class TextureEnumtor
			: public TextureContainer::ValueEnumEx
		{
		public:
			TextureEnumtor(TextureBitmapContainer& textureBitmapContainer);
			virtual int OnRectValue(const RectI& rect, const BitmapHandleWrap& textureHandle, const unsigned long &index, const RectI& clouseRect);
			virtual int OnPointValue(const RectI& rect, const BitmapHandleWrap* const pValueBitmap, const unsigned long& ulLineLength, const unsigned long &index, const RectI& clouseRect);

		private:
			TextureBitmapContainer& m_textureBitmapContainer;
		};

		bool m_isUseAlphaAsProp;
		TextureBitmapContainer m_textureBitmapContainer;

	};


	
	class IlluminationSession
	{
	public:
		static const int RetSuc = 0;
		static const int RetNoImpl = 1;
	public:
		IlluminationSession();
		virtual ~IlluminationSession();
		
		class ISessionEvent
		{
		public:
			virtual ~ISessionEvent(){}
			virtual int OnAddIlluminant(IlluminationSession& refSession) = 0;
			virtual int OnAddIlluminationLayer(IlluminationSession& refSession) = 0;

			virtual int OnRemoveIlluminant(IlluminationSession& refSession, Illuminant& refIlluminant) = 0;
			virtual int OnRemoveIlluminationLayer(IlluminationSession& refIlluminationLayer, IlluminationLayer& refLayer) = 0;
		};

		typedef std::list<Illuminant*> IlluminantList;
		typedef std::list<IlluminationLayer*> LayerList;

		///����cookie
		int AttachListener(ISessionEvent& refEvent, unsigned long* pCookie);
		int DetachListener(unsigned long ulCookie);

		int AddIlluminant(Illuminant& refIlluminant);
		////��Ϊ���ܳ���б�� ���ﲻ���κ�sort �����ӵ���βȥ ��Ҫ�ػ���sort�Ļ� ���¼����洦��
		virtual int AddIlluminationLayer(IlluminationLayer& refLayer);

		int RemoveIllumnant(Illuminant& refIlluminant);
		virtual int RemoveIlluminationLayer(IlluminationLayer& refLayer);

		const IlluminantList& GetIlluminantList() const;
		const LayerList& GetLayerList() const;
	private:
		struct ListenerStub{
			ISessionEvent* pListener;
			unsigned long ulCookie;
		};
		typedef std::list<ListenerStub> ListenerList;
		IlluminantList m_listIlluminants;
		LayerList m_listLayers;
		ListenerList m_listListeners;
		unsigned long m_ulNewEventCookie;
	};


	class IlluminantExcuter;
	class IlluminationBlender;
	class IlluminationExcuter
		: public IlluminationSession::ISessionEvent 
	{
	public:
		IlluminationExcuter();
		virtual ~IlluminationExcuter();

		int SetIlluminationSession(IlluminationSession& refSession);
		IlluminationSession& GetIlluminationSession();
		virtual int Excute();

		virtual int OnAddIlluminant(IlluminationSession& refSession);
		virtual int OnAddIlluminationLayer(IlluminationSession& refSession);
		virtual int OnRemoveIlluminant(IlluminationSession& refSession, Illuminant& refIlluminant) ;
		virtual int OnRemoveIlluminationLayer(IlluminationSession& refIlluminationLayer, IlluminationLayer& refLayer) ;

	protected:
		virtual int PrepareSession();
		virtual void CleanSession();

		typedef std::list<IlluminantExcuter*> IlluminantExcuterList;
		virtual IlluminantExcuter* CreateIlluminantExcuter(const Illuminant& refIlluminant);
		virtual void DestroyIlluminantExcuter(IlluminantExcuter* pExcuter);
		const IlluminantExcuterList& GetIlluminantExcuterList();
		virtual IlluminationBlender* GetBlender() = 0;

	
	protected:
		IlluminantExcuterList m_listIlluminantExcuters;
		IlluminationSession* m_pSession;
		unsigned long m_ulSessionEventCookie;
	};


	class IlluminantExcuter
	{
	public:
		IlluminantExcuter();
		virtual ~IlluminantExcuter();

		virtual int SetSessionExcuter(IlluminationExcuter& refSessionExcuter);
		IlluminationExcuter& GetSessionExcuter();
		virtual int SetIlluminant(const Illuminant& refIllunminant);
		const Illuminant& GetIlluminant();
		virtual int Excute();
		
	protected:
		IlluminationExcuter* m_pSessionExcuter;
		const Illuminant* m_pIlluminant;
		
	};
	


	class IlluminationBlender
	{
	public:
		IlluminationBlender();
		~IlluminationBlender();

		int SetExcuter(IlluminationExcuter& refExcuter);
		virtual int Blend(XL_BITMAP_HANDLE hCanvas);
		virtual int BlendLayer(IlluminationLayer& layer,  const RectI& rect, XL_BITMAP_HANDLE hCanvas);
		virtual int CompositeLayerWithShadow(IlluminationLayer& layer,  const RectI& rect);

		////���õ��㷨�� ����Ͳ���ͼ�ο�����ô�������߽��ж��ˣ������ü�����src alpha,
		///�ϲ�ᱣ֤ShadowBitmap��destBitmapһ����,����src���ᳬ��dst
		static XL_BITMAP_HANDLE BlendWithShadow(XL_BITMAP_HANDLE hDstBmp, Point2I ptPos
			, XL_BITMAP_HANDLE hSrcBmp, const XL_Color* pChanelIntensity, unsigned long chanelColOffset
			, unsigned long chanelLineOffset,  XLBlendHint* pBlendHint);
		static XL_BITMAP_HANDLE CompositeShadow(XL_BITMAP_HANDLE hBmp, const XL_Color* pChanelIntensity, unsigned long chanelColOffset, unsigned long chanelLineOffset);
	protected:
		IlluminationExcuter* m_pExcuter;
	};


	class EnvIlluminant
		: public Illuminant
	{
	public:
		EnvIlluminant();
		virtual ~EnvIlluminant();

		const static int IlluminantType = 0;
		virtual int GetType() const
		{
			return IlluminantType;
		}
		virtual Ray GetRayOnPos(Point3I& ptPos) const ;
		void SetDirection(const Direction3I& vecDirection);
		const Direction3I& GetDirection() const; 
	protected:
		Direction3I m_vecDirection;
	};




}

#pragma warning(pop)

#endif //__ILLUMINATION_H__