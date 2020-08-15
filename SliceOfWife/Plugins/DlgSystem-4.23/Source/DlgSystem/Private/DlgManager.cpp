// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#include "DlgManager.h"

#include "UObject/UObjectIterator.h"
#include "Engine/ObjectLibrary.h"
#include "Interfaces/IPluginManager.h"
#include "Engine/Blueprint.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"

#include "IDlgSystemModule.h"
#include "DlgSystemPrivatePCH.h"
#include "DlgDialogueParticipant.h"
#include "DlgDialogue.h"
#include "DlgMemory.h"
#include "DlgContext.h"
#include "Logging/DlgLogger.h"
#include "DlgHelper.h"
#include "NYReflectionHelper.h"

TWeakObjectPtr<const UObject> UDlgManager::UserWorldContextObjectPtr = nullptr;

UDlgContext* UDlgManager::StartDialogueWithDefaultParticipants(UObject* WorldContextObject, UDlgDialogue* Dialogue)
{
	if (!IsValid(Dialogue))
	{
		FDlgLogger::Get().Error(TEXT("StartDialogueWithDefaultParticipants - FAILED to start dialogue because the Dialogue is INVALID (is nullptr)!"));
		return nullptr;
	}

	// Create empty map of participants we need
	TSet<FName> ParticipantSet;
	Dialogue->GetAllParticipantNames(ParticipantSet);
	TArray<UObject*> Participants;

	// Maps from Participant Name => Objects that have that participant name
	TMap<FName, TArray<UObject*>> ObjectMap;
	for (const FName& Name : ParticipantSet)
	{
		ObjectMap.Add(Name, {});
	}

	// Gather all objects that have our participant name
	for (UObject* Participant : GetAllObjectsWithDialogueParticipantInterface(WorldContextObject))
	{
		const FName ParticipantName = IDlgDialogueParticipant::Execute_GetParticipantName(Participant);
		if (ObjectMap.Contains(ParticipantName))
		{
			ObjectMap[ParticipantName].AddUnique(Participant);
			Participants.AddUnique(Participant);
		}
	}

	// Find the missing names and the duplicate names
	TArray<FString> MissingNames;
	TArray<FString> DuplicatedNames;
	for (const auto& Pair : ObjectMap)
	{
		const FName ParticipantName = Pair.Key;
		const TArray<UObject*>& Objects = Pair.Value;

		if (Objects.Num() == 0)
		{
			MissingNames.Add(ParticipantName.ToString());
		}
		else if (Objects.Num() > 1)
		{
			for (UObject* Obj : Objects)
			{
				DuplicatedNames.Add(Obj->GetName() + "(" + ParticipantName.ToString() + ")");
			}
		}
	}

	if (MissingNames.Num() > 0)
	{
		const FString NameList = FString::Join(MissingNames, TEXT(", "));
		FDlgLogger::Get().Errorf(
			TEXT("StartDialogueWithDefaultParticipants - FAILED Dialogue = `%s`, the system FAILED to find the following Participant(s): %s"),
			*Dialogue->GetName(), *NameList
		);
	}

	if (DuplicatedNames.Num() > 0)
	{
		const FString NameList = FString::Join(DuplicatedNames, TEXT(", "));
		FDlgLogger::Get().Errorf(
			TEXT("StartDialogueWithDefaultParticipants - FAILED for Dialogue = `%s`, the system found multiple participants using the same name: %s"),
			*Dialogue->GetName(), *NameList
		);
	}

	if (MissingNames.Num() > 0 || DuplicatedNames.Num() > 0)
	{
		return nullptr;
	}

	return StartDialogueFromContext(TEXT("StartDialogueWithDefaultParticipants"), Dialogue, Participants);
}

UDlgContext* UDlgManager::StartDialogueFromContext(const FString& ContextString, UDlgDialogue* Dialogue, const TArray<UObject*>& Participants)
{
	const FString ContextMessage = ContextString.IsEmpty()
        ? FString::Printf(TEXT("StartDialogue"))
        : FString::Printf(TEXT("%s - StartDialogue"), *ContextString);

	TMap<FName, UObject*> ParticipantBinding;
	if (!UDlgContext::ConvertArrayOfParticipantsToMap(ContextMessage, Dialogue, Participants, ParticipantBinding))
	{
		return nullptr;
	}

	auto* Context = NewObject<UDlgContext>(Participants[0], UDlgContext::StaticClass());
	if (Context->StartFromContext(ContextMessage, Dialogue, ParticipantBinding))
	{
		return Context;
	}

	return nullptr;
}

