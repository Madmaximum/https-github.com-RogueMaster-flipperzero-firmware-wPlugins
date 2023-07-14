#pragma once
#include <furi.h>
#include <applications.h>
#include "loader_menufaps.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RECORD_LOADER "loader"
#define LOADER_APPLICATIONS_NAME "Applications"

typedef struct Loader Loader;

typedef enum {
    LoaderStatusOk,
    LoaderStatusErrorAppStarted,
    LoaderStatusErrorUnknownApp,
    LoaderStatusErrorInternal,
    LoaderStatusErrorApiMismatch,
    LoaderStatusErrorApiMismatchExit,
} LoaderStatus;

typedef enum {
    LoaderEventTypeApplicationStarted,
    LoaderEventTypeApplicationStopped
} LoaderEventType;

typedef struct {
    LoaderEventType type;
} LoaderEvent;

/**
 * @brief Start application
 * @param[in] instance loader instance
 * @param[in] name application name
 * @param[in] args application arguments
 * @param[out] error_message detailed error message, can be NULL
 * @return LoaderStatus 
 */
LoaderStatus
    loader_start(Loader* instance, const char* name, const char* args, FuriString* error_message);

/**
 * @brief Start application with GUI error message
 * @param[in] instance loader instance
 * @param[in] name application name
 * @param[in] args application arguments
 * @return LoaderStatus 
 */
LoaderStatus loader_start_with_gui_error(Loader* loader, const char* name, const char* args);

/** 
 * @brief Lock application start
 * @param[in] instance loader instance
 * @return true on success
 */
bool loader_lock(Loader* instance);

/**
 * @brief Unlock application start
 * @param[in] instance loader instance
 */
void loader_unlock(Loader* instance);

/**
 * @brief Check if loader is locked
 * @param[in] instance loader instance
 * @return true if locked
 */
bool loader_is_locked(Loader* instance);

/**
 * @brief Show loader menu
 * @param[in] instance loader instance
 */
void loader_show_menu(Loader* instance);

/**
 * @brief Get loader pubsub
 * @param[in] instance loader instance
 * @return FuriPubSub* 
 */
FuriPubSub* loader_get_pubsub(Loader* instance);

/**
* @brief Get MenuFap item
* @param[in] instance loader instance
* @param[in] size_t of item
* @return MenuFap*
*/
MenuFap* loader_get_menu_fap_item(Loader* instance, size_t x);

/**
* @brief Get MenuFapsList_t size
* @param[in] instance loader instance
* @return size_t of MenuFapsList_t size
*/
size_t loader_get_menu_faps_list_size(Loader* instance);

//const FlipperInternalApplication* loader_find_application_by_name(const char* name);

//const char* loader_find_external_application_by_name(const char* app_name);

#ifdef __cplusplus
}
#endif
