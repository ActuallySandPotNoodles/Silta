#pragma once
#include "stdafx.h"
#include "infra.h"
namespace mod {
	namespace counters {
		void LoadMapData();
		void InitMapStats();
		void StatSuccess(int event_type, int count, bool is_new);
		int  GetCategoryCurrent(int category); // CURRENT MAP collected (0..CategoryCount-1)
		int  GetCategoryMax(int category);     // CURRENT MAP available

		// Run-origin marker, stored in Source global-state (serialized into the
		// save, reset on New Game). Planted when the office loads; read back on
		// any load so an office-started run stays valid across saves / sessions.
		void MarkRunOrigin();
		bool IsRunOriginMarked();

		// Whole-campaign totals: sum every map's persisted global-state counter
		// (current) and mapdata target (max) per category. curOut/maxOut must each
		// hold CategoryCount ints.
		void ComputeCampaignTotals(int* curOut, int* maxOut);
	}
}