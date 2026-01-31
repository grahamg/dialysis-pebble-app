#include "storage.h"

#define TREATMENT_RECORD_SIZE sizeof(TreatmentRecord)

// Check if in-progress treatment exists
bool storage_has_in_progress(void) {
    return persist_exists(STORAGE_KEY_IN_PROGRESS);
}

// Save in-progress treatment
bool storage_save_in_progress(const TreatmentRecord *record) {
    int bytes = persist_write_data(STORAGE_KEY_IN_PROGRESS,
                                   record,
                                   TREATMENT_RECORD_SIZE);
    return bytes == (int)TREATMENT_RECORD_SIZE;
}

// Load in-progress treatment
bool storage_load_in_progress(TreatmentRecord *record) {
    if (!persist_exists(STORAGE_KEY_IN_PROGRESS)) {
        return false;
    }
    int bytes = persist_read_data(STORAGE_KEY_IN_PROGRESS,
                                  record,
                                  TREATMENT_RECORD_SIZE);
    return bytes == (int)TREATMENT_RECORD_SIZE;
}

// Clear in-progress when treatment completes
void storage_clear_in_progress(void) {
    persist_delete(STORAGE_KEY_IN_PROGRESS);
}

// Get number of history entries
int storage_get_history_count(void) {
    if (!persist_exists(STORAGE_KEY_HISTORY_COUNT)) {
        return 0;
    }
    return persist_read_int(STORAGE_KEY_HISTORY_COUNT);
}

// Save completed treatment to history (circular buffer)
bool storage_save_to_history(const TreatmentRecord *record) {
    int count = storage_get_history_count();
    int index = count % MAX_HISTORY_ENTRIES;  // Wrap around

    uint32_t key = STORAGE_KEY_HISTORY_BASE + index;
    int bytes = persist_write_data(key, record, TREATMENT_RECORD_SIZE);

    if (bytes == (int)TREATMENT_RECORD_SIZE) {
        // Only increment count up to MAX_HISTORY_ENTRIES
        // After that, we overwrite oldest entries
        if (count < MAX_HISTORY_ENTRIES) {
            persist_write_int(STORAGE_KEY_HISTORY_COUNT, count + 1);
        } else {
            // Keep incrementing to track position in circular buffer
            persist_write_int(STORAGE_KEY_HISTORY_COUNT, count + 1);
        }
        return true;
    }
    return false;
}

// Load treatment from history by index (0 = oldest available)
bool storage_load_from_history(int index, TreatmentRecord *record) {
    int count = storage_get_history_count();

    if (index < 0 || index >= count || index >= MAX_HISTORY_ENTRIES) {
        return false;
    }

    // Calculate actual storage index
    int actual_index;
    if (count <= MAX_HISTORY_ENTRIES) {
        actual_index = index;
    } else {
        // Circular buffer: oldest entry is at (count % MAX_HISTORY_ENTRIES)
        actual_index = (count + index) % MAX_HISTORY_ENTRIES;
    }

    uint32_t key = STORAGE_KEY_HISTORY_BASE + actual_index;

    if (!persist_exists(key)) {
        return false;
    }

    int bytes = persist_read_data(key, record, TREATMENT_RECORD_SIZE);
    return bytes == (int)TREATMENT_RECORD_SIZE;
}

// Clear all history
void storage_clear_all_history(void) {
    int count = storage_get_history_count();
    int max_to_clear = (count < MAX_HISTORY_ENTRIES) ? count : MAX_HISTORY_ENTRIES;

    for (int i = 0; i < max_to_clear; i++) {
        uint32_t key = STORAGE_KEY_HISTORY_BASE + i;
        persist_delete(key);
    }
    persist_delete(STORAGE_KEY_HISTORY_COUNT);
}
