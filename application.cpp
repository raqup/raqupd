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
//  Description: Application controller
//
//  Copyright (C) 2010 by Synematech.com
//
// -----------------------------------------------------------------------TAB=2

// http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

#include "cobalt.cpp"
#include "service.cpp"
#include "interface.cpp"

class Application {
  long          _user, _nice, _system, _idle, _iowait, _irq, _softirq, dummy;
  long          _recive_packets, _recive_errors,
                _transmit_packets, _transmit_errors, _transmit_collisions;
  unsigned int  delay, externDelay;
  FILE          *mii_tool;
  bool          _rxtx, _collision;

  char          externLine1[ 16 ],
                externLine2[ 16 ];

  Cobalt        cobalt;
  Interface     interface;
  Service       service;

  enum Screen { INFO, CPU, MEM, LOAD, UPTIME,

                EXTERN,

                SETUP_NETWORK, NETWORK,
                SETUP_SERVICE, SERVICE,
                REBOOT, REBOOT_DO,
                SHUTDOWN, SHUTDOWN_DO,
                RESET_PASSWORD,

                SKIP, SELECT
              } last, next;

  void formatNumber( char *buffer, float value ) {
    char                unit = 'K';

    if( value >= 999 ) { value /= 1024; unit = 'M'; }
    if( value >= 999 ) { value /= 1024; unit = 'G'; }
    if( value >= 999 ) { value /= 1024; unit = 'T'; }


    snprintf( buffer, 5, "%3.0f%c", value, unit );
    //printf( "FORMAT NUMBER: %3.0f %c - %s\r\n", value, unit, buffer );
  }

public:
  Application( const char* device, const unsigned int delay = 100 ) : cobalt( device ), interface( "eth0" ) {
    this->delay = delay;
    next        = Application::INFO;
    mii_tool    = NULL;

    if( interface.isSet() ) {
      char command[ 1024 ];
      sprintf( command, "exec mii-tool -w %s", interface.getInterface() );
      mii_tool    = popen( command, "r" );

      // http://stackoverflow.com/questions/149860/how-to-prevent-fgets-blocks-when-file-stream-has-no-new-data
      int miitool = fileno( mii_tool );
      int flags   = fcntl( miitool, F_GETFL, 0 );
          flags  |= O_NONBLOCK;
                    fcntl( miitool, F_SETFL, flags );
    }
    FILE *fp = fopen( "/proc/stat", "r" );
    if( fp != NULL ) {
    fscanf( fp, "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld\r\n",
              &_user,
              &_nice,
              &_system,
              &_idle,
              &_iowait,
              &_irq,
              &_softirq,
              &dummy,
              &dummy
            );
      fclose( fp );
    }
  }
  ~Application() {
    if( mii_tool != NULL ) pclose( mii_tool );

    cobalt.setTxRx( false );
    cobalt.setCollision( false );
    cobalt.setLink( false );
    cobalt.set100MBit( false );
    cobalt.clear();
  }

  void print( const char *line1, const char *line2, const unsigned int delay = 10 ) {
    if( next != Application::EXTERN ) last = next;
    externDelay = delay;
    strncpy( externLine1, line1, sizeof( externLine1 ) );
    strncpy( externLine2, line2, sizeof( externLine2 ) );
    cobalt.inject( 'X' );
  }

