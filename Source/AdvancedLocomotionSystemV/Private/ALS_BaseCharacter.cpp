// Project:         Advanced Locomotion System V4 BP Replication
// Original Author: Haziq Fadhil

#include "ALS_BaseCharacter.h"

#include "ALSCharacterMovementComponent.h"

// Sets default values
AALS_BaseCharacter::AALS_BaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UALSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AALS_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AALS_BaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MyCharacterMovementComponent = Cast<UALSCharacterMovementComponent>(Super::GetMovementComponent());
}

// Called every frame
void AALS_BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AALS_BaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

