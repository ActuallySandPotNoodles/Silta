#include "inventory.h"

#include "infra.h"
#include "shlwapi.h"
#include "overlay.h"
#include <vector>
#include <string>
#include <map>

using infra::Engine;
using infra::CMathCounter;
using infra::CHud;
using infra::CHudElement;
using infra::CFlashlightAmmo;
using infra::CCameraAmmo;
using infra::CINFRA_Player;

float *mod::inventory::osCoinsCounter = nullptr;
int* mod::inventory::flashlightBatteriesCounter = nullptr;
int* mod::inventory::cameraBatteriesCounter = nullptr;
int* mod::inventory::flashlightChargeCounter = nullptr;
int  mod::inventory::flashlightChargeOffset = 0;
bool mod::inventory::chargeAutoscan = false;
int  mod::inventory::chargeFoundOffset = 0;

// ---- Flashlight-charge autoscan ----
// The game stores the live charge as an int in percent-units (sk_ cvars: 1000
// max, -1 every 2 s while the flashlight is on), somewhere in the player entity.
// Instead of hunting it with Cheat Engine, sample the whole struct every 2.5 s
// and keep only slots whose value sits in 0..1001 and dropped by 1..6 since the
// previous sample. With the flashlight on, the real slot survives every window;
// timers and other counters die off. After 4 consecutive windows with survivors,
// a unique survivor is adopted live and reported (log + on-screen toast).
namespace {
	const int kScanInts = 0x1858 / 4;
	std::vector<int> s_scanPrev;
	std::vector<unsigned char> s_scanAlive;
	int s_scanWindows = 0;
	unsigned long long s_scanLastMs = 0;

	void ScanReset() {
		s_scanPrev.clear();
		s_scanAlive.clear();
		s_scanWindows = 0;
	}
}

void mod::inventory::ChargeAutoscanTick() {
	if (!chargeAutoscan || flashlightChargeOffset > 0 || chargeFoundOffset > 0) return;

	const unsigned long long now = GetTickCount64();
	if (now - s_scanLastMs < 2500) return;
	s_scanLastMs = now;

	CINFRA_Player* player = reinterpret_cast<CINFRA_Player*>(
		Engine()->CGlobalEntityList__FindEntityByName(nullptr, "!player"));
	if (player == nullptr) { ScanReset(); return; }
	const int* cur = reinterpret_cast<const int*>(player);

	if (s_scanPrev.size() != static_cast<size_t>(kScanInts)) {
		s_scanPrev.assign(cur, cur + kScanInts);
		s_scanAlive.assign(kScanInts, 1);
		s_scanWindows = 0;
		LogV("autoscan: baseline snapshot taken (turn the flashlight ON and keep it on)");
		return;
	}

	int aliveCount = 0, lastAlive = -1;
	for (int i = 0; i < kScanInts; ++i) {
		if (!s_scanAlive[i]) continue;
		const int v = cur[i];
		const int delta = s_scanPrev[i] - v; // positive while draining
		if (v >= 0 && v <= 1001 && delta >= 1 && delta <= 6) {
			++aliveCount; lastAlive = i;
		} else {
			s_scanAlive[i] = 0;
		}
	}
	for (int i = 0; i < kScanInts; ++i) s_scanPrev[i] = cur[i];

	if (aliveCount == 0) {
		// Flashlight off / wrong window - start over silently.
		ScanReset();
		return;
	}
	++s_scanWindows;
	LogV("autoscan: window " + std::to_string(s_scanWindows) + ", " + std::to_string(aliveCount) + " candidate(s)");

	if (s_scanWindows >= 4) {
		if (aliveCount == 1) {
			chargeFoundOffset = lastAlive * 4;
			flashlightChargeCounter = reinterpret_cast<int*>(
				reinterpret_cast<unsigned char*>(player) + chargeFoundOffset);
			char msg[160];
			sprintf_s(msg, sizeof(msg),
				"autoscan: FLASHLIGHT CHARGE FOUND at offset %d (0x%X), value %d - set [inventory] flashlight_charge_offset = %d",
				chargeFoundOffset, chargeFoundOffset, cur[lastAlive], chargeFoundOffset);
			LogI(msg);
			overlay::ShowToast(std::string("SILTA: charge found, offset ") + std::to_string(chargeFoundOffset)
				+ " - see silta.log", 6.0f);
		} else if (aliveCount <= 4) {
			// Nearly there - list the finalists so the log is useful either way.
			std::string finalists = "autoscan: finalists:";
			for (int i = 0; i < kScanInts; ++i) {
				if (s_scanAlive[i]) finalists += " off=" + std::to_string(i * 4) + " val=" + std::to_string(cur[i]) + ";";
			}
			LogI(finalists);
		}
	}
}

