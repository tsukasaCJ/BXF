#ifndef __ILLUMINATIONIMPLTEST_H__
#define __ILLUMINATIONIMPLTEST_H__

#include <string>
#include <XLGraphic.h>
#include "./AllPlaneExcuter.h"
#include "./StepExcuter.h"


namespace IlluminationImpl
{

class TestGraphic
{
public:
	class TextureContainerEnumtor
		: public TextureContainer::ValueEnum
	{
	public:
		TextureContainerEnumtor(IlluminatPropContainer& propContainer)
			: m_propContainer(propContainer)
		{

		}
		~TextureContainerEnumtor()
		{

		}

		virtual int OnRectValue(const RectI& rect, const BitmapHandleWrap& bitmapHandle) 
		{
			XL_BITMAP_HANDLE hBmp = bitmapHandle;
			if(hBmp == NULL)
			{
				return TextureContainer::RetSuc;
			}
			unsigned long index = 0;
			m_propContainer.AddPointValue(rect, index);
			IlluminatProp* pPropBitmap = NULL;
			unsigned long propLineLength = 0;
			m_propContainer.GetPointValueBitmap(index, pPropBitmap, propLineLength);
			XLBitmapInfo bitmapInfo;
			XL_GetBitmapInfo(hBmp, &bitmapInfo);
			BYTE* pBitmapBuf = XL_GetBitmapBuffer(hBmp, 0, 0);
			
			BYTE* iterPropLine = reinterpret_cast<BYTE*>(pPropBitmap);
			IlluminatProp* iterProp = NULL;
			BYTE* iterBitmapLine = pBitmapBuf;
			BYTE* iterBitmap = NULL;

			for(int iy = 0; iy < rect.bottom - rect.top; ++iy)
			{
				iterProp = reinterpret_cast<IlluminatProp*>(iterPropLine);
				iterBitmap = iterBitmapLine;
				for(int ix = 0; ix < rect.right - rect.left; ++ix)
				{
					iterProp->byTrans = iterBitmap[3];

					++iterProp;
					iterBitmap += sizeof(XL_Color);
				}
				iterPropLine += propLineLength;
				iterBitmapLine += bitmapInfo.ScanLineLength;
			}

			return TextureContainer::RetSuc;
		}
		virtual int OnPointValue(const RectI& rect, const BitmapHandleWrap* const phBitmap, const unsigned long& ulLineLength)
		{
			assert(0);
			return TextureContainer::RetSuc;
		}

	private:
		IlluminatPropContainer& m_propContainer;
	};
	static void SetPropWithTexture(IlluminationLayer& layer)
	{
		TextureContainer& textureContainer = layer.GetTextureContainer();
		IlluminatPropContainer& propContainer = layer.GetPropContainer();
		IlluminatProp defaultProp = {0};
		propContainer.SetDefaultValue(defaultProp);

		TextureContainerEnumtor enumtor(propContainer);
		textureContainer.EnumInRect(layer.GetClouseRect(), enumtor);
	}

	static XL_BITMAP_HANDLE Do()
	{
		IlluminationSession testSession;
		IlluminationLayer bkgLayer;
		RectI clouseRect = {0,0,800,600};

		Vector3I direction;
		bkgLayer.SetClouseRect(clouseRect);
		direction.start2direction.start.x = 0;
		direction.start2direction.start.y = 0;
		direction.start2direction.start.z = 0;
		direction.start2direction.direction.x = 0;
		direction.start2direction.direction.y = 0;
		direction.start2direction.direction.z = 0;
		bkgLayer.SetDirection(direction);
		TextureContainer& textureContainer = bkgLayer.GetTextureContainer();
		XL_BITMAP_HANDLE hBmp = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 800, 600);
		XL_FillBitmap(hBmp, XLCOLOR_BGRA(255,255,255,255));
		unsigned long index;
		textureContainer.AddRectValue(clouseRect, hBmp, index);
		SetPropWithTexture(bkgLayer);
		testSession.AddIlluminationLayer(bkgLayer);
		

		RectI bmpPos;
		/*IlluminationLayer layer1;
		layer1.SetClouseRect(clouseRect);
		direction.start2direction.start.z = 20;
		layer1.SetDirection(direction);
		hBmp = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 100, 100);
		XL_FillBitmap(hBmp, XLCOLOR_BGRA(255, 0, 0, 255));
		 bmpPos.left = 300;
		 bmpPos.top = 300;
		 bmpPos.right = 400;
		 bmpPos.bottom = 400;
		layer1.GetTextureContainer().AddRectValue(bmpPos, hBmp, index);
		SetPropWithTexture(layer1);
		testSession.AddIlluminationLayer(layer1);*/



		IlluminationLayer layer2;
		layer2.SetClouseRect(clouseRect);
		direction.start2direction.start.z = 30;
		layer2.SetDirection(direction);
		hBmp = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 100, 100);
		XL_FillBitmap(hBmp, XLCOLOR_BGRA(0, 255, 0, 255));
		bmpPos.left = 320;
		bmpPos.top = 320;
		bmpPos.right = 420;
		bmpPos.bottom = 420;
		layer2.GetTextureContainer().AddRectValue(bmpPos, hBmp, index);
		SetPropWithTexture(layer2);
		testSession.AddIlluminationLayer(layer2);


