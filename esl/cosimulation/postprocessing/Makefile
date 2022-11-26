CC = "g++"
PROJECT = specification
SRC = postprocessing.cpp 

LIBS = `pkg-config opencv4 --cflags --libs`

$(PROJECT) : $(SRC)
	$(CC) -O2 $(SRC) -o $(PROJECT) $(LIBS)

run:
	./$(PROJECT) 220 ../../../data/green.jpg ../../../data/green_template.jpg result_0.txt ../../../data/bubbles.jpg ../../../data/bubbles_template.jpg result_1.txt ../../../data/diamonds.jpg ../../../data/diamonds_template.jpg result_2.txt ../../../data/pumpkins.jpg ../../../data/pumpkins_template.jpg result_3.txt ../../../data/ball.jpg ../../../data/ball_template.jpg result_4.txt ../../../data/logo.jpg ../../../data/logo_template.jpg result_5.txt ../../../data/stars.jpg ../../../data/stars_template.jpg result_6.txt	../../../data/sun.jpg ../../../data/sun_template.jpg result_7.txt ../../../data/baby_yoda.jpg ../../../data/yoda_template.jpg result_8.txt ../../../data/chocolate_bubbles.jpg ../../../data/chocolate_bubbles_template.jpg result_9.txt	  

clean_spec:	
	rm specification

clean:
	rm specification *.jpg *.txt

	
