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

#include "interface.h"

Interface::Interface( const char *interface ) {
	clear();
	strncpy( _interface, interface, sizeof( _interface ) );
	load( "/etc/network/interfaces" );
}
	
void Interface::setAutoStart( const bool autoStart ) {
	_auto = autoStart;
} 

void Interface::setMode( enum MODE mode ) {
	_mode = mode;
}
	
void Interface::setAddress( const char *address ) {
	strncpy( _address, address, sizeof( _address ) );
}

void Interface::setNetmask( const char *netmask ) {
	strncpy( _netmask, netmask, sizeof( _netmask ) );
}

void Interface::setGateway( const char *gateway ) {
	strncpy( _gateway, gateway, sizeof( _gateway ) );
}
	
const bool Interface::isSet() const {
	return _mode != UNDEFINED;
}
	
const char* Interface::getInterface() const { 
  return _interface; 
}

const bool Interface::getAutoStart() const { 
  return _auto; 
}
	
const char* Interface::getMode() const {
		switch( _mode ) {
		case STATIC:	return "static";
		case DHCP:		return "dhcp";	 
	}								return "undefined";
}

const bool Interface::isMode( const MODE mode ) {
	return _mode == mode;
}

const char* Interface::getAddress()	const { 
  return _address;
}

const char* Interface::getNetmask()	const { 
  return _netmask; 
}

const char* Interface::getGateway()	const { 
  return _gateway; 
}
	
void Interface::clear() {
	_auto						= false;
	_address[ 0 ]		= 0;
	_netmask[ 0 ]		= 0;
	_gateway[ 0 ]		= 0;
	_mode						= UNDEFINED;
}
	
bool Interface::load( const char *path ) {
	clear();
	
	if( path	!= NULL ) strncpy( _path, path, sizeof( _path ) );
	if( _path == NULL ) return false;
	
	bool	active = false;
	
	char line[ 1024 ],
			 parameter[ 3 ][ 32 ];

	FILE *fp = fopen( _path, "r" );
	if( fp == NULL ) goto on_load_error;

	while( NULL != fgets( line, sizeof( line ), fp ) ) {
		if( line[ 0 ] == '#' ) continue;
		
		if( 1 == sscanf( line, "auto %s ", parameter[ 0 ] ) ) {
			if( strncmp( _interface, parameter[ 0 ], strlen( _interface ) ) == 0 ) _auto = true;
		
		} else if( 3 == sscanf( line, "iface %s %s %s", parameter[ 0 ], parameter[ 1 ], parameter[ 2 ] ) ) {
			active = ( strncmp( _interface, parameter[ 0 ], strlen( _interface ) ) == 0 );
			if( active ) {
						 if( strcmp( parameter[ 2 ], "dhcp" ) == 0 )		_mode = DHCP;
				else if( strcmp( parameter[ 2 ], "static" ) == 0 )	_mode = STATIC;
				else goto on_load_error;
			}
			
		} else if( 1 == sscanf( line, "address %s", parameter[ 0 ] ) ) {
			if( active ) strcpy( _address, parameter[ 0 ] );
		
		} else if( 1 == sscanf( line, "netmask %s", parameter[ 0 ] ) ) {
			if( active ) strcpy( _netmask, parameter[ 0 ] );
		
		} else if( 1 == sscanf( line, "gateway %s", parameter[ 0 ] ) ) {
			if( active ) strcpy( _gateway, parameter[ 0 ] );
		}
	}
	fclose( fp );
	return true;
on_load_error:
	clear();
	if( fp != NULL ) fclose( fp );
	return false;
}
	
