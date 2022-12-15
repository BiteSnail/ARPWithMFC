
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"



CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
    ResetHeader(INNER);
    ResetHeader(OUTER);
 }

CIPLayer::~CIPLayer() {

}

void CIPLayer::ResetHeader(int iosel) {
    memset(m_sHeader[iosel].ip_srcaddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader[iosel].ip_dstaddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader[iosel].ip_data, 0, IP_MAX_DATA_SIZE);
}

void CIPLayer::SetHeaderFields(unsigned char* ppayload, int iosel){
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;
    m_sHeader[iosel].ver_hlegnth = pFrame->ver_hlegnth;
    m_sHeader[iosel].tos = pFrame->tos;
    m_sHeader[iosel].tlength = pFrame->tlength;
    m_sHeader[iosel].id = pFrame->id;
    m_sHeader[iosel].ttl = pFrame->ttl;
    m_sHeader[iosel].ttl = m_sHeader[iosel].ttl - 1;        //ttl 감소시키는 것
    m_sHeader[iosel].ptype = pFrame->ptype;
    m_sHeader[iosel].checksum = pFrame->checksum;
    SetSourceAddress(pFrame->ip_srcaddr, iosel);
    SetDestinAddress(pFrame->ip_dstaddr, iosel);
}

unsigned char* CIPLayer::GetSourceAddress(int iosel) {
    return m_sHeader[iosel].ip_srcaddr;
}

unsigned char* CIPLayer::GetDestinAddress(int iosel) {
    return m_sHeader[iosel].ip_dstaddr;
}

void CIPLayer::SetSourceAddress(unsigned char* pAddress, int iosel) {
    memcpy(m_sHeader[iosel].ip_srcaddr, pAddress, IP_ADDR_SIZE);
}

void CIPLayer::SetDestinAddress(unsigned char* pAddress, int iosel) {
    memcpy(m_sHeader[iosel].ip_dstaddr, pAddress, IP_ADDR_SIZE);
}
//UpperLayer = AppLayer, UnderLayer = ARPLayer?
BOOL CIPLayer::Send(unsigned char* ppayload, int nlength, int iosel) {
    BOOL bSuccess = FALSE;
    memcpy(m_sHeader[iosel].ip_data, ppayload, nlength);
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader[iosel], IP_HEADER_SIZE + nlength, iosel);
    return bSuccess;
}

BOOL CIPLayer::RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel) {
    BOOL bSuccess = FALSE;
    bSuccess = this->GetUnderLayer()->RSend(ppayload, nlength, gatewayIP, iosel);
    return bSuccess;
}


BOOL CIPLayer::Receive(unsigned char* ppayload, int iosel) {
    //변경된 Receive 함수
    BOOL bSuccess = FALSE;
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;
    
    //m_sHeader 변수 값 채우기
    memcpy(m_sHeader[iosel].ip_data, pFrame->ip_data, IP_MAX_DATA_SIZE);
    SetHeaderFields(ppayload, iosel);
    
    Routing(pFrame->ip_dstaddr, ppayload, iosel);
    return bSuccess;
}

void CIPLayer::AddRouteTable(unsigned char* _destination_ip, unsigned char* _netmask, unsigned char* _gateway, unsigned char _flag, unsigned char interFace) {
    ROUTING_TABLE_NODE rt;
    memcpy(rt.destination_ip, _destination_ip, IP_ADDR_SIZE);
    memcpy(rt.netmask, _netmask, IP_ADDR_SIZE);
    memcpy(rt.gateway, _gateway, IP_ADDR_SIZE);
    rt.flag = _flag;
    rt._interface = interFace;

    //route_table list에서 새로운 열을 추가할 위치 찾기...
    std::list<ROUTING_TABLE_NODE>::iterator add_point = route_table.begin();
    while (add_point != route_table.end() && LongestPrefix(add_point->gateway, rt.gateway)) {
        add_point++;
    }
    route_table.insert(add_point, rt);
}

void CIPLayer::DelRouteTable(unsigned char index) {
    std::list<ROUTING_TABLE_NODE>::iterator del_point = route_table.begin();
    for (int i = 0; i < int(index) - 1; i++) {
        del_point++;
    }
    route_table.erase(del_point);
}

void CIPLayer::SetDefaultGateway(unsigned char* _gateway, unsigned char _flag, unsigned char interFace) {
    unsigned char zero[IP_ADDR_SIZE];              //비교 기준 만들기 
    memset(zero, 0, IP_ADDR_SIZE);                 //Destination_IP: '0.0.0.0' 확인용 값
    
    if (!memcmp(route_table.back().destination_ip, zero, IP_ADDR_SIZE)) {       //기존 defaultgateway가 list node로 존재할 경우
        memcpy(&route_table.back().gateway, _gateway, IP_ADDR_SIZE);
        route_table.back().flag = _flag;
        route_table.back()._interface = interFace;
    }
    else {
        ROUTING_TABLE_NODE* tmp = new ROUTING_TABLE_NODE;
        memset(&tmp->destination_ip, 0, IP_ADDR_SIZE);
        memset(&tmp->netmask, 0, IP_ADDR_SIZE);
        memcpy(&tmp->gateway, _gateway, IP_ADDR_SIZE);
        tmp->flag = _flag;
        tmp->_interface = interFace;
    
        route_table.push_back(*tmp);
    }
 }

bool CIPLayer::LongestPrefix(unsigned char* a, unsigned char* b) {
    for (int i = 0; i < IP_ADDR_SIZE; i++) {
        if (a[i] < b[i])return false;
    }
    return true;
}

void CIPLayer::Routing(unsigned char* dest_ip, unsigned char* ppayload, int iosel) {
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;
    unsigned char masked[IP_ADDR_SIZE] = { 0, };

    for each (ROUTING_TABLE_NODE row in route_table) {       //table에서 한 열씩 뽑아...
        for (int i = 0; i < IP_ADDR_SIZE; i++)masked[i] = dest_ip[i] & row.netmask[i];        //마스킹 해보고
        if (memcmp(masked, row.destination_ip, IP_ADDR_SIZE)==0) {       //IP addr의 결과가 같다면,
            if ((row.flag & IP_ROUTE_UP) && (row.flag & IP_ROUTE_HOST)) {    
                mp_UnderLayer->RSend(ppayload, pFrame->tlength, dest_ip, row._interface);
            }
            else if ((row.flag & IP_ROUTE_UP)&& (row.flag & IP_ROUTE_GATEWAY)) {
                mp_UnderLayer->RSend(ppayload, pFrame->tlength, row.gateway, row._interface); //Gateway IP의 MAC address를 얻기 위해...
            }
            return;
        }
    }
}

CIPLayer::_ROUTING_TABLE_NODE::_ROUTING_TABLE_NODE(const _ROUTING_TABLE_NODE& other)
{
    memcpy(destination_ip, other.destination_ip, IP_ADDR_SIZE);
    memcpy(gateway, other.gateway, IP_ADDR_SIZE);
    memcpy(netmask, other.netmask, IP_ADDR_SIZE);
    flag = other.flag;
    _interface = other._interface;
}

CIPLayer::_ROUTING_TABLE_NODE::_ROUTING_TABLE_NODE()
{
    memset(destination_ip, 0, IP_ADDR_SIZE);
    memset(gateway, 0, IP_ADDR_SIZE);
    memset(netmask, 0, IP_ADDR_SIZE);
    flag = 0;
    _interface = 1;
}