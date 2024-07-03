#include "../include/k_bios_called.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

uint8_t read_rtc_register(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

uint8_t bcd_to_binary(uint8_t bcd) {
    return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}

uint32_t getBIOSClock() {
    outb(CMOS_ADDRESS, 0x80);

    uint8_t seconds = read_rtc_register(0x00);
    uint8_t minutes = read_rtc_register(0x02);
    uint8_t hours = read_rtc_register(0x04);
    
    uint8_t register_b = read_rtc_register(0x0B);

    outb(CMOS_ADDRESS, 0x00);

    if (!(register_b & 0x04)) {
        seconds = bcd_to_binary(seconds);
        minutes = bcd_to_binary(minutes);
        hours = bcd_to_binary(hours);
    }

    return (hours << 16) | (minutes << 8) | seconds;
}