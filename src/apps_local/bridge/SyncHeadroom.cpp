#include "SyncHeadroom.h"

#if defined(FREEINK_NET_WOLFSSL)

#include <esp_heap_caps.h>
#include <sdkconfig.h>

namespace synchead {

bool liveHeapAllows(Heap& internalOut, Heap& psramOut, bool& psramUsedOut) {
  internalOut = {static_cast<uint32_t>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL)),
                 static_cast<uint32_t>(heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL))};
  psramOut = {0, 0};
  psramUsedOut = false;
#if CONFIG_SPIRAM_USE_MALLOC
  psramUsedOut = heap_caps_get_total_size(MALLOC_CAP_SPIRAM) > 0;
  psramOut = {static_cast<uint32_t>(heap_caps_get_free_size(MALLOC_CAP_SPIRAM)),
              static_cast<uint32_t>(heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM))};
#endif
  return enough(internalOut, psramUsedOut ? &psramOut : nullptr);
}

}  // namespace synchead

#endif
