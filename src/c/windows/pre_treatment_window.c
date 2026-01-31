#include "pre_treatment_window.h"
#include "post_treatment_window.h"
#include "../data/storage.h"
#include "../ui/number_format.h"

// Field indices
#define FIELD_PRE_WEIGHT  0
#define FIELD_DRY_WEIGHT  1
#define FIELD_TIME        2
#define FIELD_DELTA       3
#define NUM_FIELDS        4

// Input mode
typedef enum {
    MODE_NAVIGATION,
    MODE_EDITING
} InputMode;

typedef struct {
    Window *window;

    // Input field labels
    TextLayer *pre_label;
    TextLayer *dry_label;
    TextLayer *time_label;
    TextLayer *delta_label;

    // Input field values
    TextLayer *pre_value;
    TextLayer *dry_value;
    TextLayer *time_value;
    TextLayer *delta_value;

    // Result labels and values
    TextLayer *k_label;
    TextLayer *opt_label;
    TextLayer *pess_label;
    TextLayer *ufr_label;

    // State
    int active_field;
    InputMode input_mode;
    TreatmentRecord *record;

    // Text buffers
    char pre_buf[12];
    char dry_buf[12];
    char time_buf[12];
    char delta_buf[8];
    char k_buf[20];
    char opt_buf[20];
    char pess_buf[20];
    char ufr_buf[20];
} PreTreatmentWindowData;

static PreTreatmentWindowData *s_data = NULL;

static void update_display(PreTreatmentWindowData *data);
static void update_calculations(PreTreatmentWindowData *data);
static void highlight_active_field(PreTreatmentWindowData *data);

// Get the value TextLayer for a field index
static TextLayer *get_value_layer(PreTreatmentWindowData *data, int field) {
    switch (field) {
        case FIELD_PRE_WEIGHT: return data->pre_value;
        case FIELD_DRY_WEIGHT: return data->dry_value;
        case FIELD_TIME: return data->time_value;
        case FIELD_DELTA: return data->delta_value;
        default: return NULL;
    }
}

// Highlight the active field
static void highlight_active_field(PreTreatmentWindowData *data) {
    // Reset all fields to normal
    for (int i = 0; i < NUM_FIELDS; i++) {
        TextLayer *layer = get_value_layer(data, i);
        if (layer) {
            text_layer_set_background_color(layer, GColorWhite);
            text_layer_set_text_color(layer, GColorBlack);
        }
    }

    // Highlight active field
    TextLayer *active = get_value_layer(data, data->active_field);
    if (active) {
        if (data->input_mode == MODE_EDITING) {
            text_layer_set_background_color(active, GColorBlack);
            text_layer_set_text_color(active, GColorWhite);
        } else {
            text_layer_set_background_color(active, PBL_IF_COLOR_ELSE(GColorLightGray, GColorBlack));
            text_layer_set_text_color(active, PBL_IF_COLOR_ELSE(GColorBlack, GColorWhite));
        }
    }
}

// Update all display values
static void update_display(PreTreatmentWindowData *data) {
    format_weight(data->pre_buf, sizeof(data->pre_buf), data->record->pre_weight);
    format_weight(data->dry_buf, sizeof(data->dry_buf), data->record->dry_weight);
    format_time(data->time_buf, sizeof(data->time_buf), data->record->treatment_time);
    format_delta(data->delta_buf, sizeof(data->delta_buf), data->record->delta_selection);

    text_layer_set_text(data->pre_value, data->pre_buf);
    text_layer_set_text(data->dry_value, data->dry_buf);
    text_layer_set_text(data->time_value, data->time_buf);
    text_layer_set_text(data->delta_value, data->delta_buf);

    highlight_active_field(data);
}

