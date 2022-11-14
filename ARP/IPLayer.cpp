
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"



CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName){
    ResetHeader();
    m_sHeader.ver_hlegnth = 0x45;
    m_sHeader.tos = 0x00;
    m_sHeader.tlength = 0xffff;

    m_sHeader.id = 0x0000;
    m_sHeader.offset = 0x00;

    m_sHeader.ttl = 0xff;
    m_sHeader.ptype = 0x06;
    m_sHeader.checksum = 0x0000;
}

CIPLayer::~CIPLayer(){
}

void CIPLayer::ResetHeader(){
    memset(m_sHeader.ip_srcaddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader.ip_dstaddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader.ip_data, 0, IP_MAX_DATA_SIZE);
}

unsigned char* CIPLayer::GetSourceAddress(){
    return m_sHeader.ip_srcaddr;
}

unsigned char* CIPLayer::GetDestinAddress() {
    return m_sHeader.ip_dstaddr;
}

void CIPLayer::SetSourceAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_srcaddr, pAddress, IP_ADDR_SIZE);
}

void CIPLayer::SetDestinAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_dstaddr, pAddress, IP_ADDR_SIZE);
}
//UpperLayer = AppLayer, UnderLayer = ARPLayer?
BOOL CIPLayer::Send(unsigned char* ppayload, int nlength){
    memcpy(m_sHeader.ip_data, ppayload, nlength);
    BOOL bSuccess = FALSE;
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, IP_HEADER_SIZE + nlength);
    return bSuccess;
}

BOOL CIPLayer::Receive(unsigned char* ppayload){
    BOOL bSuccess = FALSE;
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;

    if(memcmp(pFrame->ip_dstaddr, m_sHeader.ip_srcaddr, sizeof(m_sHeader.ip_srcaddr)) == 0){
        bSuccess = mp_aUpperLayer[0]->Receive(pFrame->ip_data);
    }

    return bSuccess;
}