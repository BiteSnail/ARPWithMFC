#pragma once
#include "BaseLayer.h"
#include "pch.h"
#include <pcap.h>
#include <tchar.h>
#include <Packet32.h>
#include <WinSock2.h>
#include <thread>
class CNILayer :
    public CBaseLayer
{

public:
    CNILayer(char* pName);
    virtual ~CNILayer();
    virtual BOOL    Receive(unsigned char* pkt) override;
    virtual BOOL    Send(unsigned char* ppayload, int nlength) override;
    //from index number, return MAC ADDRESS
    UCHAR*          SetAdapter(const int index); 
    //Set Adapter List into combobox
    void            SetAdapterComboBox(CComboBox& adpaterlist);
    //Get MacAddressList CStringArray
    void            GetMacAddressList(CStringArray& adapterlist);
    /*Get IPv4, IPv6 address from selected Adapter
    !!You need to set adapter first!!
    */
    void            GetIPAddress(CString& ipv4addr, CString& ipv6addr);
    //if canread is True Thread activate
    static UINT     ThreadFunction_RECEIVE(LPVOID pParam);
    //set canread option reverse
    void Receiveflip();
protected:
    pcap_if_t* allDevices; //all information of adapters
    pcap_if_t* device; //selected adapter
    pcap_t* m_AdapterObject; 
    LPADAPTER adapter;
    PPACKET_OID_DATA OidData;
    UCHAR data[ETHER_MAX_SIZE];
    bool canRead;
};
