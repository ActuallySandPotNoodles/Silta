#pragma once
#include "stdafx.h"
#include <string>

namespace overlay {
	// ---- Mod identity (single source of truth for branding) ----
	// SILTA - Finnish for "bridge": Siltanen's field kit, bridging INFRA to your
	// survey tools. Bump kVersion per release.
	constexpr const char* kModName = "SILTA";
	constexpr const char* kVersion = "0.933";
	extern bool  watermark;         // small corner tag with name+version (menu only)
	extern int   watermarkCorner;   // 0 TL, 1 TR, 2 BL, 3 BR
	extern std::string watermarkText; // override text (empty = "SILTA v<ver>")
	extern bool  forceBackbuffer;   // EXPERIMENTAL: force back buffer as RT before draw
	extern bool  usePresentRender;  // EXPERIMENTAL: draw overlay in Present, not EndScene
	extern bool  inMenu;            // true while no map is loaded (main menu)

	enum class Corner { TopLeft, TopRight, BottomLeft, BottomRight };
	enum class Theme { Debug, Native };
	enum class CompleteStyle { Color, Check, Dim };

	struct OverlayLine_t {
		std::string name;
		std::string value;
		ImVec4 nameColor;
		ImVec4 valueColor;

		// blink data
		int blinksLeft;
		long long lastBlink;

		OverlayLine_t() = default;
		OverlayLine_t(std::string name, std::string value, const ImVec4& nameColor, const ImVec4& valueColor);
	};

	// Configurable style (populated from silta.ini at startup; these
	// are the defaults if the ini is missing or a key is absent).
	extern ImVec4 fontColor;       // normal counter text
	extern ImVec4 fontColorMax;    // counter text when a category is complete
	extern ImVec4 titleColor;      // map-name title + inventory labels
	extern float  backgroundAlpha; // overlay window background opacity 0..1
	extern int    margin;          // px gap from the screen edges
	extern Corner countersCorner;  // which corner the counters overlay anchors to
	extern Corner inventoryCorner; // which corner the inventory overlay anchors to

	extern bool shown;
	extern int  csSort;            // contact sheet: 0 name, 1 newest, 2 oldest
	extern bool countersEnabled;   // success-counters overlay
	extern bool inventoryEnabled;  // battery / OS-coin overlay

	// Per-counter-category style. Index order matches the displayed rows:
	// 0 Defects, 1 Corruption, 2 Repairs, 3 Geocaches, 4 Flow meters.
	// Configured from the [counters] section of the ini.
	constexpr int CategoryCount = 5;
	extern ImVec4 categoryColor[CategoryCount];   // base text colour per category
	extern bool   categoryVisible[CategoryCount]; // hide a row when false

	// Inventory (battery) overlay colours: 0 flashlight, 1 camera, 2 OS coins.
	extern ImVec4 inventoryColor[3];
	extern bool   invBatteryIcons;  // draw a battery glyph by flashlight/camera lines
	extern bool   invShowHeld;      // show the entity held with E (DT_INFRA_Player heldObject)
	extern bool   invShowFlashlightUpgrade; // show "Flashlight: upgraded" when applicable
	extern std::string invPickupWatch;  // name substring to tally as pickups (empty = off)
	extern std::string invPickupLabel;  // label for the pickup tally line
	extern bool   inventoryHiddenByMap; // current map is on [inventory] hidden_maps
	extern bool   countersFocusFade;    // counters rest transparent, wake on events
	extern bool   invFocusFade;         // same for the inventory overlay
	extern int    focusTransparency;    // idle transparency percent (90 = faint)
	extern float  focusSeconds;         // fully-visible time after an event
	extern float  focusRamp;            // fade in/out time between states
	extern bool   invCoinIcon;      // draw a coin glyph by OS coins (appears once > 0)
	extern bool   flashGauge;       // on-screen flashlight battery gauge while draining
	extern int    flashGaugeMax;    // battery value treated as 100%
	extern float  flashGaugeSeconds;// seconds the gauge lingers after draining stops
	extern float  flashGaugeFade;   // fade-out duration at the end of the linger
	extern bool   flashGaugeNumbers;// show the % (and spare count) text
	extern float flashUpgradedDays;   // cosmetic "days remaining" for the UPGRADED (infinite) flashlight (0 = off)
	extern int    flashGaugeSkin;   // 0 = instrument panel, 1 = subtle, 2 = custom
	extern ImVec4 gaugeCustom[6];   // custom-skin: bg, frame, label, fill hi/mid/low
	extern float  gaugeX;           // gauge position (-1 = auto bottom-center)
	extern float  gaugeY;           // gauge position (-1 = auto)
	void SaveGaugePos();            // writes gauge_x/gauge_y to silta.ini

