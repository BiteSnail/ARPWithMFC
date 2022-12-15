// NetworkInterfaceLayer.cpp: implementation of the NetworkInterfaceLayer class.

#include "stdafx.h"
#include "pch.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//NILayer constructor
CNILayer::CNILayer(char* pName)
	: CBaseLayer(pName), device()
	, m_AdapterObject()
	, canRead(false)
	, adapter() {
	char errbuf[PCAP_ERRBUF_SIZE];
	OidData[INNER] = nullptr;
	OidData[OUTER] = nullptr;
	memset(data[INNER], 0, ETHER_MAX_SIZE);
	memset(data[OUTER], 0, ETHER_MAX_SIZE);
	try { //Adapter를 찾을 수 없는 경우 에러 발생
		if (pcap_findalldevs(&allDevices, errbuf) == -1){
			allDevices = NULL;
			throw(CString(errbuf));
		}
	}
	catch (CString errorInfo) {
		AfxMessageBox(errorInfo);
		return;
	}

	try {
		OidData[INNER] = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
		OidData[OUTER] = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
		if (OidData[INNER] == nullptr) throw(CString("MALLOC FAIL"));
		if (OidData[OUTER] == nullptr) throw(CString("MALLOC FAIL"));

		OidData[INNER]->Oid = 0x01010101;
		OidData[OUTER]->Oid = 0x01010101;
		OidData[INNER]->Length = 6;
		OidData[OUTER]->Length = 6;
		m_AdapterObject[INNER] = nullptr;
		m_AdapterObject[OUTER] = nullptr;
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

BOOL CNILayer::Receive(unsigned char* pkt, int iosel) {
	if (pkt == nullptr) return FALSE;
	if (!(mp_aUpperLayer[0]->Receive(pkt, iosel))) return FALSE;
	return TRUE;
}

BOOL CNILayer::Send(unsigned char* ppayload, int nlength, int iosel) {
	if (pcap_sendpacket(m_AdapterObject[iosel], ppayload, nlength)) {
		AfxMessageBox(_T("Fail: Send Packet!"));
		return FALSE;
	}
	return TRUE;
}

//
void CNILayer::SetAdapterComboBox(CComboBox& adapterlist) {
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.AddString(CString(d->description));
	}
}

UCHAR* CNILayer::SetAdapter(const int index, const int iosel) {
	char errbuf[PCAP_ERRBUF_SIZE];
	device[iosel] = allDevices;
	if (m_AdapterObject[iosel] != nullptr) pcap_close(m_AdapterObject[iosel]);

	for (int i = 0; i < index && device; i++) device[iosel] = (device[iosel])->next;

	if (device[iosel] != nullptr) m_AdapterObject[iosel] = pcap_open_live((const char*)device[iosel]->name, 65536, 0, 1000, errbuf);
	if (m_AdapterObject[iosel] == nullptr) {
		AfxMessageBox(_T("Fail: Connect Adapter"));
		return nullptr;
	}

	adapter[iosel] = PacketOpenAdapter(device[iosel]->name);
	PacketRequest(adapter[iosel], FALSE, OidData[iosel]);

	//PacketCloseAdapter(adapter[iosel]);
	return (OidData[iosel]->Data);
}

void CNILayer::GetMacAddressList(CStringArray& adapterlist) {
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.Add(CString(d->description));
	}
}

void CNILayer::GetMacAddress(const int index, UCHAR *mac, const int iosel) {
	pcap_if_t* d = allDevices;
	pcap_t* tadapter = nullptr;
	LPADAPTER ad;
	char errbuf[PCAP_ERRBUF_SIZE];
	for (int i = 0; i < index && d; i++) d = d->next;

	if (d == device[iosel]) memcpy(mac, OidData[iosel]->Data, ENET_ADDR_SIZE);
	else {
		if (d != nullptr)
			tadapter = pcap_open_live((const char*)(d->name), 65536, 0, 1000, errbuf);

		ad = PacketOpenAdapter(d->name);
		PacketRequest(ad, FALSE, OidData[iosel] );

		memcpy(mac, OidData[iosel]->Data, ENET_ADDR_SIZE);
		//PacketCloseAdapter(ad);
		//pcap_close(tadapter);
	}
}

void CNILayer::GetIPAddress(CString& ipv4addr, CString& ipv6addr, const int iosel, bool isIOsel=FALSE) {
	char ip[IPV6_ADDR_STR_LEN];
	ipv4addr = ipv6addr =DEFAULT_EDIT_TEXT;
	pcap_if_t* d = allDevices;
	for (int i = 0; i < iosel; i++)d = d->next;
	pcap_addr* addr = d->addresses;
	if (isIOsel) {
		addr = (device[iosel])->addresses;
	}

	for (; addr != nullptr; addr = addr->next) {
		auto realaddr = addr->addr;
		const int sa_family = realaddr->sa_family;

		const char* ptr = 
			inet_ntop(sa_family, &(realaddr->sa_data)[sa_family == AF_INET ? 2 : 6], ip, IPV6_ADDR_STR_LEN);

		switch (sa_family){
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

	while (pNI->canRead){
		result = pcap_next_ex((pNI->m_AdapterObject)[OUTER], &header, &pkt_data);
		if (result == 1) {
			memcpy((pNI->data)[OUTER], pkt_data, ETHER_MAX_SIZE);
			pNI->Receive((pNI->data)[OUTER], OUTER);
		}
	}
	return 0;
}

UINT CNILayer::ThreadFunction_RECEIVE2(LPVOID pParam) {
	struct pcap_pkthdr* header;
	const u_char* pkt_data;
	CNILayer* pNI = (CNILayer*)pParam;
	int result = 1;

	while (pNI->canRead) {
		result = pcap_next_ex((pNI->m_AdapterObject)[INNER], &header, &pkt_data);
		if (result == 1) {
			memcpy((pNI->data)[INNER], pkt_data, ETHER_MAX_SIZE);
			pNI->Receive((pNI->data)[INNER], INNER);
		}
	}
	return 0;
}

void CNILayer::Receiveflip() {
	canRead = !canRead;
}