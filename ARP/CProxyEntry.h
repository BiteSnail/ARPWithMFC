#pragma once
#include "afxdialogex.h"


// CProxyEntry 대화 상자

class CProxyEntry : public CDialogEx
{
	DECLARE_DYNAMIC(CProxyEntry)

public:
	CProxyEntry(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CProxyEntry();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEVICE_ADD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ProxyDeviceList;
	CEdit m_editProxyEthernetAddr;
	CIPAddressCtrl m_ProxyIpAddress;
	virtual BOOL OnInitDialog();
};
