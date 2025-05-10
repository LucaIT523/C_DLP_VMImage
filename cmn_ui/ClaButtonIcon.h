#pragma once

class ClaButtonIcon : public CButton
{
    DECLARE_DYNAMIC(ClaButtonIcon)

public:
    ClaButtonIcon();
    virtual ~ClaButtonIcon();

    void SetIcon(HICON hIcon);

protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

private:
    HICON m_hIcon;
};