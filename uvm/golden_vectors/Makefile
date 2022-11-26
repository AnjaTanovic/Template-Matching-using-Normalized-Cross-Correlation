CC = "g++"
PROJECT = specification
SRC = golden_vectors.cpp

LIBS = `pkg-config opencv4 --cflags --libs`

$(PROJECT) : $(SRC)
	$(CC) -O2 $(SRC) -o $(PROJECT) $(LIBS)	

run:
	./$(PROJECT) 220 ../../data/green.jpg ../../data/green_template.jpg ../../data/bubbles.jpg ../../data/bubbles_template.jpg ../../data/diamonds.jpg ../../data/diamonds_template.jpg ../../data/pumpkins.jpg ../../data/pumpkins_template.jpg ../../data/logo.jpg ../../data/logo_template.jpg ../../data/stars.jpg ../../data/stars_template.jpg ../../data/sun.jpg ../../data/sun_template.jpg ../../data/baby_yoda.jpg ../../data/yoda_template.jpg ../../data/chocolate_bubbles.jpg ../../data/chocolate_bubbles_template.jpg ../../data/ball.jpg ../../data/ball_template.jpg 
clean_spec:	
	rm specification
clean:
	rm specification dimensions_file_* img_file_* tmpl_file_* resp_file_* sum1_file_* sum2_file_*
