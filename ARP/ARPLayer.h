#pragma once
#include "BaseLayer.h"
#include "pch.h"
#include <vector>

class ARPLayer :
    public CBaseLayer
{
private:
    inline void     ResetHeader();
public:
    BOOL            Recieve(unsigned char* ppayload);
    BOOL            SEND(unsigned char* ppayload, int nlength);

    ARPLayer(char* pName);
    virtual ~ARPLayer();

    typedef struct _ARP_NODE {
        unsigned char   protocol_addr[IP_ADDR_SIZE];
        unsigned char   hardware_addr[ENET_ADDR_SIZE];
        bool            status;
        unsigned int    ID;
        bool operator==(const unsigned char* ipaddr) {
            return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE)==0;
        }
        bool operator==(const struct _ARP_NODE& ot) {
            return *this == ot.protocol_addr;
        }
        bool operator<(const unsigned char* ipaddr) {
            return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == -1;
        }
        bool operator<(const struct _ARP_NODE& ot) {
            return *this < ot.protocol_addr;
        }
        bool operator>(const unsigned char* ipaddr) {
            return memcmp(protocol_addr, ipaddr, IP_ADDR_SIZE) == 1;
        }
        bool operator>(const struct _ARP_NODE& ot) {
            return *this > ot.protocol_addr;
        }
    }ARP_NODE;

    typedef struct _ARP_HEADER {
        unsigned short  hardware_type;
        unsigned short  protocol_type;
        unsigned char   hardware_length;
        unsigned char   protocol_length;
        unsigned short  opercode;
        unsigned char   hardware_srcaddr[ENET_ADDR_SIZE];
        unsigned char   protocol_srcaddr[IP_ADDR_SIZE];
        unsigned char   hardware_dstaddr[ENET_ADDR_SIZE];
        unsigned char   protocol_dstaddr[IP_ADDR_SIZE];
    } ARP_HEADER, *PARP_HEADER;
protected:
    ARP_HEADER m_sHeader;
    std::vector<ARP_NODE> m_arpTable;
};

