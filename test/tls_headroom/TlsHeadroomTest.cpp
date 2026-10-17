#include <gtest/gtest.h>

#include "network/TlsHeadroom.h"

using tlsheadroom::enough;
using tlsheadroom::Heap;

namespace {
constexpr Heap kAmplePsram{4000000, 4000000};
}

TEST(TlsHeadroom, NoPsramKeepsOriginalFloors) {
  EXPECT_FALSE(enough({39999, 30000}, nullptr));
  EXPECT_FALSE(enough({40000, 19999}, nullptr));
  EXPECT_TRUE(enough({40000, 20000}, nullptr));
}

TEST(TlsHeadroom, PsramBoardAcceptsTheReadingThatFailedOnAnX4Pro) {
  // "Low heap for manifest (36832 free, 20468 max block)" on an X4 Pro, whose
  // TLS record buffers come from PSRAM.
  EXPECT_FALSE(enough({36832, 20468}, nullptr));
  EXPECT_TRUE(enough({36832, 20468}, &kAmplePsram));
}

TEST(TlsHeadroom, PsramBoardStillNeedsAPsramBlockForTheRecordBuffers) {
  // Plenty free, but no single block can hold a ~17KB record buffer.
  constexpr Heap fragmentedPsram{200000, 16 * 1024};
  EXPECT_FALSE(enough({200000, 100000}, &fragmentedPsram));
  constexpr Heap nearlyFullPsram{tlsheadroom::kPsramFree - 1, tlsheadroom::kPsramBlock};
  EXPECT_FALSE(enough({200000, 100000}, &nearlyFullPsram));
}

TEST(TlsHeadroom, PsramBoardStillNeedsInternalHeadroom) {
  EXPECT_FALSE(enough({tlsheadroom::kPsramInternalFree - 1, 50000}, &kAmplePsram));
  EXPECT_FALSE(enough({200000, tlsheadroom::kPsramInternalBlock - 1}, &kAmplePsram));
  EXPECT_TRUE(enough({tlsheadroom::kPsramInternalFree, tlsheadroom::kPsramInternalBlock}, &kAmplePsram));
}
