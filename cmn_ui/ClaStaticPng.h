#pragma once

#include <gdiplus.h>

using namespace Gdiplus;

class ClaStaticPng : public CStatic
{
public:
    ClaStaticPng();
    virtual ~ClaStaticPng();

    void LoadPng(UINT resourceID);

private:
    Bitmap* m_image;
    ULONG_PTR m_gdiplusToken;
    GdiplusStartupInput m_gdiplusStartupInput;
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
};