bool UDlgManager::CanStartDialogue(UDlgDialogue* Dialogue, UPARAM(ref)const TArray<UObject*>& Participants)
{
	TMap<FName, UObject*> ParticipantBinding;
	if (!UDlgContext::ConvertArrayOfParticipantsToMap(TEXT("CanStartDialogue"), Dialogue, Participants, ParticipantBinding, false))
	{
		return false;
	}

	return UDlgContext::CanBeStarted(Dialogue, ParticipantBinding);
}

UDlgContext* UDlgManager::ResumeDialogue(
	UDlgDialogue* Dialogue,
	UPARAM(ref)const TArray<UObject*>& Participants,
	int32 StartIndex,
	const TSet<int32>& AlreadyVisitedNodes,
	bool bFireEnterEvents
)
{
	const FString ContextMessage = TEXT("ResumeDialogue");
	TMap<FName, UObject*> ParticipantBinding;
	if (!UDlgContext::ConvertArrayOfParticipantsToMap(ContextMessage, Dialogue, Participants, ParticipantBinding))
	{
		return nullptr;
	}

	auto* Context = NewObject<UDlgContext>(Participants[0], UDlgContext::StaticClass());
	if (Context->StartFromContextFromIndex(ContextMessage, Dialogue, ParticipantBinding, StartIndex, AlreadyVisitedNodes, bFireEnterEvents))
	{
		return Context;
	}

	return nullptr;
}

UDlgContext* UDlgManager::StartMonologue(UDlgDialogue* Dialogue, UObject* Participant)
{
	TArray<UObject*> Participants;
	Participants.Add(Participant);
	return StartDialogueFromContext(TEXT("StartMonologue"), Dialogue, Participants);
}

UDlgContext* UDlgManager::StartDialogue2(UDlgDialogue* Dialogue, UObject* Participant0, UObject* Participant1)
{
	TArray<UObject*> Participants;
	Participants.Add(Participant0);
	Participants.Add(Participant1);
	return StartDialogueFromContext(TEXT("StartDialogue2"), Dialogue, Participants);
}

UDlgContext* UDlgManager::StartDialogue3(UDlgDialogue* Dialogue, UObject* Participant0, UObject* Participant1, UObject* Participant2)
{
	TArray<UObject*> Participants;
	Participants.Add(Participant0);
	Participants.Add(Participant1);
	Participants.Add(Participant2);
	return StartDialogueFromContext(TEXT("StartDialogue3"), Dialogue, Participants);
}

UDlgContext* UDlgManager::StartDialogue4(UDlgDialogue* Dialogue, UObject* Participant0, UObject* Participant1, UObject* Participant2, UObject* Participant3)
{
	TArray<UObject*> Participants;
	Participants.Add(Participant0);
	Participants.Add(Participant1);
	Participants.Add(Participant2);
	Participants.Add(Participant3);

	return StartDialogueFromContext(TEXT("StartDialogue4"), Dialogue, Participants);
}

int32 UDlgManager::LoadAllDialoguesIntoMemory()
{
	// NOTE: All paths must NOT have the forward slash "/" at the end.
	// If they do, then this won't load Dialogues that are located in the Content root directory
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UDlgDialogue::StaticClass(), true, GIsEditor);
	TArray<FString> PathsToSearch = { TEXT("/Game") };
	ObjectLibrary->AddToRoot();

	// Add the current plugin dir
	// TODO maybe add all the non engine plugin paths? IPluginManager::Get().GetEnabledPlugins()
	const TSharedPtr<IPlugin> ThisPlugin = IPluginManager::Get().FindPlugin(DIALOGUE_SYSTEM_PLUGIN_NAME.ToString());
	if (ThisPlugin.IsValid())
	{
		FString PluginPath = ThisPlugin->GetMountedAssetPath();
		// See NOTE above
		PluginPath.RemoveFromEnd(TEXT("/"));
		PathsToSearch.Add(PluginPath);
	}

	ObjectLibrary->LoadAssetDataFromPaths(PathsToSearch);
	const int32 Count = ObjectLibrary->LoadAssetsFromAssetData();
	ObjectLibrary->RemoveFromRoot();
	return Count;
}

