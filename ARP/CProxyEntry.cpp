// CProxyEntry.cpp: 구현 파일
//

#include "pch.h"
#include "ARP.h"
#include "afxdialogex.h"
#include "CProxyEntry.h"


// CProxyEntry 대화 상자

IMPLEMENT_DYNAMIC(CProxyEntry, CDialogEx)

CProxyEntry::CProxyEntry(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DEVICE_ADD, pParent)
{

}

CProxyEntry::~CProxyEntry()
{
}

void CProxyEntry::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_PROXY_DEVICE_LIST, m_ProxyDeviceList);
	DDX_Control(pDX, IDC_IPADDRESS_DEVICE_ADD, m_ProxyIpAddress);
	DDX_Control(pDX, IDC_EDIT_DEVICE_ETHERNET_ADDRESS, m_editProxyEthernetAddr);
	
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CProxyEntry, CDialogEx)
END_MESSAGE_MAP()


// CProxyEntry 메시지 처리기


BOOL CProxyEntry::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ProxyDeviceList.SetCurSel(0);
	m_editProxyEthernetAddr.SetWindowTextW(_T("192.168.52.1"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
