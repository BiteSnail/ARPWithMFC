
// ARPDlg.h: 헤더 파일
//

#pragma once

#include "BaseLayer.h"
#include "CDeviceAdd.h"
#include "CRoutingTableEntry.h"
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
	CListCtrl		m_ListStaticRoutingTable;
	CIPAddressCtrl	m_SrcIPADDRESS;
	CIPAddressCtrl	m_DstIPADDRESS;

	CComboBox		m_ComboxAdapter;
	CEdit			m_editHWAddr;
	CEdit			m_editSrcHwAddr;
	CDeviceAdd		mDeviceAddDlg;
//	CProxyEntry     mProxyEntryDlg;
	CRoutingTableEntry mRoutingTabledlg;
public:
	void InitFn();
	void AddProxyArpCache(const int _index, unsigned char* ip, unsigned char* addr);
	void AddRoutingTable(const int _index, CString ip1, CString ip2, CString ip3, CString mFlag, CString mInterface);
	void SetTable();
	void SetComboBox();
	void updateTable();
	void SetPosition(CDialogEx& dlg, int IDD_dlg);
	/*IP주소를 받고 MAC에 저장한다.
	만약 ARP Table에 해당 IP가 존재하지 않는다면 false를 반환하고 MAC을 바꾸지 않는다.
	존재한다면 true를 반환하고 해당 IP에 해당하는 MAC ADDR을 MAC에 저장한다.*/
	bool bgetMACaddrInARP(unsigned char* IP, unsigned char* MAC);
	void GetADDRINFO(CString& MAC, CString& IP, unsigned char* srcip, CEdit& hw, CIPAddressCtrl& ip);
	void SetADDR(CString& MAC, CString& IP, unsigned char* srcip, int iosel);
	void SetVisible(CComboBox& adapt, CIPAddressCtrl& srcip, bool isON);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonItemDel();
	afx_msg void OnBnClickedButtonAllDel();
	afx_msg void OnCbnSelchangeComboAdapter();
	
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonSendArp();
	afx_msg void OnBnClickedButtonGArpSend();
	afx_msg void OnBnClickedButtonAddProxyEntry();
	afx_msg void OnBnClickedButtonAddRoutingTableEntry();
	afx_msg void OnBnClickedButtonDelRoutingTableEntry();
	afx_msg void OnCbnSelchangeComboAdapter2();
	CEdit m_editSrcHwAddr2;
	CIPAddressCtrl m_SrcIPADDRESS2;
	CComboBox m_ComboxAdapter2;
};
