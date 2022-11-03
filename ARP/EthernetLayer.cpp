// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
	//////////////////////// fill the blank ///////////////////////////////
	// Ethernet ������ �ּ� return
	return m_sHeader.enet_dstaddr;
	///////////////////////////////////////////////////////////////////////
}

void CEthernetLayer::SetSourceAddress(unsigned char* pAddress)
{
	//////////////////////// fill the blank ///////////////////////////////
		// �Ѱܹ��� source �ּҸ� Ethernet source�ּҷ� ����
	memcpy(m_sHeader.enet_srcaddr, pAddress, 6);
	///////////////////////////////////////////////////////////////////////
}

void CEthernetLayer::SetDestinAddress(unsigned char* pAddress)
{
	memcpy(m_sHeader.enet_dstaddr, pAddress, 6);
}

BOOL CEthernetLayer::Send(unsigned char* ppayload, int nlength, unsigned short type)
{
	// �� �������� ���� App ������ Frame ���̸�ŭ�� Ethernet������ data�� �ִ´�.
	memcpy(m_sHeader.enet_data, ppayload, nlength);
	// �� �������� ���� type ���� ����� ���Խ�Ų��.
	m_sHeader.enet_type = type;
	BOOL bSuccess = FALSE;
	//////////////////////// fill the blank ///////////////////////////////

		// Ethernet Data + Ethernet Header�� ����� ���� ũ�⸸ŭ�� Ethernet Frame��
		// File �������� ������.
	bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, ETHER_HEADER_SIZE + nlength);
	///////////////////////////////////////////////////////////////////////
	return bSuccess;
}

BOOL CEthernetLayer::Receive(unsigned char* ppayload)
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER)ppayload;

	BOOL bSuccess = FALSE;
	//////////////////////// fill the blank ///////////////////////////////
	if(memcmp(pFrame->enet_dstaddr, m_sHeader.enet_srcaddr, sizeof(m_sHeader.enet_srcaddr))==0){//�ּ� Ȯ��
			// enet_type�� �������� Ethernet Frame�� data�� �Ѱ��� ���̾ �����Ѵ�.
		if (pFrame->enet_type == 0x2080)
			bSuccess = mp_aUpperLayer[0]->Receive(pFrame->enet_data);//mp_aUpperLayer[0] == ChatApp
		else if(pFrame->enet_type == 0x2090)
			bSuccess = mp_aUpperLayer[1]->Receive(pFrame->enet_data);//mp_aUpperLayer[1] == FileApp
	}
	///////////////////////////////////////////////////////////////////////

	return bSuccess;
}
