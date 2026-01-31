#pragma once

// Suppress GCC 12+ warning about strftime return type mismatch in SDK headers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#include <pebble.h>
#pragma GCC diagnostic pop

// All weights stored as int32_t in units of 0.1 kg (e.g., 75.3 kg = 753)
// Time stored in minutes

typedef struct {
    int32_t pre_weight;         // Pre-treatment weight (x10, e.g., 753 = 75.3 kg)
    int32_t dry_weight;         // Dry weight (x10)
    int32_t post_weight;        // Post-treatment weight (x10)
    int16_t treatment_time;     // Treatment time in minutes
    int16_t delta_selection;    // 0 = 0.2, 1 = 0.4
    time_t  timestamp;          // When treatment started
    bool    is_complete;        // Whether post-treatment data recorded
} TreatmentRecord;

// Calculated values (computed on demand, not stored)
typedef struct {
    int32_t k_goal;             // Goal removal (x10): pre - dry
    int32_t optimistic;         // k - delta (x10)
    int32_t pessimistic;        // k + delta (x10)
    int32_t ufr;                // Ultrafiltration rate (x100 for 2 decimals)
    int32_t actual_removal;     // Pre - post weight (x10)
    int32_t variance;           // Actual - k (x10, can be negative)
    int32_t percentage;         // Percentage achieved (x10 for 1 decimal)
} CalculatedMetrics;

// Get delta value based on selection (returns 2 for 0.2, 4 for 0.4 in x10 units)
int32_t get_delta_value(int16_t delta_selection);

// Calculate pre-treatment metrics (k, optimistic, pessimistic, UFR)
void calculate_pre_metrics(const TreatmentRecord *record, CalculatedMetrics *metrics);

// Calculate post-treatment metrics (actual removal, variance, percentage)
void calculate_post_metrics(const TreatmentRecord *record, CalculatedMetrics *metrics);

// Initialize a new treatment record with default values
void init_treatment_record(TreatmentRecord *record);
