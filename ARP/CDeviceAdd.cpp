// CDeviceAdd.cpp: 구현 파일
//

#include "pch.h"
#include "ARP.h"
#include "afxdialogex.h"
#include "CDeviceAdd.h"


// CDeviceAdd 대화 상자

IMPLEMENT_DYNAMIC(CDeviceAdd, CDialogEx)

CDeviceAdd::CDeviceAdd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DEVICE_ADD, pParent)
{

}

CDeviceAdd::~CDeviceAdd()
{
}

void CDeviceAdd::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_DEVICE_LIST, m_DeviceList);
	DDX_Control(pDX, IDC_EDIT_DEVICE_ETHERNET_ADDRESS, m_editDeviceEthernetAddr);
	DDX_Control(pDX, IDC_IPADDRESS_DEVICE_ADD, m_IPADDRESS_DEVICE_ADD);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDeviceAdd, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDeviceAdd::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDeviceAdd::OnBnClickedCancel)
	ON_CBN_CLOSEUP(IDC_COMBO_DEVICE_LIST, &CDeviceAdd::OnCbnCloseupComboDeviceList)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_LIST, &CDeviceAdd::OnCbnSelchangeComboDeviceList)
END_MESSAGE_MAP()


// CDeviceAdd 메시지 처리기


BOOL CDeviceAdd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CDeviceAdd::InitDeviceAddDlg(int nTemp)	//nTemp는 사용은 안하지만 추가해놨습니다	
{
	SetAdapterList();
}

void CDeviceAdd::SetAdapterList()
{
	m_DeviceList.ResetContent();
	m_IPADDRESS_DEVICE_ADD.SetWindowTextW(_T("0.0.0.0"));
	theApp.MainDlg->m_Network->SetAdapterComboBox(m_DeviceList);

	UctoS(m_ucSrcAddr, m_unSrcAddr);
	m_editDeviceEthernetAddr.SetWindowTextW(m_unSrcAddr);
}

void CDeviceAdd::UctoS(UCHAR* src, CString& dst)
{
	dst.Format(_T("%02x:%02x:%02x:%02x:%02x:%02x"),
		src[0], src[1], src[2],
		src[3], src[4], src[5]);
}

void CDeviceAdd::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE _a1, _a2, _a3, _a4;
	TCHAR tszIP[MAX_PATH] = { 0, };
	CString mDeviceName;
	CString mAddr;
	//DEVICE
	m_DeviceList.GetLBText(m_DeviceList.GetCurSel(), mDeviceName);
	//IP
	m_IPADDRESS_DEVICE_ADD.GetAddress(_a1, _a2, _a3, _a4);
	_stprintf_s(tszIP, MAX_PATH, _T("%d.%d.%d.%d"), _a1, _a2, _a3, _a4);
	//ETHERNET
	m_editDeviceEthernetAddr.GetWindowTextW(mAddr);
	//
	theApp.MainDlg->AddProxyArpCache((TCHAR*)(LPCTSTR)mDeviceName, tszIP, (TCHAR*)(LPCTSTR)mAddr);
	
	CDialogEx::OnOK();
}


void CDeviceAdd::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


void CDeviceAdd::OnCbnCloseupComboDeviceList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_DeviceList.GetCurSel();
	//memcpy(m_ucSrcAddr, theApp.MainDlg->m_Network->SetAdapter(nIndex), 6);

	//UctoS(m_ucSrcAddr, m_unSrcAddr);
	//m_editDeviceEthernetAddr.SetWindowTextW(m_unSrcAddr);
}


void CDeviceAdd::OnCbnSelchangeComboDeviceList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
