#pragma once
#include "BaseLayer.h"
#include "pch.h"

class IPLayer : public CBaseLayer{

private:
    void ResetHeader();

public:

    BOOL			Receive(unsigned char* ppayload);
    BOOL			Send(unsigned char* ppayload, int nlength);
    void			SetDestinAddress(unsigned char* pAddress);
    void			SetSourceAddress(unsigned char* pAddress);
    unsigned char*  GetDestinAddress();
    unsigned char*  GetSourceAddress();

    IPLayer(char* pName);
	virtual ~IPLayer();

	typedef struct _IP_HEADER {
		unsigned char	ip_dstaddr[4];
		unsigned char	ip_srcaddr[4];
		unsigned char	ip_data[IP_MAX_DATA_SIZE];
        unsigned short  ip_ver = 4;
        unsigned short  ip_hl = 5;
        unsigned short  ip_tos = 0;
        unsigned short  ip_tol = 0;
        unsigned short  ip_fid = 0;
        unsigned short  ip_ttl = 255;
        unsigned short  ip_proto = 6;
        unsigned short  ip_chksum = 0;

	} IP_HEADER, *PIP_HEADER;

protected:
    IP_HEADER m_sHeader;
};
