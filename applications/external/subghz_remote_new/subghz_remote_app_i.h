#pragma once

#include "helpers/subrem_types.h"

#include "views/transmitter.h"

#include "scenes/subrem_scene.h"

#include <subghz_remote_new_icons.h> // TODO:
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
#include <gui/modules/text_input.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include <gui/modules/popup.h>

#include <lib/subghz/subghz_setting.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>

#include <flipper_format/flipper_format_i.h> // FIXME:

// #include "views/subghz_remote_view_programmer.h"
// #include "views/subghz_remote_view_reader.h"
// #include "views/subghz_remote_view_writer.h"
// #include "views/subghz_remote_view_chip_detect.h"

#define SUBREM_APP_EXTENSION ".txt"
#define SUBREM_APP_FOLDER "/ext/subghz_remote"
#define SUBGHZ_REMOTE_MAX_LEN_NAME 64
#define SUBREM_MAX_SUB_KEY_COUNT (5U)

typedef struct {
    uint32_t frequency;
    uint8_t* data;
} FreqPreset;

// Sub File preset
typedef struct {
    FlipperFormat* fff_data;
    FreqPreset freq_preset;
    FuriString* file_path;
    FuriString* label;
    SubRemSubKeyType type;
} SubRemSubFilePreset;

SubRemSubFilePreset* subrem_sub_file_preset_alloc();

void subrem_sub_file_preset_free(SubRemSubFilePreset* sub_preset);

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Popup* popup;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
    FuriString* file_path;
    char file_name_tmp[SUBGHZ_REMOTE_MAX_LEN_NAME];

    SubRemViewRemote* subrem_remote_view;

    SubRemSubFilePreset* subs_preset[SUBREM_MAX_SUB_KEY_COUNT];

    SubGhzSetting* setting;

    // AvrIspProgrammerView* subghz_remote_programmer_view;
    // AvrIspReaderView* subghz_remote_reader_view;
    // AvrIspWriterView* subghz_remote_writer_view;
    // AvrIspChipDetectView* subghz_remote_chip_detect_view;

    // AvrIspError error;
} SubGhzRemoteApp;

bool subrem_load_from_file(SubGhzRemoteApp* app);