cc=g++
exe=main
obj=main.o stamp.o
$(exe):$(obj)
	$(cc) $(obj) -lcrypto -o $(exe)
main.o:main.cpp stamp.cpp stamp.h
	$(cc) -g -Wall -c main.cpp
stamp.o:stamp.cpp stamp.h
	$(cc) -g -Wall -c stamp.cpp
clean:
	rm -rf *.o main

