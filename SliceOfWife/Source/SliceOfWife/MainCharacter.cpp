// Fill out your copyright notice in the Description page of Project Settings.
#include "MainCharacter.h"
#include "AssemblingTable.h"
#include "AssemblingSpot.h"
#include "BodyPart.h"
#include "BodyStorage.h"
#include "DisassemblingTable.h"
#include "FullBody.h"
#include "Soul.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//// Create the camera arm
	//CameraArm = CreateDefaultSubobject<USpringArmComponent>("Camera Arm");
	//CameraArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//CameraArm->bUsePawnControlRotation = false;

	//// Create the camera
	//Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	//Camera->AttachToComponent(CameraArm, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
	//Camera->bUsePawnControlRotation = false;

	// Create sphere collider
	DetectionCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	DetectionCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, RotationSpeed, 0);
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AMainCharacter::PickUpAndDrop);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::Interact);

	DetectionCollider->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnOverlapBegin);
}

bool AMainCharacter::HoldObject(AActor* objectToHold)
{
	if (objectToHold == nullptr)
	{
		return false;
	}

	// disable physics on the object
	TArray<UActorComponent*> primitiveComponents = objectToHold->GetComponentsByClass(UPrimitiveComponent::StaticClass());
	for (int i = 0; i < primitiveComponents.Num(); ++i)
	{
		Cast<UPrimitiveComponent>(primitiveComponents[i])->SetSimulatePhysics(false);
	}

	if (objectToHold->IsA(AFullBody::StaticClass()))
	{
		ACharacter* character = Cast<ACharacter>(objectToHold);

		float bodyHalfHeight = character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		// attach the object to the player
		character->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		
		// add offset to the object
		character->SetActorRelativeLocation(FVector(0, 0, bodyHalfHeight) + PickupOffset, false, nullptr, ETeleportType::ResetPhysics);
	}
	else if (objectToHold->IsA(ABodyPart::StaticClass()))
	{
		// get the object's skeletal mesh component
		ABodyPart* bodyPart = Cast<ABodyPart>(objectToHold);

		// calculate the mesh offset
		FVector meshCentre = bodyPart->GetMeshRelativeLocation();
		float meshHalfHeight = bodyPart->GetMeshRadius();
		FVector meshOffset = FVector(0, 0, meshHalfHeight) - meshCentre;

		// attach the object to the player
		objectToHold->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		
		// add the offset to the object
		objectToHold->SetActorRelativeLocation(FVector(0) + PickupOffset, false, nullptr, ETeleportType::ResetPhysics);
		objectToHold->AddActorLocalOffset(meshOffset, false, nullptr, ETeleportType::ResetPhysics);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Invalid object type.")));
		return false;
	}

	heldObject = objectToHold;
	return true;
}

void AMainCharacter::MoveForward(float Axis)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Axis);
}

void AMainCharacter::MoveRight(float Axis)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Axis);
}

void AMainCharacter::PickUpAndDrop()
{
	// get all nearby objects
	TArray<AActor*> nearbyObjects;
	DetectionCollider->GetOverlappingActors(nearbyObjects);

	// if the player is not holding anything
	if (heldObject == nullptr)
	{
		// check all nearby objects
		for (int i = 0; i < nearbyObjects.Num(); ++i)
		{
			AActor* objectToHold = nullptr;

			if (nearbyObjects[i]->IsA(ABodyStorage::StaticClass()))
			{
				objectToHold = Cast<ABodyStorage>(nearbyObjects[i])->TakeBody();
			}
			else if (nearbyObjects[i]->IsA(AFullBody::StaticClass()))
			{
				objectToHold = nearbyObjects[i];
			}
			else if (nearbyObjects[i]->IsA(ABodyPart::StaticClass()))
			{
				ABodyPart* aBodyPart = Cast<ABodyPart>(nearbyObjects[i]);

				// if the body part is attached to a body
				if (aBodyPart->attachedBody != nullptr)
				{
					objectToHold = aBodyPart->attachedBody;
				}
				else
				{
					objectToHold = aBodyPart;
				}
			}

			if (objectToHold != nullptr)
			{
				// get the object's attach parent
				AActor* objectAttachParent = objectToHold->GetAttachParentActor();

				// if the object has an attach parent
				if (objectAttachParent != nullptr)
				{
					if (objectAttachParent->IsA(AAssemblingTable::StaticClass()))
					{
						// remove it from the assembling table
						Cast<AAssemblingTable>(objectAttachParent)->RemoveFromTable(Cast<ABodyPart>(objectToHold));
					}
					else if (objectAttachParent->IsA(ADisassemblingTable::StaticClass()))
					{
						// remove it from the disassembling table
						Cast<ADisassemblingTable>(objectAttachParent)->RemoveFromTable();
					}
				}

				// get player to hold the object
				HoldObject(objectToHold);
				break;
			}
		}
	}
	else
	{
		bool isSnapped = false;

		// detach the held object from the player
		heldObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// check nearby objects
		for (int i = 0; i < nearbyObjects.Num(); ++i)
		{
			if (nearbyObjects[i]->IsA(AAssemblingSpot::StaticClass()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Assembling spot found.")));
				isSnapped = Cast<AAssemblingSpot>(nearbyObjects[i])->DropToTable(Cast<ABodyPart>(heldObject));
				break;
			}
			else if (nearbyObjects[i]->IsA(ADisassemblingTable::StaticClass()))
			{
				isSnapped = Cast<ADisassemblingTable>(nearbyObjects[i])->DropToTable(heldObject);
				break;
			}
		}

		if (!isSnapped)
		{
			// enable physics
			TArray<UActorComponent*> components = heldObject->GetComponentsByClass(UPrimitiveComponent::StaticClass());
			for (int i = 0; i < components.Num(); ++i)
			{
				Cast<UPrimitiveComponent>(components[i])->SetSimulatePhysics(true);
			}
		}

		heldObject = nullptr;
	}
}

void AMainCharacter::Interact()
{
	TArray<AActor*> nearbyObjects;
	DetectionCollider->GetOverlappingActors(nearbyObjects);

	for (int i = 0; i < nearbyObjects.Num(); ++i)
	{
		if (nearbyObjects[i]->IsA(ADisassemblingTable::StaticClass()))
		{
			Cast<ADisassemblingTable>(nearbyObjects[i])->Charge();
			break;
		}

		if (nearbyObjects[i]->IsA(AAssemblingSpot::StaticClass()))
		{
			Cast<AAssemblingSpot>(nearbyObjects[i])->BeginSewing();
			break;
		}

		if (nearbyObjects[i]->IsA(ASoul::StaticClass()))
		{
			Cast<ASoul>(nearbyObjects[i])->Despawn();
			break;
		}
	}
}

void AMainCharacter::OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Overlapped.")));
}