static bool FindCameraAndFlashlightCounters(int**ppFlashlightCount, int**ppCameraBatteriesCount) {
	CINFRA_Player* player = reinterpret_cast<CINFRA_Player*>(
		Engine()->CGlobalEntityList__FindEntityByName(nullptr, "!player")
	);

	if (player == nullptr) {
		return false;
	}

	*ppFlashlightCount = &(player->m_nFlashlightBatteries);
	*ppCameraBatteriesCount = &(player->m_nCameraBatteries);

	// Live flashlight charge. Default (offset 0) uses the struct member at 0x184C
	// (found via SILTA autoscan, sits right before the battery counters). A
	// positive offset overrides it (future game patches); -1 disables entirely.
	const int off = mod::inventory::flashlightChargeOffset;
	if (off == 0) {
		mod::inventory::flashlightChargeCounter = &(player->m_nFlashlightCharge);
	} else if (off > 0 && off <= 0x1854) {
		mod::inventory::flashlightChargeCounter =
			reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(player) + off);
	}

	return false;
}

static std::string g_LastMapName;

void mod::inventory::RetryTick() {
	if (flashlightBatteriesCounter != nullptr) return; // resolved
	if (g_LastMapName.empty()) return;                 // no map loaded yet

	static unsigned long long lastMs = 0;
	const unsigned long long now = GetTickCount64();
	if (now - lastMs < 1000) return;
	lastMs = now;

	FindCameraAndFlashlightCounters(&flashlightBatteriesCounter, &cameraBatteriesCounter);
	if (flashlightBatteriesCounter != nullptr) {
		LogI("inventory: counters resolved on retry (player spawned after map init)");
		// Coins live on the tenements map only; resolve them here too.
		if (StrStrA(g_LastMapName.c_str(), "tenements") != nullptr) {
			CMathCounter* mathCounter = reinterpret_cast<CMathCounter*>(
				Engine()->CGlobalEntityList__FindEntityByName(nullptr, "Opensewer_coins_counter"));
			if (mathCounter != nullptr) osCoinsCounter = &mathCounter->m_CounterValue;
		}
	}
}

void mod::inventory::MapLoaded(const char *name) {
	g_LastMapName = name ? name : "";
	osCoinsCounter = nullptr;
	flashlightBatteriesCounter = nullptr;
	cameraBatteriesCounter = nullptr;
	flashlightChargeCounter = nullptr;
	chargeFoundOffset = 0;
	ScanReset();

	FindCameraAndFlashlightCounters(&flashlightBatteriesCounter, &cameraBatteriesCounter);

	if (StrStrA(name, "tenements") != nullptr) {
		CMathCounter* mathCounter = reinterpret_cast<CMathCounter*>(
			Engine()->CGlobalEntityList__FindEntityByName(nullptr, "Opensewer_coins_counter")
		);

		if (mathCounter != nullptr) {
			osCoinsCounter = &mathCounter->m_CounterValue;
		}
	}
}

// ----- Novelty pickup counter + status lines -----
namespace {
	int g_PickupCount = 0;
	unsigned int g_LastHeldIdx = 0xFFFFFFFF;
	int g_LastFlashBatt = -1;
	int g_LastCamBatt = -1;
	int g_LastCoins = -1;
	unsigned int g_LastDocIdx = 0xFFFFFFFF;
	std::map<std::string, std::string> g_PickupNames;   // lowercased key -> friendly

	// End-report stats (accumulated across the session, like the run timer).
	int g_TotalPickups = 0;                             // every distinct held grab
	std::map<std::string, int> g_PickupTally;           // key -> times grabbed
	unsigned long long g_ReadingElapsedMs = 0;          // time with a document open
	unsigned long long g_ReadingLastTick = 0;

	std::string ToLowerStr(const std::string& s) {
		std::string r; r.reserve(s.size());
		for (size_t i = 0; i < s.size(); ++i) r += static_cast<char>(tolower(static_cast<unsigned char>(s[i])));
		return r;
	}

