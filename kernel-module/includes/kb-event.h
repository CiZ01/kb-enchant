#ifndef __KB_EVENT_H__
#define __KB_EVENT_H__

#include "kb-map.h"
#include <linux/input-event-codes.h>

static void kb_event__brightness(struct input_dev *input, u8 key_scancode);

static void kb_event__zoom(struct input_dev *input, u8 key_scancode);

static void kb_event__zoom(struct input_dev *input, u8 key_scancode)
{
    switch (key_scancode)
    {
    case WHEEL_UP:
        input_event(input, EV_KEY, KEY_UP, KEY_ON);
        input_event(input, EV_KEY, KEY_UP, KEY_OFF);
        break;
    case WHEEL_DOWN:
        input_event(input, EV_KEY, KEY_DOWN, KEY_ON);
        input_event(input, EV_KEY, KEY_DOWN, KEY_OFF);
        break;
    }

    return;
}

static void kb_event__brightness(struct input_dev *input, u8 key_scancode)
{
    switch (key_scancode)
    {
    case WHEEL_UP:
        input_event(input, EV_KEY, KEY_BRIGHTNESSUP, KEY_ON);
        input_event(input, EV_KEY, KEY_BRIGHTNESSUP, KEY_OFF);
        break;
    case WHEEL_DOWN:
        input_event(input, EV_KEY, KEY_BRIGHTNESSDOWN, KEY_ON);
        input_event(input, EV_KEY, KEY_BRIGHTNESSDOWN, KEY_OFF);
        break;
    }

    return;
}

static void kb_event__change_mode(struct input_dev *input)
{
    input_event(input, EV_KEY, KEY_MODE, KEY_ON);
    input_event(input, EV_KEY, KEY_MODE, KEY_OFF);
    return;
}

#endif // __KB_EVENT_H__
