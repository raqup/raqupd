#Wno-unused-result -Os

all:	main.cpp uart.cpp cobalt.cpp interface.cpp service.cpp application.cpp
	g++ main.cpp -s -o raqupd
	
uart.o:	uart.cpp
	g++ uart.cpp -c -o uart.o
	
cobalt.o:	cobalt.cpp
	g++ cobalt.cpp -c -o cobalt.o
	
interface.o:	interface.cpp
	g++ interface.cpp -c -o interface.o
	
service.o:	service.cpp
	g++ service.cpp -c -o service.o

application.o:	application.cpp
	g++ application.cpp -c -o application.o