	bool ContainsCI(const std::string& hay, const std::string& needle) {
		if (needle.empty() || hay.empty()) return false;
		std::string h, n;
		for (size_t i = 0; i < hay.size(); ++i) h += static_cast<char>(tolower(static_cast<unsigned char>(hay[i])));
		for (size_t i = 0; i < needle.size(); ++i) n += static_cast<char>(tolower(static_cast<unsigned char>(needle[i])));
		return h.find(n) != std::string::npos;
	}
}

void mod::inventory::PickupTick() {
	// 1) Held objects (props grabbed with E): log every change with the entity's
	//    name, model, and handle index; map to a friendly name if configured.
	unsigned int idx = 0xFFFFFFFF;
	std::string model, cls;
	std::string name = Engine()->GetHeldObjectInfo(idx, model, cls);
	if (idx != g_LastHeldIdx) {                 // handle changed => a different object now held
		g_LastHeldIdx = idx;
		if (idx != 0xFFFFFFFF) {                // grabbed something (0xFFFFFFFF = empty hand)
			std::string friendly = mod::inventory::LookupPickupName(name);
			if (friendly.empty()) friendly = mod::inventory::LookupPickupName(model);
			char b[288];
			if (!friendly.empty()) {
				sprintf_s(b, sizeof(b), "pickup(held): %s  (%s%s%s%s%s) [ent %u]", friendly.c_str(),
					name.empty() ? "?" : name.c_str(),
					(!name.empty() && !model.empty()) ? " / " : "",
					model.empty() ? "" : model.c_str(),
					cls.empty() ? "" : " : ",
					cls.empty() ? "" : cls.c_str(), idx & 0xFFFF);
			} else {
				sprintf_s(b, sizeof(b), "pickup(held): %s%s%s%s%s [ent %u : ser %u]",
					name.empty() ? "<unnamed>" : name.c_str(),
					cls.empty() ? "" : "  class=", cls.c_str(),
					model.empty() ? "" : "  model=", model.c_str(), idx & 0xFFFF, idx >> 16);
			}
			LogV(b);
			const std::string& watch = overlay::invPickupWatch;
			if (!watch.empty() && (ContainsCI(name, watch) || ContainsCI(model, watch) || ContainsCI(friendly, watch))) {
				g_PickupCount++;
			}
			// End-report tally: count every grab; remember the most-grabbed key
			// (prefer friendly label, then targetname, then model basename).
			g_TotalPickups++;
			std::string key = !friendly.empty() ? friendly
				: (!name.empty() ? name : (!model.empty() ? model : std::string("<unnamed>")));
			g_PickupTally[key]++;
		}
	}

	// Document being read (currentDocument @ 0x1840), resolved to a name.
	{
		unsigned int docIdx = 0xFFFFFFFF;
		std::string docName = Engine()->GetDocumentInfo(docIdx);
		if (docIdx != g_LastDocIdx) {
			g_LastDocIdx = docIdx;
			if (docIdx != 0xFFFFFFFF) {
				char b[160];
				sprintf_s(b, sizeof(b), "document: reading %s [ent %u]",
					docName.empty() ? "<unnamed>" : docName.c_str(), docIdx & 0xFFFF);
				LogV(b);
			} else {
				LogV("document: closed");
			}
		}
		// Accumulate reading/analyzing time (wall clock with a document open),
		// clamped like the run timer so alt-tab / load gaps don't inflate it.
		const unsigned long long nowMs = GetTickCount64();
		if (g_ReadingLastTick == 0) g_ReadingLastTick = nowMs;
		unsigned long long rdt = nowMs - g_ReadingLastTick;
		g_ReadingLastTick = nowMs;
		if (rdt > 1000) rdt = 0;
		if (docIdx != 0xFFFFFFFF) g_ReadingElapsedMs += rdt;
	}

	// 2) Consumable pickups (batteries / coins): SILTA already tracks these counts,
	//    so log when one goes UP. No per-entity id (the pickup entity is consumed),
	//    but it tells you WHAT was picked up and the new total. Values above a sane
	//    ceiling are garbage read before the player spawns (the counter address
	//    isn't valid yet) - treated as "not ready" so they don't log bogus jumps.
	const int kMaxPlausibleCount = 100000;
	auto watchCount = [kMaxPlausibleCount](int* ptr, int& last, const char* label) {
		if (ptr == nullptr) { last = -1; return; }
		const int v = *ptr;
		if (v < 0 || v > kMaxPlausibleCount) { last = -1; return; } // pre-spawn garbage
		if (last >= 0 && v > last) {
			char b[96];
			sprintf_s(b, sizeof(b), "pickup(count): %s +%d (now %d)", label, v - last, v);
			LogV(b);
		}
		last = v;
	};
	watchCount(mod::inventory::flashlightBatteriesCounter, g_LastFlashBatt, "flashlight battery");
	watchCount(mod::inventory::cameraBatteriesCounter, g_LastCamBatt, "camera battery");
	if (mod::inventory::osCoinsCounter != nullptr) {
		const int cv = static_cast<int>(*mod::inventory::osCoinsCounter);
		if (cv < 0 || cv > kMaxPlausibleCount) {
			g_LastCoins = -1; // garbage before spawn
		} else {
			if (g_LastCoins >= 0 && cv > g_LastCoins) {
				char b[96];
				sprintf_s(b, sizeof(b), "pickup(count): OS coin +%d (now %d)", cv - g_LastCoins, cv);
				LogV(b);
			}
			g_LastCoins = cv;
		}
	} else {
		g_LastCoins = -1;
	}
}