TArray<UDlgDialogue*> UDlgManager::GetAllDialoguesFromMemory()
{
	TArray<UDlgDialogue*> Array;
	for (TObjectIterator<UDlgDialogue> Itr; Itr; ++Itr)
	{
		UDlgDialogue* Dialogue = *Itr;
		if (IsValid(Dialogue))
		{
			Array.Add(Dialogue);
		}
	}
	return Array;
}

TArray<TWeakObjectPtr<AActor>> UDlgManager::GetAllActorsImplementingDialogueParticipantInterface(UWorld* World)
{
	TArray<TWeakObjectPtr<AActor>> Array;
	for (TActorIterator<AActor> Itr(World); Itr; ++Itr)
	{
		AActor* Actor = *Itr;
		if (IsValid(Actor) && Actor->GetClass()->ImplementsInterface(UDlgDialogueParticipant::StaticClass()))
		{
			Array.Add(Actor);
		}
	}
	return Array;
}

TArray<UObject*> UDlgManager::GetAllObjectsWithDialogueParticipantInterface(UObject* WorldContextObject)
{
	TArray<UObject*> Array;
	if (WorldContextObject != nullptr)
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			// TObjectIterator has some weird ghost objects in editor, I failed to find a way to validate them
			// Instead of this ActorIterate is used and the properties inside the actors are examined in a recursive way
			// Containers are not supported yet
			TSet<UObject*> VisitedSet;
			for (TActorIterator<AActor> Itr(World); Itr; ++Itr)
			{
				GatherParticipantsRecursive(*Itr, Array, VisitedSet);
			}

			//for (TObjectIterator<UObject> Itr(World); Itr; ++Itr)
			//{
			//	UObject* Object = *Itr;
			//	if (IsValid(Object) && Object->GetClass()->ImplementsInterface(UDlgDialogueParticipant::StaticClass()))
			//	{
			//		Array.AddUnique(Object);
			//	}
			//}
		}
	}
	return Array;
}

TArray<UDlgDialogue*> UDlgManager::GetDialoguesWithDuplicateGUIDs()
{
	TArray<UDlgDialogue*> Dialogues = GetAllDialoguesFromMemory();
	TArray<UDlgDialogue*> DuplicateDialogues;

	TSet<FGuid> DialogueGUIDs;
	for (UDlgDialogue* Dialogue : Dialogues)
	{
		const FGuid ID = Dialogue->GetDialogueGUID();
		if (DialogueGUIDs.Find(ID) == nullptr)
		{
			// does not exist, good
			DialogueGUIDs.Add(ID);
		}
		else
		{
			// how?
			DuplicateDialogues.Add(Dialogue);
		}
	}

	return DuplicateDialogues;
}

TMap<FGuid, UDlgDialogue*> UDlgManager::GetAllDialoguesGUIDsMap()
{
	TArray<UDlgDialogue*> Dialogues = GetAllDialoguesFromMemory();
	TMap<FGuid, UDlgDialogue*> DialoguesMap;

	for (UDlgDialogue* Dialogue : Dialogues)
	{
		const FGuid ID = Dialogue->GetDialogueGUID();
		if (DialoguesMap.Contains(ID))
		{
			FDlgLogger::Get().Errorf(
				TEXT("GetAllDialoguesGUIDsMap - ID = `%s` for Dialogue = `%s` already exists"),
				*ID.ToString(), *Dialogue->GetPathName()
			);
		}

		DialoguesMap.Add(ID, Dialogue);
	}

	return DialoguesMap;
}

const TMap<FGuid, FDlgHistory>& UDlgManager::GetDialogueHistory()
{
	return FDlgMemory::Get().GetHistoryMaps();
}

