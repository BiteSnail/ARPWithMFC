#pragma once
#include "BaseLayer.h"
#include "pch.h"
#include<list>

class CIPLayer : public CBaseLayer {

private:
    void ResetHeader(int iosel);

public:

    BOOL			Receive(unsigned char* ppayload, int iosel);
    BOOL			Send(unsigned char* ppayload, int nlength, int iosel);
    BOOL            RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel);
    void			SetDestinAddress(unsigned char* pAddress, int iosel);
    void			SetSourceAddress(unsigned char* pAddress, int iosel);
    unsigned char* GetDestinAddress(int iosel);
    unsigned char* GetSourceAddress(int iosel);
    void            AddRouteTable(unsigned char* _destination_ip, unsigned char* _netmask, unsigned char* _gateway, unsigned char _flag, unsigned char interFace);
    void            DelRouteTable(unsigned char index);
    void            SetDefaultGateway(unsigned char* _gateway, unsigned char _flag, unsigned char interFace);
    void            SetHeaderFields(unsigned char* ppayload, int iosel);

    bool            LongestPrefix(unsigned char* a, unsigned char* b);

    void Routing(unsigned char* dest_ip, unsigned char* ppayload, int iosel);

    CIPLayer(char* pName);
    virtual ~CIPLayer();

    typedef struct _IP_HEADER {
        unsigned char   ver_hlegnth; // 4-bit IPv4 version, 4-bit header length
        unsigned char   tos;         // IP type of service
        unsigned short  tlength;     // Total length

        unsigned short  id;          // Unique identifier
        unsigned short  offset;      // Fragment offset field

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
        struct _ROUTING_TABLE_NODE();
        struct _ROUTING_TABLE_NODE(const struct _ROUTING_TABLE_NODE& other);
    }ROUTING_TABLE_NODE, * IP_TABLE;


    std::list<ROUTING_TABLE_NODE> route_table;

protected:
    IP_HEADER m_sHeader[2];
};
