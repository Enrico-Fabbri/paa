#include "Game/PlayerPawn.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = false; // Disable ticking for optimization.
	
	// Create and attach the spring arm component.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	RootComponent = SpringArm.Get();
	
	// Create and attach the camera component.
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm.Get());	
}

float APlayerPawn::GetCameraHeight() const
{
	return CameraHeight; // Return the current camera height.
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	// Set the spring arm length to the configured camera height.
	SpringArm->TargetArmLength = CameraHeight;
	SpringArm->bDoCollisionTest = false; // Disable collision testing for the spring arm.
}