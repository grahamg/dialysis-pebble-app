#pragma once

// Suppress GCC 12+ warning about strftime return type mismatch in SDK headers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#include <pebble.h>
#pragma GCC diagnostic pop

// Format a fixed-point value (x10) as "XX.X" for weights
void format_weight(char *buffer, size_t size, int32_t value_x10);

// Format time in minutes as "H:MM"
void format_time(char *buffer, size_t size, int16_t minutes);

// Format UFR (x100) as "X.XX"
void format_ufr(char *buffer, size_t size, int32_t ufr_x100);

// Format percentage (x10) as "XXX.X"
void format_percentage(char *buffer, size_t size, int32_t percent_x10);

// Format delta selection as "0.2" or "0.4"
void format_delta(char *buffer, size_t size, int16_t delta_selection);

// Format variance (x10) with +/- sign as "+X.X" or "-X.X"
void format_variance(char *buffer, size_t size, int32_t variance_x10);