  int run() {
    while( true ) switch( next ) {
      // INFO Screens

      case Application::INFO:           next = sysInfo( UPTIME, CPU );    break;
      case Application::CPU:            next = cpuInfo( INFO, MEM );      break;
      case Application::MEM:            next = memInfo( CPU, LOAD );      break;
      case Application::LOAD:           next = loadInfo( MEM, UPTIME );   break;
      case Application::UPTIME:         next = uptimeInfo( LOAD, INFO );  break;

      case Application::EXTERN:
        cobalt.screen( externLine1, externLine2 );
        for( unsigned int i = 0; i < externDelay * 1000 / delay; i++ ) {
          switch( cobalt.getc( false ) ) {
          case 'L': case 'R': case 'U': case 'D':
          case 'S': case 'E': case 'P':
            i = externDelay * 1000 / delay;
            break;
          } cobalt.sleep( 0, delay );
        }
        next = last;
        break;

      // SETUP_NETWORK;
      case Application::SETUP_NETWORK:
        next = ( interface.isMode( Interface::UNDEFINED ) ? SETUP_SERVICE : menuItem( "SELECT:", "  SETUP NETWORK", SETUP_SERVICE, NETWORK ) );
        break;
      case Application::NETWORK:
        switch( request( "IP ALLOCATION:", "[S]TATIC", "[D]HCP", ( interface.isMode( Interface::STATIC ) ? 0 : 1 ) ) ) {
        case 0:
          interface.setMode( Interface::STATIC );
          interface.setAddress( editIP( "ENTER IP ADDR:", interface.getAddress() ) );
          interface.setNetmask( editIP( "ENTER NETMASK:", interface.getNetmask() ) );
          interface.setGateway( editIP( "ENTER GATEWAY:", interface.getGateway() ) );
          break;
        case 1:
          interface.setMode( Interface::DHCP );
          break;
        }
        switch( request( "SAVE SETTINGS", "[S]AVE", "[C]ANCLE" ) ) {
        case 0:
          interface.save();
          interface.restart();
        case 1:
          interface.load();
        }
        next = INFO;
        break;

      // SETUP SERVICES
      case Application::SETUP_SERVICE:
        next = menuItem( "SELECT:", "  SETUP SERVICES", REBOOT, SERVICE );
        break;
      case Application::SERVICE:
        configServices();
        next = INFO;
        break;

      // Management Functions
      case Application::REBOOT:
        next = menuItem( "SELECT:", "  REBOOT", SHUTDOWN, REBOOT_DO );
        break;
      case Application::REBOOT_DO:
        switch( request( "REBOOT:", "[Y]ES", "[N]O", 1 ) ) {
        case 0:
          system( "reboot" );
          cobalt.reBoot();
          return 0;
        case 1:
          next = INFO;
        } break;

      case Application::SHUTDOWN:
        next = menuItem( "SELECT:", "  POWER DOWN", INFO, SHUTDOWN_DO );
        break;
      case Application::SHUTDOWN_DO:
        switch( request( "POWER DOWN:", "[Y]ES", "[N]O", 1 ) ) {
        case 0:
          system( "shutdown -P now" );
          cobalt.shutDown();
          return 0;
        case 1:
          next = INFO;
          break;
        } break;

      case Application::RESET_PASSWORD:
        if( 0 == request( "RESET PASSWORD:", "[Y]ES", "[N]O", 1 ) ) resetPassword();
        next = INFO;
        break;

    } return 0;
  }

