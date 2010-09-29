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

#include "service.h"

Service::Service() {
	_serviceCounter = 0;
}
	
void Service::load() {
	FILE *fp = popen( "service --status-all 2> /dev/null", "r" );
	if( fp == NULL ) return;
	char line[ 1024 ], state, name[ 1024 ];
	for( _serviceCounter = 0; NULL != fgets( line, sizeof( line ), fp ); ) {
		sscanf( line, " [ %c ]	%s", &state, name );
		if( strcmp( name, "raqup" ) == 0 ) continue;
		
		_services[ _serviceCounter ].state = state;
		strncpy( _services[ _serviceCounter ].name, name, sizeof( _services[ _serviceCounter ].name ) );
		//sscanf( line, " [ %c ]	%s", &(_services[ _serviceCounter ].state), _services[ _serviceCounter ].name );
	
		_serviceCounter++;
	}
	pclose( fp );
}

const unsigned int Service::length() const { 
  return _serviceCounter; 
}

const char* Service::getServiceName( const unsigned int service ) const {
	if( service > _serviceCounter ) return NULL;
	return _services[ service ].name;
}

const char Service::getServiceState( const unsigned int service ) const {
	if( service > _serviceCounter ) return 0;
	return _services[ service ].state;
}
	
bool Service::start( const unsigned int service ) {
	if( service > _serviceCounter ) return false;
	
	char command[ 1024 ];
	
	sprintf( command, "update-rc.d %s defaults 1> /dev/null 2> /dev/null", getServiceName( service ) );
	system( command );
	
	sprintf( command, "service %s start 1> /dev/null 2> /dev/null", getServiceName( service ) );
	int result = system( command );
	
	return result > 0 && result < 127;
} 

bool Service::stop( const unsigned int service ) {
	if( service > _serviceCounter ) return false;
	
	char command[ 1024 ];

	sprintf( command, "service %s stop 1> /dev/null 2> /dev/null", getServiceName( service ) );
	int result = system( command );
	
	sprintf( command, "update-rc.d -f %s remove 1> /dev/null 2> /dev/null", getServiceName( service ) );
	system( command );
	
	return result > 0 && result < 127;
}

