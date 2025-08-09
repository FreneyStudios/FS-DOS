global mouse_irq12_handler_wrapper

extern mouse_irq12_handler

mouse_irq12_handler_wrapper:
    pushad
    call mouse_irq12_handler
    popad
    iret
