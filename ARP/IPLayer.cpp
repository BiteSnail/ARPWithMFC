
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"



CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
    ResetHeader();
    m_sHeader.ver_hlegnth = 0x45;
    m_sHeader.tos = 0x00;
    m_sHeader.tlength = 0xffff;

    m_sHeader.id = 0x0000;
    m_sHeader.offset = 0x00;

    m_sHeader.ttl = 0xff;
    m_sHeader.ptype = 0x06;         //<- tcp number. icmp number로 나중에 수정 필요
    m_sHeader.checksum = 0x0000;
 }

CIPLayer::~CIPLayer() {
}

void CIPLayer::ResetHeader() {
    memset(m_sHeader.ip_srcaddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader.ip_dstaddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader.ip_data, 0, IP_MAX_DATA_SIZE);
}

unsigned char* CIPLayer::GetSourceAddress() {
    return m_sHeader.ip_srcaddr;
}

unsigned char* CIPLayer::GetDestinAddress() {
    return m_sHeader.ip_dstaddr;
}

void CIPLayer::SetSourceAddress(unsigned char* pAddress) {
    memcpy(m_sHeader.ip_srcaddr, pAddress, IP_ADDR_SIZE);
}

void CIPLayer::SetDestinAddress(unsigned char* pAddress) {
    memcpy(m_sHeader.ip_dstaddr, pAddress, IP_ADDR_SIZE);
}
//UpperLayer = AppLayer, UnderLayer = ARPLayer?
BOOL CIPLayer::Send(unsigned char* ppayload, int nlength) {
    memcpy(m_sHeader.ip_data, ppayload, nlength);
    BOOL bSuccess = FALSE;
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, IP_HEADER_SIZE + nlength);
    return bSuccess;
}

BOOL CIPLayer::Receive(unsigned char* ppayload) {
    //변경된 Receive 함수
    BOOL bSuccess = FALSE;
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;

    Routing(pFrame->ip_dstaddr, ppayload);
    return bSuccess;
}

void CIPLayer::AddRouteTable(unsigned char* _destination_ip, unsigned char* _netmask, unsigned char* _gateway, unsigned char _flag, unsigned char interFace) {
    ROUTING_TABLE_NODE rt;
    memcpy(&rt.destination_ip, _destination_ip, IP_ADDR_SIZE);
    memcpy(&rt.netmask, _netmask, IP_ADDR_SIZE);
    memcpy(&rt.gateway, _gateway, IP_ADDR_SIZE);
    rt.flag = _flag;
    rt._interface = interFace;

    //route_table list에서 새로운 열을 추가할 위치 찾기...
    std::list<ROUTING_TABLE_NODE>::iterator add_point = route_table.begin();
    while (LongestPrefix(add_point->gateway, rt.gateway)) {
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

bool CIPLayer::LongestPrefix(unsigned char* a, unsigned char* b) {
    for (int i = 0; i < 4; i++) {
        if (a[i] < b[i])return false;
    }
    return true;
}

void CIPLayer::Routing(unsigned char* dest_ip, unsigned char* ppayload) {
    unsigned char masked[4];

    for each (ROUTING_TABLE_NODE row in route_table) {       //table에서 한 열씩 뽑아...
        for (int i = 0; i < IP_ADDR_SIZE; i++)masked[i] = dest_ip[i] & row.netmask[i];        //마스킹 해보고
        if (memcmp(masked, row.destination_ip, 4) == 0) {       //결과가 같다면,
            if (row.flag == 1) {    //flag: Host
                SetDestinAddress(dest_ip);
                Send(ppayload, sizeof(ppayload));
            }
            else {                  //flag: Gateway
                SetDestinAddress(row.destination_ip);
                Send(ppayload, sizeof(ppayload));
            }
        }
    }

}