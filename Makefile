.PHONY : clean

SOURCES = $(shell echo *.c)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)

TARGET = uart_app

CC := gcc

FLAGS := -g -pthread


all: clean $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(FLAGS)
	sudo ./$(TARGET) /dev/ttyUSB1
# create GUI terminal
# sudo gnome-terminal -- sh -c "gdb --args $(TARGET) /dev/ttyUSB0 $(FILE)"
$(OBJECTS):
	$(CC) -c $(SOURCES) $(FLAGS)

clean: 
	rm -f $(OBJECTS) $(TARGET)
	sudo rm -f uart_log.txt

