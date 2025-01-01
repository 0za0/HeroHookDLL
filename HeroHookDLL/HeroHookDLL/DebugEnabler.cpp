#include "../imGUI/imgui.h"
#include <vector>
#include <string>
#include "GameManip.h"


struct DebugEnabler {

	std::vector<std::string> MenuItems{ "Nearly Complete","Map Hack","Shop","LotsLego","LotsLegoMillion","MaskInfo","NoPlayerDeath","DieNow","AllToa","No EdgeA on Collision","LevelComplete","LevelPerfect","PermGoldMode","X2LEGO","PickUnlocks","HealthPickup","No Gui","PlayerShadow","Coords","RenderPlayerHealthBar","SeeAllRadar","RenderContextMovement","AutoSelectToa","RayCastSplashDamage","ShowNumPartKeys","ScaleVortex","RenderCheckPoints","AiOnScreenToShoot","NotAiOnScreenToDamage","TestStatusLoad","AllowSkip","NoAmbSfx","AmbSfxDebug","NoStatus","DebugRanking","SfxLoading","MemOutput","noidles","UnlockLevels","CrudStrafe","NoAutoTarget","JonMode","OldGold","NORTLShadows","Quad","UnlockAllPuzzles","CutToa","NoKamikazee","InstantGold","OldUnlocks","NoWeaponAnims","LimitAiDeathFx","NoSkipCredits","KillBgLoad","KillRadar","ShowTimeStamps","GiveLots","0","DrawSFXMem","DrawActiveTargets","DrawTargets","DrawAbility","DrawEdgeCollision","RenderLevelProgress","DynoTerrain","DynoColliders","DynoGeoms","DynoContact","DynoIslands","DrawCollisionShootable","Render CamPan","ExtraCollision","0","AiMessage","CreatureDebug1","CreatureSet","CreatureSphere","CreatureGeom","CreatureFacing","CreatureLocator","CreatureAttachedTo","CreatureAttachedMtx","CreatureMagnititsedMtx","CreatureMoveP","CreatureGoalP","DrawPathsSolid","DrawPathsAll","DrawPathsLevelPath","DrawAiLocators","DrawAiMoveR","DrawAiFacing","DynoTerrain","DynoColliders","DynoGeoms","DynoContact","DynoIslands","DrawCollisionShootable","NoDynoFancyCast","0","InitialHelp","DrawPoi","LaserCross","GuiMakeDbg","DrawBodyBitsPoi","NoToaGlow","BackFaceFlash","Grass","FreePlay","NoShadows","ContextSplineCentre","FaceOff","ClockDeath","CheckPointDeath","WeaponPoi","NoFreeLookMove","NoAutoMaskSelect","NoRadar","Border","DrawCam","FreePlay","AllowAviDump","NoToy","NoJiggy","Demo","FMV","Editors","CreEdit","LevEdit","RtlEdit","Music","NoRaderMap","OldLegoMode","SoakTest","NoPostFX","DrawSpinePoi","NoPiraka","Hints","Kenny","OldAnims","OldZamorWin","CfgDebug","AllMem","NoFx","AutosaveNow","NoMemCard","LateAutoSave","NoPete","QuickStart","DynamicIdle","NoCinema","mouse","GoldModeFreeze","FreezeTest","HitFlashTest","Language","NoWeaponAim","NoReserve","KillThenGrow","AttractFMV","EnterLevSel","OrigMasks","StatusMusic","Trailer","ForceFailController","MetalDetector","NoCustomFade","OldZoom","SimpleLoad","NewStatus","AutoComplete","StressCutscenes","PreSeekBehindCutscenes","0","Useful" };
	ImVector<bool> MenuItemEnabled;

	DebugEnabler() {
		for (size_t i = 0; i < MenuItems.size(); i++)
		{
			MenuItemEnabled.push_back(false);
		}
	}

	~DebugEnabler() {

	}
	void Draw(const char* title, bool* p_open) {
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		for (size_t i = 0; i < MenuItems.size(); i++)
		{
			if (ImGui::Checkbox(MenuItems[i].c_str(), &MenuItemEnabled[i]))
				SetDebugFlag(i);
		}


		ImGui::End();
	}
};