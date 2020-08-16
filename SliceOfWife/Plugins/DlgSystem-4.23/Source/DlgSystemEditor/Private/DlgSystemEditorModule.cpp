// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#include "DlgSystemEditorModule.h"

#include "DialogueContentBrowserExtensions.h"
#include "Engine/BlueprintCore.h"
#include "Templates/SharedPointer.h"
#include "AssetRegistryModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "WorkspaceMenuStructureModule.h"
#include "WorkspaceMenuStructure.h"
#include "Widgets/Docking/SDockTab.h"
#include "FileHelpers.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "LevelEditor.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Editor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "DialogueGraphFactories.h"
#include "DlgSystemEditorPrivatePCH.h"
#include "DialogueAssetTypeActions.h"
#include "DialogueCommands.h"
#include "DialogueEditor/Nodes/DialogueGraphNode.h"
#include "DialogueBrowser/SDialogueBrowser.h"
#include "DialogueSearch/DialogueSearchManager.h"
#include "DialogueEditor/DetailsPanel/Dialogue_Details.h"
#include "DialogueEditor/DetailsPanel/DialogueGraphNode_Details.h"
#include "DialogueEditor/DetailsPanel/DialogueNode_Details.h"
#include "DialogueEditor/DetailsPanel/DialogueEdge_Details.h"
#include "DialogueEditor/DetailsPanel/DialogueCondition_Details.h"
#include "DialogueEditor/DetailsPanel/DialogueEvent_Details.h"
#include "DialogueEditor/DetailsPanel/DialogueTextArgument_Details.h"
#include "DialogueEditor/DetailsPanel/DialogueSpeechSequenceEntry_Details.h"
#include "DlgManager.h"
#include "IDlgSystemModule.h"

#include "IO/DlgConfigWriter.h"
#include "IO/DlgConfigParser.h"
#include "Logging/DlgLogger.h"

#define LOCTEXT_NAMESPACE "DlgSystemEditor"

//////////////////////////////////////////////////////////////////////////
DEFINE_LOG_CATEGORY(LogDlgSystemEditor)
//////////////////////////////////////////////////////////////////////////

// Just some constants
static const FName DIALOGUE_BROWSER_TAB_ID("DialogueBrowser");


FDlgSystemEditorModule::FDlgSystemEditorModule() : DlgSystemAssetCategoryBit(EAssetTypeCategories::UI)
{
}