// Update calculated results
static void update_calculations(PreTreatmentWindowData *data) {
    CalculatedMetrics metrics;
    calculate_pre_metrics(data->record, &metrics);

    snprintf(data->k_buf, sizeof(data->k_buf), "Goal: ");
    format_weight(data->k_buf + 6, sizeof(data->k_buf) - 6, metrics.k_goal);
    strcat(data->k_buf, " kg");

    snprintf(data->opt_buf, sizeof(data->opt_buf), "Opt:  ");
    format_weight(data->opt_buf + 6, sizeof(data->opt_buf) - 6, metrics.optimistic);
    strcat(data->opt_buf, " kg");

    snprintf(data->pess_buf, sizeof(data->pess_buf), "Pess: ");
    format_weight(data->pess_buf + 6, sizeof(data->pess_buf) - 6, metrics.pessimistic);
    strcat(data->pess_buf, " kg");

    snprintf(data->ufr_buf, sizeof(data->ufr_buf), "UFR:  ");
    format_ufr(data->ufr_buf + 6, sizeof(data->ufr_buf) - 6, metrics.ufr);
    strcat(data->ufr_buf, " kg/h");

    text_layer_set_text(data->k_label, data->k_buf);
    text_layer_set_text(data->opt_label, data->opt_buf);
    text_layer_set_text(data->pess_label, data->pess_buf);
    text_layer_set_text(data->ufr_label, data->ufr_buf);
}

// Adjust value based on direction (+1 or -1)
static void adjust_value(PreTreatmentWindowData *data, int direction) {
    switch (data->active_field) {
        case FIELD_PRE_WEIGHT:
            data->record->pre_weight += direction;
            if (data->record->pre_weight < 300) data->record->pre_weight = 300;  // 30 kg min
            if (data->record->pre_weight > 2000) data->record->pre_weight = 2000; // 200 kg max
            break;

        case FIELD_DRY_WEIGHT:
            data->record->dry_weight += direction;
            if (data->record->dry_weight < 300) data->record->dry_weight = 300;
            if (data->record->dry_weight > 2000) data->record->dry_weight = 2000;
            break;

        case FIELD_TIME:
            data->record->treatment_time += direction * 15;  // 15 min increments
            if (data->record->treatment_time < 60) data->record->treatment_time = 60;   // 1 hr min
            if (data->record->treatment_time > 480) data->record->treatment_time = 480; // 8 hr max
            break;

        case FIELD_DELTA:
            // Toggle between 0 and 1
            data->record->delta_selection = 1 - data->record->delta_selection;
            break;
    }

    update_display(data);
    update_calculations(data);
    storage_save_in_progress(data->record);
}

// Button handlers
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    PreTreatmentWindowData *data = (PreTreatmentWindowData *)context;

    if (data->input_mode == MODE_NAVIGATION) {
        data->active_field = (data->active_field - 1 + NUM_FIELDS) % NUM_FIELDS;
        highlight_active_field(data);
    } else {
        adjust_value(data, 1);
    }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    PreTreatmentWindowData *data = (PreTreatmentWindowData *)context;

    if (data->input_mode == MODE_NAVIGATION) {
        data->active_field = (data->active_field + 1) % NUM_FIELDS;
        highlight_active_field(data);
    } else {
        adjust_value(data, -1);
    }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    PreTreatmentWindowData *data = (PreTreatmentWindowData *)context;

    if (data->input_mode == MODE_NAVIGATION) {
        data->input_mode = MODE_EDITING;
        vibes_short_pulse();
    } else {
        data->input_mode = MODE_NAVIGATION;
    }
    highlight_active_field(data);
}

static void select_long_handler(ClickRecognizerRef recognizer, void *context) {
    PreTreatmentWindowData *data = (PreTreatmentWindowData *)context;
    vibes_double_pulse();
    post_treatment_window_push(data->record);
}

static void click_config_provider(void *context) {
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, up_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, down_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_long_click_subscribe(BUTTON_ID_SELECT, 500, select_long_handler, NULL);
}

// Create a text layer helper
static TextLayer *create_text_layer(GRect frame, GFont font, GTextAlignment align) {
    TextLayer *layer = text_layer_create(frame);
    text_layer_set_font(layer, font);
    text_layer_set_text_alignment(layer, align);
    text_layer_set_background_color(layer, GColorWhite);
    text_layer_set_text_color(layer, GColorBlack);
    return layer;
}

