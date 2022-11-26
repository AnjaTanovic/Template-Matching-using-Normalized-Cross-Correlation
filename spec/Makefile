CC = "g++"
PROJECT = specification
SRC = Source.cpp 

LIBS = `pkg-config opencv4 --cflags --libs`

$(PROJECT) : $(SRC)
	$(CC) -g $(SRC) -o $(PROJECT) $(LIBS)

profiling: 
	valgrind --tool=callgrind ./specification
clean:
	rm callgrind.* specification
