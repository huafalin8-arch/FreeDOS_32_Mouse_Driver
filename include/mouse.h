#ifndef MOUSE_H
#define MOUSE_H

#define MOUSE_LEFT     (1 << 0)
#define MOUSE_RIGHT    (1 << 1)
#define MOUSE_MIDDLE   (1 << 2)

#include <stdint.h>

typedef struct {
    int32_t x, y;
    uint8_t buttons;
    int8_t  wheel;
    uint16_t reserved;
} mouse_state_t;

typedef enum {
    MOUSE_EV_NONE = 0,
    MOUSE_EV_MOVE,
    MOUSE_EV_BUTTON_DOWN,
    MOUSE_EV_BUTTON_UP,
    MOUSE_EV_WHEEL
} mouse_event_type_t;
/* 我是不是写反了O.o似乎也没什么大碍 */
#endif