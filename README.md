# SILTA
**Infra tool-set mod.**

```
                                                                                             ███
                                                                               ███         ██████
                                                                              █████        ███████
                                                                              ██████        ██████
                                                                                            ██████                          █████████
                                                                                  █████     ██████                          ██████████████
                                                                         ██████   ██████     ██████                         █████████████████
                                                                     ███████████   █████     ██████                               ██████████████
                                                                 ███████████████   ██████     █████                                     ████████████
                                                               ███████████          █████     █████                                     █████████████
                                                             ████████               █████     █████      ██████████████████             █████   ██████
                                                           ███████                  █████      ████████████████████████████████         █████    ██████
                                                          ██████                    █████      ████████████████        ███████████    ███████      ███████
                                                          ████████████              █████       ████████                   ███████   ███████        ██████
                                                          █████████████████         █████                                     ███    ██████           ██
                                                            █████████████████      ██████                                             ███
                                                                      ███████        ██                                                        █████████████
                                                                        █████                                                                  ████████████████████
                                                                      ██████                                                                   ██████████████████████
                                                                     ██████                                                                                     ██████
                                                                   ███████                                                                             ████       █████
                                                                 ███████                                                                              ███████     █████
                                                              █████████                                                                                ███████    █████
                                                           ██████████                                                                                    ███████  █████
                                                         ██████████                                                                                       ███████ █████
                                                     ███████████                                                                                            ███   █████
                                                  ███████████                                                                                                     █████
                                              ███████████                                                                                                         █████
                                         █████████████                                                                                                            █████
                                      ████████████                                                                                                                █████
                                 █████████████                                                                                                                     ████
                             █████████████
                         ██████████████
                     █████████████
                ██████████████
           █████████████
       █████████████
 ██████████████
██████████
 █████

                                                   ___  __          ___  __   __           __   ___ ___           __   __
                                           | |\ | |__  |__)  /\      |  /  \ /  \ |    __ /__` |__   |      |\/| /  \ |  \
                                           | | \| |    |  \ /~~\     |  \__/ \__/ |___    .__/ |___  |      |  | \__/ |__/ .
```

A utility mod for [INFRA](https://store.steampowered.com/app/251110/INFRA/), implemented as a
Source engine plugin. This is a **fork** of Floorb's Infra Mod that merges three
community projects into a single DLL and adds extra configuration:

| Source | What it contributed |
| --- | --- |
| [Neetpone's InfraMod](https://github.com/Neetpone/InfraMod) | Base code: success counters, **functional camera (photo)**, **inventory (battery) overlay** |
| [INFRA Success Counters](https://www.moddb.com/mods/infra-success-counters) by *fehc* | The original success-counters concept this is based on |
| [INFRA Success Counters FIX](https://www.speedrun.com/infra/resources/mjtgi) by *MrMagnetix* | Corrected per-map maximum counts (baked in + drop-in support) |

## Why a merge was needed
The original **Success Counters** mod (fehc, ModDB) is built on *x360ce* and ships as a proxy
`xinput1_3.dll`. Neetpone's mod is **also** a proxy `xinput1_3.dll`. A game folder can only
contain **one** `xinput1_3.dll`, so the two mods **cannot be installed at the same time** — they
directly conflict.

This merged build resolves that by being the single DLL that does everything: it already contains
the success counters, so you do **not** install fehc's original DLL alongside it. Drop this in and
remove any other `xinput1_3.dll` from the INFRA folder.

## Corrected map data (the FIX)
MrMagnetix's corrected maximum counts are **baked into the default data** (see the diff in
`MERGE_NOTES.md`), so the counters are right out of the box — including the `c6_m6_central`
defect/corruption swap and several wrong photo totals (bunker, stormdrain, cistern, npp, …).

In addition, on startup the mod looks for a **`mapdata.txt`** next to `infra.exe`. If present and
valid, it is used instead of the embedded data. This is the exact filename/location MrMagnetix's FIX
uses, so any future updated `mapdata.txt` is a **drop-in** — and you can tweak counts without
rebuilding the DLL. A ready-to-use copy is included in this repo.

## Installation
1. Locate your INFRA installation folder (the folder with `infra.exe`) and go one level deeper into
   the `infra/` folder under it — the one that contains `gameinfo.txt`.
2. Place the plugin files. With the included `.vdf` (whose `"file"` value is the bare
   `silta`), Source looks for the DLL at the `infra/` root, so you have:
   - `infra/silta.dll`
   - `infra/addons/silta.vdf`  *(included in this repo under `addons/`)*

   The `.vdf` (which lives in `addons/`) is what tells Source to load the plugin; its `"file"` path is
   resolved relative to the `infra/` mod folder. If you'd rather keep the DLL inside `addons/`, set the
   `"file"` value back to `addons/silta`.
3. **Remove any other `xinput1_3.dll`** (e.g. fehc's original Success Counters DLL) from the INFRA
   folder so they don't conflict.
4. *(Optional)* copy the included `mapdata.txt` next to `infra.exe`. Not required — the corrected
   values are already built in — but handy if you want to edit counts or apply a newer FIX.
5. Launch the game. Press **Insert** to toggle the overlays.

## Features

Everything below is configurable (or fully disableable) in `silta.ini`.

**Survey & progress**
- **Success counters** — per-map `current / maximum` for photos, corruption, repairs,
  geocaches and flow meters, with MrMagnetix's corrected map data baked in (+ drop-in
  `mapdata.txt` support), per-row colours, canon labels, completion styles and a total line.
- **Study outlook** (F1) — live photo/corruption tallies against the game's real ending
  thresholds (>=50% study, 90% Raven Research), with an "on track for" verdict.
- **Progress popup** — per-category % and next achievement whenever the cursor is out.
- **End-of-game survey report** — on the ending map, writes `NCG_Survey_Report.txt` next to
  `infra.exe` and shows a themed N.C.G. report card with the verdict.

**Camera (IMAGE-IN Crystal-shot)**
- Photos save hitch-free to `DCIM\<Site>\DSC#####.jpg` (per-site subfolders, digital-camera
  numbering; encoding runs on a background thread).
