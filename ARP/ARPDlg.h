
// ARPDlg.h: 헤더 파일
//

#pragma once

#include "LayerManager.h"
#include "ARPLayer.h"
#include "IPLayer.h"
#include "EthernetLayer.h"
#include "NILayer.h"

#define DEFAULT_EDIT_TEXT "-"

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
	//Object
	CIPLayer*		m_IPLayer;
	CARPLayer*		m_ARPLayer;
	CEthernetLayer*	m_EtherLayer;
	CNILayer*		m_NILayer;

	CListCtrl		m_ListARPTable;
	CComboBox		m_ComboAdapter;
	CIPAddressCtrl	m_Addr;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeComboAdapter();
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedSend();

	void SetTable();
	void SetComboBox();
	void updateTable();
};
