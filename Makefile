#Wno-unused-result -Os

all:	uart.o cobalt.o interface.o service.o application.o main.o
	g++ *.o -s -o raqupd
	
uart.o:	uart.cpp uart.h
	g++ uart.cpp -c -o uart.o
	
cobalt.o:	cobalt.cpp cobalt.h
	g++ cobalt.cpp -c -o cobalt.o
	
interface.o:	interface.cpp interface.h
	g++ interface.cpp -c -o interface.o
	
service.o:	service.cpp service.h
	g++ service.cpp -c -o service.o

application.o:	application.cpp application.h
	g++ application.cpp -c -o application.o
	
main.o:	main.cpp 
	g++ main.cpp -c -o main.o
