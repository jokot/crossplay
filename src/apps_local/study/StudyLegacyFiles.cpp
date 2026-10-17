#include "StudyLegacyFiles.h"

#include <HalStorage.h>

#include <cstdio>

namespace study {

void removeLegacyGlyphLists(const char* deckDir) {
  static constexpr const char* kLists[] = {"headword", "sentence", "latin", "ruby"};
  char path[96];
  for (const char* list : kLists) {
    std::snprintf(path, sizeof(path), "%s/glyphs-%s.txt", deckDir, list);
    if (Storage.exists(path)) Storage.remove(path);
  }
}

}  // namespace study
