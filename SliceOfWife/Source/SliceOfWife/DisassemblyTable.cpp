#include "DisassemblyTable.h"
#include "Accessory.h"
#include "BodyPart.h"
#include "Creature.h"
#include "Soul.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"

// Sets default values
ADisassemblyTable::ADisassemblyTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADisassemblyTable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADisassemblyTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ADisassemblyTable::DropToTable(AActor* body)
{
	if (body->IsA(ACreature::StaticClass()) && !IsOccupied())
	{
		// snap the body to the table
		body->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		body->SetActorLocation(this->GetActorLocation() + SnapPosition);
		body->SetActorRotation(SnapRotation, ETeleportType::ResetPhysics);

		this->bodyOnTable = Cast<ACreature>(body);
		Charge = 0;

		return true;
	}

	return false;
}

bool ADisassemblyTable::RemoveFromTable()
{
	if (IsOccupied())
	{
		// detach the object from the table
		bodyOnTable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		bodyOnTable = nullptr;
		return true;
	}
	
	return false;
}

bool ADisassemblyTable::ChargeMagic()
{
	if (!IsOccupied())
		return false;

	Charge += ChargeRate;
	
	if (Charge >= MaxCharge)
	{
		DisassembleBody();
		TempTutorialBool = true;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, FString::Printf(TEXT("Charge: %f"), Charge));
	return true;
}

void ADisassemblyTable::DisassembleBody()
{
	int bodyPartsCount = bodyOnTable->GetBodyPartsCount();

	for (int i = 0; i < bodyPartsCount; ++i)
	{
		UClass* uClass = bodyOnTable->bodyParts[i]->GetClass();
		FTransform transform = bodyOnTable->skeletalMeshComponent->GetComponentTransform();
		ABodyPart* splitBodyPart = Cast<ABodyPart>(GetWorld()->SpawnActor(uClass, &transform));

		if (splitBodyPart)
		{
			// enable the body part's physics
			splitBodyPart->SetPhysicsState(true);

			// assign a soul to the body part
			if (SoulBP)
			{
				ASoul* soul = Cast<ASoul>(GetWorld()->SpawnActor(SoulBP.Get(), &FTransform::Identity));
				soul->possession = splitBodyPart;
			}

			if (splitBodyPart->AccessoryBlueprint)
			{
				uClass = splitBodyPart->AccessoryBlueprint.Get();
				transform = splitBodyPart->skeletalMeshComponent->GetSocketTransform("Accessory_Socket");
				AAccessory* accessory = Cast<AAccessory>(GetWorld()->SpawnActor(uClass, &transform));

				if (accessory)
				{
					accessory->SetPhysicsState(true);
				}
			}
		}
	}

	bodyOnTable->Destroy();
	bodyOnTable = nullptr;
}

bool ADisassemblyTable::IsOccupied()
{
	return bodyOnTable != nullptr;
}

bool ADisassemblyTable::CanDisassemble(ACreature* creature)
{
	return creature != nullptr && creature->bodyParts.Num() > 0 && creature->CreatureType != ECreatureType::Custom;
}
