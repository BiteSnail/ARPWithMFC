#include "pch.h"
#include "ARPLayer.h"

inline void ARPLayer::ResetHeader() {
	m_sHeader.hardware_type = 0x0000;
	m_sHeader.protocol_type = 0x0000;
	m_sHeader.hardware_length = 0x00;
	m_sHeader.protocol_length = 0x00;
	m_sHeader.opercode = 0x0000;
	memset(m_sHeader.hardware_srcaddr, 0x00, ENET_ADDR_SIZE);
	memset(m_sHeader.protocol_srcaddr, 0x00, IP_ADDR_SIZE);
	memset(m_sHeader.hardware_dstaddr, 0x00, ENET_ADDR_SIZE);
	memset(m_sHeader.protocol_dstaddr, 0x00, IP_ADDR_SIZE);
}

BOOL ARPLayer::Receive(unsigned char* ppayload) {

}

BOOL ARPLayer::Send(unsigned char* ppayload, int nlength) {

}