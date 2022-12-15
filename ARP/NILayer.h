#pragma once
#include "BaseLayer.h"
#include "pch.h"
#include <pcap.h>
#include <tchar.h>
#include <Packet32.h>
#include <WinSock2.h>
#include <thread>

const int INNER = 0;
const int OUTER = 1;

class CNILayer :
    public CBaseLayer
{

public:
    CNILayer(char* pName);
    virtual ~CNILayer();
    virtual BOOL    Receive(unsigned char* pkt, int iosel) override;
    virtual BOOL    Send(unsigned char* ppayload, int nlength, int iosel) override;
    //from index number, return MAC ADDRESS
    UCHAR*          SetAdapter(const int index, const int iosel); 
    //Set Adapter List into combobox
    void            SetAdapterComboBox(CComboBox& adpaterlist);
    //Get MacAddressList CStringArray
    void            GetMacAddressList(CStringArray& adapterlist);
    void            GetMacAddress(const int index, UCHAR *mac, const int iosel);
    /*Get IPv4, IPv6 address from selected Adapter
    !!You need to set adapter first!!
    */
    void            GetIPAddress(CString& ipv4addr, CString& ipv6addr, const int index, bool isIOsel);
    //if canread is True Thread activate
    static UINT     ThreadFunction_RECEIVE(LPVOID pParam);
    static UINT     ThreadFunction_RECEIVE2(LPVOID pParam);
    //set canread option reverse
    void Receiveflip();
protected:
    pcap_if_t* allDevices; //all information of adapters
    pcap_if_t* device[2]; //selected adapter
    pcap_t* m_AdapterObject[2]; 
    LPADAPTER adapter[2];
    PPACKET_OID_DATA OidData[2];
    UCHAR data[2][ETHER_MAX_SIZE];
    bool canRead;
};