void UDlgManager::SetDialogueHistory(const TMap<FGuid, FDlgHistory>& DlgHistory)
{
	FDlgMemory::Get().SetHistoryMap(DlgHistory);
}

void UDlgManager::ClearDialogueHistory()
{
	FDlgMemory::Get().Empty();
}

bool UDlgManager::DoesObjectImplementDialogueParticipantInterface(const UObject* Object)
{
	if (!Object)
	{
		return false;
	}

	// Apparently blueprints only work this way
	// NOTE this is the blueprint assets, not an instance, used only by the custom graph nodes
	if (const UBlueprint* Blueprint = Cast<UBlueprint>(Object))
	{
		if (const UClass* GeneratedClass = Cast<UClass>(Blueprint->GeneratedClass))
		{
			return DoesClassImplementParticipantInterface(GeneratedClass);
		}
	}

	// A class object, does this ever happen?
	if (const UClass* Class = Cast<UClass>(Object))
	{
		return DoesClassImplementParticipantInterface(Class);
	}

	// All other object types
	return DoesClassImplementParticipantInterface(Object->GetClass());
}

bool UDlgManager::IsObjectACustomEvent(const UObject* Object)
{
	return FDlgHelper::IsObjectAChildOf(Object, UDlgEventCustom::StaticClass());
}

bool UDlgManager::IsObjectACustomCondition(const UObject* Object)
{
	return FDlgHelper::IsObjectAChildOf(Object, UDlgConditionCustom::StaticClass());
}

bool UDlgManager::IsObjectACustomTextArgument(const UObject* Object)
{
	return FDlgHelper::IsObjectAChildOf(Object, UDlgTextArgumentCustom::StaticClass());
}

TArray<UDlgDialogue*> UDlgManager::GetAllDialoguesForParticipantName(FName ParticipantName)
{
	TArray<UDlgDialogue*> DialoguesArray;
	for (UDlgDialogue* Dialogue : GetAllDialoguesFromMemory())
	{
		if (Dialogue->HasParticipant(ParticipantName))
		{
			DialoguesArray.Add(Dialogue);
		}
	}

	return DialoguesArray;
}

void UDlgManager::GetAllDialoguesParticipantNames(TArray<FName>& OutArray)
{
	TSet<FName> UniqueNames;
	for (const UDlgDialogue* Dialogue : GetAllDialoguesFromMemory())
	{
		Dialogue->GetAllParticipantNames(UniqueNames);
	}

	FDlgHelper::AppendSortedSetToArray(UniqueNames, OutArray);
}

void UDlgManager::GetAllDialoguesSpeakerStates(TArray<FName>& OutArray)
{
	TSet<FName> UniqueNames;
	for (const UDlgDialogue* Dialogue : GetAllDialoguesFromMemory())
	{
		Dialogue->GetAllSpeakerState(UniqueNames);
	}

	FDlgHelper::AppendSortedSetToArray(UniqueNames, OutArray);
}

void UDlgManager::GetAllDialoguesIntNames(FName ParticipantName, TArray<FName>& OutArray)
{
	TSet<FName> UniqueNames;
	for (const UDlgDialogue* Dialogue : GetAllDialoguesFromMemory())
	{
		Dialogue->GetIntNames(ParticipantName, UniqueNames);
	}

	FDlgHelper::AppendSortedSetToArray(UniqueNames, OutArray);
}

void UDlgManager::GetAllDialoguesFloatNames(FName ParticipantName, TArray<FName>& OutArray)
{
	TSet<FName> UniqueNames;
	for (const UDlgDialogue* Dialogue : GetAllDialoguesFromMemory())
	{
		Dialogue->GetFloatNames(ParticipantName, UniqueNames);
	}

	FDlgHelper::AppendSortedSetToArray(UniqueNames, OutArray);
}

void UDlgManager::GetAllDialoguesBoolNames(FName ParticipantName, TArray<FName>& OutArray)
{
	TSet<FName> UniqueNames;
	for (const UDlgDialogue* Dialogue : GetAllDialoguesFromMemory())
	{
		Dialogue->GetBoolNames(ParticipantName, UniqueNames);
	}

	FDlgHelper::AppendSortedSetToArray(UniqueNames, OutArray);
}

