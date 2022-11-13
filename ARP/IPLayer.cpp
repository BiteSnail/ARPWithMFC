
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"

#define IP__MAX_SIZE = null//??
#define IP_HEADER_SIZE = null//??

IPLayer::IPLayer(char* pName) : CBaseLayer(pName){
    ResetHeader();
}

IPLayer::~IPLayer(){
}

void IPLayer::ResetHeader(){
    memset(m_sHeader.ip_data, 0, 6);
    memset(m_sHeader.ip_dstaddr, 0, 6);
    memset(m_sHeader.ip_src_addr, IP_MAX_DATA_SIZE, 6);//IP_MAX_DATA_SIZE = IP_MAX_SIZE - IP_HEADER_SIZE
    //m_sHeader.ip_type = 0;
}

unsigned char* IPLayer::GetSourceAddress(){
    return m_sHeader.ip_srcaddr;
}

unsigned char* IPLayer::GetDestinAddress{
    return m_sHeader.ip_dstaddr;
}

void IPLayer::SetSourceAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_srcaddr, pAddress, 6);
}

void IPLayer::SetDestinAddress(unsigned char* pAddress){
    memcpy(m_sHeader.ip_dstaddr, pAddress, 6);
}
//UpperLayer = AppLayer, UnderLayer = ARPLayer -> GetUnderLayer함수로 작동?
BOOL IPLayer::Send(unsigned char* ppayload, int nlength){
    memcpy(m_sHeader.ip_data, ppayload, nlength);
    BOOL bSuccess = FALSE;
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, IP_HEADER_SIZE + nlength);
    return bSuccess;
}

BOOL IPLayer::Receive(unsigned char* ppayload){
    BOOL bSuccess = FALSE;
    IP_HEADER pFrame = (IP_HEADER)ppayload;

    if(memcmp(pFrame->ip_dstaddr, m_sHeader.ip_srcaddr, sizeof(m_sHeader.ip_srcaddr)) == 0){
        bSuccess = mp_aUpperLayer[0]->Receive(pFrame->ip_data)
    }

    return bSuccess;
}