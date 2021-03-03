#use /usr/lib

GPP=g++ -std=c++11
INCLUDES=/home/user/Documents/chilkat/include #colocar pasta de include do chilkat
CPPFILES=*.cpp #./utils/*.cpp
LIBS=/usr/lib #libs
 

main: queryfinder.cpp 
	$(GPP) -I$(INCLUDES) $(CPPFILES)  -I. -g -o queryfinder -L$(LIBS) -lgumbo -lchilkat-9.5.0 -lpthread 

clean:
	rm queryfinder


.PHONY: all clean
