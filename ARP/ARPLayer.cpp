#include "pch.h"
#include "ARPLayer.h"

CARPLayer::_ARP_NODE::_ARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char bincomplete = false) {
	memcpy(protocol_addr, cipaddr, IP_ADDR_SIZE);
	memcpy(hardware_addr, cenetaddr, ENET_ADDR_SIZE);
	status = bincomplete;
	spanTime = CTime::GetCurrentTime();
}
CARPLayer::_ARP_NODE::_ARP_NODE(unsigned int ipaddr = 0, unsigned int enetaddr = 0, unsigned char incomplete = false) {
	memset(protocol_addr, ipaddr, IP_ADDR_SIZE);
	memset(hardware_addr, enetaddr, ENET_ADDR_SIZE);
	status = incomplete;
	spanTime = CTime::GetCurrentTime();
}
CARPLayer::_ARP_NODE::_ARP_NODE(const struct _ARP_NODE& ot) {
	memcpy(protocol_addr, ot.protocol_addr, IP_ADDR_SIZE);
	memcpy(hardware_addr, ot.hardware_addr, ENET_ADDR_SIZE);
	status = ot.status;
	spanTime = ot.spanTime;
}

inline void CARPLayer::ResetHeader() {
	m_sHeader[INNER] = ARP_HEADER();
	m_sHeader[OUTER] = ARP_HEADER();
}

bool CARPLayer::getMACinARP(unsigned char* dstIP, unsigned char* MAC)
{
	int index = inCache(dstIP);
	bool isfound = FALSE;
	if (index != -1) {
		memcpy(MAC, m_arpTable[index].hardware_addr, ENET_ADDR_SIZE);
		isfound = TRUE;
	}
	return isfound;
}

void CARPLayer::isGARP(BOOL yes)
{
	is_Garp = !yes;
}

BOOL CARPLayer::Receive(unsigned char* ppayload, int iosel) {
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;
	int index = inCache(arp_data->hardware_srcaddr);

	if (memcmp(myip, arp_data->protocol_srcaddr, IP_ADDR_SIZE) == 0) {
		return FALSE;
	}

	switch (arp_data->opercode) {
	case ARP_OPCODE_REQUEST:
		if (index >= 0) {
			memcpy(m_arpTable[index].hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
			m_arpTable[index].status = true;
			m_arpTable[index].spanTime = CTime::GetCurrentTime();
		}
		else {
			m_arpTable.push_back(ARP_NODE(arp_data->protocol_srcaddr, arp_data->hardware_srcaddr, TRUE));
		}

		//전달된 request의 dst ip가 나의 ip와 같은 경우 나의 enetaddr를 반환
		if (memcmp(arp_data->protocol_dstaddr, myip, IP_ADDR_SIZE) == 0) {
			memcpy(arp_data->hardware_dstaddr, mymac, ENET_ADDR_SIZE);
			arp_data->opercode = ARP_OPCODE_REPLY;
			swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
			swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

			m_ether->SetDestinAddress(arp_data->hardware_dstaddr, iosel);
			return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE, iosel);
		}
		//만약 request의 dst ip가 나의 ip가 아닌 경우 proxy table 확인 후 등록된 정보가 있다면 reply
		else {
			for (auto &a = m_proxyTable.begin(); a != m_proxyTable.end(); a++) {
				//proxy에 등록된 ip가 arp request의 dst addr이라면 proxy arp reply 수행
				if (memcmp(a->protocol_addr, arp_data->protocol_dstaddr, IP_ADDR_SIZE) == 0) {
					memcpy(arp_data->hardware_dstaddr, a->srchardware_addr, ENET_ADDR_SIZE);
					arp_data->opercode = ARP_OPCODE_REPLY;
					swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
					swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

					m_ether->SetDestinAddress(arp_data->hardware_dstaddr, iosel);
					return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE, iosel);
				}
			}
		}
		break;
	case ARP_OPCODE_REPLY:
		for (auto& node : m_arpTable) {
			if (node == arp_data->protocol_srcaddr) {
				memcpy(node.hardware_addr, arp_data->hardware_srcaddr, ENET_ADDR_SIZE);
				node.status = true;
				node.spanTime = CTime::GetCurrentTime();
				break;
			}
		}
		break;
	case ARP_OPCODE_RREQUEST:
		break;
	case ARP_OPCODE_RREPLY:
		break;
	default:
		throw("unknown Opcode Error");
		return false;
	}

	return true;
}