void FDlgSystemEditorModule::StartupModule()
{
#if ENGINE_MINOR_VERSION >= 24
	// Fix blueprint Nativization https://gitlab.com/NotYetGames/DlgSystem/-/issues/28
	const FString LongName = FPackageName::ConvertToLongScriptPackageName(TEXT("DlgSystemEditor"));
	if (UPackage* Package = Cast<UPackage>(StaticFindObjectFast(UPackage::StaticClass(), nullptr, *LongName, false, false)))
	{
		Package->SetPackageFlags(PKG_EditorOnly);
	}
#endif

	UE_LOG(LogDlgSystemEditor, Log, TEXT("DlgSystemEditorModule: StartupModule"));
	OnPostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &Self::HandleOnPostEngineInit);
	OnBeginPIEHandle = FEditorDelegates::BeginPIE.AddRaw(this, &Self::HandleOnBeginPIE);
	OnPostPIEStartedHandle = FEditorDelegates::PostPIEStarted.AddRaw(this, &Self::HandleOnPostPIEStarted);
	OnEndPIEHandle = FEditorDelegates::EndPIE.AddRaw(this, &Self::HandleOnEndPIEHandle);

	// Register slate style overrides
	FDialogueStyle::Initialize();

	// Register commands
	FDialogueCommands::Register();

	// Register asset types, add the right click submenu
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(NAME_MODULE_AssetTools).Get();

	// make the DlgSystem be displayed in the filters menu and in the create new menu
	DlgSystemAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(DIALOGUE_SYSTEM_MENU_CATEGORY_KEY, DIALOGUE_SYSTEM_MENU_CATEGORY_KEY_TEXT);
	{
		auto Action = MakeShared<FDialogueAssetTypeActions>(DlgSystemAssetCategoryBit);
		AssetTools.RegisterAssetTypeActions(Action);
		RegisteredAssetTypeActions.Add(Action);
	}

	// Register the details panel customizations
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(NAME_MODULE_PropertyEditor);

		// For classes:
		// NOTE Order of these two arrays must match
		TArray<FOnGetDetailCustomizationInstance> CustomClassLayouts = {
			  FOnGetDetailCustomizationInstance::CreateStatic(&FDialogue_Details::MakeInstance),
			  FOnGetDetailCustomizationInstance::CreateStatic(&FDialogueGraphNode_Details::MakeInstance),
			  FOnGetDetailCustomizationInstance::CreateStatic(&FDialogueNode_Details::MakeInstance)
		};
		RegisteredCustomClassLayouts = {
			UDlgDialogue::StaticClass()->GetFName(),
			UDialogueGraphNode::StaticClass()->GetFName(),
			UDlgNode::StaticClass()->GetFName()
		};
		for (int32 i = 0; i < RegisteredCustomClassLayouts.Num(); i++)
		{
			PropertyModule.RegisterCustomClassLayout(RegisteredCustomClassLayouts[i], CustomClassLayouts[i]);
		}

		// For structs:
		// NOTE Order of these two arrays must match
		TArray<FOnGetPropertyTypeCustomizationInstance> CustomPropertyTypeLayouts = {
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogueEdge_Details::MakeInstance),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogueCondition_Details::MakeInstance),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogueEvent_Details::MakeInstance),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogueSpeechSequenceEntry_Details::MakeInstance),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogueTextArgument_Details::MakeInstance)
		};
		RegisteredCustomPropertyTypeLayout = {
			FDlgEdge::StaticStruct()->GetFName(),
			FDlgCondition::StaticStruct()->GetFName(),
			FDlgEvent::StaticStruct()->GetFName(),
			FDlgSpeechSequenceEntry::StaticStruct()->GetFName(),
			FDlgTextArgument::StaticStruct()->GetFName()
		};
		for (int32 i = 0; i < RegisteredCustomPropertyTypeLayout.Num(); i++)
		{
			PropertyModule.RegisterCustomPropertyTypeLayout(RegisteredCustomPropertyTypeLayout[i], CustomPropertyTypeLayouts[i]);
		}

		PropertyModule.NotifyCustomizationModuleChanged();
	}

	// Register the thumbnail renderers
//	UThumbnailManager::Get().RegisterCustomRenderer(UDlgDialogue::StaticClass(), UDlgDialogueThumbnailRenderer::StaticClass());

	// Create factories
	DialogueGraphNodeFactory = MakeShared<FDialogueGraphNodeFactory>();
	FEdGraphUtilities::RegisterVisualNodeFactory(DialogueGraphNodeFactory);

	DialogueGraphPinFactory = MakeShared<FDialogueGraphPinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory(DialogueGraphPinFactory);

	// Bind Editor commands
	LevelMenuEditorCommands = MakeShared<FUICommandList>();
	MapActionsForFileMenuExtender(LevelMenuEditorCommands.ToSharedRef());
	MapActionsForHelpMenuExtender(LevelMenuEditorCommands.ToSharedRef());

	// Content Browser extension
	FDialogueContentBrowserExtensions::InstallHooks();

	// Extend menu/toolbar
	ExtendMenu();
}

