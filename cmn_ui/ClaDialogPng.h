#pragma once

#include <afxwin.h>
#include <gdiplus.h>
using namespace Gdiplus;


class ClaDialogPng : public CDialog
{
    DECLARE_DYNAMIC(ClaDialogPng)

public:
    ClaDialogPng(UINT nIDTemplate, CWnd* pParent = nullptr)
        : CDialog(nIDTemplate, pParent), m_pBitmap(nullptr)
    {
        // Initialize GDI+
        colorBG = RGB(241, 247, 254);
        _nBgID = -1;
        _nTitleHeight = 50;
    }

    ~ClaDialogPng()
    {
        // Cleanup
        if (m_pBitmap) DeleteObject(m_pBitmap);
    }

public:
    UINT _nBgID;
    void setBackground(UINT p_nBG) { _nBgID = p_nBG; };
    COLORREF colorBG; 
	COLORREF colorBD;
	COLORREF colorTITLE;
    void setColor(COLORREF bg, COLORREF bd, COLORREF title) { 
        colorBG = bg; 
        colorBD = bd;
        colorTITLE = title;
    };
    int _nTitleHeight;
    void setTitleHeight(int h) { _nTitleHeight = h; };

private:
    Bitmap* m_pBitmap;  // Handle to the bitmap

protected:
    virtual BOOL OnInitDialog()
    {
        CDialog::OnInitDialog();

        if (_nBgID != -1)
        {
            HRSRC hResource = FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(_nBgID), L"PNG");
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
                            m_pBitmap = Bitmap::FromStream(pStream);
                            pStream->Release();
                        }
                    }
                }

                // Free the loaded resource
                FreeResource(hLoadedResource);
            }
        }

        return TRUE;
    }
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg LRESULT OnNcHitTest(CPoint point);
};

