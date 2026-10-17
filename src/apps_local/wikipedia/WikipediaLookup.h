#pragma once

#include <memory>
#include <string>

class Activity;
class GfxRenderer;
class MappedInputManager;

// The reader's way into Wikipedia: the selection bar's Wikipedia button.
namespace wikipedia_lookup {

// A pack is on the card. The button is offered only then.
bool available();

// The app, opened on the article whose title matches `title` (prefix,
// redirect and case folded, as the app's own search), on top of the reader.
// Every way out returns to the book, and with no pack it never hands the card
// to USB.
std::unique_ptr<Activity> open(GfxRenderer& renderer, MappedInputManager& mappedInput, std::string title);

}  // namespace wikipedia_lookup