bool Interface::save() {	
	if( _mode == UNDEFINED ) return false;
	if( _path == NULL )			 return false;
	
	char line[ 1024 ],
			 tempPath[ 255 ],
			 parameter[ 3 ][ 32 ];
			 
	bool	active = false;
	
	strncpy( tempPath, _path, sizeof( tempPath ) );
	strcat( tempPath, "_temp" );
	
	rename( _path, tempPath );
	
	FILE *in	= fopen( tempPath, "r" );
	FILE *out = fopen( _path, "w" );
	
	if( in	== NULL 
	||	out == NULL ) goto on_save_error;
	
	while( NULL != fgets( line, sizeof( line ), in ) ) {
		
		if( 1 == sscanf( line, "auto %s ", parameter[ 0 ] ) ) {
			if( strncmp( _interface, parameter[ 0 ], strlen( _interface ) ) == 0 ) {
				if( _auto ) fprintf( out, "auto %s\n", _interface );
			} else        fputs( line, out );
		
		} else if( 3 == sscanf( line, "iface %s %s %s", parameter[ 0 ], parameter[ 1 ], parameter[ 2 ] ) ) {
			active = ( strncmp( _interface, parameter[ 0 ], strlen( _interface ) ) == 0 );
			
			if( active )	fprintf( out, "iface %s inet %s\n", _interface, ( _mode == STATIC ? "static" : "dhcp" ) );
			else					fputs( line, out );
			
		} else if( 1 == sscanf( line, "address %s", parameter[ 0 ] ) ) {
			if( active )	fprintf( out, "address %s\n", _address );
			else					fputs( line, out );
		
		} else if( 1 == sscanf( line, "netmask %s", parameter[ 0 ] ) ) {
			if( active )	fprintf( out, "netmask %s\n", _netmask );
			else					fputs( line, out );
		
		} else if( 1 == sscanf( line, "gateway %s", parameter[ 0 ] ) ) {
			if( active )	fprintf( out, "gateway %s\n", _gateway );
			else					fputs( line, out );
		
		} else fputs( line, out );
	}
	
	fclose( in );
	fclose( out );
	
	remove( tempPath );
	return true;
	
on_save_error:
 
	if( in	!= NULL ) fclose( in );
	if( out != NULL ) fclose( out );
	
	remove( _path );
	rename( tempPath, _path );
	
	return false;
}

void Interface::print() {
	printf( "INTERFACE: %s\r\n", getInterface() );
	printf( "AUTO: %s\r\n", ( getAutoStart() ? "YES" : "NO" ) );
	printf( "MODE: %s\r\n", getMode());
	printf( "ADDRESS: %s\r\n", getAddress() );
	printf( "NETMASK: %s\r\n", getNetmask() );
	printf( "GATEWAY: %s\r\n", getGateway() );
}
	
bool Interface::stop() {
	if( _mode == UNDEFINED ) return false;
	char command[ 1024 ];
	sprintf( command, "service network-interface stop INTERFACE=%s 1> /dev/null 2> /dev/null", getInterface() );
	int result = system( command );
	return result > 0 && result < 127;
}

bool Interface::start() {
		if( _mode == UNDEFINED ) return false;
	char command[ 1024 ];
	sprintf( command, "service network-interface start INTERFACE=%s 1> /dev/null 2> /dev/null", getInterface() );
	int result = system( command );
	return result > 0 && result < 127;
}
	
bool Interface::restart() {
	if( _mode == UNDEFINED ) return false;
	char command[ 1024 ];
	sprintf( command, "service network-interface restart INTERFACE=%s 1> /dev/null 2> /dev/null", getInterface() );
	int result = system( command );
	return result > 0 && result < 127;
}

const char* Interface::getHostName() {
	// http://www.kernel.org/doc/man-pages/online/pages/man3/getifaddrs.3.html
	// http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html

	if( 0 != gethostname( _host, sizeof( _host ) ) ) {
		if( NULL != getenv( "HOSTNAME" ) ) strncpy( _host, getenv( "HOSTNAME" ), sizeof( _host ) );
		else {
			FILE *fp = fopen( "/etc/hostname", "r" );
			if( fp != NULL ) {
				fgets( _host, sizeof( _host ), fp );
				fclose( fp );
			}
		}
	}
	return _host;
}
	
const char* Interface::getDomainName() {
	// returns static data (http://www.opengroup.org/onlinepubs/009695399/functions/gethostbyaddr.html)
	
	getdomainname( _domain,	 sizeof( _domain ) );
	if( strcmp( _domain, "(none)" ) == 0 ) {
		FILE *fp = fopen( "/etc/resolv.conf", "r" );
		if( fp != NULL ) {
			char line[ 1024 ];
			while( fgets( line, sizeof( line ), fp ) != NULL ) {
				if( sscanf( line, "domain %s", _domain ) == 1 ) break;
			} fclose( fp );
		} else {
			in_addr_t ip					= inet_addr( isSet() ? getAddress() : getIP( getInterface() ) ); 
			struct hostent *host	= gethostbyaddr( &ip, INET_ADDRSTRLEN, AF_INET ); 
			sprintf( _domain, "%s", host->h_name );
		}
	}
	return _domain;
}

const char* Interface::getIP( const char *interface ) {
	struct ifaddrs	*ifaddr = NULL, *ifa;
	
	if( getifaddrs( &ifaddr ) != -1 ) for( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next ) {
		if( ifa->ifa_addr->sa_family != AF_INET )				continue;
		if( strcmp( ifa->ifa_name, interface ) != 0 )		continue;
		
		void *tmpAddrPtr = &( (struct sockaddr_in *)ifa->ifa_addr )->sin_addr;
		inet_ntop( AF_INET, tmpAddrPtr, _ip, INET_ADDRSTRLEN );
		break;
	} freeifaddrs( ifaddr );
	 
	return _ip;
}