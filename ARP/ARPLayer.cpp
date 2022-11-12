#include "pch.h"
#include "ARPLayer.h"

ARPLayer::_ARP_NODE::_ARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, bool bincomplete = false) {
	memcpy(protocol_addr, cipaddr, IP_ADDR_SIZE);
	memcpy(hardware_addr, cenetaddr, ENET_ADDR_SIZE);
	status = bincomplete;
	spanTime = CTime::GetCurrentTime();
}
ARPLayer::_ARP_NODE::_ARP_NODE(unsigned int ipaddr = 0, unsigned int enetaddr = 0, bool incomplete = false) {
	memset(protocol_addr, ipaddr, IP_ADDR_SIZE);
	memset(hardware_addr, enetaddr, ENET_ADDR_SIZE);
	status = incomplete;
	spanTime = CTime::GetCurrentTime();
}
ARPLayer::_ARP_NODE::_ARP_NODE(const struct _ARP_NODE& ot) {
	memcpy(protocol_addr, ot.protocol_addr, IP_ADDR_SIZE);
	memcpy(hardware_addr, ot.hardware_addr, ENET_ADDR_SIZE);
	status = ot.status;
	spanTime = ot.spanTime;
}

inline void ARPLayer::ResetHeader() {
	m_sHeader = ARP_HEADER();
}

BOOL ARPLayer::Receive(unsigned char* ppayload) {
	PARP_HEADER arp_data = (PARP_HEADER)ppayload;


	switch (arp_data->opercode) {
	case ARP_OPCODE_REQUEST:
		for (auto& node : m_arpTable) {
			if (node == arp_data->protocol_dstaddr) {
				memcpy(arp_data->hardware_dstaddr, node.hardware_addr, ENET_ADDR_SIZE);
				swapaddr(arp_data->hardware_srcaddr, arp_data->hardware_dstaddr, ENET_ADDR_SIZE);
				swapaddr(arp_data->protocol_srcaddr, arp_data->protocol_dstaddr, IP_ADDR_SIZE);
				node.spanTime = CTime::GetCurrentTime();
				break;
			}
		}
		return mp_UnderLayer->Send((unsigned char*)arp_data, ARP_HEADER_SIZE);
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
		throw("unable Opcode Error");
		return false;
	}

	return true;
}

BOOL ARPLayer::Send(unsigned char* ppayload, int nlength) {
	PIP_HEADER ip_data = (PIP_HEADER)ppayload;
	CEthernetLayer* m_ether = (CEthernetLayer*)mp_UnderLayer;
	unsigned char broadcastAddr[ENET_ADDR_SIZE];
	memset(broadcastAddr, 255, ENET_ADDR_SIZE);

	setOpcode(ARP_OPCODE_REQUEST);

	//check given address is in arp cache table
	if (inCache(ip_data->dstaddr)) {
		AfxMessageBox(_T("Already In Cache!"));
		return true;
	}
	else {
		ARP_NODE newNode(ip_data->dstaddr, broadcastAddr);
		m_arpTable.push_back(newNode);

		setSrcAddr(m_ether->GetSourceAddress(), ip_data->srcaddr);
		setDstAddr(broadcastAddr, ip_data->dstaddr);

		return mp_UnderLayer->Send((unsigned char*)&m_sHeader, ARP_HEADER_SIZE);
	}

	return true;
}

BOOL ARPLayer::inCache(const unsigned char* ipaddr) {
	BOOL isIn = false;
	for (auto& info : m_arpTable) {
		if (info == ipaddr) {
			isIn = true;
			break;
		}
	}
	return isIn;
}

void ARPLayer::setType(const unsigned short htype, const unsigned short ptype) {
	m_sHeader.hardware_type = htype;
	m_sHeader.protocol_type = ptype;

	switch(m_sHeader.hardware_type) {
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

void ARPLayer::setOpcode(const unsigned short opcode) {
	if (opcode >= ARP_OPCODE_REQUEST && opcode <= ARP_OPCODE_RREPLY) {
		m_sHeader.opercode = opcode;
	}
	else
		throw("Operator code Error!");
}

void ARPLayer::setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hardware_srcaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader.protocol_srcaddr, ipAddr, IP_ADDR_SIZE);
}
void ARPLayer::setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]) {
	memcpy(m_sHeader.hardware_dstaddr, enetAddr, ENET_ADDR_SIZE);
	memcpy(m_sHeader.protocol_dstaddr, ipAddr, IP_ADDR_SIZE);
}

void ARPLayer::swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size) {
	unsigned char tempAddr[ENET_ADDR_SIZE]= {0, };

	memcpy(tempAddr, lAddr, size);
	memcpy(lAddr, rAddr, size);
	memcpy(rAddr, tempAddr, size);
}

ARPLayer::ARPLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
	setType(ARP_ENET_TYPE, ARP_IP_TYPE);
}

ARPLayer::~ARPLayer() {

}

bool ARPLayer::_ARP_NODE::operator==(const unsigned char* ipaddr) {
	return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == 0;
}
bool ARPLayer::_ARP_NODE::operator==(const struct _ARP_NODE& ot) {
	return *this == ot.protocol_addr;
}
bool ARPLayer::_ARP_NODE::operator<(const unsigned char* ipaddr) {
	return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == -1;
}
bool ARPLayer::_ARP_NODE::operator<(const struct _ARP_NODE& ot) {
	return *this < ot.protocol_addr;
}
bool ARPLayer::_ARP_NODE::operator>(const unsigned char* ipaddr) {
	return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == 1;
}
bool ARPLayer::_ARP_NODE::operator>(const struct _ARP_NODE& ot) {
	return *this > ot.protocol_addr;
}

ARPLayer::_ARP_HEADER::_ARP_HEADER() {
	hardware_type = protocol_type = 0x0000;
	hardware_length = protocol_length = 0x00;
	opercode = 0x0000;
	memset(hardware_srcaddr, 0x00, ENET_ADDR_SIZE);
	memset(protocol_srcaddr, 0x00, IP_ADDR_SIZE);
	memset(hardware_dstaddr, 0x00, ENET_ADDR_SIZE);
	memset(protocol_dstaddr, 0x00, IP_ADDR_SIZE);
};

ARPLayer::_ARP_HEADER::_ARP_HEADER(const struct _ARP_HEADER& ot)
	:hardware_type(ot.hardware_type), protocol_type(ot.protocol_type),
	hardware_length(ot.hardware_length), protocol_length(ot.protocol_length),
	opercode(ot.opercode)
{
	memcpy(hardware_srcaddr, ot.hardware_srcaddr, ENET_ADDR_SIZE);
	memcpy(hardware_dstaddr, ot.hardware_dstaddr, ENET_ADDR_SIZE);
	memcpy(protocol_srcaddr, ot.protocol_srcaddr, IP_ADDR_SIZE);
	memcpy(protocol_dstaddr, ot.protocol_dstaddr, IP_ADDR_SIZE);
}