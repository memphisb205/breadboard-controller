TARGET = bb_controller
SRC = breadboard_controller.c
SERVICE = breadboard_controller.service
PREFIX = /home/pi

CC = gcc
CFLAGS = -lwiringPi

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

install: $(TARGET)
	sudo cp $(TARGET) $(PREFIX)/
	sudo cp $(SERVICE) /etc/systemd/system/
	sudo systemctl daemon-reload
	sudo systemctl enable $(SERVICE)
	sudo systemctl start $(SERVICE)

uninstall:
	sudo systemctl stop $(SERVICE)
	sudo systemctl disable $(SERVICE)
	sudo rm -f /etc/systemd/system/$(SERVICE)
	sudo rm -f $(PREFIX)/$(TARGET)
	sudo systemctl daemon-reload

clean:
	rm -f $(TARGET)
