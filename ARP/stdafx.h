// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)
#define AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}

#define MAX_LAYER_NUMBER		0xff

#define ETHER_MAX_SIZE			1514
#define ETHER_HEADER_SIZE		14
#define ETHER_MAX_DATA_SIZE		( ETHER_MAX_SIZE - ETHER_HEADER_SIZE )

#define IP_MAX_SIZE             1500
#define IP_HEADER_SIZE          20
#define IP_MAX_DATA_SIZE        IP_MAX_SIZE - IP_HEADER_SIZE

#define ARP_HEADER_SIZE			28
#define ENET_ADDR_SIZE			6
#define IP_ADDR_SIZE			4
#define ARP_IP_TYPE				0x0008
#define ARP_ENET_TYPE			0x0100

#define ETHER_ARP_TYPE			0x0608
#define ETHER_IP_TYPE			0x0008

#define ARP_TIME_OUT			2
#define ARP_OPCODE_REQUEST		0x0100
#define ARP_OPCODE_REPLY		0x0200
#define ARP_OPCODE_RREQUEST		0x0300
#define ARP_OPCODE_RREPLY		0x0400
#define IPV6_ADDR_LEN			16
#define IPV6_ADDR_STR_LEN		((IPV6_ADDR_LEN*2)+7)

#define DEFAULT_EDIT_TEXT	"-"

#define IP_ROUTE_HOST					0x01
#define IP_ROUTE_GATEWAY				0x02
#define IP_ROUTE_UP						0x40
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)