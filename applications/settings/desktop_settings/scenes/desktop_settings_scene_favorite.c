#include "../desktop_settings_app.h"
#include <applications.h>
#include "desktop_settings_scene.h"
#include <flipper_application/flipper_application.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>

#define ALL_APPS_COUNT \
    (FLIPPER_APPS_COUNT + FLIPPER_EXTERNAL_APPS_COUNT + FLIPPER_SETTINGS_APPS_COUNT)

#define EXTERNAL_BROWSER_NAME ("Applications")
#define EXTERNAL_BROWSER_INDEX (ALL_APPS_COUNT + 1)

#define EXTERNAL_APPLICATION_NAME ("[External Application]")
#define EXTERNAL_APPLICATION_INDEX (ALL_APPS_COUNT + 2)

#define NONE_APPLICATION_INDEX (ALL_APPS_COUNT + 3)

static bool favorite_fap_selector_item_callback(
    FuriString* file_path,
    void* context,
    uint8_t** icon_ptr,
    FuriString* item_name) {
    UNUSED(context);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool success = flipper_application_load_name_and_icon(file_path, storage, icon_ptr, item_name);
    furi_record_close(RECORD_STORAGE);
    return success;
}

static bool favorite_fap_selector_file_exists(char* file_path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool exists = storage_file_exists(storage, file_path);
    furi_record_close(RECORD_STORAGE);
    return exists;
}

