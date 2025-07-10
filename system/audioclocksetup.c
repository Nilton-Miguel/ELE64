#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>


// #define BCM2835_PERI_BASE      0x3F000000   // For Raspberry Pi 2, 3
#define BCM2835_PERI_BASE      0x20000000   // For Raspberry Pi 1, Zero
// #define BCM2835_PERI_BASE      0xFE000000   // For Raspberry Pi 4

#define GPCLK_BASE             (BCM2835_PERI_BASE + 0x101000)
#define GPIO_BASE              (BCM2835_PERI_BASE + 0x200000) // GPIO controller base
#define GPCLK0_CTRL            (0x70 / 4)
#define GPCLK0_DIV             (0x74 / 4)
#define BLOCK_SIZE             (0x100)

// Pointer to the mapped memory
volatile unsigned int *gpclk;
volatile unsigned int *gpio;

// Function to set up the memory mapping for GPIO access
void setup_gpclk() {
    int mem_fd;
    void *gpclk_map;
    void *gpio_map;

    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("Failed to open /dev/mem, try running as root");
        exit(EXIT_FAILURE);
    }
    // Map GPIO memory to our address space
    gpio_map = mmap(
        NULL,                 // Any address in our space will do
        BLOCK_SIZE,           // Map length
        PROT_READ | PROT_WRITE, // Enable reading & writing to mapped memory
        MAP_SHARED,           // Shared with other processes
        mem_fd,               // File descriptor for /dev/mem
        GPIO_BASE             // Offset to GPIO peripheral
    );

    // Map GPCLK memory to our address space
    gpclk_map = mmap(
        NULL,                 // Any address in our space will do
        BLOCK_SIZE,           // Map length
        PROT_READ | PROT_WRITE, // Enable reading & writing to mapped memory
        MAP_SHARED,           // Shared with other processes
        mem_fd,               // File descriptor for /dev/mem
        GPCLK_BASE             // Offset to GPIO peripheral
    );

    close(mem_fd);

    if (gpio_map == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }
    if (gpclk_map == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    // volatile pointer to prevent compiler optimizations
    gpio = (volatile unsigned int *)gpio_map;
    gpclk = (volatile unsigned int *)gpclk_map;
}

/*
// Set GPIO pin as output
void set_gpio_output(int pin) {
    int reg = pin / 10;
    int shift = (pin % 10) * 3;
    // Clear the 3 bits for the pin and set it to 001 (output)
    gpio[reg] = (gpio[reg] & ~(7 << shift)) | (1 << shift);
}

// Set GPIO function
void set_gpio(int pin) {
    gpio[8] = (1 << pin);
}

// Reset GPIO function
void reset_gpio(int pin) {
    gpio[11] = (1 << pin);
}
*/

int main() {
    printf("Configuring clock.\n");

    setup_gpclk();

    // Matar o gerador de clock
    gpclk[GPCLK0_CTRL] = 0x5A000020;
    // Configurar o divisor como 8,7891 = 8 + 3232 / 4096
    // assim, parte inteira 8 (0x008) e parte fracionária 3232 (0xCA0)
    gpclk[GPCLK0_DIV] = 0x5A008CA0;
    // Rodar gerador com divisor MASH de 2 estágios, a partir do sinal HDMI auxiliary (216 MHz)
    gpclk[GPCLK0_CTRL] = 0x5A000217;

    // Após a configuração, teremos um clock de 216 / 8,7891 = 24,576 MHz em GPCLK0

    // Configurar o pino GPIO4 como função alternativa (para que assuma o sinal GPCLK0)
    gpio[0] = (0b100 << 12);

    printf("Clock configured.\n");

    return 0;
}
