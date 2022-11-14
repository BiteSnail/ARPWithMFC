#pragma once
#include "BaseLayer.h"
#include "pch.h"
#include <vector>

void  addrToStr(unsigned short type, CString& dst, unsigned char* src);
void  StrToaddr(unsigned short type, unsigned char* dst, CString& src);

class CARPLayer :
    public CBaseLayer
{
private:
    inline void     ResetHeader();
public:
    BOOL            Receive(unsigned char* ppayload);
    BOOL            Send(unsigned char* ppayload, int nlength);
    int             inCache(const unsigned char* ipaddr);
    void            setType(const unsigned short htype, const unsigned short ptype);
    void            setOpcode(const unsigned short opcode);
    void            setSrcAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]);
    void            setDstAddr(const unsigned char enetAddr[], const unsigned char ipAddr[]);
    void            swapaddr(unsigned char lAddr[], unsigned char rAddr[], const unsigned char size);
    void            updateTable();
    void            setmyAddr(CString MAC, CString IP);
    void            deleteItem(CString IP);
    void            clearTable();
    CARPLayer(char* pName);
    virtual ~CARPLayer();

    typedef struct _ARP_NODE {
        unsigned char   protocol_addr[IP_ADDR_SIZE];
        unsigned char   hardware_addr[ENET_ADDR_SIZE];
        unsigned char   status;
        CTime           spanTime;
        struct _ARP_NODE(unsigned int ipaddr, unsigned int enetaddr, unsigned char incomplete);
        struct _ARP_NODE(unsigned char* cipaddr, unsigned char* cenetaddr, unsigned char bincomplete);
        struct _ARP_NODE(const struct _ARP_NODE& ot);

        bool operator==(const unsigned char* ipaddr);
        bool operator==(const struct _ARP_NODE& ot);
        bool operator<(const unsigned char* ipaddr);
        bool operator<(const struct _ARP_NODE& ot);
        bool operator>(const unsigned char* ipaddr);
        bool operator>(const struct _ARP_NODE& ot);
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
        struct _ARP_HEADER();
        struct _ARP_HEADER(const struct _ARP_HEADER& ot);
    } ARP_HEADER, * PARP_HEADER;

    //DEBUG
    typedef struct _IP_HEADER {
        unsigned char   ver_hlegnth;
        unsigned char   tos;
        unsigned short  tlength;

        unsigned short  id;
        unsigned char   offset;

        unsigned char   ttl;
        unsigned char   ptype;
        unsigned short  checksum;

        unsigned char   srcaddr[IP_ADDR_SIZE];
        unsigned char   dstaddr[IP_ADDR_SIZE];
        unsigned char   data[ETHER_MAX_SIZE - ETHER_HEADER_SIZE - 20];
    }IP_HEADER, * PIP_HEADER;

    std::vector<ARP_NODE> getTable();

protected:
    unsigned char myip[IP_ADDR_SIZE];
    unsigned char mymac[ENET_ADDR_SIZE];
    ARP_HEADER m_sHeader;
    std::vector<ARP_NODE> m_arpTable;
};