		IlluminationLayer layer3;
		layer3.SetClouseRect(clouseRect);
		direction.start2direction.start.z = 40;
		layer3.SetDirection(direction);
		hBmp = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 100, 100);
		XL_FillBitmap(hBmp, XLCOLOR_BGRA(0, 0, 255, 125));
		bmpPos.left = 400;
		bmpPos.top = 400;
		bmpPos.right = 500;
		bmpPos.bottom = 500;
		layer3.GetTextureContainer().AddRectValue(bmpPos, hBmp, index);
		//SetPropWithTexture(layer3);
		layer3.SetIsUseAlphaAsProp(true);
		testSession.AddIlluminationLayer(layer3);
		
		
		
		Direction3I iDirection;
		EnvIlluminant illuminant1;
		
		iDirection.x = -1;
		iDirection.y = -1;
		iDirection.z = -1;
		illuminant1.SetDirection(iDirection);
		illuminant1.SetColor(XLCOLOR_BGRA(255,255,255,125));
		testSession.AddIlluminant(illuminant1);


		EnvIlluminant illuminant2;
		iDirection.x = 0;
		iDirection.y = 0;
		iDirection.z = -1;
		illuminant2.SetDirection(iDirection);
		illuminant2.SetColor(XLCOLOR_BGRA(255,255,255,125));
		testSession.AddIlluminant(illuminant2);

		

		XL_BITMAP_HANDLE hCanvas = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 800, 600);
		XL_FillBitmap(hCanvas, XLCOLOR_BGRA(255, 255, 255, 255));
		AllPlaneExcuter excuter;
		excuter.SetIlluminationSession(testSession);
		excuter.Excute();
		//excuter.GetBlender()->BlendLayer(bkgLayer, hCanvas);
		excuter.GetBlender()->Blend(hCanvas);
		
		
		return hCanvas;
	}



	static XL_BITMAP_HANDLE DoStep()
	{
		DynamicLayerSession testSession;
		RectI bmpPos;
		bmpPos.left = 0;
		bmpPos.top = 0;
		bmpPos.right = 800;
		bmpPos.bottom = 600;
		testSession.SetClouseRect(bmpPos);
		StepExcuter stepExcuter;
		stepExcuter.SetIlluminationSession(testSession);


		XL_BITMAP_HANDLE hCanvas = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 800, 600);
		XL_FillBitmap(hCanvas, XLCOLOR_BGRA(255, 255, 255, 255));


		Direction3I iDirection;
		EnvIlluminant illuminant1;

		iDirection.x = -1;
		iDirection.y = -1;
		iDirection.z = -1;
		illuminant1.SetDirection(iDirection);
		illuminant1.SetColor(XLCOLOR_BGRA(255,255,255,125));
		testSession.AddIlluminant(illuminant1);


		EnvIlluminant illuminant2;
		iDirection.x = 0;
		iDirection.y = 0;
		iDirection.z = -1;
		illuminant2.SetDirection(iDirection);
		illuminant2.SetColor(XLCOLOR_BGRA(255,255,255,125));
		testSession.AddIlluminant(illuminant2);


		XL_BITMAP_HANDLE hBmp;
		hBmp = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 100, 100);
		XL_FillBitmap(hBmp, XLCOLOR_BGRA(0, 0, 255, 125));
		RectI bmpPos1;
		bmpPos1.left = 400;
		bmpPos1.top = 400;
		bmpPos1.right = 500;
		bmpPos1.bottom = 500;
		stepExcuter.ExcuteOnHeight(50, hBmp, bmpPos1);


		hBmp = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 100, 100);
		XL_FillBitmap(hBmp, XLCOLOR_BGRA(0, 255, 0, 255));
		RectI bmpPos2;
		bmpPos2.left = 320;
		bmpPos2.top = 320;
		bmpPos2.right = 420;
		bmpPos2.bottom = 420;
		stepExcuter.ExcuteOnHeight(30, hBmp, bmpPos2);


		hBmp = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 800, 600);
		XL_FillBitmap(hBmp, XLCOLOR_BGRA(255,255,255,255));
		RectI bmpPos3;
		bmpPos3.left = 0;
		bmpPos3.top = 0;
		bmpPos3.right = 800;
		bmpPos3.bottom = 600;
		stepExcuter.ExcuteOnHeight(0, hBmp, bmpPos3);
		

		stepExcuter.BlendOnHeight(hCanvas, 0, bmpPos3);
		stepExcuter.BlendOnHeight(hCanvas, 30, bmpPos2);
		stepExcuter.BlendOnHeight(hCanvas, 50, bmpPos1);
		
		
		return hCanvas;
	}
	class TestValueEnumtor
		: public RayIntensityContainer::ValueEnum
	{
	public:
		TestValueEnumtor(XL_BITMAP_HANDLE hCanvas)
			: m_hCanvas(hCanvas)
		{

		}
		virtual int OnRectValue(const RectI& rect, const RayIntensity& rayInensity) 
		{
			RECT blendRect = {rect.left, rect.top, rect.right, rect.bottom};
			XL_BITMAP_HANDLE hBlendBitmap = XL_ClipSubBindBitmap(m_hCanvas, &blendRect);
			XL_FillBitmap(hBlendBitmap, XLCOLOR_BGRA(rayInensity, rayInensity, rayInensity, rayInensity, 255));

			XL_ReleaseBitmap(hBlendBitmap);
			return RayIntensityContainer::RetSuc;
		}
		virtual int OnPointValue(const RectI& rect, const RayIntensity const *pRayInensity, const unsigned long& ulLineLength)
		{
			assert(0);
			return RayIntensityContainer::RetSuc;
		}

	private:
		XL_BITMAP_HANDLE m_hCanvas;
	};


	class AssignRect
		: public RectContainerRectMap
	{
	public:
		virtual RectI& Map(const RectI& thisRect, RectI& mapRect) 
		{
			mapRect = thisRect;
			return mapRect;
		}
		virtual RectI& ReverseMap(const RectI& mapRect, RectI& thisRect) 
		{
			thisRect = mapRect;
			return thisRect;
		}
	};

	class AssignValue
		: public RayIntensityContainer::AddValue<RayIntensity>
	{
	public:
		virtual RayIntensity& operator()(RayIntensity& leftValue, const RayIntensity& rightValue) 
		{
			leftValue = rightValue;
			return leftValue;
		}
	};


	static XL_BITMAP_HANDLE TestRectContainer()
	{
		RayIntensityContainer container;
		RectI rect = {300,300,400,400};
		unsigned long index;
		container.SetDefaultValue(255);
		

		container.AddRectValue(rect, 125, index);



		RayIntensityContainer container2;
	
		container2.SetDefaultValue(0);
		rect.left = 320;
		rect.top = 320;
		rect.right = 380;
		rect.bottom = 380;
		container2.AddRectValue(rect, 255, index);


		rect.left = 100;
		rect.top = 100;
		rect.right = 500;
		rect.bottom = 500;
		AssignRect rectMap;
		AssignValue addValue;
		container.Add<RayIntensity>(container2, rect, rectMap, addValue);

		XL_BITMAP_HANDLE hCanvas = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, 800, 600);
		TestValueEnumtor enumtor(hCanvas);
		container2.SetDefaultValue(0);
		rect.left = 0;
		rect.top = 0;
		rect.right = 800;
		rect.bottom = 600;
		container.EnumInRect(rect, enumtor);
		return hCanvas;
	}


	


	static void SaveBitmap(XL_BITMAP_HANDLE hResult, const wchar_t* szName)
	{
		XL_BITMAP_HANDLE dibForTest = hResult;
		XLBitmapInfo dibInfoForTest;
		XL_GetBitmapInfo(dibForTest,&dibInfoForTest);
		BITMAPFILEHEADER fileHeader;
		unsigned long nSizeImage = 32 * dibInfoForTest.Height * dibInfoForTest.Width;
		static const unsigned int nSizeofHead = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fileHeader.bfOffBits = nSizeofHead;
		fileHeader.bfReserved1 = fileHeader.bfReserved2 = 0;
		fileHeader.bfType = 0x4d42;
		fileHeader.bfSize = nSizeImage + nSizeofHead;
		BITMAPINFOHEADER infoHeader;
		infoHeader.biBitCount = 32;
		infoHeader.biCompression = BI_RGB;
		infoHeader.biWidth = dibInfoForTest.Width;
		infoHeader.biHeight = dibInfoForTest.Height;
		infoHeader.biPlanes = 1;
		infoHeader.biXPelsPerMeter = 1;
		infoHeader.biYPelsPerMeter = 1;
		infoHeader.biSize = sizeof(BITMAPINFOHEADER);
		infoHeader.biClrUsed = 0;
		infoHeader.biSizeImage = 0;

		std::wstring strName = L"D:\\";
		strName = strName + szName;
		strName = strName + L".bmp";
		HANDLE outputFile = ::CreateFile(strName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (outputFile == INVALID_HANDLE_VALUE)
		{
			return;
		}
		DWORD dwBytesWrite = 0;
		::WriteFile(outputFile,&fileHeader,sizeof(BITMAPFILEHEADER),&dwBytesWrite,NULL);
		::WriteFile(outputFile,&infoHeader,sizeof(BITMAPINFOHEADER),&dwBytesWrite,NULL);
		for (int ix = dibInfoForTest.Height - 1;ix > -1 ;ix--)
		{
			::WriteFile(outputFile,XL_GetBitmapBuffer(dibForTest,0,ix),dibInfoForTest.ScanLineLength,&dwBytesWrite,NULL);
		}
		::CloseHandle(outputFile);
	}
};


}


#endif //__ILLUMINATIONIMPLTEST_H__