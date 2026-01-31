// Suppress GCC 12+ warning about strftime return type mismatch in SDK headers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#include <pebble.h>
#pragma GCC diagnostic pop
#include "data/treatment_data.h"
#include "data/storage.h"
#include "windows/pre_treatment_window.h"

// Global treatment record (shared between windows)
static TreatmentRecord s_current_treatment;

static void init(void) {
    // Check if there's an in-progress treatment to resume
    if (storage_has_in_progress()) {
        if (storage_load_in_progress(&s_current_treatment)) {
            // Successfully loaded - resume with saved data
            APP_LOG(APP_LOG_LEVEL_INFO, "Resuming in-progress treatment");
        } else {
            // Failed to load - start fresh
            init_treatment_record(&s_current_treatment);
        }
    } else {
        // No in-progress treatment - start fresh
        init_treatment_record(&s_current_treatment);
    }

    // Save as in-progress (creates new entry or updates existing)
    storage_save_in_progress(&s_current_treatment);

    // Push the pre-treatment window
    pre_treatment_window_push(&s_current_treatment);
}

static void deinit(void) {
    // Save current state before exiting
    if (!s_current_treatment.is_complete) {
        storage_save_in_progress(&s_current_treatment);
    }
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
