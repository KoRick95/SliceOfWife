#include "DisassemblingTable.h"
#include "BodyPart.h"
#include "FullBody.h"
#include "Soul.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"

// Sets default values
ADisassemblingTable::ADisassemblingTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADisassemblingTable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADisassemblingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ADisassemblingTable::DropToTable(AActor* body)
{
	if (body->IsA(AFullBody::StaticClass()) && bodyOnTable == nullptr)
	{
		// snap the body to the table
		body->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		body->SetActorLocation(this->GetActorLocation() + SnapPosition);
		body->SetActorRotation(SnapRotation, ETeleportType::ResetPhysics);

		this->bodyOnTable = body;
		return true;
	}

	return false;
}

bool ADisassemblingTable::RemoveFromTable()
{
	if (bodyOnTable != nullptr)
	{
		// detach the object from the table
		bodyOnTable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		bodyOnTable = nullptr;
		return true;
	}
	
	return false;
}

void ADisassemblingTable::Charge()
{
	if (bodyOnTable == nullptr)
		return;

	charge += ChargeRate;
	
	if (charge >= MaxCharge)
	{
		DisassembleBody();

		// reset the charge
		charge = 0;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, FString::Printf(TEXT("Charge: %f"), charge));
}

void ADisassemblingTable::DisassembleBody()
{
	// get all of the body's components
	TArray<AActor*> bodyParts;
	bodyOnTable->GetAllChildActors(bodyParts, false);

	for (int i = 0; i < bodyParts.Num(); ++i)
	{
		UClass* uClass = bodyParts[i]->GetClass();
		FTransform transform = bodyParts[i]->GetActorTransform();

		AActor* bodyPart = GetWorld()->SpawnActor(uClass, &transform);

		// if the body part has a primitive component, enable its physics
		UActorComponent* getPrimComp = bodyPart->GetComponentByClass(UPrimitiveComponent::StaticClass());
		if (getPrimComp != nullptr)
		{
			Cast<UPrimitiveComponent>(getPrimComp)->SetSimulatePhysics(true);
		}

		// assign a soul to the body part
		if (SoulBP != nullptr)
		{
			ASoul* soul = Cast<ASoul>(GetWorld()->SpawnActor(SoulBP.Get(), &FTransform::Identity));
			soul->hauntedObject = bodyPart;
		}
	}

	// destroy the base body
	bodyOnTable->Destroy();
	bodyOnTable = nullptr;
}