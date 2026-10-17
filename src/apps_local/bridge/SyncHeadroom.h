#pragma once

#include <cstdint>

// Whether there is room to start a TLS request, for the fork's own sync
// transports (StudySync, BridgeHttp). Freestanding so it is host-testable;
// SyncHeadroom::fromLiveHeap() reads the device.
//
// The same rule as upstream's TLS pre-flight for its downloads, kept here
// rather than shared because those transports live in apps_local and must
// not reach into upstream's network code for a decision this small.
namespace synchead {

struct Heap {
  uint32_t freeBytes;
  uint32_t largestBlock;
};

// Boards where every allocation is internal: the KOSync numbers, measured with
// the same wolfSSL build.
inline constexpr uint32_t kInternalFree = 35000;
inline constexpr uint32_t kInternalBlock = 20000;

// Boards whose malloc() spills to PSRAM (CONFIG_SPIRAM_USE_MALLOC): wolfSSL
// allocates through plain malloc(), so its record buffers land in PSRAM and
// internal RAM carries only the session's small allocations, lwIP and WiFi.
inline constexpr uint32_t kPsramFree = 64 * 1024;
inline constexpr uint32_t kPsramBlock = 32 * 1024;
inline constexpr uint32_t kPsramInternalFree = 12 * 1024;
inline constexpr uint32_t kPsramInternalBlock = 8 * 1024;

// `psram` is null on a board whose malloc() never reaches PSRAM.
inline bool enough(const Heap& internal, const Heap* psram) {
  if (psram == nullptr) {
    return internal.freeBytes >= kInternalFree && internal.largestBlock >= kInternalBlock;
  }
  return internal.freeBytes >= kPsramInternalFree && internal.largestBlock >= kPsramInternalBlock &&
         psram->freeBytes >= kPsramFree && psram->largestBlock >= kPsramBlock;
}

#if defined(FREEINK_NET_WOLFSSL)
// The live heap, measured the way ESP.getFreeHeap()/getMaxAllocHeap() do
// (MALLOC_CAP_INTERNAL), plus PSRAM when malloc() can reach it.
bool liveHeapAllows(Heap& internalOut, Heap& psramOut, bool& psramUsedOut);
#endif

}  // namespace synchead