  void updateLEDs() {
    long  recive_packets, recive_errors, transmit_packets, transmit_errors, transmit_collisions;

    char  sdummy[ 1024 ],
          interface[ 10 ];
    long  ldummy;
    char  i, j;
    bool  rxtx      = false,
          collision = false;

    FILE *fp = fopen( "/proc/net/dev", "r" );
    if( fp != NULL ) {
      fgets( sdummy, sizeof( sdummy ), fp );
      fgets( sdummy, sizeof( sdummy ), fp );

      while( fgets( sdummy, sizeof( sdummy ), fp ) != NULL ) {
        //printf( "line>%s\r\n", sdummy );

        for( i = 0, j = 0; i < sizeof( interface ) - 1 && sdummy[ j ] != ':'; j++ )
          if( sdummy[ j ] != ' ' ) interface[ i++ ] = sdummy[ j ];
        interface[ i ] = 0;
        j++;
        //printf( "interface>%s\r\n", interface );
        //printf( "line>%s\r\n", sdummy + j );
        if( 0 == sscanf( sdummy + j, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
                       &ldummy,
                       &recive_packets,
                       &recive_errors,
                       &ldummy,
                       &ldummy,
                       &ldummy,
                       &ldummy,
                       &ldummy,
                       &ldummy,
                       &transmit_packets,
                       &transmit_errors,
                       &ldummy,
                       &ldummy,
                       &transmit_collisions,
                       &ldummy,
                       &ldummy
        )              ) break;

        if( strcmp( interface, this->interface.getInterface() ) != 0 ) continue;

        if( recive_packets      != _recive_packets )      rxtx      = true;
        if( transmit_packets    != _transmit_packets )    rxtx      = true;

        if( recive_errors       != _recive_errors )       collision = true;
        if( transmit_errors     != _transmit_errors )     collision = true;
        if( transmit_collisions != _transmit_collisions ) collision = true;

        _recive_packets         = recive_packets;
        _transmit_packets       = transmit_packets;

        _recive_errors          = recive_errors;
        _transmit_errors        = transmit_errors;
        _transmit_collisions    = transmit_collisions;

        if( rxtx      != _rxtx      ) cobalt.setTxRx( rxtx );
        if( collision != _collision ) cobalt.setCollision( collision );

        _rxtx                 = rxtx;
        _collision            = collision;

//      printf( "%s, %ld, %ld, %ld, %ld, %ld\r\n", interface, recive_packets, recive_errors, transmit_packets, transmit_errors, transmit_collisions );
//      printf( "%s, %ld, %ld, %ld, %ld, %ld\r\n", interface, _recive_packets, _recive_errors, _transmit_packets, _transmit_errors, _transmit_collisions );
//    printf( "%d %d\r\n", rxtx, collision );
      }
      fclose( fp );
     }

    if( mii_tool != NULL ) {
      if( fgets( sdummy, sizeof( sdummy ), mii_tool ) != NULL ) {
        cobalt.setLink( strstr( sdummy, "link ok" ) != NULL );
        cobalt.set100MBit( strstr( sdummy, "100" ) != NULL );
        // printf( ">%s\r\n", sdummy );
      }
    }

  }

  enum Screen sysInfo( enum Screen previous, enum Screen next ) {
    char  line1[ 80 ],
          line2[ 80 ],

          oline1[ 80 ],
          oline2[ 80 ];
          
    line1[ 0 ]  = 0;
    line2[ 0 ]  = 0;
    oline1[ 0 ] = 0;
    oline2[ 0 ] = 0;
    
    while( true ) {
      sprintf( line2, "%s", ( interface.isSet() ? interface.getAddress() : interface.getIP( interface.getInterface() ) ) );
      sprintf( line1, "%s.%s", interface.getHostName(), interface.getDomainName() );

      if( strcmp( line1, oline1 ) != 0
      ||  strcmp( line2, oline2 ) != 0 ) cobalt.screen( line1, line2 );

      strcpy( oline1, line1 );
      strcpy( oline2, line2 );

      for( int i = 0; i < 10 * 1000 / delay; i++ ) { // alle 10 sec
        updateLEDs();
        switch( cobalt.getc( false ) ) {
          case 'L': return previous;
          case 'R': return next;
          case '*': return Application::SETUP_NETWORK;
          case 'P': return Application::RESET_PASSWORD;
          case 'X': return Application::EXTERN;
        } cobalt.sleep( 0, delay );
      }
    }
  }
  enum Screen cpuInfo( enum Screen previous, enum Screen next ) {
  long  user,  nice,  system,  idle,  iowait,  irq,  softirq,
         duser, dnice, dsystem, didle, diowait, dirq, dsoftirq,
         all;

    char line2[ 50 ],
         load,
         width = 0;

    cobalt.screen( "CPU UTILISATION:", "|          |" );