void UDlgManager::GetAllDialoguesNameNames(FName ParticipantName, TArray<FName>& OutArray)
{
	TSet<FName> UniqueNames;
	for (const UDlgDialogue* Dialogue : GetAllDialoguesFromMemory())
	{
		Dialogue->GetNameNames(ParticipantName, UniqueNames);
	}

	FDlgHelper::AppendSortedSetToArray(UniqueNames, OutArray);
}

void UDlgManager::GetAllDialoguesConditionNames(FName ParticipantName, TArray<FName>& OutArray)
{
	TSet<FName> UniqueNames;
	for (const UDlgDialogue* Dialogue : GetAllDialoguesFromMemory())
	{
		Dialogue->GetConditions(ParticipantName, UniqueNames);
	}

	FDlgHelper::AppendSortedSetToArray(UniqueNames, OutArray);
}

void UDlgManager::GetAllDialoguesEventNames(FName ParticipantName, TArray<FName>& OutArray)
{
	TSet<FName> UniqueNames;
	for (const UDlgDialogue* Dialogue : GetAllDialoguesFromMemory())
	{
		Dialogue->GetEvents(ParticipantName, UniqueNames);
	}

	FDlgHelper::AppendSortedSetToArray(UniqueNames, OutArray);
}

bool UDlgManager::RegisterDialogueConsoleCommands()
{
	if (!IDlgSystemModule::IsAvailable())
	{
		FDlgLogger::Get().Error(TEXT("RegisterDialogueConsoleCommands - The Dialogue System Module is NOT Loaded"));
		return false;
	}

	IDlgSystemModule::Get().RegisterConsoleCommands(GetDialogueWorld());
	return true;
}

bool UDlgManager::UnregisterDialogueConsoleCommands()
{
	if (!IDlgSystemModule::IsAvailable())
	{
		FDlgLogger::Get().Error(TEXT("UnregisterDialogueConsoleCommands - The Dialogue System Module is NOT Loaded"));
		return false;
	}

	IDlgSystemModule::Get().UnregisterConsoleCommands();
	return true;
}

void UDlgManager::GatherParticipantsRecursive(UObject* Object, TArray<UObject*>& Array, TSet<UObject*>& AlreadyVisited)
{
	if (!IsValid(Object) || AlreadyVisited.Contains(Object))
	{
		return;
	}

	AlreadyVisited.Add(Object);
	if (Object->GetClass()->ImplementsInterface(UDlgDialogueParticipant::StaticClass()))
	{
		Array.Add(Object);
	}

	for (auto* Property = Object->GetClass()->PropertyLink; Property != nullptr; Property = Property->PropertyLinkNext)
	{
		if (auto* ObjectProperty = FNYReflectionHelper::CastProperty<FNYObjectProperty>(Property))
		{
			GatherParticipantsRecursive(ObjectProperty->GetPropertyValue_InContainer(Object), Array, AlreadyVisited);
		}

		// TODO: handle containers and structs
	}
}

UWorld* UDlgManager::GetDialogueWorld()
{
	// Try to use the user set one
	if (UserWorldContextObjectPtr.IsValid())
	{
		if (auto* WorldContextObject = UserWorldContextObjectPtr.Get())
		{
			if (auto* World = WorldContextObject->GetWorld())
			{
				return World;
			}
		}
	}

	// Fallback to default autodetection
	if (GEngine)
	{
		// Get first PIE world
		// Copied from TakeUtils::GetFirstPIEWorld()
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			UWorld* World = Context.World();
			if (!World || !World->IsPlayInEditor())
				continue;

			if (World->GetNetMode() == ENetMode::NM_Standalone ||
				(World->GetNetMode() == ENetMode::NM_Client && Context.PIEInstance == 2))
			{
				return World;
			}
		}

		// Otherwise get the first Game World
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			UWorld* World = Context.World();
			if (!World || !World->IsGameWorld())
				continue;

			return World;
		}
	}

	FDlgLogger::Get().Error(TEXT("GetDialogueWorld - Could NOT find any valid world. Call SetPersistentWorldContextObject in the Being Play of your GameMode"));
	return nullptr;
}
