// Project:         Advanced Locomotion System V4 BP Replication
// Original Author: Haziq Fadhil

#include "ALS_BaseCharacter.h"

#include "ALSCharacterMovementComponent.h"

// Sets default values
AALS_BaseCharacter::AALS_BaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UALSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
}

void AALS_BaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MyCharacterMovementComponent = Cast<UALSCharacterMovementComponent>(Super::GetMovementComponent());
}

