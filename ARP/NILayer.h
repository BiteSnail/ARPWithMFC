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
    UCHAR*          SetAdapter(const int index); //set inum, return value is MAC ADDRESS
    void            SetAdapterComboBox(CComboBox& adpaterlist);
    void            GetMacAddressList(CStringArray& adapterlist);
    static UINT     ThreadFunction_RECEIVE(LPVOID pParam);
    void Receiveflip();
protected:
    pcap_if_t* allDevices; //all information of 
    pcap_if_t* device; //adapter index
    pcap_t* m_AdapterObject;
    LPADAPTER adapter = NULL;
    PPACKET_OID_DATA OidData;
    UCHAR data[ETHER_MAX_SIZE];
    bool canRead = false;
};

