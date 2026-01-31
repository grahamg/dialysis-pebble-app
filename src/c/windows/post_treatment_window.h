#pragma once

// Suppress GCC 12+ warning about strftime return type mismatch in SDK headers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#include <pebble.h>
#pragma GCC diagnostic pop
#include "../data/treatment_data.h"

// Create and push the post-treatment window
void post_treatment_window_push(TreatmentRecord *record);