BOOL CARPLayer::Send(unsigned char* ppayload, int nlength, int iosel) {
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;
	unsigned char broadcastAddr[ENET_ADDR_SIZE];
	memset(broadcastAddr, 255*is_Garp, ENET_ADDR_SIZE);
	
	ARP_NODE newNode(ip_data->ip_dstaddr, broadcastAddr);

	if (memcmp(ip_data->ip_srcaddr, ip_data->ip_dstaddr, IP_ADDR_SIZE) != 0) {
		//check given address is in arp cache table
		int idx = inCache(ip_data->ip_dstaddr);
		if (idx != -1) {
			m_arpTable[idx] = newNode;
		}
		else {
			m_arpTable.push_back(newNode);
		}
	}
	m_ether->SetDestinAddress(broadcastAddr, iosel);
	m_ether->SetType(ETHER_ARP_TYPE, iosel);
	setOpcode(ARP_OPCODE_REQUEST, iosel);
	setSrcAddr(m_ether->GetSourceAddress(iosel), ip_data->ip_srcaddr, iosel);
	setDstAddr(broadcastAddr, ip_data->ip_dstaddr, iosel);

	return mp_UnderLayer->Send((unsigned char*)&m_sHeader[iosel], ARP_HEADER_SIZE, iosel);
}

void CARPLayer::Wait(DWORD dwMillisecond)
{
	MSG msg;
	DWORD dwStart;
	dwStart = GetTickCount();

	while (GetTickCount() - dwStart < dwMillisecond)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

BOOL CARPLayer::RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel) {
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;
	unsigned char temp[IP_MAX_SIZE] = { 0, };
	nlength = nlength > IP_MAX_SIZE ? IP_MAX_SIZE : nlength;
	memcpy(temp, ppayload, nlength);
	
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;
	int idx = 0;

	//proxy테이블에 있다면 그쪽으로 바로 보내줌
	for (int i = 0; i < m_proxyTable.size(); i++) {
		if (memcmp(m_proxyTable[i].protocol_addr, gatewayIP, IP_ADDR_SIZE) == 0)
		{
			m_ether->SetDestinAddress(m_proxyTable[i].hardware_addr, iosel);
			m_ether->SetType(ETHER_IP_TYPE, iosel);
			return mp_UnderLayer->Send(temp, nlength, iosel);
		}
	}

	//proxy table에 없다면 arp table 찾아보기
	idx = inCache(gatewayIP);
	if (idx == -1) {
		//arp table에도 없으면 해당 ip주소로 arp request 날려보기
		IP_HEADER forarp;
		memcpy(forarp.ip_srcaddr, myip[iosel], IP_ADDR_SIZE);
		memcpy(forarp.ip_dstaddr, gatewayIP, IP_ADDR_SIZE);
		Send((unsigned char*)&forarp, nlength, iosel);
		
		Wait(3000);
		
		//TODO arp reply 받으면 그쪽으로 다시 보내주기
		idx = inCache(gatewayIP);
		if (idx == -1) {
			return FALSE;
		}
	}
	//arp table에 있으면 해당 주소로 보내줌
	m_ether->SetDestinAddress(m_arpTable[idx].hardware_addr, iosel);
	m_ether->SetType(ETHER_IP_TYPE, iosel);
	return mp_UnderLayer->Send(temp, nlength, iosel);
}


int CARPLayer::inCache(unsigned char* ipaddr) {
	int res = -1;
	for (int i = 0; i < m_arpTable.size(); i++) {
		if (m_arpTable[i] == ipaddr) {
			res = i;
			break;
		}
	}
	return res;
}

