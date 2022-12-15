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
	ResetHeader(INNER);
	ResetHeader(OUTER);
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader(int iosel)
{
	memset(m_sHeader[iosel].enet_dstaddr, 0, 6);
	memset(m_sHeader[iosel].enet_srcaddr, 0, 6);
	memset(m_sHeader[iosel].enet_data, 0, ETHER_MAX_DATA_SIZE);
	m_sHeader[iosel].enet_type = 0;
}

unsigned char* CEthernetLayer::GetSourceAddress(int iosel)
{
	return m_sHeader[iosel].enet_srcaddr;
}

unsigned char* CEthernetLayer::GetDestinAddress(int iosel)
{

	// Ethernet ������ �ּ� return
	return m_sHeader[iosel].enet_dstaddr;

}

void CEthernetLayer::SetSourceAddress(unsigned char* pAddress, int iosel)
{

		// �Ѱܹ��� source �ּҸ� Ethernet source�ּҷ� ����
	memcpy(m_sHeader[iosel].enet_srcaddr, pAddress, 6);

}

void CEthernetLayer::SetDestinAddress(unsigned char* pAddress, int iosel)
{
	memcpy(m_sHeader[iosel].enet_dstaddr, pAddress, 6);
}

BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength, int iosel)
{
	// �� �������� ���� App ������ Frame ���̸�ŭ�� Ethernet������ data�� �ִ´�.
	memcpy(m_sHeader[iosel].enet_data, ppayload, nlength);
	BOOL bSuccess = FALSE;
	bSuccess = mp_UnderLayer->Send((unsigned char*)&m_sHeader[iosel], ETHER_HEADER_SIZE + nlength > ETHER_MAX_SIZE ? ETHER_MAX_SIZE: ETHER_HEADER_SIZE+nlength, iosel);
	return bSuccess;
}

void CEthernetLayer::SetType(unsigned short type, int iosel)
{
	m_sHeader[iosel].enet_type = type;
}

BOOL CEthernetLayer::Receive(unsigned char* ppayload, int iosel)
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	unsigned char broad[6] = { 255, 255, 255, 255, 255, 255 };

	if (memcmp(pFrame->enet_srcaddr, m_sHeader[iosel].enet_srcaddr, ENET_ADDR_SIZE) == 0) {
		return FALSE;
	}

	BOOL bSuccess = FALSE;
	if(memcmp(pFrame->enet_dstaddr, m_sHeader[iosel].enet_srcaddr, sizeof(m_sHeader[iosel].enet_srcaddr)) == 0) {//�ּ� Ȯ��
			// enet_type�� �������� Ethernet Frame�� data�� �Ѱ��� ���̾ �����Ѵ�.
		if (pFrame->enet_type == ETHER_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data, iosel);
		else if(pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data, iosel);
	}
	else if (memcmp(pFrame->enet_dstaddr, broad, ENET_ADDR_SIZE) == 0) {
		if (pFrame->enet_type == ETHER_ARP_TYPE)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data, iosel);
		else if (pFrame->enet_type == ETHER_IP_TYPE)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data, iosel);
	}

	return bSuccess;
}
