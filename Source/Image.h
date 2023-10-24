#pragma once
#include "PCH.h"

struct Image
{
	ComPtr<ID2D1Bitmap> m_Bitmap;
	D2D1_POINT_2F m_Pivot = {};
	D2D1_SIZE_F m_Size = {};
};

Image CreateTextImage(ID2D1DeviceContext* dc, const std::wstring& text, float width, float height, float fontsize, float verticaloffset);
