// Project:         Advanced Locomotion System V4 BP Replication
// Original Author: Haziq Fadhil

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALS_BaseCharacter.generated.h"

UCLASS()
class ADVANCEDLOCOMOTIONSYSTEMV_API AALS_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AALS_BaseCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE class UALSCharacterMovementComponent* GetMyMovementComponent() const { return MyCharacterMovementComponent; }

protected:

	virtual void PostInitializeComponents() override;

	UALSCharacterMovementComponent* MyCharacterMovementComponent;

};