    while( true ) {
      FILE *fp = fopen( "/proc/stat", "r" );
      if( fp != NULL ) {
        fscanf( fp, "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld\r\n",
                  &user,
                  &nice,
                  &system,
                  &idle,
                  &iowait,
                  &irq,
                  &softirq,
                  &dummy,
                  &dummy
                );
        fclose( fp );

        // Delta Values
        duser    = user    - _user,
        dnice    = nice    - _nice,
        dsystem  = system  - _system,
        didle    = idle    - _idle,
        diowait  = iowait  - _iowait,
        dirq     = irq     - _irq,
        dsoftirq = softirq - _softirq;

        all      = duser + dnice + dsystem + didle + diowait + dirq + dsoftirq;

      // Update last values
        _user    = user;
        _nice    = nice;
        _system  = system;
        _idle    = idle;
        _iowait  = iowait;
        _irq     = irq;
        _softirq = softirq;

        if( all != 0 ) {
          user          = 10000 * duser    / all;
          nice          = 10000 * dnice    / all;
          system        = 10000 * dsystem  / all;
          idle          = 10000 * didle    / all;
          iowait        = 10000 * diowait  / all;
          irq           = 10000 * dirq     / all;
          softirq       = 10000 * dsoftirq / all;

          load          =  0.01 * ( 10000 - idle ); // 0..100

                                        width = load / 10;
          if( width == 0 && load > 0 )  width = 1;
          if( width > 10 )              width = 10;

          sprintf( line2, "|          |%3d%%", load );
          for( int i = 0; i < width; i++ ) line2[ 1 + i ] = 0xFF;
          
          cobalt.locate( 0, 1 ).print( line2 );

          for( int i = 0; i < 1 * 1000 / delay; i++ ) { // alle 1 Sekunden
            updateLEDs();
            switch( cobalt.getc( false ) ) {
              case 'L': return previous;
              case 'R': return next;
              case '*': return Application::SETUP_NETWORK;
              case 'P': return Application::RESET_PASSWORD;
              case 'X': return Application::EXTERN;
            } cobalt.sleep( 0, delay );
          }
        }
      }
    }
  }
  enum Screen memInfo( enum Screen previous, enum Screen next ) {
    float in, out;
    long  dummy,
          ram_total, ram_free, ram_buffers, ram_cached,
          swap_total, swap_free;

    char  line[1024],
      line1[ 17 ],
          line2[ 17 ],
          used[ 5 ], total[ 5 ];

    cobalt.screen( "RAM:", "SWAP:" );

    while( true ) {
      FILE *fp = fopen( "/proc/meminfo", "r" );
      if( fp != NULL ) {
        while( fgets( line, sizeof( line ), fp ) != NULL ) {
        sscanf( line, "MemTotal: %ld kB\r\n", &ram_total );
        sscanf( line, "MemFree: %ld kB\r\n", &ram_free );
        sscanf( line, "SwapTotal: %ld kB\r\n", &swap_total );
        sscanf( line, "SwapFree: %ld kB\r\n", &swap_free );
      }
        fclose( fp );

        formatNumber( used, ram_total - ram_free );
        formatNumber( total, ram_total );
        sprintf( line1, "%s/%s", used, total );
        cobalt.locate( 6, 0 ).print( line1 );


        formatNumber( used, swap_total - swap_free );
        formatNumber( total, swap_total );
        sprintf( line2, "%s/%s", used, total );
        cobalt.locate( 6, 1 ).print( line2 );
      }

      for( int i = 0; i < 1 * 1000 / delay; i++ ) { // alle 1 Sekunden
        updateLEDs();
        switch( cobalt.getc( false ) ) {
          case 'L': return previous;
          case 'R': return next;
          case '*': return Application::SETUP_NETWORK;
          case 'P': return Application::RESET_PASSWORD;
          case 'X': return Application::EXTERN;
        } cobalt.sleep( 0, delay );
    }
    }
  }
  enum Screen loadInfo( enum Screen previous, enum Screen next ) {
    float l1, l5, l10;
    char  line2[ 17 ];

    cobalt.showCursor( false );
    cobalt.clear().home().print( "SYSTEM LOAD:" );

    while( true ) {
      FILE *fp = fopen( "/proc/loadavg", "r" );
      if( fp != NULL ) {
        fscanf( fp, "%f %f %f", &l1, &l5, &l10 );
        fclose( fp );

        sprintf( line2, "%.2f %.2f %.2f", l1, l5, l10 );
        cobalt.locate( 0, 1 ).print( line2 );

        for( int i = 0; i < 2 * 1000 / delay; i++ ) { // alle 2 sekunden
          updateLEDs();
          switch( cobalt.getc( false ) ) {
            case 'L': return previous;
            case 'R': return next;
            case '*': return Application::SETUP_NETWORK;
            case 'P': return Application::RESET_PASSWORD;
            case 'X': return Application::EXTERN;
          } cobalt.sleep( 0, delay );
        }
      }
    }
  }
  enum Screen uptimeInfo( enum Screen previous, enum Screen next ) {
    float s;

