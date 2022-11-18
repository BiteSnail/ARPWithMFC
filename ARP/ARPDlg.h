
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

private:
	CLayerManager	m_LayerMgr;
public:
	CIPLayer*		m_IPLayer;
	CARPLayer*		m_ARPLayer;
	CEthernetLayer*	m_EtherLayer;
	CNILayer*		m_NILayer;
	CNILayer*		m_Network;

	CListCtrl		m_ListARPTable;
	CListCtrl		m_ctrlListControlProxy;
	CIPAddressCtrl	m_SrcIPADDRESS;
	CIPAddressCtrl	m_DstIPADDRESS;

	CComboBox		m_ComboxAdapter;
	CEdit			m_editHWAddr;
	CEdit			m_editSrcHwAddr;
	CDeviceAdd		mDeviceAddDlg;

public:
	void InitFn();
	void AddProxyArpCache(TCHAR* _Device, TCHAR* _ip, TCHAR* _Ethernet);
	void SetTable();
	void SetComboBox();
	void updateTable();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonItemDel();
	afx_msg void OnBnClickedButtonAllDel();
	afx_msg void OnCbnSelchangeComboAdapter();
	
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonSendArp();
	afx_msg void OnBnClickedButtonGArpSend();
};