	// Free-form notes window (resizable scratchpad for puzzles/secrets/ARGs).
	enum class NotesSkin { Ncg, Pad, Plain, Geolog, Custom };

	extern bool   notesEnabled;     // feature active (responds to its hotkey)
	extern bool   notesShown;       // window currently visible
	extern ImVec4 notesColor;       // notes text colour (plain style)
	extern NotesSkin notesSkin;     // visual skin for the notes window
	extern ImVec4 notesCustom[4];   // custom skin: paper, ink, rule, margin
	extern ImVec4 sketchPaper;      // sketchbook paper (window bg)
	extern ImVec4 sketchToolInk;    // sketchbook toolbar text
	extern ImVec4 sketchGrid;       // survey grid lines (alpha respected)
	extern ImVec4 sketchHead;       // letterhead / title-block ink
	extern bool   notesAutoOpen;    // open the notes window when the cursor is out
	extern float  notesFontScale;   // text scale for the notes window

	extern bool   hintsEnabled;     // show the hotkey tip bar when the cursor is out
	extern bool   tipFade;          // fade the tip bar out after a delay
	extern float  tipFadeSeconds;   // seconds visible before it fades

	extern bool   saveLayout;       // persist dragged window positions across runs
	extern bool   canonLabels;      // use the game's wording for counter rows
	extern bool   locationNames;    // title counters with the real location name

	extern bool   showProgress;     // progress popup (%/next achievement) on phone
	extern float  progressSeconds;  // seconds the progress popup stays

	enum class CalcMode { Basic, Scientific, Programmer, Text, Style };
	extern bool   calcEnabled;      // N.C.G. field calculator panel (incl. cipher tools)
	extern bool   calcShown;
	extern CalcMode calcMode;       // basic / scientific / programmer / text / style
	extern ImVec4 calcCustom[7];    // custom-skin colors: window, button, hover,
	                                // active, accent, display text, display bg
	void SaveCalcCustomColors();    // writes calcCustom + skin=custom to silta.ini
	extern ImVec4 tipKeyColor;      // hotkey tip bar: key column
	extern ImVec4 tipTextColor;     // hotkey tip bar: description column
	extern int    calcSkin;         // 0 = N.C.G. (default), 1 = Osmo Olut

	extern bool   endingEnabled;    // N.C.G. study / ending-outlook panel
	extern bool   endingShown;

	extern bool   contactEnabled;   // photo contact-sheet viewer
	extern bool   contactShown;

	// Sketchbook (NCG survey sketchpad): a paintable square canvas saved as PNG.
	extern bool   sketchEnabled;
	extern bool   sketchShown;
	extern int    sketchSize;       // square fallback (used when width/height unset)
	extern int    sketchW;          // canvas width  in pixels
	extern int    sketchH;          // canvas height in pixels
	extern bool   sketchSurvey;     // draw a survey grid + title block
	extern bool   sketchTransparent;// transparent page (trace over the game)
	extern bool   sketchSmooth;     // bilinear (soft) vs point (crisp) canvas display
	extern int    sketchDefaultBrush; // starting brush size
	extern float  sketchDefaultInk[3];// starting ink colour (RGB 0..1)

	// Shared survey identity (sketch title block + camera photo caption).
	extern std::string surveyorName; // e.g. "M. SILTANEN"
	extern std::string surveyDate;   // e.g. "08.08.2016"