- **Lore-accurate 480p** by default: the Crystal-shot's box art says "480p ready", so photos
  save at 640x480 with a real-camera center-crop. Fully configurable (`photo_width/height`,
  `aspect = crop|stretch`; 0x0 = native full resolution).
- **Burn-in caption** — N.C.G. survey strip (location / date / surveyor), colours configurable,
  with `band` / `plain` / `minimal` styles and an optional top-edge position.
- **In-lore date stamp** — the caption and EXIF timestamp follow INFRA's canon timeline: the
  game starts the morning of 08.08.2016 and crosses midnight into 09.08.2016 at `officeblackout`,
  so a reactor photo reads 09.08 (`[survey] date_auto`, on by default).
- **EXIF metadata** — camera identity (IMAGE-IN / Crystal-shot by default, configurable via
  `exif_make/model/software`), per-chapter in-world timestamp (+20 min per mission),
  site description, N.C.G. asset tag, and **player coordinates** (comment + GPS tags anchored to
  Stolland's canonical Baltic position) out of the box (origin member at 0x2C baked in; `calibrate` remains as a recovery tool).
- **Radiation corruption** — a photo taken in radiation has a dose-scaled chance of its saved
  **metadata** being damaged (garbled EXIF text, an impossible/wiped timestamp, scrambled GPS),
  and very rarely the burned-in date glitches. The image and file always open — only the
  recorded data is hit (`radiation_exif_corruption`).
- **Survey log** — every shot appended to `DCIM\survey_log.txt` (file, site, time, coords).
- **Contact sheet** (F5) — thumbnail grid of every photo on the card (previous sessions
  included), lazy-loaded with zero stutter; click to isolate a shot with zoom/pan; Rescan.

**Radiation photography**
- **Radiation grain** — photos taken in a radioactive area get sensor-damage speckle scaled by
  dose: colored "sensor confetti" (RGB hot pixels + white/cyan/magenta) by default, plus `bw`
  and `white` modes and speckle / clusters / streaks patterns (`photo_radiation_noise`).
- **Radiation zones** (`[radiation_zones]`) — hand-placed spheres or boxes per map, with optional
  per-zone strength, a time-**ramp** that builds exposure like the game's dose, and a **trigger**
  sphere that keeps a zone asleep until you reach it. The reactor and the bunker (lab / demon
  core / uranium hangar) ship pre-surveyed.
- **Ambient radiation** (`[radiation_ambient]`) — a map-wide random baseline outside zones, for
  maps that are hot all over (roof, wasteland). On maps that use the game's geiger (e.g.
  `infra_ee_wasteland`) the grain triggers automatically from the live reading.
- Collect coordinates for your own maps with the `mark_position` hotkey (writes ZONE-MARK lines
  to `silta.log`); both config sections are heavily commented in `silta.ini`.

**Field kit**
- **N.C.G. Field Calculator** (F2) — Basic / Scientific / Programmer (bases + bitwise) /
  Text (hex<->ASCII, binary, Caesar cipher) modes plus structural-analyst helpers (unit
  conversions, flow Q=V·A, stress σ=F/A, slope). Skins: N.C.G., "Osmo Olut", or **custom** —
  a live in-game Style tab with color pickers and a save-to-ini button.
