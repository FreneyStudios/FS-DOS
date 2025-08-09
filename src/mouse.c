#include "mouse/mouse.h"
#include "ports/io.h"
#include "print/print_types.h" // For debugging
#include "interrupts/idt.h"
#include "interrupts/pic.h"

extern void mouse_irq12_handler_wrapper(void);

static mouse_state_t g_mouse_state;
static uint8_t g_mouse_cycle = 0;
static int8_t g_mouse_byte[3];

// Helper functions to interact with the PS/2 controller
static inline void mouse_wait_write() {
    while ((inb(MOUSE_STATUS_PORT) & 2) != 0);
}

static inline void mouse_wait_read() {
    while ((inb(MOUSE_STATUS_PORT) & 1) == 0);
}

static inline void mouse_write(uint8_t data) {
    mouse_wait_write();
    outb(MOUSE_CMD_PORT, 0xD4); // Command to send data to the mouse
    mouse_wait_write();
    outb(MOUSE_DATA_PORT, data);
}

static inline uint8_t mouse_read() {
    mouse_wait_read();
    return inb(MOUSE_DATA_PORT);
}

void mouse_irq12_handler(void) {
    // Acknowledge interrupt to PIC
    outb(PIC_2_CMD, 0x20);

    uint8_t status = inb(MOUSE_STATUS_PORT);
    if (!(status & 0x20)) {
        // Data is from keyboard, not mouse. This shouldn't happen on IRQ12.
        return;
    }

    switch (g_mouse_cycle) {
        case 0:
            g_mouse_byte[0] = inb(MOUSE_DATA_PORT);
            // Check for overflow bits and Y-axis sign
            if (g_mouse_byte[0] & 0x08) {
                g_mouse_cycle++;
            }
            break;
        case 1:
            g_mouse_byte[1] = inb(MOUSE_DATA_PORT);
            g_mouse_cycle++;
            break;
        case 2:
            g_mouse_byte[2] = inb(MOUSE_DATA_PORT);
            g_mouse_cycle = 0;

            // Update mouse state based on the packet
            g_mouse_state.left_button = g_mouse_byte[0] & 0x1;
            g_mouse_state.right_button = g_mouse_byte[0] & 0x2;
            g_mouse_state.middle_button = g_mouse_byte[0] & 0x4;

            int32_t delta_x = g_mouse_byte[1];
            int32_t delta_y = g_mouse_byte[2];

            // Handle 9-bit sign extension for X
            if (g_mouse_byte[0] & 0x10) {
                delta_x |= 0xFFFFFF00;
            }
            // Handle 9-bit sign extension for Y
            if (g_mouse_byte[0] & 0x20) {
                delta_y |= 0xFFFFFF00;
            }

            g_mouse_state.x += delta_x;
            g_mouse_state.y -= delta_y; // Y is inverted

            // Clamp coordinates to screen boundaries (assuming 1920x1080 for now)
            if (g_mouse_state.x < 0) g_mouse_state.x = 0;
            if (g_mouse_state.x > 1919) g_mouse_state.x = 1919;
            if (g_mouse_state.y < 0) g_mouse_state.y = 0;
            if (g_mouse_state.y > 1079) g_mouse_state.y = 1079;

            break;
    }
}


void init_mouse() {
    uint8_t status;

    // 1. Enable the auxiliary mouse device
    mouse_wait_write();
    outb(MOUSE_CMD_PORT, MOUSE_CMD_ENABLE_AUX_DEVICE);

    // 2. Enable the interrupts
    mouse_wait_write();
    outb(MOUSE_CMD_PORT, MOUSE_CMD_GET_COMPAQ_STATUS);
    status = mouse_read();
    status |= 0x02;  // Set bit 1 (IRQ12)
    status &= ~0x20; // Clear bit 5 (disable mouse clock)
    mouse_wait_write();
    outb(MOUSE_CMD_PORT, MOUSE_CMD_SET_COMPAQ_STATUS);
    outb(MOUSE_DATA_PORT, status);

    // 3. Set mouse defaults
    mouse_write(MOUSE_CMD_SET_DEFAULTS);
    if (mouse_read() != MOUSE_ACK) {
        // Failed to set defaults
        return;
    }

    // 4. Enable packet streaming
    mouse_write(MOUSE_CMD_ENABLE_PACKET_STREAMING);
    if (mouse_read() != MOUSE_ACK) {
        // Failed to enable packet streaming
        return;
    }

    // 5. Set up IRQ12 handler
    set_idt_descriptor_32(0x2C, mouse_irq12_handler_wrapper, INT_GATE_FLAGS);
    clear_irq_mask(12); // Unmask IRQ12

    // Initialize mouse state
    g_mouse_state.x = 1920 / 2;
    g_mouse_state.y = 1080 / 2;
    g_mouse_state.left_button = false;
    g_mouse_state.right_button = false;
    g_mouse_state.middle_button = false;
}

mouse_state_t *get_mouse_state(void) {
    return &g_mouse_state;
}