void FDlgSystemEditorModule::ShutdownModule()
{
	const FModuleManager& ModuleManger = FModuleManager::Get();
	if (DialogueGraphPinFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(DialogueGraphPinFactory);
	}

	if (DialogueGraphNodeFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(DialogueGraphNodeFactory);
	}

	if (UObjectInitialized())
	{
		// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
		// we call this function before unloading the module.
		FDialogueContentBrowserExtensions::RemoveHooks();
	}

	// Unregister the custom details panel stuff
	if (ModuleManger.IsModuleLoaded(NAME_MODULE_PropertyEditor))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(NAME_MODULE_PropertyEditor);
		for (int32 i = 0; i < RegisteredCustomClassLayouts.Num(); i++)
		{
			PropertyModule.UnregisterCustomClassLayout(RegisteredCustomClassLayouts[i]);
		}

		for (int32 i = 0; i < RegisteredCustomPropertyTypeLayout.Num(); i++)
		{
			PropertyModule.UnregisterCustomPropertyTypeLayout(RegisteredCustomPropertyTypeLayout[i]);
		}
	}
	RegisteredCustomClassLayouts.Empty();
	RegisteredCustomPropertyTypeLayout.Empty();

	// Unregister all the asset types that we registered
	if (ModuleManger.IsModuleLoaded(NAME_MODULE_AssetTools))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>(NAME_MODULE_AssetTools).Get();
		for (auto TypeAction : RegisteredAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(TypeAction.ToSharedRef());
		}
	}
	RegisteredAssetTypeActions.Empty();

	// unregister commands
	FDialogueCommands::Unregister();

	// Unregister slate style overrides
	FDialogueStyle::Shutdown();

	// Unregister the Dialogue Browser
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(DIALOGUE_BROWSER_TAB_ID);

	// Unregister the Dialogue Search
	FDialogueSearchManager::Get()->DisableGlobalFindResults();

	if (OnBeginPIEHandle.IsValid())
	{
		FEditorDelegates::BeginPIE.Remove(OnBeginPIEHandle);
	}
	if (OnPostPIEStartedHandle.IsValid())
	{
		FEditorDelegates::PostPIEStarted.Remove(OnPostPIEStartedHandle);
	}
	if (OnEndPIEHandle.IsValid())
	{
		FEditorDelegates::EndPIE.Remove(OnEndPIEHandle);
	}

	UE_LOG(LogDlgSystemEditor, Log, TEXT("DlgSystemEditorModule: ShutdownModule"));
}

bool FDlgSystemEditorModule::SaveAllDialogues()
{
	const TArray<UDlgDialogue*> Dialogues = UDlgManager::GetAllDialoguesFromMemory();
	TArray<UPackage*> PackagesToSave;
	const bool bBatchOnlyInGameDialogues = GetDefault<UDlgSystemSettings>()->bBatchOnlyInGameDialogues;

	for (UDlgDialogue* Dialogue : Dialogues)
	{
		// Ignore, not in game directory
		if (bBatchOnlyInGameDialogues && !Dialogue->IsInProjectDirectory())
		{
			continue;
		}

		Dialogue->MarkPackageDirty();
		PackagesToSave.Add(Dialogue->GetOutermost());
	}

	static constexpr bool bCheckDirty = false;
	static constexpr bool bPromptToSave = false;
	return FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, bCheckDirty, bPromptToSave) == FEditorFileUtils::EPromptReturnCode::PR_Success;
}

void FDlgSystemEditorModule::HandleOnSaveAllDialogues()
{
	const EAppReturnType::Type Response = FPlatformMisc::MessageBoxExt(EAppMsgType::YesNo,
		TEXT("Save all Dialogue assets/files? This will save both the .uasset and the text files depending on the TextFormat from the Dialogue Settings."),
		TEXT("Save Dialogues?")
	);
	if (Response == EAppReturnType::No)
	{
		return;
	}

	if (!SaveAllDialogues())
	{
		UE_LOG(LogDlgSystemEditor, Error, TEXT("Failed To save all Dialogues. An error occurred."));
	}
}

void FDlgSystemEditorModule::HandleOnDeleteAllDialoguesTextFiles()
{
	const TSet<FString> AllFileExtensions = GetDefault<UDlgSystemSettings>()->GetAllTextFileExtensions();
	const FString StringAllFileExtensions = FString::Join(AllFileExtensions, TEXT(","));
	const FString Text = FString::Printf(
		TEXT("Delete all Dialogues text files? Delete all dialogues text files on the disk with the following extensions: %s"),
		*StringAllFileExtensions
	);

	const EAppReturnType::Type Response = FPlatformMisc::MessageBoxExt(EAppMsgType::YesNo, *Text, TEXT("Delete All Dialogues text files?"));
	if (Response == EAppReturnType::No)
	{
		return;
	}

	if (!DeleteAllDialoguesTextFiles())
	{
		UE_LOG(LogDlgSystemEditor, Error, TEXT("Failed To delete all Dialogues text files. An error occurred."));
	}
}

