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
	m_sHeader = ARP_HEADER();
}

BOOL CARPLayer::Receive(unsigned char* ppayload) {
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;
	int index = inCache(arp_data->hardware_srcaddr);

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

		if (memcmp(arp_data->protocol_dstaddr, myip, IP_ADDR_SIZE) == 0) {
			memcpy(arp_data->hardware_dstaddr, mymac, ENET_ADDR_SIZE);
			arp_data->opercode = ARP_OPCODE_REPLY;
			swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
			swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);

			m_ether->SetDestinAddress(arp_data->hardware_dstaddr);
			return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
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

BOOL CARPLayer::Send(unsigned char* ppayload, int nlength) {
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;
	unsigned char broadcastAddr[ENET_ADDR_SIZE];
	memset(broadcastAddr, 255, ENET_ADDR_SIZE);
	
	ARP_NODE newNode(ip_data->dstaddr, broadcastAddr);
	m_ether->SetDestinAddress(broadcastAddr);
	setOpcode(ARP_OPCODE_REQUEST);

	if (memcmp(ip_data->srcaddr, ip_data->dstaddr, IP_ADDR_SIZE) != 0) {
		//check given address is in arp cache table
		int idx = inCache(ip_data->dstaddr);
		if (idx != -1) {
			if (m_arpTable[idx].status == FALSE) {
				AfxMessageBox(_T("Already In Cache!"));
				return true;
			}
			else {
				m_arpTable[idx] = newNode;
			}
		}
		else {
			m_arpTable.push_back(newNode);
		}
	}
	setSrcAddr(m_ether->GetSourceAddress(), ip_data->srcaddr);
	setDstAddr(broadcastAddr, ip_data->dstaddr);

	return ((CEthernetLayer*)mp_UnderLayer)->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE);

	return true;
}

int CARPLayer::inCache(const unsigned char* ipaddr) {
	int res = -1;
	for (int i = 0; i < m_arpTable.size(); i++) {
		if (m_arpTable[i] == ipaddr) {
			res = i;
			break;
		}
	}
	return res;
}

void CARPLayer::setmyAddr(CString MAC, CString IP) {
	StrToaddr(ARP_IP_TYPE, myip, IP);
	StrToaddr(ARP_ENET_TYPE, mymac, MAC);
}

void CARPLayer::setType(const unsigned short htype, const unsigned short ptype) {
	m_sHeader.hardware_type = htype;
	m_sHeader.protocol_type = ptype;

	switch (m_sHeader.hardware_type) {
	case ARP_ENET_TYPE:
		m_sHeader.hardware_length = ENET_ADDR_SIZE;
		break;
	default:
		throw("Hardware Type Error!");
	}

	switch (m_sHeader.protocol_type) {
	case ARP_IP_TYPE:
		m_sHeader.protocol_length = IP_ADDR_SIZE;
		break;
	default:
		throw("Protocol Type Error!");
	}
}

void CARPLayer::setOpcode(const unsigned short opcode) {
	if (opcode >= ARP_OPCODE_REQUEST && opcode <= ARP_OPCODE_RREPLY) {
		m_sHeader.opercode = opcode;
	}
	else
		throw("Operator code Error!");
}

void CARPLayer::setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hardware_srcaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader.protocol_srcaddr, ipAddr, IP_ADDR_SIZE);
}
void CARPLayer::setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hardware_dstaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader.protocol_dstaddr, ipAddr, IP_ADDR_SIZE);
}

void CARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size) {
	unsigned char tempAddr[ENET_ADDR_SIZE] = { 0, };

	memcpy(tempAddr, lAddr, size);
	memcpy(lAddr, rAddr, size);
	memcpy(rAddr, tempAddr, size);
}

CARPLayer::CARPLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
	setType(ARP_ENET_TYPE, ARP_IP_TYPE);
	memset(myip, 0, IP_ADDR_SIZE);
	memset(mymac, 0, ENET_ADDR_SIZE);
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