void CARPLayer::setmyAddr(CString MAC, CString IP, int iosel) {
	StrToaddr(ARP_IP_TYPE, &myip[iosel][0], IP);
	StrToaddr(ARP_ENET_TYPE, &mymac[iosel][0], MAC);
}

void CARPLayer::setType(const unsigned short htype, const unsigned short ptype, int iosel) {
	m_sHeader[iosel].hardware_type = htype;
	m_sHeader[iosel].protocol_type = ptype;

	switch (m_sHeader[iosel].hardware_type) {
	case ARP_ENET_TYPE:
		m_sHeader[iosel].hardware_length = ENET_ADDR_SIZE;
		break;
	default:
		throw("Hardware Type Error!");
	}

	switch (m_sHeader[iosel].protocol_type) {
	case ARP_IP_TYPE:
		m_sHeader[iosel].protocol_length = IP_ADDR_SIZE;
		break;
	default:
		throw("Protocol Type Error!");
	}
}

void CARPLayer::setOpcode(const unsigned short opcode, int iosel) {
	if (opcode >= ARP_OPCODE_REQUEST && opcode <= ARP_OPCODE_RREPLY) {
		m_sHeader[iosel].opercode = opcode;
	}
	else
		throw("Operator code Error!");
}

void CARPLayer::setSrcAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel) {
	memcpy(m_sHeader[iosel].hardware_srcaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader[iosel].protocol_srcaddr, ipAddr, IP_ADDR_SIZE);
}
void CARPLayer::setDstAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel) {
	memcpy(m_sHeader[iosel].hardware_dstaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader[iosel].protocol_dstaddr, ipAddr, IP_ADDR_SIZE);
}

void CARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], unsigned char size) {
	unsigned char tempAddr[ENET_ADDR_SIZE] = { 0, };

	memcpy(tempAddr, lAddr, size);
	memcpy(lAddr, rAddr, size);
	memcpy(rAddr, tempAddr, size);
}

CARPLayer::CARPLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
	setType(ARP_ENET_TYPE, ARP_IP_TYPE, INNER);
	setType(ARP_ENET_TYPE, ARP_IP_TYPE, OUTER);
	memset(myip, 0, IP_ADDR_SIZE);
	memset(mymac, 0, ENET_ADDR_SIZE);
	is_Garp = 1;
}

CARPLayer::~CARPLayer() {

}

bool CARPLayer::_ARP_NODE::operator==(const unsigned char* ipaddr) {
	return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == 0;
}
bool CARPLayer::_ARP_NODE::operator==(const struct _ARP_NODE& ot) {
	return *this == ot.protocol_addr;
}
bool CARPLayer::_ARP_NODE::operator<(const unsigned char* ipaddr) {
	return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == -1;
}
bool CARPLayer::_ARP_NODE::operator<(const struct _ARP_NODE& ot) {
	return *this < ot.protocol_addr;
}
bool CARPLayer::_ARP_NODE::operator>(const unsigned char* ipaddr) {
	return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == 1;
}
bool CARPLayer::_ARP_NODE::operator>(const struct _ARP_NODE& ot) {
	return *this > ot.protocol_addr;
}

CARPLayer::_ARP_HEADER::_ARP_HEADER() {
	hardware_type = protocol_type = 0x0000;
	hardware_length = protocol_length = 0x00;
	opercode = 0x0000;
	memset(hardware_srcaddr, 0x00, ENET_ADDR_SIZE);
	memset(protocol_srcaddr, 0x00, IP_ADDR_SIZE);
	memset(hardware_dstaddr, 0x00, ENET_ADDR_SIZE);
	memset(protocol_dstaddr, 0x00, IP_ADDR_SIZE);
};