bool FDlgSystemEditorModule::DeleteAllDialoguesTextFiles()
{
	const TArray<UDlgDialogue*> Dialogues = UDlgManager::GetAllDialoguesFromMemory();
	const bool bBatchOnlyInGameDialogues = GetDefault<UDlgSystemSettings>()->bBatchOnlyInGameDialogues;
	for (const UDlgDialogue* Dialogue : Dialogues)
	{
		// Ignore, not in game directory
		if (bBatchOnlyInGameDialogues && !Dialogue->IsInProjectDirectory())
		{
			continue;
		}

		Dialogue->DeleteAllTextFiles();
	}

	return true;
}

void FDlgSystemEditorModule::HandleOnPostEngineInit()
{
	if (!OnPostEngineInitHandle.IsValid())
	{
		return;
	}

	UE_LOG(LogDlgSystemEditor, Log, TEXT("DlgSystemEditorModule::HandleOnPostEngineInit"));
	FCoreDelegates::OnPostEngineInit.Remove(OnPostEngineInitHandle);
	OnPostEngineInitHandle.Reset();

	//const int32 NumDialoguesBefore = UDlgManager::GetAllDialoguesFromMemory().Num();
	const int32 NumLoadedDialogues = UDlgManager::LoadAllDialoguesIntoMemory();
	//const int32 NumDialoguesAfter = UDlgManager::GetAllDialoguesFromMemory().Num();
	//check(NumDialoguesBefore == NumDialoguesAfter);
	UE_LOG(LogDlgSystemEditor, Log, TEXT("UDlgManager::LoadAllDialoguesIntoMemory loaded %d Dialogues into Memory"), NumLoadedDialogues);

	// Try to fix duplicate GUID
	// Can happen for one of the following reasons:
	// - duplicated files outside of UE
	// - somehow loaded from text files?
	// - the universe hates us? +_+
	for (UDlgDialogue* Dialogue : UDlgManager::GetDialoguesWithDuplicateGUIDs())
	{
		UE_LOG(
			LogDlgSystemEditor,
			Warning,
			TEXT("Dialogue = `%s`, GUID = `%s` has a Duplicate GUID. Regenerating."),
			*Dialogue->GetPathName(), *Dialogue->GetDialogueGUID().ToString()
		)
		Dialogue->RegenerateGUID();
		Dialogue->MarkPackageDirty();
	}

	// Give it another try, Give up :((
	// May the math Gods have mercy on us!
	for (const UDlgDialogue* Dialogue : UDlgManager::GetDialoguesWithDuplicateGUIDs())
	{
		// GUID already exists (╯°□°）╯︵ ┻━┻
		// Does this break the universe?
		UE_LOG(
			LogDlgSystemEditor,
			Error,
			TEXT("Dialogue = `%s`, GUID = `%s`"),
			*Dialogue->GetPathName(), *Dialogue->GetDialogueGUID().ToString()
		)

		UE_LOG(
			LogDlgSystemEditor,
			Fatal,
			TEXT("(╯°□°）╯︵ ┻━┻ Congrats, you just broke the universe, are you even human? Now please go and proove an NP complete problem."
				"The chance of generating two equal random FGuid (picking 4, uint32 numbers) is p = 9.3132257 * 10^(-10) % (or something like this)")
		)
	}
}

void FDlgSystemEditorModule::HandleOnBeginPIE(bool bIsSimulating)
{
	if (!OnBeginPIEHandle.IsValid())
	{
		return;
	}
}

void FDlgSystemEditorModule::HandleOnPostPIEStarted(bool bIsSimulating)
{
	if (!OnPostPIEStartedHandle.IsValid())
	{
		return;
	}

	const UDlgSystemSettings* Settings = GetDefault<UDlgSystemSettings>();
	if (!Settings)
	{
		return;
	}

	if (Settings->bClearDialogueHistoryAutomatically)
	{
		FDlgLogger::Get().Debugf(TEXT("BeginPIE(bIsSimulating = %d). Clearing Dialogue History"), bIsSimulating);
		UDlgManager::ClearDialogueHistory();
	}

	if (Settings->bRegisterDialogueConsoleCommandsAutomatically)
	{
		FDlgLogger::Get().Debugf(TEXT("BeginPIE(bIsSimulating = %d). Registering Console commands"), bIsSimulating);
		UDlgManager::RegisterDialogueConsoleCommands();
	}
}

