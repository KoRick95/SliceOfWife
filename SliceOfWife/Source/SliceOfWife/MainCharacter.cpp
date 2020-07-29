// Fill out your copyright notice in the Description page of Project Settings.
#include "MainCharacter.h"
#include "DisassemblingTable.h"
#include "AssemblingTable.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//// Create the camera arm
	//CameraArm = CreateDefaultSubobject<USpringArmComponent>("Camera Arm");
	//CameraArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//CameraArm->bUsePawnControlRotation = true;

	//// Create the camera
	//Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	//Camera->AttachToComponent(CameraArm, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
	//Camera->bUsePawnControlRotation = false;

	Tags.Add("Player");

	// Create sphere collider
	SphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	SphereCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

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

	TArray<AActor*> actors;
	SphereCollider->GetOverlappingActors(actors);

	if (heldObject == nullptr)
	{
		for (int i = 0; i < actors.Num(); ++i)
		{
			AActor* actor = actors[i];

			if (actor->ActorHasTag("Pickup"))
			{
				actor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				actor->SetActorRelativeLocation(FVector(0) + PickupOffset);
				actor->SetActorEnableCollision(false);
				heldObject = actor;
				break;
			}
		}
	}
	else
	{
		bool isDropped = false;

		// detach the held object from the player
		heldObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// check nearby objects
		for (int i = 0; i < actors.Num(); ++i)
		{
			// if the assembling table is nearby
			if (actors[i]->ActorHasTag("AssemblingTable"))
			{
				// get all of its scene components
				TArray<UActorComponent*> components;
				components = actors[i]->GetComponentsByClass(USceneComponent::StaticClass());
				
				// check its components
				for (UActorComponent* component : components)
				{
					// check each component's tags
					for (int c = 0; c < component->ComponentTags.Num(); ++c)
					{
						// check the held object's tags
						for (int h = 0; h < heldObject->Tags.Num(); ++h)
						{
							// if there is a matching tag
							if (component->ComponentTags[c] == heldObject->Tags[h])
							{
								// cast the component as a scene component
								USceneComponent* sceneComponent = Cast<USceneComponent>(component);

								// snap the held object to the component
								heldObject->SetActorRotation(FRotator(0, 0, 0), ETeleportType::ResetPhysics);
								heldObject->AttachToComponent(sceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
								heldObject->SetActorLocation(sceneComponent->GetComponentLocation());
								isDropped = true;
							}
							
							if (isDropped)
								break;
						}

						if (isDropped)
							break;
					}

					if (isDropped)
						break;
				}
			}

			if (isDropped)
				break;
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
		if (actors[i]->ActorHasTag("AssemblingTable"))
		{
			AAssemblingTable* ATable = Cast<AAssemblingTable>(actors[i]);

		}
	}
}

void AMainCharacter::OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Overlapped.")));
}
