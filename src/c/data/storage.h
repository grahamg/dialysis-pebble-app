#pragma once

// Suppress GCC 12+ warning about strftime return type mismatch in SDK headers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#include <pebble.h>
#pragma GCC diagnostic pop
#include "treatment_data.h"

// Storage key definitions
#define STORAGE_KEY_IN_PROGRESS      0x0001  // Current in-progress treatment
#define STORAGE_KEY_HISTORY_COUNT    0x0002  // Number of saved treatments
#define STORAGE_KEY_HISTORY_BASE     0x0100  // History entries start here

// Storage limits
#define MAX_HISTORY_ENTRIES          15      // Circular buffer size

// In-progress treatment functions
bool storage_has_in_progress(void);
bool storage_save_in_progress(const TreatmentRecord *record);
bool storage_load_in_progress(TreatmentRecord *record);
void storage_clear_in_progress(void);

// History functions
int storage_get_history_count(void);
bool storage_save_to_history(const TreatmentRecord *record);
bool storage_load_from_history(int index, TreatmentRecord *record);
void storage_clear_all_history(void);
