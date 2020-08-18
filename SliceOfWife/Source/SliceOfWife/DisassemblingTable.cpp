#include "DisassemblingTable.h"
#include "BodyPart.h"
#include "Soul.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"

// Sets default values
ADisassemblingTable::ADisassemblingTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("DisassemblingTable");
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
	if (body->ActorHasTag(TagToCheck) && bodyOnTable == nullptr)
	{
		// snap the body to the table
		body->SetActorRotation(SnapRotation, ETeleportType::ResetPhysics);
		body->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		body->SetActorLocation(this->GetActorLocation() + SnapPosition);

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
		// get all of the body's components
		TArray<AActor*> bodyParts;
		bodyOnTable->GetAllChildActors(bodyParts, false);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("BodyParts = %i"), bodyParts.Num()));

		// get all of the table's components
		TArray<UActorComponent*> tableComponents;
		tableComponents = this->GetComponentsByClass(USceneComponent::StaticClass());

		// check each table component
		for (int tc = 0; tc < tableComponents.Num(); ++tc)
		{
			// check each body component
			for (int bp = 0; bp < bodyParts.Num(); ++bp)
			{
				// check each table component's tags
				for (int tt = 0; tt < tableComponents[tc]->ComponentTags.Num(); ++tt)
				{
					// if the body part has a matching tag with the table component
					if (bodyParts[bp]->ActorHasTag(tableComponents[tc]->ComponentTags[tt]))
					{
						// detach the body part from the body
						UClass* uClass = bodyParts[bp]->GetClass();
						FTransform transform = bodyParts[bp]->GetActorTransform();
						FActorSpawnParameters spawnParams;
						AActor* bodyPart = GetWorld()->SpawnActor(uClass, &transform, spawnParams);

						// snap the body part to the table component
						bodyPart->SetActorLocation(Cast<USceneComponent>(tableComponents[tc])->GetComponentLocation());

						// if the body part has a primitive component, enable its physics
						UActorComponent* getPrimComp = bodyPart->GetComponentByClass(UPrimitiveComponent::StaticClass());
						if (getPrimComp != nullptr)
						{
							Cast<UPrimitiveComponent>(getPrimComp)->SetSimulatePhysics(true);
						}

						// assign a soul to the body part
						if (SoulBP != nullptr)
						{
							ASoul* soul = Cast<ASoul>(GetWorld()->SpawnActor(SoulBP.Get(), &FTransform::Identity, spawnParams));
							soul->hauntedObject = bodyPart;
						}
					}
				}
			}
		}

		// destroy the base body
		bodyOnTable->Destroy();
		bodyOnTable = nullptr;

		// reset the charge
		charge = 0;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Charge: %f"), charge));
}

