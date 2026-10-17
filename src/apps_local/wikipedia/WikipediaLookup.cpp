#include "WikipediaLookup.h"

#include <HalStorage.h>

#include <utility>

#include "WikipediaActivity.h"
#include "WikipediaPack.h"

namespace wikipedia_lookup {

bool available() { return Storage.exists(wikipedia::Pack::kManifestPath); }

std::unique_ptr<Activity> open(GfxRenderer& renderer, MappedInputManager& mappedInput, std::string title) {
  return WikipediaActivity::createForTitle(renderer, mappedInput, std::move(title));
}

}  // namespace wikipedia_lookup
