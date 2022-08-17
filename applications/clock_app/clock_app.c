#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <input/input.h>
#include <dolphin/dolphin.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define TAG "Clock"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    FuriHalRtcDateTime datetime;
} ClockState;

uint32_t timerStartTime;
uint32_t timerLastRunTime;
bool timerStarted;
int timerSecs;
int songSelect;
int timerTempSecs;

static void clock_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clock_render_callback(Canvas* const canvas, void* ctx) {
    ClockState* state = acquire_mutex((ValueMutex*)ctx, 25);
    if(state == NULL) {
        FURI_LOG_E(TAG, "it null");
        return;
    }
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    char strings[3][20];
    timerTempSecs = timerSecs;
    // FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&state->datetime);
    if(timerStarted) timerTempSecs = timerSecs + (int) ((furi_hal_rtc_datetime_to_timestamp(&state->datetime) - timerStartTime));
    int curMin = (timerTempSecs / 60);
    int curSec = timerTempSecs - (curMin * 60);
    snprintf(
        strings[0],
        20,
        "%.4d-%.2d-%.2d",
        state->datetime.year,
        state->datetime.month,
        state->datetime.day);
    snprintf(
        strings[1],
        20,
        "%.2d:%.2d:%.2d",
        state->datetime.hour,
        state->datetime.minute,
        state->datetime.second);
    snprintf(strings[2], 20, "%.2d:%.2d", curMin, curSec);
    canvas_set_font(canvas, FontBigNumbers);
    if(timerStarted || timerTempSecs!=0) {
        canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, strings[1]);
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignTop, strings[2]);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignTop, strings[0]);
    } else {
        canvas_draw_str_aligned(canvas, 64, 26, AlignCenter, AlignCenter, strings[1]);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 38, AlignCenter, AlignTop, strings[0]);
    }
    if(timerStarted) {
        elements_button_center(canvas, "Stop");
    } else {
        elements_button_center(canvas, "Start");
    }
    if(timerTempSecs!=0) elements_button_left(canvas, "Reset");
    if(timerStarted) {
        if(songSelect == 0) {
            elements_button_right(canvas, "S:OFF");
        } else if(songSelect == 1) {
            elements_button_right(canvas, "S:PoRa");
        } else if(songSelect == 2) {
            elements_button_right(canvas, "S:Mario");
        } else if(songSelect == 3) {
            elements_button_right(canvas, "S:ByMin");
        }
    }
    release_mutex((ValueMutex*)ctx, state);
}

static void clock_state_init(ClockState* const state) {
    furi_hal_rtc_get_datetime(&state->datetime);
    timerStarted = false;
    timerSecs = 0;
    timerTempSecs = 0;
    timerStartTime = 0;
    songSelect = 2;
}

const NotificationSequence clock_alert_silent = {
    &message_vibro_on,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_vibro_off,
    &message_display_backlight_off,
    &message_delay_50,
    &message_display_backlight_on,
    NULL,
};
const NotificationSequence clock_alert_pr1 = {
    &message_vibro_on,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    &message_vibro_off,
    &message_display_backlight_off,
    &message_delay_50,
    &message_display_backlight_on,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    NULL,
};
const NotificationSequence clock_alert_pr2 = {
    &message_vibro_on,
    &message_note_fs5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_display_backlight_off,
    &message_vibro_off,
    &message_delay_50,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_display_backlight_on,
    &message_delay_50,
    &message_note_a5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    NULL,
};
const NotificationSequence clock_alert_pr3 = {
    &message_display_backlight_off,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_delay_50,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_delay_100,
    NULL,
};
const NotificationSequence clock_alert_mario1 = {
    &message_vibro_on,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_note_e5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    &message_note_e5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    &message_vibro_off,
    &message_display_backlight_off,
    &message_delay_100,
    &message_display_backlight_on,
    &message_delay_100,
    &message_note_e5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    NULL,
};
const NotificationSequence clock_alert_mario2 = {
    &message_vibro_on,
    &message_display_backlight_off,
    &message_delay_100,
    &message_display_backlight_on,
    &message_delay_100,
    &message_note_c5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_display_backlight_off,
    &message_vibro_off,
    &message_delay_50,
    &message_note_e5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_display_backlight_on,
    NULL,
};
const NotificationSequence clock_alert_mario3 = {
    &message_display_backlight_off,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_delay_50,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_delay_100,
    &message_note_g4,
    &message_delay_100,
    &message_delay_100,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    NULL,
};
const NotificationSequence clock_alert_perMin = {
    &message_note_g5,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    &message_delay_10,
    &message_note_g4,
    &message_delay_50,
    &message_delay_10,
    &message_delay_10,
    &message_sound_off,
    NULL,
};
const NotificationSequence clock_alert_startStop = {
    &message_note_d6,
    &message_delay_100,
    &message_delay_10,
    &message_delay_10,
    &message_sound_off,
    NULL,
};

