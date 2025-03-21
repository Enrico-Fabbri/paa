#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerPawn.generated.h"

/**
 * PlayerPawn class represents the player-controlled pawn in the game.
 * It handles camera setup and provides functionality to adjust camera height.
 */
UCLASS()
class PAA_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();

	/**
	 * Returns the current height of the camera.
	 * @return The height of the camera.
	 */
	UFUNCTION()
	float GetCameraHeight() const;
	
protected:
	/**
	 * Called when the game starts or when spawned.
	 */
	UFUNCTION()
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TWeakObjectPtr<USpringArmComponent> SpringArm; // Spring arm component for camera attachment.

	UPROPERTY()
	TWeakObjectPtr<UCameraComponent> Camera; // Camera component for player view.

	UPROPERTY(EditAnywhere)
	float CameraHeight = 2300.f; // Height of the camera above the ground.
};