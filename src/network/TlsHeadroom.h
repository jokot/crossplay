#pragma once

#include <cstdint>

// Whether there is room to start a TLS transfer. Kept free of Arduino and
// ESP-IDF so the decision is host-testable; HttpDownloader::hasTlsHeadroom()
// reads the live heap and asks it.
namespace tlsheadroom {

struct Heap {
  uint32_t freeBytes;
  uint32_t largestBlock;
};

// Boards where every allocation is internal RAM. Below these the session or its
// ~17KB record buffer fails mid-stream (wolfSSL MEMORY_E), or an interior
// allocation abort()s the device.
inline constexpr uint32_t kInternalFree = 40000;
inline constexpr uint32_t kInternalBlock = 20000;

// Boards whose malloc() spills to PSRAM (CONFIG_SPIRAM_USE_MALLOC): wolfSSL
// allocates through plain malloc(), so its record buffers and big temporaries
// land above CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL in PSRAM. Internal RAM still
// carries the session's small allocations, lwIP and the WiFi driver.
inline constexpr uint32_t kPsramFree = 64 * 1024;
inline constexpr uint32_t kPsramBlock = 32 * 1024;
inline constexpr uint32_t kPsramInternalFree = 24 * 1024;
inline constexpr uint32_t kPsramInternalBlock = 8 * 1024;

// `psram` is null on a board whose malloc() never reaches PSRAM.
inline bool enough(const Heap& internal, const Heap* psram) {
  if (psram == nullptr) {
    return internal.freeBytes >= kInternalFree && internal.largestBlock >= kInternalBlock;
  }
  return internal.freeBytes >= kPsramInternalFree && internal.largestBlock >= kPsramInternalBlock &&
         psram->freeBytes >= kPsramFree && psram->largestBlock >= kPsramBlock;
}

}  // namespace tlsheadroom