int  mod::inventory::PickupCount() { return g_PickupCount; }
void mod::inventory::ResetPickups() {
	g_PickupCount = 0; g_LastHeldIdx = 0xFFFFFFFF;
	g_TotalPickups = 0; g_PickupTally.clear();
	g_ReadingElapsedMs = 0; g_ReadingLastTick = 0; g_LastDocIdx = 0xFFFFFFFF;
}

unsigned long long mod::inventory::ReadingElapsedMs() { return g_ReadingElapsedMs; }
int mod::inventory::TotalPickups() { return g_TotalPickups; }

// Most-grabbed key + its count (0 / "" until something is picked up).
std::string mod::inventory::FavoritePickup(int& outCount) {
	outCount = 0;
	const std::string* best = nullptr;
	for (std::map<std::string, int>::const_iterator it = g_PickupTally.begin(); it != g_PickupTally.end(); ++it) {
		if (it->second > outCount) { outCount = it->second; best = &it->first; }
	}
	return best ? *best : std::string();
}

// On-demand full identity of the currently-held object (class + targetname +
// model + handle) -> silta.log (always, not verbose-gated) + a toast.
void mod::inventory::DumpHeldObject() {
	unsigned int idx = 0xFFFFFFFF;
	std::string model, cls;
	std::string name = Engine()->GetHeldObjectInfo(idx, model, cls);
	if (idx == 0xFFFFFFFF) {
		LogRaw("dump-held: nothing held (grab an object with E first)");
		overlay::ShowToast("Dump held: nothing in hand", 2.5f);
		return;
	}
	std::string friendly = mod::inventory::LookupPickupName(name);
	if (friendly.empty()) friendly = mod::inventory::LookupPickupName(model);
	char b[320];
	sprintf_s(b, sizeof(b), "dump-held: class=%s  name=%s  model=%s  friendly=%s  [ent %u : ser %u]",
		cls.empty() ? "?" : cls.c_str(),
		name.empty() ? "?" : name.c_str(),
		model.empty() ? "?" : model.c_str(),
		friendly.empty() ? "-" : friendly.c_str(), idx & 0xFFFF, idx >> 16);
	LogRaw(b);
	char t[160];
	sprintf_s(t, sizeof(t), "Dumped held: %s -> silta.log",
		!name.empty() ? name.c_str() : (!model.empty() ? model.c_str() : (cls.empty() ? "?" : cls.c_str())));
	overlay::ShowToast(t, 3.0f);
}

// ----- Pickup naming table ([pickup_names] in silta.ini) -----
void mod::inventory::ClearPickupNames() { g_PickupNames.clear(); }

void mod::inventory::AddPickupName(const char* key, const char* friendly) {
	if (key == nullptr || friendly == nullptr || key[0] == '\0' || friendly[0] == '\0') return;
	g_PickupNames[ToLowerStr(key)] = friendly;
}

// Look up a friendly name by targetname or model basename (case-insensitive).
std::string mod::inventory::LookupPickupName(const std::string& id) {
	if (id.empty() || g_PickupNames.empty()) return std::string();
	auto it = g_PickupNames.find(ToLowerStr(id));
	return (it != g_PickupNames.end()) ? it->second : std::string();
}
