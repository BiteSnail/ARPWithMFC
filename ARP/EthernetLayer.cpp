#include "stdafx.h"
#include "pch.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CEthernetLayer::CEthernetLayer(char* pName)
	: CBaseLayer(pName)
{
	ResetHeader();
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
	memset(m_sHeader.enet_dstaddr, 0, 6);
	memset(m_sHeader.enet_srcaddr, 0, 6);
	memset(m_sHeader.enet_data, ETHER_MAX_DATA_SIZE, 6);
	m_sHeader.enet_type = 0;
}

unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.enet_srcaddr;
}

unsigned char* CEthernetLayer::GetDestinAddress()
{

	// Ethernet 목적지 주소 return
	return m_sHeader.enet_dstaddr;

}

void CEthernetLayer::SetSourceAddress(unsigned char* pAddress)
{

		// 넘겨받은 source 주소를 Ethernet source주소로 지정
	memcpy(m_sHeader.enet_srcaddr, pAddress, 6);

}

void CEthernetLayer::SetDestinAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.enet_dstaddr, pAddress, 6);
}

BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength)
{
	// 윗 계층에서 받은 App 계층의 Frame 길이만큼을 Ethernet계층의 data로 넣는다.
	memcpy(m_sHeader.enet_data, ppayload, nlength);
	// 윗 계층에서 받은 type 또한 헤더에 포함시킨다.
	//memset(m_sHeader.enet_dstaddr, 255, 6);
	m_sHeader.enet_type = ETHER_ARP_TYPE;
	BOOL bSuccess = FALSE;

		// Ethernet Data + Ethernet Header의 사이즈를 합한 크기만큼의 Ethernet Frame을
		// File 계층으로 보낸다.
	bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, ETHER_HEADER_SIZE + nlength);
	return bSuccess;
}

BOOL CEthernetLayer::Receive(unsigned char* ppayload)
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	unsigned char broad[6] = { 255, 255, 255, 255, 255, 255 };

	BOOL bSuccess = FALSE;
	if(memcmp(pFrame->enet_dstaddr, m_sHeader.enet_srcaddr, sizeof(m_sHeader.enet_srcaddr))==0){//주소 확인
			// enet_type을 기준으로 Ethernet Frame의 data를 넘겨줄 레이어를 지정한다.
		if (pFrame->enet_type == ETHER_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data);
		else if(pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data);
	}
	else if (memcmp(pFrame->enet_dstaddr, broad, ENET_ADDR_SIZE) == 0) {
		if (pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data);
	}

	return bSuccess;
}