	// Current map's friendly location name (set on map load when known).
	extern std::string locationName;

	// Brief on-screen toast (e.g. camera save confirmation). seconds <= 0 = off.
	void ShowToast(const std::string& text, float seconds);
	void ShowReport(const std::string& title, const std::string& body, float seconds);
	extern int debugReportKey;            // *** DEBUG ONLY *** key to force the report (0 = off)
	extern volatile bool forceReportRequested; // set by the debug key, serviced in EndScene
	void RunTweakKey(int vk);             // run a [tweaks] key-bind command (if any)
}

// Leveled logging into silta.log (defined in infra.cpp). LogV lines only appear
// with [log] verbose = true, which also timestamps + flushes every line.
void LogV(const std::string& msg);
void LogI(const std::string& msg);
void LogRaw(const std::string& msg);
void ClearSiltaLog();
void LogE(const std::string& msg);

namespace overlay {

	extern Theme         theme;          // visual style of the overlay windows
	extern CompleteStyle completeStyle;  // how a finished category is marked
	extern bool          showTotal;      // show overall completion line

	// Live total (overall completion across the map), maintained by counters.cpp.
	extern int totalCurrent;
	extern int totalMax;
	extern int catCurrent[CategoryCount]; // per-category progress (for the popup)
	extern int catMax[CategoryCount];
	extern int currentAct;                // 1..3, inferred from the map name

	// Runtime positioning state.
	extern bool locked;           // true = pinned to corners; false = draggable
	extern bool forceReposition;  // one-shot: snap back to corners next frame
	extern float borderColor[4];      // counters/inventory window border RGBA (alpha<0 = theme default)
	extern int   editAutoCloseSecs;   // auto re-lock edit mode after N idle seconds (0 = off)
	extern bool  autoAlignCorners;    // per-map: re-snap counters+inventory to their corners

	// A reload of the ini was requested via hotkey; serviced on the render thread.
	extern bool reloadRequested;

	// Hotkeys (Win32 virtual-key codes; 0 = disabled).
	struct Hotkeys {
		int reloadConfig;
		int toggleCounters;
		int toggleInventory;
		int cycleCountersCorner;
		int cycleInventoryCorner;
		int toggleLock;
		int resetPosition;
		int toggleNotes;
		int toggleSketch;
		int toggleCalculator;
		int toggleEnding;
		int toggleContact;
		int clearLog;
		int dumpHeld;   // log full identity of the held object (for [pickup_names])
	};
	extern Hotkeys hotkeys;

	extern OverlayLine_t title;
	extern std::vector<OverlayLine_t> lines;
	extern bool imGuiInitialized;
	extern int fontSize;

	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DispatchHotkey(int vk);    // run the action bound to a key (shared)
	void PollHotkeys();             // GetAsyncKeyState fallback (render thread)

	// ----- Speedrun timer / death counter -----
	extern bool srShowTimer;         // draw the on-screen run timer
	extern bool srCountDeaths;       // count deaths (needs srHealthOffset)
	extern unsigned int srHealthOffset; // player-struct offset of health (0 = off)
	extern bool srShowVelocity;      // draw hammer-units speed (needs srVelOffset)
	extern unsigned int srVelOffset;  // player-struct offset of m_vecVelocity (0 = off)
	extern bool srShowPos;           // draw player position (needs srPosOffset)
	extern bool srPosAxis;           // label position components X: Y: Z:
	extern unsigned int srPosOffset;  // player-struct offset of m_vecOrigin (0 = off)
	extern unsigned int srProbeOffset;// offset-finder: dump floats here to silta.log (0 = off)
	extern int srProbeCount;          // how many consecutive floats the probe dumps
	extern bool srScanOffsets;        // AUTO-FINDER: sweep the player struct for vel/pos
	void ResetOffsetScan();           // restart the auto-finder's tracking
	extern bool srDeathToast;         // pop a "Death #n" toast on death
	extern int  srPanelX, srPanelY;   // speedrun panel starting position (px)
	extern float srBgAlpha;           // speedrun panel background opacity
	extern float srScale;             // speedrun panel font/size scale
	extern bool srInReport;          // include time/deaths in the end-of-game report
	extern bool rpShowReading;       // include document-reading time in the report
	extern bool rpShowPickups;       // include pickup total + favorite in the report
	// Run-origin detection (report validity). Validity is persisted in Source
	// global-state (see counters::MarkRunOrigin), so it survives save/load.
	extern std::string startMapToken; // map that counts as "the beginning" (lowercased)
	extern bool runFromStart;         // true when the office-origin marker is present
	extern std::string runOriginMap;  // first gameplay map seen this session (lowercased)
	extern bool rpResetOnStart;       // reset session stats when the start map loads fresh
	void TickSpeedrun();            // per-frame accumulate + death detect (EndScene)
	void TickFlashlightBattery();  // per-frame flashlight-on accumulation
	void RenderSpeedrun();          // on-screen widget
	unsigned long long RunElapsedMs();
	int DeathCount();
	void ResetRun();
	extern bool useHotkeyPolling;   // EXPERIMENTAL: poll keys instead of window msgs
	void Render(HWND hWnd, LPDIRECT3DDEVICE9 pDevice);

