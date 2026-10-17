#include <gtest/gtest.h>

#include "network/ProtectedPaths.h"

using protectedpaths::isPluginPath;
using protectedpaths::isRemovableOsJunk;
using protectedpaths::isSensitivePath;

TEST(ProtectedPaths, CredentialStoresAreSensitive) {
  EXPECT_TRUE(isSensitivePath("/.crosspoint/wifi.json"));
  EXPECT_TRUE(isSensitivePath("/.crosspoint/opds.json"));
  EXPECT_TRUE(isSensitivePath("/.crosspoint/koreader.json"));
  EXPECT_TRUE(isSensitivePath("/.crosspoint/wifi.json.tmp"));
  EXPECT_TRUE(isSensitivePath("/.CrossPoint/WIFI.json"));
}

TEST(ProtectedPaths, SpellingVariantsCannotDodgeTheMatch) {
  EXPECT_TRUE(isSensitivePath("/.crosspoint/./wifi.json"));
  EXPECT_TRUE(isSensitivePath("//.crosspoint//wifi.json"));
  EXPECT_TRUE(isSensitivePath("/.crosspoint/plugins/../wifi.json"));
  EXPECT_TRUE(isSensitivePath(".crosspoint/opds.json"));
}

TEST(ProtectedPaths, NamesSdFatWouldTrimOrAliasStillMatch) {
  EXPECT_TRUE(isSensitivePath("/.crosspoint/wifi.json."));
  EXPECT_TRUE(isSensitivePath("/.crosspoint./wifi.json"));
  EXPECT_TRUE(isSensitivePath("/ .crosspoint/ wifi.json"));
  EXPECT_TRUE(isSensitivePath("/.crosspoint ./opds.json"));
  EXPECT_TRUE(isSensitivePath("/CROSSP~1/wifi.json"));
  EXPECT_TRUE(isSensitivePath("/.crosspoint/WIFI~1.JSO"));
  EXPECT_FALSE(isSensitivePath("/Books/Photos/IMG~1.JPG"));  // too deep to reach a store
  EXPECT_FALSE(isSensitivePath("/Books/a~b.epub"));
}

TEST(ProtectedPaths, EverythingElseStaysReachable) {
  EXPECT_FALSE(isSensitivePath("/.crosspoint/settings.json"));
  EXPECT_FALSE(isSensitivePath("/.crosspoint/library.idx"));
  EXPECT_FALSE(isSensitivePath("/.crosspoint/month-wallpaper.json"));
  EXPECT_FALSE(isSensitivePath("/.fonts/Noto/Noto_14.cpfont"));
  EXPECT_FALSE(isSensitivePath("/Books/wifi.json"));
}

TEST(ProtectedPaths, PluginPathRules) {
  EXPECT_FALSE(isPluginPath("/.crosspoint/plugins/../wifi.json"));
  EXPECT_FALSE(isPluginPath("/.crosspoint/./koreader.json"));
  EXPECT_FALSE(isPluginPath("relative/path"));
  EXPECT_FALSE(isPluginPath("/"));
  EXPECT_TRUE(isPluginPath("/.crosspoint/month-wallpaper.json"));
  EXPECT_TRUE(isPluginPath("/Books/new.epub"));
}

TEST(OsJunk, MacMetadataAtAnyUserLevelIsRemovable) {
  EXPECT_TRUE(isRemovableOsJunk("/.Spotlight-V100"));
  EXPECT_TRUE(isRemovableOsJunk("/.Trashes"));
  EXPECT_TRUE(isRemovableOsJunk("/.fseventsd"));
  EXPECT_TRUE(isRemovableOsJunk("/.TemporaryItems"));
  EXPECT_TRUE(isRemovableOsJunk("/.DS_Store"));
  EXPECT_TRUE(isRemovableOsJunk("/._study"));
  EXPECT_TRUE(isRemovableOsJunk("/Books/._novel.epub"));
  EXPECT_TRUE(isRemovableOsJunk("/Books/.DS_Store"));
}

TEST(OsJunk, NothingInsideAProtectedFolder) {
  EXPECT_FALSE(isRemovableOsJunk("/.crosspoint/.DS_Store"));
  EXPECT_FALSE(isRemovableOsJunk("/.fonts/._Noto"));
  EXPECT_FALSE(isRemovableOsJunk("/.crosspoint/._wifi.json"));
  EXPECT_FALSE(isRemovableOsJunk("/.Spotlight-V100/._x"));
}

TEST(OsJunk, RealDataIsNotJunk) {
  EXPECT_FALSE(isRemovableOsJunk("/.crosspoint"));
  EXPECT_FALSE(isRemovableOsJunk("/.fonts"));
  EXPECT_FALSE(isRemovableOsJunk("/.dictionaries"));
  EXPECT_FALSE(isRemovableOsJunk("/.sleep"));
  EXPECT_FALSE(isRemovableOsJunk("/Books/novel.epub"));
  EXPECT_FALSE(isRemovableOsJunk("/._"));
  EXPECT_FALSE(isRemovableOsJunk("/"));
  EXPECT_FALSE(isRemovableOsJunk(""));
  EXPECT_FALSE(isRemovableOsJunk("/.spotlight-v100"));  // macOS writes one exact spelling
}
