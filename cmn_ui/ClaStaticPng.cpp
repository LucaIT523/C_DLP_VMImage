#include "pch.h"
#include "ClaStaticPng.h"

ClaStaticPng::ClaStaticPng() : m_image(nullptr)
{
}

ClaStaticPng::~ClaStaticPng()
{
    delete m_image;
}

void ClaStaticPng::LoadPng(UINT resourceID)
{
    HRSRC hResource = FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(resourceID), L"PNG");
    if (hResource != nullptr)
    {
        HGLOBAL hLoadedResource = LoadResource(AfxGetInstanceHandle(), hResource);
        DWORD resourceSize = SizeofResource(AfxGetInstanceHandle(), hResource);
        void* pResource = LockResource(hLoadedResource);

        // Create a global memory handle for the stream
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, resourceSize);
        if (hGlobal)
        {
            void* pGlobalMemory = GlobalLock(hGlobal);
            if (pGlobalMemory)
            {
                // Copy the resource data into the global memory
                memcpy(pGlobalMemory, pResource, resourceSize);
                GlobalUnlock(hGlobal);

                // Create a stream on the global memory
                IStream* pStream = nullptr;
                HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
                if (SUCCEEDED(hr) && pStream)
                {
                    // Load the bitmap from the stream
                    m_image = Bitmap::FromStream(pStream);
                    pStream->Release();
                }
            }
        }

        // Free the loaded resource
        FreeResource(hLoadedResource);
    }
}

BEGIN_MESSAGE_MAP(ClaStaticPng, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()


void ClaStaticPng::OnPaint()
{
    CRect rt;
    GetClientRect(rt);

    CPaintDC dc(this); // device context for painting
    if (m_image)
    {
        Graphics graphics(dc.m_hDC);
        graphics.DrawImage(m_image, 0, 0, rt.Width(), rt.Height());
    }
}
