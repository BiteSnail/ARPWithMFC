#pragma once
#include "afxdialogex.h"


// CDeviceAdd 대화 상자

class CDeviceAdd : public CDialogEx
{
	DECLARE_DYNAMIC(CDeviceAdd)

public:
	CDeviceAdd(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDeviceAdd();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEVICE_ADD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


public:

	virtual BOOL OnInitDialog();

	void InitDeviceAddDlg(int nTemp);

	void SetAdapterList();
	static void		UctoS(UCHAR* src, CString& dst);
public:
	CIPAddressCtrl m_IPADDRESS_DEVICE_ADD;
	CComboBox m_DeviceList;
	UCHAR m_ucSrcAddr[6];
	CString m_unSrcAddr;
	CEdit m_editDeviceEthernetAddr;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnCloseupComboDeviceList();
	afx_msg void OnCbnSelchangeComboDeviceList();
};
