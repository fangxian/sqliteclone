sqliteclone:main.o InputBuffer.o
	g++ main.0 InputBuffer.o sqliteclone
main.o:main.cpp InputBuffer.h
	g++ -c main.cpp -o main.o
InputBuffer.o:InputBuffer.cpp InputBuffer.h
	g++ -c InputBuffer.cpp -o InputBuffer.o 

clean:
	rm -rf *.o sqliteclone
