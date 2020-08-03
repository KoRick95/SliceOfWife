// Fill out your copyright notice in the Description page of Project Settings.
#include "MainCharacter.h"
#include "AssemblingTable.h"
#include "BodyStorage.h"
#include "DisassemblingTable.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the camera arm
	CameraArm = CreateDefaultSubobject<USpringArmComponent>("Camera Arm");
	CameraArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CameraArm->bUsePawnControlRotation = false;

	// Create the camera
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->AttachToComponent(CameraArm, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// Create sphere collider
	SphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	SphereCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Tags.Add("Player");

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
	//PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AMainCharacter::PickUp);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::Interact);

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnOverlapBegin);
}

void AMainCharacter::HoldObject(AActor* objectToHold)
{
	// attach the object to the player
	objectToHold->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	objectToHold->SetActorRelativeLocation(FVector(0) + PickupOffset);
	objectToHold->SetActorEnableCollision(false);
	heldObject = objectToHold;
}

void AMainCharacter::MoveForward(float Axis)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Axis);
	//AddActorLocalRotation(FQuat(Rotation));
}

void AMainCharacter::MoveRight(float Axis)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FRotator Rotation = Controller->GetControlRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Axis);
}

void AMainCharacter::PickUp()
{
	/*TArray<FHitResult> HitResults;
	FVector Start = this->GetActorLocation();
	FVector End = this->GetActorLocation() + FVector(1, 1, 1);
	FCollisionShape ColShape = FCollisionShape::MakeSphere(DetectionRadius);
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Called.")));
	DrawDebugSphere(GetWorld(), GetActorLocation(), DetectionRadius, 64, FColor::Red, true, -1, 0, 8);
	bool isHit = GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, ECC_Visibility, ColShape);

	if (isHit)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit something.")));
		for (int i = 0; i < HitResults.Num(); ++i)
		{
			AActor* HitActor = HitResults[i].GetActor();
			
			if (HitActor->ActorHasTag("Pickup"))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Detected.")));
				HitActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				HitActor->AddActorLocalOffset(PickupOffset);
				break;
			}
		}
	}*/

	// get all nearby objects
	TArray<AActor*> nearbyObjects;
	SphereCollider->GetOverlappingActors(nearbyObjects);

	// if the player is not holding anything
	if (heldObject == nullptr)
	{
		// check all nearby objects
		for (int i = 0; i < nearbyObjects.Num(); ++i)
		{
			if (nearbyObjects[i]->ActorHasTag("BodyStorage"))
			{
				ABodyStorage* bodyStorage = Cast<ABodyStorage>(nearbyObjects[i]);
				HoldObject(bodyStorage->TakeBody());
				break;
			}

			// if an object has the pickup tag
			if (nearbyObjects[i]->ActorHasTag("Pickup"))
			{
				// if the object is attached to a parent object
				if (nearbyObjects[i]->GetAttachParentActor() != nullptr)
				{
					// if that object's parent is an assembling table
					if (nearbyObjects[i]->GetAttachParentActor()->ActorHasTag("AssemblingTable"))
					{
						// remove it from the table
						AAssemblingTable* aTable = Cast<AAssemblingTable>(nearbyObjects[i]->GetAttachParentActor());
						aTable->RemoveFromTable(nearbyObjects[i]);
					}
				}

				// get player to hold the object
				HoldObject(nearbyObjects[i]);
				break;
			}
		}
	}
	else
	{
		// detach the held object from the player
		heldObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// check nearby objects
		for (int i = 0; i < nearbyObjects.Num(); ++i)
		{
			// if the assembling table is nearby
			if (nearbyObjects[i]->ActorHasTag("AssemblingTable"))
			{
				// call the table's object snapping function
				AAssemblingTable* aTable = Cast<AAssemblingTable>(nearbyObjects[i]);
				aTable->DropToTable(heldObject);
				break;
			}

			if (nearbyObjects[i]->ActorHasTag("DisassemblingTable"))
			{
				ADisassemblingTable* dTable = Cast<ADisassemblingTable>(nearbyObjects[i]);
				dTable->DropToTable(heldObject);
				break;
			}
		}
		
		heldObject->SetActorEnableCollision(true);
		heldObject = nullptr;
	}
}

void AMainCharacter::Interact()
{
	TArray<AActor*> actors;
	SphereCollider->GetOverlappingActors(actors);

	for (int i = 0; i < actors.Num(); ++i)
	{
		if (actors[i]->ActorHasTag("DisassemblingTable"))
		{
			ADisassemblingTable* dTable = Cast<ADisassemblingTable>(actors[i]);
			dTable->Charge();
			break;
		}

		if (actors[i]->ActorHasTag("AssemblingTable"))
		{
			AAssemblingTable* aTable = Cast<AAssemblingTable>(actors[i]);
			aTable->Animate();
			break;
		}
	}
}

void AMainCharacter::OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Overlapped.")));
}
