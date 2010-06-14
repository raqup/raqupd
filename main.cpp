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
//  Description: Main loop
//
//  Copyright (C) 2010 by Synematech.com
//
// -----------------------------------------------------------------------TAB=2

#include "application.cpp"

int main( int argc, char **argv ) {
  bool  becomeDaemon = false;
  char  device[]  = "/dev/ttyUSB0",
        line1[]   = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
        line2[]   = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    
  for( int i = 1; i < argc; i++ ) {
    if( argv[ i ][ 0 ] == '-' ) for( int j = 1; argv[ i ][ j ] != 0; j++ ) {
           if( argv[ i ][ j ] == 'd' )  becomeDaemon = true;
      else if( argv[ i ][ j ] == '1' )  strncpy( line1, argv[ ++i ], sizeof( line1 ) );
      else if( argv[ i ][ j ] == '2' )  strncpy( line2, argv[ ++i ], sizeof( line2 ) );
      else if( argv[ i ][ j ] == 'c' )  {
        Cobalt cobalt( device );
        cobalt.clear();
        return 0;
      }
    }
  }
  
  if( line1[ 0 ] != 0 ) {
    Cobalt cobalt( device );
    cobalt.screen( line1, line2 );
    return 0;
  }
  
  if( becomeDaemon ) {
    pid_t                               pid;
          if( ( pid = fork() ) < 0 )    return -1;
    else  if( pid != 0 )                exit( 0 );
                                        setsid();
                                        chdir( "/" );
                                        umask( 0 );
  }
  
  Application                           app( device );
  return                                app.run();
}
