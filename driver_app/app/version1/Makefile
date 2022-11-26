CC = "g++"
PROJECT = cross_correlation_app
SRC = cross_correlation_app.cpp 

LIBS = `pkg-config opencv4 --cflags --libs`

$(PROJECT) : $(SRC)
	$(CC) -O2 $(SRC) -o $(PROJECT) $(LIBS)

run:
	./$(PROJECT)
	
clean:
	rm cross_correlation_app *.jpg