// Runs every 1000ms by default
static void clock_tick(void* ctx) {
    furi_assert(ctx);
    ClockState* state = (ClockState*)acquire_mutex((ValueMutex*)ctx, 25);
    if(state == NULL) {
        FURI_LOG_D(TAG, "NULL ISSUE?");
        return;
    }
    FuriMessageQueue* event_queue = ctx;
    PluginEvent event = {.type = EventTypeTick};
    if(timerStarted) {
        if(timerTempSecs % 60 == 0 && songSelect != 0 && timerTempSecs!=timerSecs) {
            FURI_LOG_D(TAG, "DO HOUR ALERT");
            NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
            notification_message(notification, &clock_alert_perMin);
            furi_record_close(RECORD_NOTIFICATION);
        }
        if(songSelect == 1) {
            if(timerTempSecs == 80) {
                FURI_LOG_D(TAG, "DO ALERT");
                DOLPHIN_DEED(DolphinDeedU2fAuthorized);
                NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
                notification_message(notification, &clock_alert_pr1);
                furi_record_close(RECORD_NOTIFICATION);
            }
            if(timerTempSecs == 81) {
                NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
                notification_message(notification, &clock_alert_pr2);
                furi_record_close(RECORD_NOTIFICATION);
            }
            if(timerTempSecs == 82) {
                NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
                notification_message(notification, &clock_alert_pr3);
                furi_record_close(RECORD_NOTIFICATION);
            }
        } else if(songSelect == 2) {
            if(timerTempSecs == 80) {
                FURI_LOG_D(TAG, "DO ALERT");
                DOLPHIN_DEED(DolphinDeedU2fAuthorized);
                NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
                notification_message(notification, &clock_alert_mario1);
                furi_record_close(RECORD_NOTIFICATION);
            }
            if(timerTempSecs == 81) {
                NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
                notification_message(notification, &clock_alert_mario2);
                furi_record_close(RECORD_NOTIFICATION);
            }
            if(timerTempSecs == 82) {
                NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
                notification_message(notification, &clock_alert_mario3);
                furi_record_close(RECORD_NOTIFICATION);
            }
        } else {
            if(timerTempSecs == 80) {
                FURI_LOG_D(TAG, "DO ALERT");
                NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
                notification_message(notification, &clock_alert_silent);
                furi_record_close(RECORD_NOTIFICATION);
            }
        }
    }
    // It's OK to loose this event if system overloaded
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t clock_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    ClockState* plugin_state = malloc(sizeof(ClockState));
    clock_state_init(plugin_state);
    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, plugin_state, sizeof(ClockState))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        furi_message_queue_free(event_queue);
        free(plugin_state);
        return 255;
    }
    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, clock_render_callback, &state_mutex);
    view_port_input_callback_set(view_port, clock_input_callback, event_queue);
    FuriTimer* timer = furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency());
    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    // Main loop
    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        ClockState* plugin_state = (ClockState*)acquire_mutex_block(&state_mutex);
        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                furi_hal_rtc_get_datetime(&plugin_state->datetime);
                if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        if(timerStarted) timerSecs = timerSecs + 5;
                        break;
                    case InputKeyDown:
                        if(timerStarted) timerSecs = timerSecs - 5;
                        break;
                    case InputKeyRight:
                        if(songSelect == 0) {
                            songSelect = 1;
                        } else if(songSelect == 1) {
                            songSelect = 2;
                        } else if(songSelect == 2) {
                            songSelect = 3;
                        } else {
                            songSelect = 0;
                        }
                        break;
                    case InputKeyLeft:
                        timerStartTime = furi_hal_rtc_datetime_to_timestamp(&plugin_state->datetime);
                        timerSecs = 0;
                        timerTempSecs = 0;
                        break;
                    case InputKeyOk:
                        if(songSelect == 1 || songSelect == 2 || songSelect == 3) {
                            NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
                            notification_message(notification, &clock_alert_startStop);
                            furi_record_close(RECORD_NOTIFICATION);
                        }
                        if(timerStarted) {
                            timerStarted = false;
                            timerSecs = timerSecs + (int) ((furi_hal_rtc_datetime_to_timestamp(&plugin_state->datetime) - timerStartTime));
                        } else {
                            timerStarted = true;
                            timerStartTime = furi_hal_rtc_datetime_to_timestamp(&plugin_state->datetime);
                        }
                        break;
                    case InputKeyBack:
                        // Exit the plugin
                        processing = false;
                        break;
                    }
                }
            } else if(event.type == EventTypeTick) {
                furi_hal_rtc_get_datetime(&plugin_state->datetime);
            }
        } else {
            FURI_LOG_D(TAG, "osMessageQueue: event timeout");
            // event timeout
        }
        view_port_update(view_port);
        release_mutex(&state_mutex, plugin_state);
    }
    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);
    free(plugin_state);
    return 0;
}