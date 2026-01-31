#include "number_format.h"
#include <stdlib.h>

// Helper to get absolute value
static int32_t abs_val(int32_t x) {
    return (x < 0) ? -x : x;
}

// Format a fixed-point value (x10) as "XX.X"
void format_weight(char *buffer, size_t size, int32_t value_x10) {
    int32_t whole = value_x10 / 10;
    int32_t frac = abs_val(value_x10 % 10);
    snprintf(buffer, size, "%ld.%ld", (long)whole, (long)frac);
}

// Format time in minutes as "H:MM"
void format_time(char *buffer, size_t size, int16_t minutes) {
    int hours = minutes / 60;
    int mins = minutes % 60;
    snprintf(buffer, size, "%d:%02d", hours, mins);
}

// Format UFR (x100) as "X.XX"
void format_ufr(char *buffer, size_t size, int32_t ufr_x100) {
    int32_t whole = ufr_x100 / 100;
    int32_t frac = abs_val(ufr_x100 % 100);
    snprintf(buffer, size, "%ld.%02ld", (long)whole, (long)frac);
}

// Format percentage (x10) as "XXX.X"
void format_percentage(char *buffer, size_t size, int32_t percent_x10) {
    int32_t whole = percent_x10 / 10;
    int32_t frac = abs_val(percent_x10 % 10);
    snprintf(buffer, size, "%ld.%ld%%", (long)whole, (long)frac);
}

// Format delta selection as "0.2" or "0.4"
void format_delta(char *buffer, size_t size, int16_t delta_selection) {
    if (delta_selection == 0) {
        snprintf(buffer, size, "0.2");
    } else {
        snprintf(buffer, size, "0.4");
    }
}

// Format variance (x10) with +/- sign
void format_variance(char *buffer, size_t size, int32_t variance_x10) {
    int32_t whole = variance_x10 / 10;
    int32_t frac = abs_val(variance_x10 % 10);
    if (variance_x10 >= 0) {
        snprintf(buffer, size, "+%ld.%ld", (long)whole, (long)frac);
    } else {
        snprintf(buffer, size, "%ld.%ld", (long)whole, (long)frac);
    }
}
