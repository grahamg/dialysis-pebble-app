#pragma once

// Suppress GCC 12+ warning about strftime return type mismatch in SDK headers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#include <pebble.h>
#pragma GCC diagnostic pop
#include "../data/treatment_data.h"

// Create and push the pre-treatment window
void pre_treatment_window_push(TreatmentRecord *record);

// Get the window pointer (for checking if already exists)
Window *pre_treatment_window_get_window(void);
