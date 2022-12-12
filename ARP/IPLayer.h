#pragma once
#include "BaseLayer.h"
#include "pch.h"
#include<list>

class CIPLayer : public CBaseLayer {

private:
    void ResetHeader();

public:

    BOOL			Receive(unsigned char* ppayload);
    BOOL			Send(unsigned char* ppayload, int nlength);
    void			SetDestinAddress(unsigned char* pAddress);
    void			SetSourceAddress(unsigned char* pAddress);
    unsigned char* GetDestinAddress();
    unsigned char* GetSourceAddress();
    void            AddRouteTable(unsigned char* _destination_ip, unsigned char* _netmask, unsigned char* _gateway, unsigned char _flag, unsigned char interFace);

    bool LongestPrefix(unsigned char* a, unsigned char* b);

    void Routing(unsigned char* dest_ip, unsigned char* ppayload);

    CIPLayer(char* pName);
    virtual ~CIPLayer();

    typedef struct _IP_HEADER {
        unsigned char   ver_hlegnth; // 4-bit IPv4 version, 4-bit header length
        unsigned char   tos;         // IP type of service
        unsigned short  tlength;     // Total length

        unsigned short  id;          // Unique identifier
        unsigned char   offset;      // Fragment offset field

        unsigned char   ttl;         // Time to live
        unsigned char   ptype;       // Protocol type
        unsigned short  checksum;    // IP checksum

        unsigned char   ip_srcaddr[IP_ADDR_SIZE];
        unsigned char   ip_dstaddr[IP_ADDR_SIZE];
        unsigned char   ip_data[IP_MAX_DATA_SIZE];

    } IP_HEADER, * PIP_HEADER;

    typedef struct _ROUTING_TABLE_NODE {
        unsigned char destination_ip[IP_ADDR_SIZE];
        unsigned char netmask[IP_ADDR_SIZE];
        unsigned char gateway[IP_ADDR_SIZE];
        unsigned char flag;          // Host: 1, Gateway:2
        unsigned char _interface;
    }ROUTING_TABLE_NODE, * IP_TABLE;


    std::list<ROUTING_TABLE_NODE> route_table;

protected:
    IP_HEADER m_sHeader;
};