void FDlgSystemEditorModule::HandleOnEndPIEHandle(bool bIsSimulating)
{
	if (!OnEndPIEHandle.IsValid())
	{
		return;
	}
	const UDlgSystemSettings* Settings = GetDefault<UDlgSystemSettings>();
	if (!Settings)
	{
		return;
	}

	if (Settings->bRegisterDialogueConsoleCommandsAutomatically)
	{
		FDlgLogger::Get().Debugf(TEXT("EndPIE(bIsSimulating = %d). Unregistering Console commands"), bIsSimulating);
		UDlgManager::UnregisterDialogueConsoleCommands();
	}
}

void FDlgSystemEditorModule::ExtendMenu()
{
	// Running in game mode (standalone game) exit as we can't get the LevelEditorModule.
	if (IsRunningGame() || IsRunningCommandlet())
	{
		return;
	}

	// File and Help Menu Extenders
	{
		const TSharedRef<FExtender> FileMenuExtender = CreateFileMenuExtender(LevelMenuEditorCommands.ToSharedRef());
		const TSharedRef<FExtender> HelpMenuExtender = CreateHelpMenuExtender(LevelMenuEditorCommands.ToSharedRef());

		// Add to the level editor
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(NAME_MODULE_LevelEditor);
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(FileMenuExtender);
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(HelpMenuExtender);
	}

	// Window -> Dialogue search, Dialogue Browse, Dialogue Data Display
	{
		ToolsDialogueCategory = WorkspaceMenu::GetMenuStructure().GetStructureRoot()
			->AddGroup(
				LOCTEXT("WorkspaceMenu_DialogueCategory", "Dialogue" ),
				FSlateIcon(
					FDialogueStyle::GetStyleSetName(),
					FDialogueStyle::PROPERTY_DialogueClassIcon
				),
				false
			);

		// Register the Dialogue Overview Browser
		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(DIALOGUE_BROWSER_TAB_ID,
			FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args) -> TSharedRef<SDockTab>
			{
				const TSharedRef<SDockTab> DockTab = SNew(SDockTab)
					.TabRole(ETabRole::NomadTab)
					[
						SNew(SDialogueBrowser)
					];
				return DockTab;
			}))
			.SetDisplayName(LOCTEXT("DialogueBrowserTabTitle", "Dialogue Browser"))
			.SetTooltipText(LOCTEXT("DialogueBrowserTooltipText", "Open the Dialogue Overview Browser tab."))
			.SetIcon(FSlateIcon(FDialogueStyle::GetStyleSetName(), FDialogueStyle::PROPERTY_DialogueBrowser_TabIcon))
			.SetGroup(ToolsDialogueCategory.ToSharedRef());

		// Register the Dialogue Search
		FDialogueSearchManager::Get()->Initialize(ToolsDialogueCategory);

		// Register the Dialogue Data Display
		FTabSpawnerEntry* TabDialogueDataDisplay = IDlgSystemModule::Get().GetDialogueDataDisplaySpawnEntry();
		TabDialogueDataDisplay->SetGroup(ToolsDialogueCategory.ToSharedRef());
		TabDialogueDataDisplay->SetIcon(FSlateIcon(FDialogueStyle::GetStyleSetName(), FDialogueStyle::PROPERTY_DialogueDataDisplay_TabIcon));
	}
}

