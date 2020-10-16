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

	class USpringArmComponent* springArm = nullptr;
	class UCameraComponent* camera = nullptr;

	UPROPERTY(BlueprintReadOnly)
	AActor* HeldObject = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanMove = true;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float MoveSpeed = 1;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float HorizontalCameraSensitivity = 3;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float VerticalCameraSensitivity = 1;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-90", ClampMax  = "0"))
	float CameraVerticalMin = -65;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "90"))
	float CameraVerticalMax = 25;

	UPROPERTY(EditAnywhere)
	FVector PickupOffset = { 100, 0, 100 };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float axis);
	void MoveRight(float axis);
	void LookUp(float axis);
	void LookRight(float axis);

	void PickUpAndDrop();
	bool HoldObject(AActor* objectToHold);

	UFUNCTION(BlueprintCallable)
	void Interact();

	UFUNCTION(BlueprintCallable)
	bool IsHoldingObject();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
