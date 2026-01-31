#include "post_treatment_window.h"
#include "../data/storage.h"
#include "../ui/number_format.h"

typedef struct {
    Window *window;

    // Input section
    TextLayer *title_label;
    TextLayer *post_label;
    TextLayer *post_value;

    // Results section
    TextLayer *results_header;
    TextLayer *removed_label;
    TextLayer *goal_label;
    TextLayer *variance_label;
    TextLayer *percent_label;

    // Hint
    TextLayer *hint_label;

    // State
    TreatmentRecord *record;
    CalculatedMetrics metrics;

    // Text buffers
    char post_buf[12];
    char removed_buf[24];
    char goal_buf[24];
    char variance_buf[24];
    char percent_buf[24];
} PostTreatmentWindowData;

static PostTreatmentWindowData *s_data = NULL;

static void update_display(PostTreatmentWindowData *data);
static void update_results(PostTreatmentWindowData *data);

static void update_display(PostTreatmentWindowData *data) {
    format_weight(data->post_buf, sizeof(data->post_buf), data->record->post_weight);
    text_layer_set_text(data->post_value, data->post_buf);
}

static void update_results(PostTreatmentWindowData *data) {
    calculate_post_metrics(data->record, &data->metrics);

    char temp[12];

    // Removed
    format_weight(temp, sizeof(temp), data->metrics.actual_removal);
    snprintf(data->removed_buf, sizeof(data->removed_buf), "Removed: %s kg", temp);
    text_layer_set_text(data->removed_label, data->removed_buf);

    // Goal
    format_weight(temp, sizeof(temp), data->metrics.k_goal);
    snprintf(data->goal_buf, sizeof(data->goal_buf), "Goal:    %s kg", temp);
    text_layer_set_text(data->goal_label, data->goal_buf);

    // Variance (with +/- sign)
    format_variance(temp, sizeof(temp), data->metrics.variance);
    snprintf(data->variance_buf, sizeof(data->variance_buf), "Diff:    %s kg", temp);
    text_layer_set_text(data->variance_label, data->variance_buf);

    // Color the variance on color displays
    #ifdef PBL_COLOR
    if (data->metrics.variance >= 0) {
        text_layer_set_text_color(data->variance_label, GColorDarkGreen);
    } else {
        text_layer_set_text_color(data->variance_label, GColorRed);
    }
    #endif

    // Percentage
    format_percentage(temp, sizeof(temp), data->metrics.percentage);
    snprintf(data->percent_buf, sizeof(data->percent_buf), "Achieved: %s", temp);
    text_layer_set_text(data->percent_label, data->percent_buf);
}

static void adjust_post_weight(PostTreatmentWindowData *data, int direction) {
    data->record->post_weight += direction;
    if (data->record->post_weight < 300) data->record->post_weight = 300;
    if (data->record->post_weight > 2000) data->record->post_weight = 2000;

    update_display(data);
    update_results(data);
    storage_save_in_progress(data->record);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    PostTreatmentWindowData *data = (PostTreatmentWindowData *)context;
    adjust_post_weight(data, 1);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    PostTreatmentWindowData *data = (PostTreatmentWindowData *)context;
    adjust_post_weight(data, -1);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    PostTreatmentWindowData *data = (PostTreatmentWindowData *)context;

    // Mark as complete and save to history
    data->record->is_complete = true;
    storage_save_to_history(data->record);
    storage_clear_in_progress();

    vibes_long_pulse();

    // Pop back to pre-treatment window
    window_stack_pop(true);

    // Reset the record for next treatment
    init_treatment_record(data->record);
}

static void click_config_provider(void *context) {
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, up_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, down_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static TextLayer *create_text_layer(GRect frame, GFont font, GTextAlignment align) {
    TextLayer *layer = text_layer_create(frame);
    text_layer_set_font(layer, font);
    text_layer_set_text_alignment(layer, align);
    text_layer_set_background_color(layer, GColorWhite);
    text_layer_set_text_color(layer, GColorBlack);
    return layer;
}

static void window_load(Window *window) {
    PostTreatmentWindowData *data = window_get_user_data(window);
    Layer *root = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root);

    GFont title_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    GFont value_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    GFont result_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    GFont hint_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);

    int x_offset = PBL_IF_ROUND_ELSE(20, 5);
    int width = bounds.size.w - (2 * x_offset);

    int y = 2;

    // Title
    data->title_label = create_text_layer(GRect(x_offset, y, width, 20), title_font, GTextAlignmentCenter);
    text_layer_set_text(data->title_label, "POST-TREATMENT");
    layer_add_child(root, text_layer_get_layer(data->title_label));
    y += 22;

    // Post-weight input
    data->post_label = create_text_layer(GRect(x_offset, y, 50, 18), label_font, GTextAlignmentLeft);
    text_layer_set_text(data->post_label, "Post:");
    layer_add_child(root, text_layer_get_layer(data->post_label));

    data->post_value = create_text_layer(GRect(x_offset + 50, y - 4, width - 50, 28), value_font, GTextAlignmentLeft);
    text_layer_set_background_color(data->post_value, GColorBlack);
    text_layer_set_text_color(data->post_value, GColorWhite);
    layer_add_child(root, text_layer_get_layer(data->post_value));
    y += 28;

    // Results header
    data->results_header = create_text_layer(GRect(x_offset, y, width, 16), label_font, GTextAlignmentCenter);
    text_layer_set_text(data->results_header, "--- RESULTS ---");
    layer_add_child(root, text_layer_get_layer(data->results_header));
    y += 18;

    // Results
    int row_height = 16;

    data->removed_label = create_text_layer(GRect(x_offset, y, width, row_height), result_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->removed_label));
    y += row_height;

    data->goal_label = create_text_layer(GRect(x_offset, y, width, row_height), result_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->goal_label));
    y += row_height;

    data->variance_label = create_text_layer(GRect(x_offset, y, width, row_height), result_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->variance_label));
    y += row_height;

    data->percent_label = create_text_layer(GRect(x_offset, y, width, row_height), result_font, GTextAlignmentLeft);
    layer_add_child(root, text_layer_get_layer(data->percent_label));
    y += row_height + 2;

    // Hint
    data->hint_label = create_text_layer(GRect(x_offset, y, width, 16), hint_font, GTextAlignmentCenter);
    text_layer_set_text(data->hint_label, "SELECT to save");
    layer_add_child(root, text_layer_get_layer(data->hint_label));

    // Initialize post-weight to dry weight as a starting point
    if (data->record->post_weight == 0) {
        data->record->post_weight = data->record->dry_weight;
    }

    update_display(data);
    update_results(data);
}

static void window_unload(Window *window) {
    PostTreatmentWindowData *data = window_get_user_data(window);

    text_layer_destroy(data->title_label);
    text_layer_destroy(data->post_label);
    text_layer_destroy(data->post_value);
    text_layer_destroy(data->results_header);
    text_layer_destroy(data->removed_label);
    text_layer_destroy(data->goal_label);
    text_layer_destroy(data->variance_label);
    text_layer_destroy(data->percent_label);
    text_layer_destroy(data->hint_label);

    window_destroy(window);
    free(data);
    s_data = NULL;
}

void post_treatment_window_push(TreatmentRecord *record) {
    if (s_data != NULL) {
        return;
    }

    s_data = calloc(1, sizeof(PostTreatmentWindowData));
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
