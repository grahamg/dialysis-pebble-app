#include "treatment_data.h"

int32_t get_delta_value(int16_t delta_selection) {
    // delta_selection: 0 = 0.2, 1 = 0.4
    // Return value in x10 units (2 or 4)
    return (delta_selection == 0) ? 2 : 4;
}

void calculate_pre_metrics(const TreatmentRecord *record, CalculatedMetrics *metrics) {
    // k (goal) = pre_weight - dry_weight
    metrics->k_goal = record->pre_weight - record->dry_weight;

    // Get delta in x10 units
    int32_t delta = get_delta_value(record->delta_selection);

    // Optimistic = k - delta
    metrics->optimistic = metrics->k_goal - delta;

    // Pessimistic = k + delta
    metrics->pessimistic = metrics->k_goal + delta;

    // UFR = k / treatment_time (in kg/hr)
    // k is in x10 (0.1 kg units), time is in minutes
    // UFR result in x100 for 2 decimal places
    // Formula: (k_x10 * 60 * 10) / time_minutes = UFR_x100
    if (record->treatment_time > 0) {
        metrics->ufr = (metrics->k_goal * 600) / record->treatment_time;
    } else {
        metrics->ufr = 0;
    }
}

void calculate_post_metrics(const TreatmentRecord *record, CalculatedMetrics *metrics) {
    // First calculate pre-metrics to get k_goal
    calculate_pre_metrics(record, metrics);

    // Actual removal = pre_weight - post_weight
    metrics->actual_removal = record->pre_weight - record->post_weight;

    // Variance = actual - k (can be negative)
    metrics->variance = metrics->actual_removal - metrics->k_goal;

    // Percentage = (actual / k) * 100
    // Both in x10 units, result in x10 for 1 decimal
    // Formula: (actual * 1000) / k = percentage_x10
    if (metrics->k_goal != 0) {
        metrics->percentage = (metrics->actual_removal * 1000) / metrics->k_goal;
    } else {
        metrics->percentage = 0;
    }
}

void init_treatment_record(TreatmentRecord *record) {
    // Default pre-weight: 75.0 kg
    record->pre_weight = 750;
    // Default dry weight: 72.0 kg
    record->dry_weight = 720;
    // Default post-weight: same as dry weight initially
    record->post_weight = 720;
    // Default treatment time: 4 hours (240 minutes)
    record->treatment_time = 240;
    // Default delta: 0.2
    record->delta_selection = 0;
    // Timestamp: now
    record->timestamp = time(NULL);
    // Not complete yet
    record->is_complete = false;
}
