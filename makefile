.PHONY: clean
CLIENTE = frogger
SERVIDOR=servidor

all: $(CLIENTE)
all: $(SERVIDOR)

$(CLIENTE): $(CLIENTE).o
	g++ -g -o $(CLIENTE) $(CLIENTE).o sprite.cpp SDL.cpp -lSDL -lSDL_image -lSDL_ttf -lpthread

$(SERVIDOR): $(SERVIDOR).o
	g++ -g -o $(SERVIDOR) $(SERVIDOR).o sprite.cpp SDL.cpp -lSDL -lSDL_image -lSDL_ttf -lpthread
	
clean:
	rm -f *.o $(CLIENTE)  $(SERVIDOR) 
