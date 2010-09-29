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
//  Description: Service controller
//
//  Copyright (C) 2010 by Synematech.com
//
// -----------------------------------------------------------------------TAB=2

#ifndef _SERVICE
#define _SERVICE

#include <string.h> 
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

class Service {
	struct SERVICE {
		char	name[ 80 ],
					state;
	} _services[ 200 ];
	unsigned int _serviceCounter;

public:
  Service() ;
  void load();
	
  const unsigned int length() const;
  const char* getServiceName( const unsigned int service ) const;
  const char getServiceState( const unsigned int service ) const;
	
  bool start( const unsigned int service );
  bool stop( const unsigned int service );
};

#endif
