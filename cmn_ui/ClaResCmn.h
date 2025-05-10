#pragma once

#include <afxwin.h>
#include <gdiplus.h>

using namespace Gdiplus;


class ClaResCmn
{
public:
	ClaResCmn() {};
	~ClaResCmn() {};

public:
	static void GetImage(UINT p_nID, Bitmap** pBitmap) {
        HRSRC hResource = FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(p_nID), L"PNG");
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
                        *pBitmap = Bitmap::FromStream(pStream);
                        pStream->Release();
                    }
                }
            }

            // Free the loaded resource
            FreeResource(hLoadedResource);
        }
	}
};

