// NetworkInterfaceLayer.cpp: implementation of the NetworkInterfaceLayer class.

#include "stdafx.h"
#include "pch.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CNILayer::CNILayer(char* pName)
	: CBaseLayer(pName), device(NULL) {
	char errbuf[PCAP_ERRBUF_SIZE];
	m_AdapterObject = nullptr;
	canRead = false;
	adapter = nullptr;
	OidData = nullptr;
	memset(data, 0, ETHER_MAX_SIZE);
	try {
		if (pcap_findalldevs(&allDevices, errbuf) == -1)
		{
			allDevices = NULL;
			//fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
			throw(CString(errbuf));
		}
	}
	catch (CString errorInfo) {
		AfxMessageBox(errorInfo);
		return;
	}

	try {
		OidData = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
		if (OidData == nullptr) {
			throw(CString("MALLOC FAIL"));
		}
		OidData->Oid = 0x01010101;
		OidData->Length = 6;
		m_AdapterObject = nullptr;
		memset(data, 0, ETHER_MAX_SIZE);
	}
	catch (CString errorInfo) {
		AfxMessageBox(errorInfo);
		return;
	}
}

CNILayer::~CNILayer() {
	pcap_if_t* temp = nullptr;

	while (allDevices) {
		temp = allDevices;
		allDevices = allDevices->next;
		delete(temp);
	}
	delete(OidData);
}

BOOL CNILayer::Receive(unsigned char* pkt) {
	if (pkt == nullptr) {
		return FALSE;
	}
	if (!(mp_aUpperLayer[0]->Receive(pkt))) {
		return FALSE;
	}

	return TRUE;
}

BOOL CNILayer::Send(unsigned char* ppayload, int nlength) {
	if (pcap_sendpacket(m_AdapterObject, ppayload, nlength))
	{
		AfxMessageBox(_T("Fail: Send Packet!"));
		return FALSE;
	}
	return TRUE;
}

void CNILayer::SetAdapterComboBox(CComboBox& adapterlist) {
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.AddString(CString(d->description));
	}
}

UCHAR* CNILayer::SetAdapter(const int index) {
	char errbuf[PCAP_ERRBUF_SIZE];
	device = allDevices;
	if (m_AdapterObject != nullptr)
		pcap_close(m_AdapterObject);

	for (int i = 0; i < index && device; i++) {
		device = device->next;
	}
	if (device != nullptr)
		m_AdapterObject = pcap_open_live((const char*)device->name, 65536, 0, 1000, errbuf);
	if (m_AdapterObject == nullptr)
	{
		AfxMessageBox(_T("Fail: Connect Adapter"));
		return nullptr;
	}

	adapter = PacketOpenAdapter(device->name);
	PacketRequest(adapter, FALSE, OidData);

	PacketCloseAdapter(adapter);
	//AfxBeginThread(ThreadFunction_RECEIVE, this);

	return (OidData->Data);
}


void CNILayer::GetMacAddressList(CStringArray& adapterlist) {
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.Add(CString(d->description));
	}
}

void CNILayer::GetIPAddress(CString& ipv4addr, CString& ipv6addr) {
	char ip[IPV6_ADDR_STR_LEN];
	ipv4addr = DEFAULT_EDIT_TEXT;
	ipv6addr = DEFAULT_EDIT_TEXT;

	for (auto addr = device->addresses; addr != nullptr; addr = addr->next)
	{
		auto realaddr = addr->addr;
		const int sa_family = realaddr->sa_family;

		const char* ptr = inet_ntop(sa_family, &realaddr->sa_data[sa_family == AF_INET ? 2 : 6], ip, IPV6_ADDR_STR_LEN);

		switch (sa_family)
		{
		case AF_INET:
			ipv4addr = ptr;
			break;
		case AF_INET6:
			ipv6addr = ptr;
			break;
		default:
			return;
		}
	}
}

UINT CNILayer::ThreadFunction_RECEIVE(LPVOID pParam) {
	struct pcap_pkthdr* header;
	const u_char* pkt_data;
	CNILayer* pNI = (CNILayer*)pParam;

	int result = 1;

	while (pNI->canRead)
	{
		result = pcap_next_ex(pNI->m_AdapterObject, &header, &pkt_data);
		if (result == 1) {
			memcpy(pNI->data, pkt_data, ETHER_MAX_SIZE);
			pNI->Receive(pNI->data);
		}
	}
	return 0;
}

void CNILayer::Receiveflip() {
	canRead = !canRead;
}