	// ----- Binocular zoom overlay -----
	extern bool  binoEnabled;
	extern int   binoKey;          // activation vk (0x04 = middle mouse)
	extern bool  binoToggle;       // true = toggle, false = hold
	extern bool  binoHidePhone;    // hide the scope while the phone is out
	extern bool  binoHideFlash;    // hide the scope while the flashlight is on
	extern unsigned int binoCamOffset; // player offset of a camera-out flag (0 = off)
	extern int   binoOpacity;      // 0-255 darkness outside the circles
	extern int   binoSoftness;     // edge falloff in px
	extern float binoRadius;       // circle radius as fraction of screen height
	extern float binoSeparation;   // circle centre offset from mid as fraction of width
	extern float binoColor[3];     // mask colour (0..1 RGB), default black
	extern int   binoFadeMs;       // fade in/out duration
	extern bool  binoSound;        // play the phone deploy/holster jacket sound
	extern std::string binoSoundDeploy;   // e.g. Phone.Deploy
	extern std::string binoSoundHolster;  // e.g. Phone.Holster
	extern unsigned int binoPhoneWeaponOffset; // EHANDLE offset of the active weapon (0=off)
	extern std::string  binoPhoneClass;        // classname that counts as "phone out"
	extern bool         binoFovGate;           // drive the scope off the live FOV (zoom state)
	extern unsigned int binoFovOffset;         // player-struct offset of m_iFOV (int)
	extern int          binoFovZoomMax;        // FOV <= this counts as "zoomed" -> scope shown
	void BinocularTick(LPDIRECT3DDEVICE9 pDevice);
	void RenderBinocular(LPDIRECT3DDEVICE9 pDevice);
	void ReleaseBinocular();

	// ----- Max Payne-style silhouette health bar (debug: verifies health offset) -----
	extern bool showHealthBar;
	extern int  healthMax;
	extern bool healthFillGradient;  // vertical gradient inside the fill vs flat
	extern int  healthBarPx;         // silhouette height in px when the SVG loads
	extern bool healthNoBg;          // transparent window background
	extern bool healthShowHp;        // draw the "HP N" number
	extern float healthFillFull[3];  // fill colour at 100%% (0-1 rgb)
	extern float healthFillLow[3];   // fill colour at 0%%
	extern float healthEmpty[3];     // drained-silhouette colour
	void ReleaseHealthTexture();     // drop the cached silhouette (rebuilds on reload)
	void QueueEngineCommand(const std::string& cmd); // run a console command (defined in infra.cpp)

	// True if a typing window (notes/sketch) is open and ImGui wants to capture
	// this input message (so the host WndProc can withhold it from the game).
	bool WantsToCapture(UINT uMsg);
	void LoadNotes(); // load notes text from disk
	void SaveNotes(); // persist notes text to disk
	void SketchSaveOnUnload(); // save the sketch if it has unsaved content
}