TSharedRef<FExtender> FDlgSystemEditorModule::CreateFileMenuExtender(
	TSharedRef<FUICommandList> Commands,
	const TArray<TSharedPtr<FUICommandInfo>>& AdditionalMenuEntries
)
{
	// Fill after the File->FileLoadAndSave
	TSharedRef<FExtender> FileMenuExtender(new FExtender);
	FileMenuExtender->AddMenuExtension(
		"FileLoadAndSave",
		EExtensionHook::After,
		Commands,
		FMenuExtensionDelegate::CreateLambda([AdditionalMenuEntries](FMenuBuilder& MenuBuilder)
		{
			// Save Dialogues
			MenuBuilder.BeginSection("Dialogue", LOCTEXT("DialogueMenuKeyCategory", "Dialogue"));
			{
				MenuBuilder.AddMenuEntry(FDialogueCommands::Get().SaveAllDialogues);
				MenuBuilder.AddMenuEntry(FDialogueCommands::Get().DeleteAllDialoguesTextFiles);
				MenuBuilder.AddMenuSeparator();
				for (auto& MenuEntry : AdditionalMenuEntries)
				{
					MenuBuilder.AddMenuEntry(MenuEntry);
				}
			}
			MenuBuilder.EndSection();
		})
	);

	return FileMenuExtender;
}

TSharedRef<FExtender> FDlgSystemEditorModule::CreateHelpMenuExtender(TSharedRef<FUICommandList> Commands)
{
	// Fill after the Help->HelpBrowse
	TSharedRef<FExtender> HelpMenuExtender(new FExtender);
	HelpMenuExtender->AddMenuExtension(
		"HelpBrowse",
		EExtensionHook::After,
		Commands,
		FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
		{
			// Save Dialogues
			MenuBuilder.BeginSection("Dialogue", LOCTEXT("DialogueMenuKeyCategory", "Dialogue"));
			{
				MenuBuilder.AddMenuEntry(FDialogueCommands::Get().OpenNotYetPlugins);
				MenuBuilder.AddMenuEntry(FDialogueCommands::Get().OpenMarketplace);
				MenuBuilder.AddMenuEntry(FDialogueCommands::Get().OpenWiki);
				MenuBuilder.AddMenuEntry(FDialogueCommands::Get().OpenDiscord);
				MenuBuilder.AddMenuEntry(FDialogueCommands::Get().OpenForum);
			}
			MenuBuilder.EndSection();
		})
	);

	return HelpMenuExtender;
}

void FDlgSystemEditorModule::MapActionsForFileMenuExtender(TSharedRef<FUICommandList> Commands)
{
	Commands->MapAction(
		FDialogueCommands::Get().SaveAllDialogues,
		FExecuteAction::CreateStatic(&Self::HandleOnSaveAllDialogues)
	);
	Commands->MapAction(
		FDialogueCommands::Get().DeleteAllDialoguesTextFiles,
		FExecuteAction::CreateStatic(&Self::HandleOnDeleteAllDialoguesTextFiles)
	);
}

void FDlgSystemEditorModule::MapActionsForHelpMenuExtender(TSharedRef<FUICommandList> Commands)
{
	const UDlgSystemSettings& Settings = *GetDefault<UDlgSystemSettings>();
	Commands->MapAction(
		FDialogueCommands::Get().OpenNotYetPlugins,
		FExecuteAction::CreateLambda([&Settings]()
		{
			FPlatformProcess::LaunchURL(*Settings.URLNotYetPlugins, nullptr, nullptr );
		})
	);
	Commands->MapAction(
		FDialogueCommands::Get().OpenMarketplace,
		FExecuteAction::CreateLambda([&Settings]()
		{
			FPlatformProcess::LaunchURL(*Settings.URLMarketplace, nullptr, nullptr );
		})
	);
	Commands->MapAction(
		FDialogueCommands::Get().OpenDiscord,
		FExecuteAction::CreateLambda([&Settings]()
		{
			FPlatformProcess::LaunchURL(*Settings.URLDiscord, nullptr, nullptr );
		})
	);
	Commands->MapAction(
		FDialogueCommands::Get().OpenForum,
		FExecuteAction::CreateLambda([&Settings]()
		{
			FPlatformProcess::LaunchURL(*Settings.URLForum, nullptr, nullptr );
		})
	);
	Commands->MapAction(
		FDialogueCommands::Get().OpenWiki,
		FExecuteAction::CreateLambda([&Settings]()
		{
			FPlatformProcess::LaunchURL(*Settings.URLWiki, nullptr, nullptr );
		})
	);
}

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_MODULE(FDlgSystemEditorModule, DlgSystemEditor)
//////////////////////////////////////////////////////////////////////////
