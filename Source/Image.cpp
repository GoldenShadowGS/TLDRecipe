#include "PCH.h"
#include "Image.h"
#include "Renderer.h"

Image CreateTextImage(ID2D1DeviceContext* dc, const std::wstring& text, float width, float height, float fontsize, float verticaloffset)
{
	Image image = {};
	image.m_Size = { width, height };
	image.m_Pivot = { width / 2.0f, height / 2.0f };
	image.m_Bitmap;
	{
		ComPtr<IDWriteFactory> DWriteFactory;
		ComPtr<IDWriteTextFormat> TextFormat;

		HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(DWriteFactory.ReleaseAndGetAddressOf())));

		HR(DWriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontsize, L"en-us", TextFormat.ReleaseAndGetAddressOf()));

		TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		ComPtr<ID2D1BitmapRenderTarget> BitmapRenderTarget;
		HR(dc->CreateCompatibleRenderTarget(image.m_Size, &BitmapRenderTarget));

		ComPtr<ID2D1SolidColorBrush> Brush;
		HR(BitmapRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f), Brush.ReleaseAndGetAddressOf()));

		BitmapRenderTarget->BeginDraw();
		BitmapRenderTarget->Clear();

		BitmapRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(image.m_Pivot.x, image.m_Pivot.y));

		D2D1_RECT_F rect = { -image.m_Pivot.x, -image.m_Pivot.y + verticaloffset, image.m_Pivot.x, image.m_Pivot.y + verticaloffset };

		BitmapRenderTarget->DrawTextW(text.c_str(), (UINT)text.size(), TextFormat.Get(), rect, Brush.Get());

		HR(BitmapRenderTarget->EndDraw());

		HR(BitmapRenderTarget->GetBitmap(image.m_Bitmap.ReleaseAndGetAddressOf()));
	}
	return image;
}