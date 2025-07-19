#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <wiringPi.h>
#include <string.h>
#include <sys/time.h>

#define DEBOUNCE_DELAY 50 //ms

// Struct for each button
typedef struct {
    int gpio_pin;    // wiringPi pin number
    int  keycode;    // Linux input even key code
    int last_state;  // Track state changes
} Button;

// Map buttons
Button buttons[] = {
    {0, KEY_UP,     0},  // GPIO 17 (Up)
    {2, KEY_DOWN,   0},  // GPIO 27 (Down)
    {3, KEY_LEFT,   0},  // GPIO 22 (Left)
    {4, KEY_RIGHT,  0},  // GPIO 23 (Right)
    {21, KEY_Z,     0},  // GPIO 5 (A)
    {22, KEY_X,     0},  // GPIO 6 (B)
    {23, KEY_SPACE, 0},  // GPIO 13 (Select)
    {24, KEY_ENTER, 0},  // GPIO 19 (Start)
};
int button_count = sizeof(buttons) / sizeof(Button);

// Initialize uinput virtual keyboard
int setup_uinput_device(int *fd) {
    struct uinput_setup usetup;

    *fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (*fd < 0) {
        perror("open /dev/uinput");
        return -1;
    }

    ioctl(*fd, UI_SET_EVBIT, EV_KEY);
    for (int i = 0; i < button_count; i++) {
        ioctl(*fd, UI_SET_KEYBIT, buttons[i].keycode);
    }

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "NES GPIO Controller");

    ioctl(*fd, UI_DEV_SETUP, &usetup);
    ioctl(*fd, UI_DEV_CREATE);

    sleep(1);
    return 0;
}

// Send press or release event
void send_key_event(int fd, int keycode, int pressed) {
    struct input_event ev;

    memset(&ev, 0, sizeof(ev));
    ev.type = EV_KEY;
    ev.code = keycode;
    ev.value = pressed;
    gettimeofday(&ev.time, NULL);
    write(fd, &ev, sizeof(ev));

    // SYN event to finalize
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    gettimeofday(&ev.time, NULL);
    write(fd, &ev, sizeof(ev));
}

int main() {
    int fd;

    if (wiringPiSetup() == -1) {
        fprintf(stderr, "Failed to init wiringPi\n");
        return 1;
    }

    // Set pin modes
    for (int i = 0; i < button_count; i++) {
        pinMode(buttons[i].gpio_pin, INPUT);
        pullUpDnControl(buttons[i].gpio_pin, PUD_UP); // Enable internal pull-up
        buttons[i].last_state = digitalRead(buttons[i].gpio_pin);
    }

    if (setup_uinput_device(&fd) < 0) return 1;

    printf("NES Controller running... Press buttons!\n");

    while (1) {
        for (int i = 0; i < button_count; i++) {
            int state = digitalRead(buttons[i].gpio_pin);

            if (state != buttons[i].last_state) {
                usleep(DEBOUNCE_DELAY * 1000);
                int stable = digitalRead(buttons[i].gpio_pin);
                if (stable == state) {
                    int pressed = (state == LOW); // Active LOW
                    send_key_event(fd, buttons[i].keycode, pressed);
                    buttons[i].last_state = state;
                }
            }
        }
        usleep(1000); // poll delay
    }

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    return 0;
}