- **NCG Sketchbook** (F3) — paintable survey sheet (letterhead, grid, title block), square or
  rectangular canvas, trace mode over the game scene, PNG export to `sketches\`. Eight brush
  types (Pen / Marker / Spray / Chalk / Square / Pencil / Highlighter / Splatter).
- **Notes scratchpad** (F4) — persistent notes in several paper styles.

**HUD**
- **Inventory overlay** — flashlight/camera batteries with trailing icons, OS coins (coin icon
  appears once collected), hidden automatically on maps that don't use it (`hidden_maps`).
- **Flashlight gauge** — real-time battery charge (segments + %) while the flashlight drains,
  with the spare count; reads the live charge member found via SILTA's built-in autoscanner.
  Shows on toggle/drain then fades; the **upgraded** flashlight instead pins a fixed cosmetic
  "days left" readout while on. Three skins (default / subtle / custom colors), an optional
  timer on the subtle skin, numbers toggle.
- **Hotkey tip bar** — sorted F1..F12; **draggable overlays** (F11 unlock) with a curated
  default layout and positions saved across sessions; version watermark in the main menu.
- **Health bar** — a Max-Payne-style Mark Siltanen silhouette that fills bottom-up as a mask,
  colouring low->full via configurable RGB (defaults to INFRA's bioluminescent-mushroom green),
  with an optional liquid gradient, a no-background mode and a toggleable HP number. The
  silhouette is a vector (NanoSVG) baked into the DLL, so it stays crisp at any `bar_px` size.
  Drop **any** `.svg` next to the game exe to swap the figure (name doesn't matter; only the
  shape is used, the bar tints it); with none present the embedded Mark vector is used.

## Hotkeys (defaults, all rebindable)

| Key | Action |
| --- | --- |
| Insert | Show / hide all overlays |
| F1 | Study outlook | 
| F2 | Field calculator |
| F3 | Sketchbook |
| F4 | Notes scratchpad |
| F5 | Contact sheet |
| F6 | Reload `silta.ini` live |
| F7 / F8 | Toggle counters / inventory |
| F9 / F10 | Cycle counters / inventory corner |
| F11 | Unlock (drag overlays) / lock |
| F12 | Reset positions |

Every key is rebindable. There's also a `mark_position` hotkey (unbound by default) for
surveying radiation-zone coordinates.

### Rebinding (and why binds sometimes didn't stick)

Editing key **names** in `silta.ini` works on a US layout but not reliably elsewhere: Windows
key codes are layout-dependent, so on ABNT / German / AZERTY / Nordic keyboards a typed name
could map to the wrong physical key — which is why the F-key **defaults** worked for everyone
but **custom** binds sometimes didn't. The fix is the **binding wizard**: a **"SILTA - Bind
keys"** button on the main menu (and, optionally, the in-game pause menu) opens a
press-to-capture rebinder — click Rebind, press the key you want, and it records the *real* key
your layout produced. It works on any layout. Binds save to **`silta_binds.ini`**, which
overrides `[hotkeys]`. Toggle the button per context with `bind_wizard_button_menu` (on) and
`bind_wizard_button_ingame` (off); position it with `bind_wizard_button_x/y`.

## Configuration

On first run the mod writes a fully-commented **`silta.ini`** next to `infra.exe` — that file
(and the `silta.ini.example` in this repo) is the authoritative reference for every option.
Delete `silta.ini` to regenerate documented defaults; press **F6** in-game to reload edits live.
Window positions persist in `silta_layout.ini` (delete it once to re-seed the curated default
layout); wizard rebinds live in `silta_binds.ini` (delete it to fall back to `[hotkeys]`). Set `[log] verbose = true` when hunting a bug — timestamps + immediate flush + crash
marker in `silta.log`.

## Version history

- **v0.9 (pre-release)** — everything above: counters + FIX data, hitch-free Crystal-shot
  camera (480p lore default, EXIF with in-world time / coordinates / Stolland GPS, survey
  log, per-site folders), contact sheet, study outlook, end-of-game report with the SILTA
  banner, field calculator (4 modes + skins + live Style tab), sketchbook, notes,
  real-time flashlight gauge (draggable), per-window layouts, custom skins, verbose
  logging with crash marker, menu watermark. First public release will be v1.0.
- **v0.9x (pre-release, ongoing)** — radiation photography (grain + zones/ambient + the reactor
  and bunker pre-surveyed + EXIF corruption), INFRA-canon date stamping, eight sketch brushes,
  burn-in caption styles, flashlight-gauge fixes with the upgraded "days left" readout, and a
  keyboard-layout-proof **binding wizard**. Plus a hotkey audit (typing guard, `0` unbinds,
  duplicate-bind warnings, rebindable main key).

## Known limitations

- **Photo capture can fail after a save-load, death, or alt-tab, on some maps.** The camera
  reads the game's freeze-frame texture; when the game rebuilds its texture dictionary
  (which those events trigger) the freeze-frame can be stale or, on certain maps, have no
  usable backing texture at all. SILTA detects this and **skips the shot cleanly (no crash,
  no corrupt file)** with an on-screen notice. A reload or moving to a different spot often
  restores it; a few maps don't recover until a map transition. This is a limitation of the
  game's render-texture lifecycle, not a bug introduced by the mod — it affects the capture
  the same way the original camera code did.

- **Custom bind names on non-US keyboards** — typing a key *name* in `[hotkeys]` may not resolve
  correctly on localized layouts; use the on-screen **binding wizard** (press-to-capture), which
  is layout-proof.
- **Radiation dose isn't read from the game** — INFRA keeps the reactor's rising dose in map
  logic, not on the player, so the photo effect is driven by hand-placed zones / the geiger /
  a map-wide baseline rather than the exact in-game value. The wizard also shows layout-specific
  keys (e.g. an ABNT `Ã`) by their hex code rather than a pretty name, though they bind fine.
