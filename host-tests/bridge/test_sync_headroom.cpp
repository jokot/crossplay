// The sync transports' TLS pre-flight on boards whose malloc() reaches PSRAM.
//
// Study's sync and every BridgeHttp app (Wallpapers, Instapaper, Trivia, Live)
// refused below 35000 bytes free / a 20000-byte block of INTERNAL heap. On an
// X4 Pro wolfSSL's record buffers come from PSRAM, and a reader with a book
// open sits around 23-37KB internal: "Not enough memory free to sync right
// now", every time, on a device with 8MB of PSRAM free.
#include <cstdio>

#include "SyncHeadroom.h"

static int checks = 0;
static int failures = 0;

#define CHECK(cond)                                               \
  do {                                                            \
    ++checks;                                                     \
    if (!(cond)) {                                                \
      ++failures;                                                 \
      std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); \
    }                                                             \
  } while (0)

using synchead::enough;
using synchead::Heap;

namespace {

constexpr Heap kAmplePsram{8000000, 8000000};

void testBoardsWithoutPsramKeepTheOldFloor() {
  CHECK(!enough({34999, 30000}, nullptr));
  CHECK(!enough({35000, 19999}, nullptr));
  CHECK(enough({35000, 20000}, nullptr));
}

void testTheX4ProReadingThatWasRefused() {
  // The reading Study refused with, and the font installer's after a lookup.
  CHECK(!enough({23096, 12788}, nullptr));
  CHECK(enough({23096, 12788}, &kAmplePsram));
}

void testPsramStillHasToHoldTheRecordBuffers() {
  constexpr Heap fragmented{200000, 16 * 1024};
  CHECK(!enough({200000, 100000}, &fragmented));
  constexpr Heap nearlyFull{synchead::kPsramFree - 1, synchead::kPsramBlock};
  CHECK(!enough({200000, 100000}, &nearlyFull));
}

void testInternalHeadroomIsStillRequired() {
  CHECK(!enough({synchead::kPsramInternalFree - 1, 50000}, &kAmplePsram));
  CHECK(!enough({200000, synchead::kPsramInternalBlock - 1}, &kAmplePsram));
  CHECK(enough({synchead::kPsramInternalFree, synchead::kPsramInternalBlock}, &kAmplePsram));
}

}  // namespace

int main() {
  testBoardsWithoutPsramKeepTheOldFloor();
  testTheX4ProReadingThatWasRefused();
  testPsramStillHasToHoldTheRecordBuffers();
  testInternalHeadroomIsStillRequired();
  std::printf("%s %d checks, %d failed\n", failures == 0 ? "PASS" : "FAIL", checks, failures);
  return failures == 0 ? 0 : 1;
}