CARPLayer::_ARP_HEADER::_ARP_HEADER(const struct _ARP_HEADER& ot)
	:hardware_type(ot.hardware_type), protocol_type(ot.protocol_type),
	hardware_length(ot.hardware_length), protocol_length(ot.protocol_length),
	opercode(ot.opercode)
{
	memcpy(hardware_srcaddr, ot.hardware_srcaddr, ENET_ADDR_SIZE);
	memcpy(hardware_dstaddr, ot.hardware_dstaddr, ENET_ADDR_SIZE);
	memcpy(protocol_srcaddr, ot.protocol_srcaddr, IP_ADDR_SIZE);
	memcpy(protocol_dstaddr, ot.protocol_dstaddr, IP_ADDR_SIZE);
}

void addrToStr(unsigned short type, CString& dst, unsigned char* src) {
	switch (type) {
	case ARP_IP_TYPE:
		dst.Format(_T("%hhu.%hhu.%hhu.%hhu"),
			src[0], src[1], src[2], src[3]);
		break;
	case ARP_ENET_TYPE:
		dst.Format(_T("%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"),
			src[0], src[1], src[2], src[3], src[4], src[5]);
		break;
	default:
		break;
	}
}
void StrToaddr(unsigned short type, unsigned char* dst, CString& src) {
	switch (type) {
	case ARP_IP_TYPE:
		swscanf_s(src, _T("%hhu.%hhu.%hhu.%hhu"),
			&dst[0], &dst[1], &dst[2], &dst[3]);
		break;
	case ARP_ENET_TYPE:
		swscanf_s(src, _T("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"),
			&dst[0], &dst[1], &dst[2], &dst[3], &dst[4], &dst[5]);
		break;
	default:
		break;
	}
}

void CARPLayer::updateTable() {
	CTime cur = CTime::GetCurrentTime();
	for (int i = 0; i < m_arpTable.size(); i++) {
		if (m_arpTable[i].status == ARP_TIME_OUT) continue;

		if ((cur - m_arpTable[i].spanTime) > (m_arpTable[i].status == TRUE ? 600 : 180)) {
			m_arpTable[i].status = ARP_TIME_OUT;
		}
	}
}

void CARPLayer::deleteItem(CString IP) {
	auto k = m_arpTable.begin();
	unsigned char addr[IP_ADDR_SIZE] = { 0, };
	StrToaddr(ARP_IP_TYPE, addr, IP);

	for (; k != m_arpTable.end(); k++) {
		if (*k == addr) {
			break;
		}
	}

	m_arpTable.erase(k);
}

std::vector<CARPLayer::ARP_NODE> CARPLayer::getTable() {
	return m_arpTable;
}

void CARPLayer::clearTable() {
	m_arpTable.clear();
}

CARPLayer::_PROXY_ARP_NODE::_PROXY_ARP_NODE(unsigned char* srcenet, unsigned char* dstip, unsigned char* dstenet) {
	if (srcenet) memcpy(srchardware_addr, srcenet, ENET_ADDR_SIZE);
	else memset(srchardware_addr, 0, ENET_ADDR_SIZE);
	if (dstip) memcpy(protocol_addr, dstip, IP_ADDR_SIZE);
	else memset(protocol_addr, 0, IP_ADDR_SIZE);
	if (dstenet) memcpy(hardware_addr, dstenet, ENET_ADDR_SIZE);
	else memset(hardware_addr, 0, ENET_ADDR_SIZE);
}

bool CARPLayer::_PROXY_ARP_NODE::operator==(const struct _PROXY_ARP_NODE& ot) {
	return (memcmp(ot.hardware_addr, hardware_addr, ENET_ADDR_SIZE) == 0) &&
		(memcmp(ot.srchardware_addr, srchardware_addr, ENET_ADDR_SIZE) == 0) &&
		(memcmp(ot.protocol_addr, protocol_addr, IP_ADDR_SIZE) == 0);
}

void CARPLayer::createProxy(unsigned char* src, unsigned char* ip, unsigned char* enet) {
	m_proxyTable.push_back(PROXY_ARP_NODE(src, ip, enet));
}

void CARPLayer::deleteProxy(const int index) {
	m_proxyTable.erase(m_proxyTable.begin() + index);
}