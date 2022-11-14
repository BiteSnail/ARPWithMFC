
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"



IPLayer::IPLayer(char* pName) : CBaseLayer(pName){
    ResetHeader();
}

IPLayer::~IPLayer(){
}

void IPLayer::ResetHeader(){
    memset(m_sHeader.ip_data, 0, 6);
    memset(m_sHeader.ip_dstaddr, 0, 4);
    memset(m_sHeader.ip_srcaddr, 0, 4);
}

unsigned char* IPLayer::GetSourceAddress(){
    return m_sHeader.ip_srcaddr;
}

unsigned char* IPLayer::GetDestinAddress() {
    return m_sHeader.ip_dstaddr;
}

void IPLayer::SetSourceAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_srcaddr, pAddress, 4);
}

void IPLayer::SetDestinAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_dstaddr, pAddress, 4);
}
//UpperLayer = AppLayer, UnderLayer = ARPLayer?
BOOL IPLayer::Send(unsigned char* ppayload, int nlength){
    memcpy(m_sHeader.ip_data, ppayload, nlength);
    BOOL bSuccess = FALSE;
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, IP_HEADER_SIZE + nlength);
    return bSuccess;
}

BOOL IPLayer::Receive(unsigned char* ppayload){
    BOOL bSuccess = FALSE;
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;

    if(memcmp(pFrame->ip_dstaddr, m_sHeader.ip_srcaddr, sizeof(m_sHeader.ip_srcaddr)) == 0){
        bSuccess = mp_aUpperLayer[0]->Receive(pFrame->ip_data);
    }

    return bSuccess;
}