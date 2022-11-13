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
        //IP헤더 type 필요??
		unsigned char	ip_dstaddr[6];
		unsigned char	ip_srcaddr[6];
		//unsigned short	type;
		unsigned char	ip_data[ETHER_MAX_DATA_SIZE];

	} IP_HEADER, *PIP_HEADER;

protected:
    IP_HEADER m_sHeader
};
