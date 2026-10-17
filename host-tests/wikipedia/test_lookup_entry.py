#!/usr/bin/env python3
"""Wikipedia opened from the reader's selection bar comes back to the book.

The selection bar's Wikipedia button starts WikipediaActivity on top of the
open book (wikipedia_lookup::open). Three things must hold, and none of them
can be exercised on the host, so this reads the source the way
test_usb_exclusive.py does:

1. The button exists only when a pack is on the card. ClipSelectionActivity
   adds ClippingResult::Action::Wikipedia only behind
   wikipedia_lookup::available(), which asks for the pack manifest.
2. Every way out of the app returns to the book. On the shelf the app leaves
   through shelf::leave(); opened from the reader it must finish() instead, or
   Back drops the reader on the Apps folder with the book closed.
3. With no pack the app never hands the card to USB. The install screen calls
   Storage.beginUsbDrive(); doing that over an open book detaches the card the
   reader is reading from.
"""

import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
WIKI = ROOT / "src" / "apps_local" / "wikipedia"
CLIP = ROOT / "src" / "activities" / "reader" / "ClipSelectionActivity.cpp"

checks = 0
bad = []


def check(ok, message):
    global checks
    checks += 1
    if not ok:
        bad.append(message)


def body_of(source, signature):
    """The brace-balanced body of the first function whose definition starts with `signature`."""
    start = source.find(signature)
    if start < 0:
        return ""
    open_brace = source.find("{", start)
    depth = 0
    for i in range(open_brace, len(source)):
        depth += {"{": 1, "}": -1}.get(source[i], 0)
        if depth == 0:
            return source[open_brace : i + 1]
    return ""


lookup = (WIKI / "WikipediaLookup.cpp").read_text(encoding="utf-8") if (WIKI / "WikipediaLookup.cpp").exists() else ""
activity = (WIKI / "WikipediaActivity.cpp").read_text(encoding="utf-8")
clip = CLIP.read_text(encoding="utf-8")

# 1. Only with a pack.
check("Pack::kManifestPath" in body_of(lookup, "bool available()"),
      "wikipedia_lookup::available() must ask for the pack manifest")
check("createForTitle(" in body_of(lookup, "std::unique_ptr<Activity> open("),
      "wikipedia_lookup::open() must start the app on the selected title")
adds = [m.start() for m in re.finditer(r"ClippingResult::Action::Wikipedia", clip)]
check(len(adds) >= 1, "ClipSelectionActivity never offers the Wikipedia action")
guarded = re.search(r"if\s*\(\s*wikipedia_lookup::available\(\)\s*\)[^;]*Action::Wikipedia", clip)
check(guarded is not None, "the Wikipedia action must be added only behind wikipedia_lookup::available()")

# 2. Every exit goes back to the book.
leaves = [m.start() for m in re.finditer(r"shelf::leave\(", activity)]
check(len(leaves) == 1, f"shelf::leave() must have exactly one caller (leaveApp), found {len(leaves)}")
leave_app = body_of(activity, "void WikipediaActivity::leaveApp()")
check("returnToCaller_" in leave_app and "finish()" in leave_app and "shelf::leave(" in leave_app,
      "leaveApp() must finish() back to the reader when opened from it, and use shelf::leave() otherwise")
check("returnToCaller_" in body_of(activity, "void WikipediaActivity::popHistory()"),
      "an empty trail opened from the reader must return to the book, not the search screen")

# 3. Never a USB handoff over a book.
on_enter = body_of(activity, "void WikipediaActivity::onEnter()")
install_call = on_enter.find("enterInstall()")
check(install_call < 0 or "returnToCaller_" in on_enter[:install_call],
      "onEnter() must not reach enterInstall() when opened from the reader")
check("pendingTitle_" in on_enter, "onEnter() must open the title it was started with")

for b in bad:
    print(f"FAIL lookup-entry  {b}")
print(f"{'FAIL' if bad else 'ok  '}  wikipedia lookup-entry: {checks} checks, {len(bad)} failed")
sys.exit(1 if bad else 0)
