#include <stdint.h>
#include <stddef.h>

static volatile uint16_t *const VGA_MEMORY =
    (volatile uint16_t *)0xB8000;

static size_t cursor = 0;

static void print(const char *text)
{
    while (*text != '\0' && cursor < 80 * 25) {
        if (*text == '\n') {
            cursor = ((cursor / 80) + 1) * 80;
        } else {
            VGA_MEMORY[cursor++] =
                (uint16_t)(0x0F00 | (uint8_t)*text);
        }
        text++;
    }
}

void kernel_main(void)
{
    for (size_t i = 0; i < 80 * 25; i++) {
        VGA_MEMORY[i] = 0x0F20;
    }

    cursor = 0;
    print("RaktSetu Mini OS Kernel");
    print("\nHello from RaktSetu Kernel!");
    print("\nBooted successfully through GRUB.");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
