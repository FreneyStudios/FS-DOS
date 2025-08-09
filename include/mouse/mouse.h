#pragma once

#include "C/stdint.h"
#include "C/stdbool.h"

#define MOUSE_DATA_PORT   0x60
#define MOUSE_STATUS_PORT 0x64
#define MOUSE_CMD_PORT    0x64

#define MOUSE_CMD_ENABLE_AUX_DEVICE 0xA8
#define MOUSE_CMD_GET_COMPAQ_STATUS 0x20
#define MOUSE_CMD_SET_COMPAQ_STATUS 0x60
#define MOUSE_CMD_ENABLE_PACKET_STREAMING 0xF4
#define MOUSE_CMD_SET_DEFAULTS 0xF6

#define MOUSE_ACK 0xFA

typedef struct {
    int32_t x;
    int32_t y;
    bool left_button;
    bool right_button;
    bool middle_button;
} mouse_state_t;

void init_mouse(void);
void mouse_irq12_handler(void);
mouse_state_t *get_mouse_state(void);
