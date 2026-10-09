#include <atomic>
#include "SceTypes.hpp"
#include "libScePlayerInvitationDialog.h"
#include "prx/libc/include/General.hpp"

namespace {

constexpr std::int32_t COMMON_DIALOG_STATUS_NONE = 0;
constexpr std::int32_t COMMON_DIALOG_STATUS_INITIALIZED = 1;
constexpr std::int32_t COMMON_DIALOG_STATUS_RUNNING = 2;
constexpr std::int32_t COMMON_DIALOG_STATUS_FINISHED = 3;
constexpr std::int32_t COMMON_DIALOG_ERROR_NOT_INITIALIZED = static_cast<std::int32_t>(0x80B80003u);
constexpr std::int32_t COMMON_DIALOG_ERROR_ALREADY_INITIALIZED = static_cast<std::int32_t>(0x80B80004u);
constexpr std::int32_t COMMON_DIALOG_ERROR_NOT_FINISHED = static_cast<std::int32_t>(0x80B80005u);
constexpr std::int32_t COMMON_DIALOG_ERROR_BUSY = static_cast<std::int32_t>(0x80B80007u);
constexpr std::int32_t COMMON_DIALOG_ERROR_ARG_NULL = static_cast<std::int32_t>(0x80B8000Du);

std::atomic<std::int32_t> g_status{COMMON_DIALOG_STATUS_NONE};

}

extern "C" {

std::int32_t APS5_VABI scePlayerInvitationDialogInitialize(void) {
    std::int32_t expected = COMMON_DIALOG_STATUS_NONE;
    if (!g_status.compare_exchange_strong(expected, COMMON_DIALOG_STATUS_INITIALIZED)) return COMMON_DIALOG_ERROR_ALREADY_INITIALIZED;
    return 0;
}

std::int32_t APS5_VABI scePlayerInvitationDialogOpen(const ScePlayerInvitationDialogParam* param) {
    const std::int32_t status = g_status.load();
    if (status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
    if (status == COMMON_DIALOG_STATUS_RUNNING) return COMMON_DIALOG_ERROR_BUSY;
    if (param == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
    g_status = COMMON_DIALOG_STATUS_FINISHED;
    return 0;
}

std::int32_t APS5_VABI scePlayerInvitationDialogUpdateStatus(void) {
    return g_status.load();
}

std::int32_t APS5_VABI scePlayerInvitationDialogGetStatus(void) {
    return g_status.load();
}

std::int32_t APS5_VABI scePlayerInvitationDialogClose(void) {
    if (g_status.load() == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
    return 0;
}

std::int32_t APS5_VABI scePlayerInvitationDialogTerminate(void) {
    if (g_status.exchange(COMMON_DIALOG_STATUS_NONE) == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
    return 0;
}

std::int32_t APS5_VABI scePlayerInvitationDialogGetResult(ScePlayerInvitationDialogResult* result) {
    const std::int32_t status = g_status.load();
    if (status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
    if (result == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
    if (status != COMMON_DIALOG_STATUS_FINISHED) return COMMON_DIALOG_ERROR_NOT_FINISHED;
    result->errorCode = 0;
    result->result = 0;
    return 0;
}

}
