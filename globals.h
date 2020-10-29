#ifndef GLOBALS_H
#define GLOBALS_H

#include <qnamespace.h>

constexpr int PAGE_WIDTH = 300;
constexpr int ROLE_ID = Qt::UserRole;
constexpr int ROLE_ELEMENT = Qt::UserRole + 1;
constexpr int ALIGN_LEFT = 0;
constexpr int ALIGN_CENTRE = 1;
constexpr int ALIGN_RIGHT = 2;
constexpr const char * TYPE_WEBPAGE = "Webpage";
constexpr const char * TYPE_GIF = "Gif";
constexpr const char * TYPE_TEXT = "Text";
constexpr const char * EVENT_DEFAULT = "DEFAULT";
constexpr int EVENT_ACTIVE_ROLE = Qt::UserRole + 1;
constexpr int GIF_ANIMATION = 0;
constexpr int GIF_MOUSE_OVER_ANIMATION = 1;
constexpr int GIF_STILL_IMAGE = -1;
constexpr int GIF_SIMULATE_BUTTON = -2;

enum DefinitionData {
    DefType,
    DefId,
    DefName
};

enum WebpageData {
    WebEvent,
    WebTitle,
    WebUsername,
    WebHeight,
    WebMusic,
    WebBGImage,
    WebMouseFX,
    WebBGColor,
    WebDescriptionAndTags,
    WebPageStyle,
    WebUserHOME,
    WebOnLoadScript
};

enum TextData {
    TextEvent,
    TextX,
    TextY,
    TextWidth,
    TextCaseTag,
    TextString,
    TextColor,
    TextFont,
    TextStyle,
    TextAlign,
    TextLinkOrScript,
    TextLawBroken,
    TextAnimation,
    TextAnimSpeed,
    TextColorFadeTo,
    TextColorFadeSpeed,
    TextNoContent
};

enum GifData {
    GifEvent,
    GifX,
    GifY,
    GifHSL,
    GifCaseTag,
    GifNameOf,
    GifScale,
    GifRotation,
    GifMirror,
    GifFlip,
    GifLinkOrScript,
    GifLawBroken,
    GifAnimFlipX,
    GifAnimFlipY,
    GifAnimFade,
    GifAnimTurn,
    GifAnimTurnSpeed,
    GifFPS,
    GifOffset,
    GifSync,
    GifAnimMouseOver
};

#endif // GLOBALS_H
