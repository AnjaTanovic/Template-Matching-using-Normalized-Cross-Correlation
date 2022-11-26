CC = "g++"
PROJECT = specification
SRC = main.cpp interconnect.cpp soft.cpp bram.cpp functions.cpp ip_hard.cpp vp.cpp

LIBS = `pkg-config opencv4 --cflags --libs`

$(PROJECT) : $(SRC)
	$(CC) -g -lsystemc $(SRC) -o $(PROJECT) $(LIBS) 
run:  
	touch frame_rate.txt
	./$(PROJECT) 240 ../../data/pumpkins.jpg ../../data/pumpkins_template.jpg
#../../data/green.jpg ../../data/green_template.jpg ../../data/bubbles.jpg ../../data/bubbles_template.jpg ../../data/coffee.jpg ../../data/coffee_template.jpg ../../data/diamonds.jpg ../../data/diamonds_template.jpg ../../data/pumpkins.jpg ../../data/pumpkins_template.jpg ../../data/ball.jpg ../../data/ball_template.jpg ../../data/logo.jpg ../../data/logo_template.jpg ../../data/stars.jpg ../../data/stars_template.jpg	../../data/sun.jpg ../../data/sun_template.jpg ../../data/baby_yoda.jpg ../../data/yoda_template.jpg ../../data/chocolate_bubbles.jpg ../../data/chocolate_bubbles_template.jpg  
#240 ../../data/bubbles.jpg ../../data/bubbles_template.jpg ../../data/pattern2.jpg ../../data/pattern1_template.jpg

clean:
	rm specification *.jpg *.txt
