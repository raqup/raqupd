//      _
//     (_)           _____
//      _ \ _   _   / ____|                                 _            _
//     (_) (_) (_) | (___  _   _ _ __   ___ _ __ ___   __ _| |_ ___  ___| |__
//  _ / _   _ /     \___ \| | | | '_ \ / _ \ '_ ` _ \ / _` | __/ _ \/ __| '_ \
// (_) (_) (_)      ____) | |_| | | | |  __/ | | | | | (_| | ||  __/ (__| | | |
//        \ _      |_____/ \__, |_| |_|\___|_| |_| |_|\__,_|\__\___|\___|_| |_|
//         (_)              __/ |
//                         |___/
//
//  Product:     RaqUp.com - Universal LINUX display driver
//  Version:     Number $Revision: 1.0.0 $ $Build: 48 $
//               of $Date: 2010/05/12 15:48:45 $
//               by $Author: Andreas Pfeil <Andreas.Pfeil@Synematech.com> $
//
//  Description: Network Interface controller & friends
//
//  Copyright (C) 2010 by Synematech.com
//
// -----------------------------------------------------------------------TAB=2

#ifndef _INTERFACE
#define _INTERFACE

#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h> 
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

class Interface {
	char	_path[ 255 ],
	
				_interface[ 32 ],
				_address[ 32 ],
				_netmask[ 32 ],
				_gateway[ 32 ],
				
				_host[ 32 ],
				_domain[ 32 ],
				_ip[ 32 ];
					
	bool	_auto;
	
public:
	enum        MODE { UNDEFINED, STATIC, DHCP };
              Interface( const char *interface = "eth0" );
	
  void        setAutoStart( const bool autoStart );
  void        setMode( enum MODE mode );
  void        setAddress( const char *address );
  void        setNetmask( const char *netmask );
  void        setGateway( const char *gateway );
	
  const bool  isSet() const;
  const char* getInterface() const;
  const bool  getAutoStart() const;
  const char* getMode() const;
  const bool  isMode( const MODE mode );
  const char* getAddress()	const;
  const char* getNetmask()	const;
  const char* getGateway()	const;
	
  void        clear();
  bool        load( const char *path = NULL );
	
  bool        save();
  void        print();
  bool        stop();
  bool        start() ;
  bool        restart();

  const char* getHostName();
  const char* getDomainName();
  const char* getIP( const char *interface );
	
private:
	MODE	_mode;
};

#endif