    char  line1[ 17 ],
          line2[ 17 ];

    cobalt.showCursor( false );
    cobalt.clear().home().print( "SYSTEM UPTIME:" );

    while( true ) {
      FILE *fp = fopen( "/proc/uptime", "r" );
      if( fp != NULL ) {
        fscanf( fp, "%f", &s );
        fclose( fp );

        unsigned long _s = s;

        int seconds = _s % 60; _s /= 60;
        int minutes = _s % 60; _s /= 60;
        int hours   = _s % 24; _s /= 24;
        int days    = _s;

        sprintf( line2, "%d days %02d:%02d:%02d", days, hours, minutes, seconds );
        cobalt.locate( 0, 1 ).print( line2 );

        for( int i = 0; i < 1 * 1000 / delay; i++ ) { // alle 1 sekunden
          updateLEDs();
          switch( cobalt.getc( false ) ) {
            case 'L': return previous;
            case 'R': return next;
            case '*': return Application::SETUP_NETWORK;
            case 'P': return Application::RESET_PASSWORD;
            case 'X': return Application::EXTERN;
          } cobalt.sleep( 0, delay );
        }
      }
    }
  }

  enum Screen menuItem( const char *line1, const char *line2, enum Screen next, enum Screen select ) {
    cobalt.screen( line1, line2 );
    while( true ) {
      updateLEDs();
      switch( cobalt.getc( false ) ) {
        case 'S': return next;
        case 'E': return select;
        case 'P': return Application::RESET_PASSWORD;
      } cobalt.sleep( 0, delay );
    }
  }
  int request( const char *question, const char *option1, const char *option2, const int defaultOption = 0 ) {
    char  line1[ 32 ] = "",
          line2[ 32 ] = "",
          option[]    = { 1, 2 + strlen( option1 ) },
          selection   = defaultOption;;

    sprintf( line2, "%s %s", option1, option2 );
    while( strlen( line2 ) < 15 ) {
      strncpy( line1, line2, sizeof( line2 ) );
      sprintf( line2, " %s ", line1 );
      option[ 0 ]++; option[ 1 ]++;
    }

    cobalt.screen( question, line2 );
    cobalt.locate( option[ selection ], 1 );
    cobalt.showCursor( true );

    while( true ) {
      updateLEDs();
      switch( cobalt.getc( false ) ) {
        case 'L':
          selection = 0;
          cobalt.locate( option[ selection ], 1 );
          break;
        case 'R':
          selection = 1;
          cobalt.locate( option[ selection ], 1 );
          break;
        case 'S':
          cobalt.showCursor( false );
          return selection;
      } cobalt.sleep( 0, delay );
    }
  }
  const char* editIP( const char *line1, const char *address ) {
    int           digit,
                  ip[ 4 ],
                  mul = 1;
    char          cursor = 0;
    static char   line2[ 16 ];

    sscanf( address, "%d.%d.%d.%d", &ip[ 0 ], &ip[ 1 ], &ip[ 2 ], &ip[ 3 ] );
    sprintf( line2, "%03d.%03d.%03d.%03d", ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ] );

    cobalt.screen( line1, line2 );
    cobalt.locate( cursor, 1 );
    cobalt.showCursor( true );

