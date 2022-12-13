#pragma once
#include "afxdialogex.h"


// CRoutingTableEntry 대화 상자

class CRoutingTableEntry : public CDialogEx
{
	DECLARE_DYNAMIC(CRoutingTableEntry)

public:
	CRoutingTableEntry(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CRoutingTableEntry();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ROUTING_TABLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CIPAddressCtrl m_IPADDRESS_Destination;
	CIPAddressCtrl m_IPADDRESS_NetMask;
	CIPAddressCtrl m_IPADDRESS_GateWay;
	CButton m_RoutingCheckUp;
	CButton m_RoutingCheckDown;
	CButton m_RoutingCheckHost;

	CComboBox m_RoutingInterface;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