static void window_load(Window *window) {
    PreTreatmentWindowData *data = window_get_user_data(window);
    Layer *root = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root);

    // Fonts
    GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    GFont value_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    GFont result_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);

    // Adjust for round displays
    int x_offset = PBL_IF_ROUND_ELSE(20, 5);
    int width = bounds.size.w - (2 * x_offset);

    int y = 5;
    int row_height = 20;
    int label_width = 45;
    int value_width = width - label_width - 5;

    // Pre-weight row
    data->pre_label = create_text_layer(GRect(x_offset, y, label_width, row_height), label_font, GTextAlignmentLeft);
    text_layer_set_text(data->pre_label, "Pre:");
    data->pre_value = create_text_layer(GRect(x_offset + label_width, y, value_width, row_height), value_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->pre_label));
    layer_add_child(root, text_layer_get_layer(data->pre_value));
    y += row_height;

    // Dry weight row
    data->dry_label = create_text_layer(GRect(x_offset, y, label_width, row_height), label_font, GTextAlignmentLeft);
    text_layer_set_text(data->dry_label, "Dry:");
    data->dry_value = create_text_layer(GRect(x_offset + label_width, y, value_width, row_height), value_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->dry_label));
    layer_add_child(root, text_layer_get_layer(data->dry_value));
    y += row_height;

    // Treatment time row
    data->time_label = create_text_layer(GRect(x_offset, y, label_width, row_height), label_font, GTextAlignmentLeft);
    text_layer_set_text(data->time_label, "Time:");
    data->time_value = create_text_layer(GRect(x_offset + label_width, y, value_width, row_height), value_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->time_label));
    layer_add_child(root, text_layer_get_layer(data->time_value));
    y += row_height;

    // Delta row
    data->delta_label = create_text_layer(GRect(x_offset, y, label_width, row_height), label_font, GTextAlignmentLeft);
    text_layer_set_text(data->delta_label, "Delta:");
    data->delta_value = create_text_layer(GRect(x_offset + label_width, y, value_width, row_height), value_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->delta_label));
    layer_add_child(root, text_layer_get_layer(data->delta_value));
    y += row_height + 5;

    // Results section
    data->k_label = create_text_layer(GRect(x_offset, y, width, row_height), result_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->k_label));
    y += row_height - 2;

    data->opt_label = create_text_layer(GRect(x_offset, y, width, row_height), result_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->opt_label));
    y += row_height - 2;

    data->pess_label = create_text_layer(GRect(x_offset, y, width, row_height), result_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->pess_label));
    y += row_height - 2;

    data->ufr_label = create_text_layer(GRect(x_offset, y, width, row_height), result_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->ufr_label));

    // Initialize display
    data->active_field = FIELD_PRE_WEIGHT;
    data->input_mode = MODE_NAVIGATION;
    update_display(data);
    update_calculations(data);
}

static void window_unload(Window *window) {
    PreTreatmentWindowData *data = window_get_user_data(window);

    text_layer_destroy(data->pre_label);
    text_layer_destroy(data->pre_value);
    text_layer_destroy(data->dry_label);
    text_layer_destroy(data->dry_value);
    text_layer_destroy(data->time_label);
    text_layer_destroy(data->time_value);
    text_layer_destroy(data->delta_label);
    text_layer_destroy(data->delta_value);
    text_layer_destroy(data->k_label);
    text_layer_destroy(data->opt_label);
    text_layer_destroy(data->pess_label);
    text_layer_destroy(data->ufr_label);

    window_destroy(window);
    free(data);
    s_data = NULL;
}

void pre_treatment_window_push(TreatmentRecord *record) {
    if (s_data != NULL) {
        return;  // Window already exists
    }

    s_data = calloc(1, sizeof(PreTreatmentWindowData));
    s_data->record = record;
    s_data->window = window_create();

    window_set_user_data(s_data->window, s_data);
    window_set_click_config_provider_with_context(s_data->window, click_config_provider, s_data);
    window_set_window_handlers(s_data->window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });

    window_stack_push(s_data->window, true);
}

Window *pre_treatment_window_get_window(void) {
    return s_data ? s_data->window : NULL;
}
