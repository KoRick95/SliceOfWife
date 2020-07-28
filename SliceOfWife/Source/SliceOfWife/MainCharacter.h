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

	//UPROPERTY(EditAnywhere, Category = "Camera")
	//class USpringArmComponent* CameraArm;

	//UPROPERTY(EditAnywhere, Category = "Camera")
	//class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float RotationSpeed = 100;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION() void MoveForward(float Axis);
	UFUNCTION() void MoveRight(float Axis);
	void Raycast();
};