static void desktop_settings_scene_favorite_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void desktop_settings_scene_favorite_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_reset(submenu);

    uint32_t primary_favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);
    uint32_t pre_select_item = 0;
    FavoriteApp* curr_favorite_app = NULL;
    if(primary_favorite == 0) {
        curr_favorite_app = &app->settings.favorite_primary;
    } else if(primary_favorite == 1) {
        curr_favorite_app = &app->settings.favorite_secondary;
    } else if(primary_favorite == 2) {
        curr_favorite_app = &app->settings.favorite_tertiary;
    } else if(primary_favorite == 3) {
        curr_favorite_app = &app->settings.favorite_quaternary;
    } else {
        curr_favorite_app = &app->settings.favorite_primary;
    }
    if(curr_favorite_app == NULL) {
        // This should not happen!
        return;
    }

    // External Menu Apps
    for(size_t i = 0; i < FLIPPER_EXTERNAL_APPS_COUNT; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_EXTERNAL_APPS[i].name,
            (uint32_t)FLIPPER_EXTERNAL_APPS[i].path,
            desktop_settings_scene_favorite_submenu_callback,
            app);

        // Select favorite item in submenu
        if(!curr_favorite_app->is_external &&
           !strcmp(FLIPPER_EXTERNAL_APPS[i].name, curr_favorite_app->name_or_path)) {
            pre_select_item = (uint32_t)FLIPPER_EXTERNAL_APPS[i].path;
        }
    }

    // Internal Apps
    for(size_t i = 0; i < FLIPPER_APPS_COUNT; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_APPS[i].name,
            (uint32_t)FLIPPER_APPS[i].name,
            desktop_settings_scene_favorite_submenu_callback,
            app);

        // Select favorite item in submenu
        if(!curr_favorite_app->is_external &&
           !strcmp(FLIPPER_APPS[i].name, curr_favorite_app->name_or_path)) {
            pre_select_item = (uint32_t)FLIPPER_APPS[i].name;
        }
    }

    // Internal Settings Apps
    for(size_t i = 0; i < FLIPPER_SETTINGS_APPS_COUNT; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_SETTINGS_APPS[i].name,
            (uint32_t)FLIPPER_SETTINGS_APPS[i].name,
            desktop_settings_scene_favorite_submenu_callback,
            app);

        // Select favorite item in submenu
        if(!curr_favorite_app->is_external &&
           !strcmp(FLIPPER_SETTINGS_APPS[i].name, curr_favorite_app->name_or_path)) {
            pre_select_item = (uint32_t)FLIPPER_SETTINGS_APPS[i].name;
        }
    }

    // Special case: Application browser
    submenu_add_item(
        submenu,
        EXTERNAL_BROWSER_NAME,
        EXTERNAL_BROWSER_INDEX,
        desktop_settings_scene_favorite_submenu_callback,
        app);

    // Special case: Specific application
    submenu_add_item(
        submenu,
        EXTERNAL_APPLICATION_NAME,
        EXTERNAL_APPLICATION_INDEX,
        desktop_settings_scene_favorite_submenu_callback,
        app);

    if(curr_favorite_app->is_external) {
        if(strcmp(curr_favorite_app->name_or_path, "Applications") == 0) {
            pre_select_item = EXTERNAL_BROWSER_INDEX;
        } else {
            pre_select_item = EXTERNAL_APPLICATION_INDEX;
        }
    }

    submenu_add_item(
        submenu,
        "None (disable)",
        NONE_APPLICATION_INDEX,
        desktop_settings_scene_favorite_submenu_callback,
        app);

    if(primary_favorite == 0) {
        submenu_set_header(submenu, "Primary favorite app:");
    } else if(primary_favorite == 1) {
        submenu_set_header(submenu, "Secondary favorite app:");
    } else if(primary_favorite == 2) {
        submenu_set_header(submenu, "Tertiary favorite app:");
    } else if(primary_favorite == 3) {
        submenu_set_header(submenu, "Quaternary favorite app:");
    }
    submenu_set_selected_item(submenu, pre_select_item); // If set during loop, visual glitch.

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_favorite_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;
    FuriString* temp_path = furi_string_alloc_set_str(EXT_PATH("apps"));

    uint32_t primary_favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);
    FavoriteApp* curr_favorite_app = NULL;
    if(primary_favorite == 0) {
        curr_favorite_app = &app->settings.favorite_primary;
    } else if(primary_favorite == 1) {
        curr_favorite_app = &app->settings.favorite_secondary;
    } else if(primary_favorite == 2) {
        curr_favorite_app = &app->settings.favorite_tertiary;
    } else if(primary_favorite == 3) {
        curr_favorite_app = &app->settings.favorite_quaternary;
    } else {
        curr_favorite_app = &app->settings.favorite_primary;
    }
    if(curr_favorite_app == NULL) {
        // This should not happen!
        furi_string_free(temp_path);
        return consumed;
    }

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == EXTERNAL_BROWSER_INDEX) {
            curr_favorite_app->is_external = true;
            strncpy(curr_favorite_app->name_or_path, "Applications", MAX_APP_LENGTH);
            consumed = true;
        } else if(event.event == EXTERNAL_APPLICATION_INDEX) {
            const DialogsFileBrowserOptions browser_options = {
                .extension = ".fap",
                .icon = &I_unknown_10px,
                .skip_assets = true,
                .hide_ext = true,
                .item_loader_callback = favorite_fap_selector_item_callback,
                .item_loader_context = app,
                .base_path = EXT_PATH("apps"),
            };

            // Select favorite fap in file browser
            if(favorite_fap_selector_file_exists(curr_favorite_app->name_or_path)) {
                furi_string_set_str(temp_path, curr_favorite_app->name_or_path);
            }

            if(dialog_file_browser_show(app->dialogs, temp_path, temp_path, &browser_options)) {
                submenu_reset(app->submenu); // Prevent menu from being shown when we exiting scene
                curr_favorite_app->is_external = true;
                strncpy(
                    curr_favorite_app->name_or_path,
                    furi_string_get_cstr(temp_path),
                    MAX_APP_LENGTH);
                consumed = true;
            }
        } else if(event.event == NONE_APPLICATION_INDEX) {
            curr_favorite_app->is_external = false;
            strncpy(curr_favorite_app->name_or_path, "n", MAX_APP_LENGTH);
            consumed = true;
        } else {
            curr_favorite_app->is_external = false;
            strncpy(curr_favorite_app->name_or_path, (const char*)event.event, MAX_APP_LENGTH);
            consumed = true;
        }
        if(consumed) {
            scene_manager_previous_scene(app->scene_manager);
        }
        consumed = true;
    }

    furi_string_free(temp_path);
    return consumed;
}

void desktop_settings_scene_favorite_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    DESKTOP_SETTINGS_SAVE(&app->settings);
    submenu_reset(app->submenu);
}