    while( true ) {
      updateLEDs();

      switch( cobalt.getc( false ) ) {
        case 'L':
          cursor--;
          if( cursor % 4 == 3 ) cursor--;
          if( cursor < 0 )      cursor = 14;
          cobalt.locate( cursor, 1 );
          break;
        case 'R':
          cursor++;
          if( cursor % 4 == 3 ) cursor++;
          if( cursor > 14 )     cursor = 0;
          cobalt.locate( cursor, 1 );
          break;

        case 'U':
          digit = ip[ cursor / 4 ];
          digit += mul;
          if( digit > 255 ) digit = ip[ cursor / 4 ];
          ip[ cursor / 4 ] = digit;
          sprintf( line2, "%03d.%03d.%03d.%03d", ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ] );
          digit = ( cursor / 4) * 4; // 0, 4, 8, 12
          cobalt.locate( digit + 0, 1 ).poke( line2[ digit + 0 ] );
          cobalt.locate( digit + 1, 1 ).poke( line2[ digit + 1 ] );
          cobalt.locate( digit + 2, 1 ).poke( line2[ digit + 2 ] );
          cobalt.locate( cursor, 1 );
          break;
        case 'D':
          digit = ip[ cursor / 4 ];
          digit -= mul;
          if( digit < 0 ) digit = ip[ cursor / 4 ];
          ip[ cursor / 4 ] = digit;
          sprintf( line2, "%03d.%03d.%03d.%03d", ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ] );
          digit = ( cursor / 4) * 4; // 0, 4, 8, 12
          cobalt.locate( digit + 0, 1 ).poke( line2[ digit + 0 ] );
          cobalt.locate( digit + 1, 1 ).poke( line2[ digit + 1 ] );
          cobalt.locate( digit + 2, 1 ).poke( line2[ digit + 2 ] );
          cobalt.locate( cursor, 1 );
          break;

        case 'E': // Exit / Enter
          cobalt.showCursor( false );
          sprintf( line2, "%d.%d.%d.%d", ip[ 0 ], ip[ 1 ], ip[ 2 ], ip[ 3 ] );
          return line2;
      }
           if( cursor % 4 == 0 ) mul = 100;
      else if( cursor % 4 == 1 ) mul = 10;
      else                       mul = 1;

      cobalt.sleep( 0, delay );
    }
  }

  void configServices() {
    char  line1[ 255 ], line2[ 255 ];
    int   selection = 0,
          oldState,
          newState;

    service.load();

    while( true ) {
      if( selection >= service.length() ) return;

      oldState = ( service.getServiceState( selection ) == '+' ? 0 : 1 );

      sprintf( line1, "SERVICE:   %5s", ( oldState == 0 ? "[ON]" : "[OFF]" ) );
      sprintf( line2, "  %s", service.getServiceName( selection ) );
      for( unsigned int i = 0; line2[ i ] != 0; i++ ) line2[ i ] = toupper( line2[ i ] );

      switch( menuItem( line1, line2, SKIP, SELECT ) ) {
      case Application::SELECT:
        sprintf( line1, "%s", service.getServiceName( selection ) );
        for( unsigned int i = 0; line1[ i ] != 0; i++ ) line1[ i ] = toupper( line1[ i ] );

        newState = request( line1, "[O]N", "[O]FF", oldState );

        if( oldState != newState ) {
          if( newState == 0 ) service.start( selection );
          else                service.stop( selection );
        }
      case Application::SKIP:
        selection++;
        break;
      }
    }
  }
  void resetPassword() {
    char  command[ 1024 ],
          password[ 10 ],
          chars[] = "2345679ABCEFHKLMNPRSTWXYZ-",
          user[]  = "root";

    srand( time( NULL ) );
    for( unsigned int i = 0; i < sizeof( password ); i++ )
      password[ i ] = chars[ rand() % sizeof( chars ) ];

    password[ sizeof( password ) - 1 ] = 0;

    sprintf( command, "(echo \"%s\" > pw; echo \"%s\" >> pw; passwd -d %s; passwd -q %s < pw; rm pw) 1> /dev/null 2> /dev/null", password, password, user, user );

    //printf( "RESET PASSWORD: %s\r\n", command );
    system( command );

    cobalt.screen( "NEW ROOT PASSWD:", password );
    for( unsigned int i = 0; i < 60 * 1000 / delay ; i++ ) { // 60 sekunden
      switch( cobalt.getc( false ) ) {
        case 'L': case 'R': case 'U': case 'D':
        case 'S': case 'E': case 'P':
          return;
      }
      cobalt.sleep( 0, delay );
    }
  }

};
