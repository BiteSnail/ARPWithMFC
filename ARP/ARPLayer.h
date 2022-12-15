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
    BOOL            Receive(unsigned char* ppayload, int iosel);
    BOOL            Send(unsigned char* ppayload, int nlength, int iosel);
    BOOL            RSend(unsigned char* ppayload, int nlength, unsigned char* gatewayIP, int iosel);
    int             inCache(unsigned char* ipaddr); //없으면 -1 있으면 해당 인덱스 반환
    void            setType(unsigned short htype, unsigned short ptype, int iosel);
    void            setOpcode(unsigned short opcode, int iosel);
    void            setSrcAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel);
    void            setDstAddr(unsigned char enetAddr[], unsigned char ipAddr[], int iosel);
    void            swapaddr(unsigned char lAddr[], unsigned char rAddr[], unsigned char size);
    void            updateTable();
    void            setmyAddr(CString MAC, CString IP, int iosel);
    void            deleteItem(CString IP);
    void            clearTable();
    void            createProxy(unsigned char *src, unsigned char* ip, unsigned char* enet);
    void            deleteProxy(const int index);
    bool            getMACinARP(unsigned char* dstIP, unsigned char* MAC);
    void            Wait(DWORD dwMillisecond);
    void            isGARP(BOOL yes);
    CARPLayer(char* pName);
    virtual ~CARPLayer();

    typedef struct _PROXY_ARP_NODE {
        unsigned char   srchardware_addr[ENET_ADDR_SIZE];
        unsigned char   protocol_addr[IP_ADDR_SIZE];
        unsigned char   hardware_addr[ENET_ADDR_SIZE];
    public:
        struct _PROXY_ARP_NODE(unsigned char* srcenet, unsigned char* dstip, unsigned char* dstenet);
        bool operator==(const struct _PROXY_ARP_NODE& ot);
    }PROXY_ARP_NODE;

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

    std::vector<ARP_NODE> getTable();

protected:
    unsigned char myip[2][IP_ADDR_SIZE];
    unsigned char mymac[2][ENET_ADDR_SIZE];
    BOOL is_Garp;
    ARP_HEADER m_sHeader[2];
    std::vector<ARP_NODE> m_arpTable;
    std::vector<PROXY_ARP_NODE> m_proxyTable;
};
