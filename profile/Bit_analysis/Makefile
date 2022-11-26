CC = "g++"
PROJECT = bit
SRC = bit.cpp 

LIBS = `pkg-config opencv4 --cflags --libs`

$(PROJECT) : $(SRC)
	$(CC) -O2 $(SRC) -lsystemc -o $(PROJECT) $(LIBS)

profiling: 
	valgrind --tool=callgrind ./specification ../../data/bubbles.jpg ../../data/bubbles_template.jpg 
# ../../data/coffee.jpg ../../data/coffee_template.jpg ../../data/pattern1.jpg ../../data/pattern1_template.jpg ../../data/diamonds.jpg ../../data/diamonds_template.jpg ../../data/pumpkins.jpg ../../data/pumpkins_template.jpg ../../data/ball.jpg ../../data/ball_template.jpg ../../data/logo.jpg ../../data/logo_template.jpg ../../data/pictures.jpg ../../data/pictures_template.jpg ../../data/river.jpg ../../data/river_template.jpg	
run:
	./$(PROJECT) ../../data/bubbles.jpg ../../data/bubbles_template.jpg
	#../../data/pumpkins.jpg ../../data/pumpkins_template.jpg ../../data/bubbles.jpg ../../data/bubbles_template.jpg ../../data/coffee.jpg ../../data/coffee_template.jpg ../../data/diamonds.jpg ../../data/diamonds_template.jpg ../../data/logo.jpg ../../data/logo_template.jpg
#../../data/bubbles.jpg ../../data/bubbles_template.jpg  ../../data/coffee.jpg ../../data/coffee_template.jpg ../../data/pattern1.jpg ../../data/pattern1_template.jpg ../../data/diamonds.jpg ../../data/diamonds_template.jpg ../../data/pumpkins.jpg ../../data/pumpkins_template.jpg ../../data/ball.jpg ../../data/ball_template.jpg ../../data/logo.jpg ../../data/logo_template.jpg ../../data/pictures.jpg ../../data/pictures_template.jpg ../../data/river.jpg ../../data/river_template.jpg	
clean:
	rm callgrind.* bit *.jpg
