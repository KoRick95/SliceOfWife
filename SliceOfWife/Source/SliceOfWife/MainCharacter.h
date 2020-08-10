// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UCLASS()
class SLICEOFWIFE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	AActor* heldObject = nullptr;

	UPROPERTY(EditAnywhere, Category = "Camera")
	class USpringArmComponent* CameraArm;

	UPROPERTY(EditAnywhere, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Colliders")
	class USphereComponent* SphereCollider;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 100;

	UPROPERTY(EditAnywhere, Category = "Detection")
	float DetectionRadius = 100;

	UPROPERTY(EditAnywhere, Category = "PickUp")
	FVector PickupOffset = { 100, 0, 0 };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HoldObject(AActor* objectToHold);
	void HoldBody(ACharacter* body);

	void MoveForward(float Axis);
	void MoveRight(float Axis);
	void PickUp();
	void Interact();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
