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
//  Description: UART Interface controller
//
//  Copyright (C) 2010 by Synematech.com
//
// -----------------------------------------------------------------------TAB=2

#include "uart.h"

void UART::set_fl( int fd, int flags ) { 
  int val;
  if( (val = fcntl( fd, F_GETFL, 0 ) ) < 0 ) perror( "fcntl F_GETFL error" ); 
  val |= flags; // turn on flags 
  if( fcntl( fd, F_SETFL, val ) < 0 ) perror( "fcntl F_SETFL error" );
}

void UART::clr_fl( int fd, int flags ) { 
  int val;
  if( (val = fcntl( fd, F_GETFL, 0 ) ) < 0 ) perror( "fcntl F_GETFL error" ); 
  val &= ~flags; // turn off flags 
  if( fcntl( fd, F_SETFL, val ) < 0 ) perror( "fcntl F_SETFL error" );
}

UART::UART( const char* device ) : fd( -1 ) {
  fd = open( device, O_RDWR | O_NOCTTY | O_NONBLOCK ); // 
  if( fd < 0 ) {
    perror( device ); 
    exit( -1 ); 
  }
 // fcntl(fd, F_SETFL, FASYNC);
  
  tcgetattr( fd, &parameter_backup ); /* save current serial port settings */
  
  bzero( &parameter, sizeof(parameter) ); /* clear struct for new port settings */
  //  BAUD | CRTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;

  parameter.c_cflag = B4800 | CS8 | CLOCAL | CREAD;
  parameter.c_iflag = IGNPAR | ICRNL | IGNBRK;
  parameter.c_oflag = 0;
  parameter.c_lflag =  0; // ICANON;
  parameter.c_cc[VMIN] = 1;
  parameter.c_cc[VTIME] = 0;

  tcflush( fd, TCIFLUSH );
  tcsetattr( fd, TCSANOW, &parameter );
  
  FD_ZERO( &readset );
  FD_ZERO( &writeset );  
  
  FD_SET( fd, &readset );
}

UART::~UART() {
  if( fd < 0 ) return;
  tcsetattr( fd, TCSANOW, &parameter_backup );
  close( fd );
}

bool UART::hasData() {
  struct timeval noWait = { 0, 0 };
  return select( fd + 1, &readset, &writeset, NULL, &noWait ) > 0;
}

char UART::getc( const bool blocking ) {
  char  buffer[] = { 0, 0 };
  
  if( blocking ) clr_fl( fd, O_NONBLOCK );
  
  int length = ::read( fd, buffer, 1 );
  //printf( "RECEIVE FROM DISPLAY: %s %d\r\n", buffer, buffer[0]);
  if( blocking ) set_fl( fd, O_NONBLOCK );
  
  return buffer[ 0 ];
}

int UART::read( char *buffer, unsigned int maxLength ) {
  int length = ::read( fd, buffer, maxLength );
  if( length < 0 ) length = 0;
  buffer[ length ] = 0;
  return length;
}

void UART::write( const char *buffer, const int length ) {
  int l = length;
  if( l < 0 ) l = strlen( buffer );
//printf( "SEND TO DISPLAY: >%s< (%d)\r\n", buffer, l );
  ::write( fd, buffer, l );
}
