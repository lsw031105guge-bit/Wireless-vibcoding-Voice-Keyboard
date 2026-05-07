#pragma once

#include <stdint.h>

void oled_init();
void oled_show_hollow();
void oled_show_receipt(uint32_t totalTokens);