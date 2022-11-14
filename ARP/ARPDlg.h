
// ARPDlg.h: 헤더 파일
//

#pragma once

#include "BaseLayer.h"
#include "CDeviceAdd.h"

// CARPDlg 대화 상자
class CARPDlg : public CDialogEx, public CBaseLayer
{
// 생성입니다.
public:
	CARPDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ARP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	void InitFn();
	void AddArpCache(TCHAR* _ip, TCHAR* _Ethernet, TCHAR* _Status);
	void AddProxyArpCache(TCHAR* _Device, TCHAR* _ip, TCHAR* _Ethernet);
public:
	CLayerManager	m_LayerMgr;
	CNILayer* m_Network;
	CListCtrl m_ctrlListControl;
	CListCtrl m_ctrlListControlProxy;
	CIPAddressCtrl m_IPADDRESS;
	CIPAddressCtrl m_IPADDRESS2;
	
	CEdit m_editHWAddr;
	CEdit m_edit1;
	CComboBox m_Combox1;
	CDeviceAdd mDeviceAddDlg;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonItemDel();
	afx_msg void OnBnClickedButtonAllDel();
};
