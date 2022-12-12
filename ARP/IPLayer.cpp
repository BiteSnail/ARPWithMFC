
#include "pch.h"
#include "stdafx.h"
#include "IPLayer.h"



CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName) {
    ResetHeader();
 }

CIPLayer::~CIPLayer() {
}

void CIPLayer::ResetHeader() {
    memset(m_sHeader.ip_srcaddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader.ip_dstaddr, 0, IP_ADDR_SIZE);
    memset(m_sHeader.ip_data, 0, IP_MAX_DATA_SIZE);
}

void CIPLayer::SetHeaderFields(unsigned char* ppayload){
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;
    memcpy(&m_sHeader.ver_hlegnth, &pFrame->ver_hlegnth, 1);
    memcpy(&m_sHeader.tos, &pFrame->tos, 1);
    memcpy(&m_sHeader.tlength, &pFrame->tlength, 2);
    memcpy(&m_sHeader.id, &pFrame->id, 2);
    memcpy(&m_sHeader.ttl, &pFrame->ttl, 1);
    m_sHeader.ttl = m_sHeader.ttl - 1;        //ttl ���ҽ�Ű�� ��
    memcpy(&m_sHeader.ptype, &pFrame->ptype, 1);
    memcpy(&m_sHeader.checksum, &pFrame->checksum, 2);
    SetSourceAddress(pFrame->ip_srcaddr);
    SetDestinAddress(pFrame->ip_dstaddr);
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
    BOOL bSuccess = FALSE;
    bSuccess = this->GetUnderLayer()->Send((unsigned char*)&m_sHeader, IP_HEADER_SIZE + nlength);
    return bSuccess;
}

BOOL CIPLayer::RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP) {
    BOOL bSuccess = FALSE;
    bSuccess = this->GetUnderLayer()->RSend((unsigned char*)&m_sHeader, IP_HEADER_SIZE + nlength, gatewayIP);
    return bSuccess;
}


BOOL CIPLayer::Receive(unsigned char* ppayload) {
    //����� Receive �Լ�
    BOOL bSuccess = FALSE;
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;
    
    //m_sHeader ���� �� ä���
    memcpy(m_sHeader.ip_data, pFrame->ip_data, IP_MAX_DATA_SIZE);       
    SetHeaderFields(ppayload);
    
    Routing(pFrame->ip_dstaddr, ppayload);
    return bSuccess;
}

void CIPLayer::AddRouteTable(unsigned char* _destination_ip, unsigned char* _netmask, unsigned char* _gateway, unsigned char _flag, unsigned char interFace) {
    ROUTING_TABLE_NODE* rt = new ROUTING_TABLE_NODE;
    memcpy(&rt->destination_ip, _destination_ip, IP_ADDR_SIZE);
    memcpy(&rt->netmask, _netmask, IP_ADDR_SIZE);
    memcpy(&rt->gateway, _gateway, IP_ADDR_SIZE);
    rt->flag = _flag;
    rt->_interface = interFace;

    //route_table list���� ���ο� ���� �߰��� ��ġ ã��...
    std::list<ROUTING_TABLE_NODE>::iterator add_point = route_table.begin();
    while (LongestPrefix(add_point->gateway, rt->gateway)) {
        add_point++;
    }
    route_table.insert(add_point, *rt);
}

void CIPLayer::DelRouteTable(unsigned char index) {
    std::list<ROUTING_TABLE_NODE>::iterator del_point = route_table.begin();
    for (int i = 0; i < int(index) - 1; i++) {
        del_point++;
    }
    route_table.erase(del_point);
}

void CIPLayer::SetDefaultGateway(unsigned char* _gateway, unsigned char _flag, unsigned char interFace) {
    unsigned char zero[IP_ADDR_SIZE];              //�� ���� ����� 
    memset(zero, 0, IP_ADDR_SIZE);                 //Destination_IP: '0.0.0.0' Ȯ�ο� ��
    
    if (!memcmp(route_table.back().destination_ip, zero, IP_ADDR_SIZE)) {       //���� defaultgateway�� list node�� ������ ���
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

void CIPLayer::Routing(unsigned char* dest_ip, unsigned char* ppayload) {
    PIP_HEADER pFrame = (PIP_HEADER)ppayload;
    unsigned char masked[IP_ADDR_SIZE] = { 0, };

    for each (ROUTING_TABLE_NODE row in route_table) {       //table���� �� ���� �̾�...
        for (int i = 0; i < IP_ADDR_SIZE; i++)masked[i] = dest_ip[i] & row.netmask[i];        //����ŷ �غ���
        if (memcmp(masked, row.destination_ip, IP_ADDR_SIZE)==0) {       //IP addr�� ����� ���ٸ�,
            if ((row.flag & IP_ROUTE_UP) && (row.flag & IP_ROUTE_HOST)) {    
                Send(ppayload, pFrame->tlength);
            }
            else if ((row.flag & IP_ROUTE_UP)&& (row.flag & IP_ROUTE_GATEWAY)) {
                RSend(ppayload, pFrame->tlength, row.gateway); //Gateway IP�� MAC address�� ��� ����...
            }
            return;
        }
    }
    //TODO default gateway�� ���� �� �����

}