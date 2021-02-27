// Project:         Advanced Locomotion System V4 BP Replication
// Original Author: Haziq Fadhil

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ALSCharacterMovementComponent.generated.h"

class AALS_BaseCharacter;

/**
 * Authoritative networked Character Movement
 */
UCLASS()
class ADVANCEDLOCOMOTIONSYSTEMV_API UALSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

	class FSavedMove_My : public FSavedMove_Character
	{
	public:

		typedef FSavedMove_Character Super;

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel,
		                        class FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(class ACharacter* Character) override;

		// Walk Speed Update
		uint8 bSavedRequestMaxWalkSpeedChange : 1;
		float SavedNewMaxWalkSpeed = 0.f;
	};

	class FNetworkPredictionData_Client_My : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_My(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity);

	// Movement Settings Override
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;

	// Set Movement Curve (Called in every instance)
	UFUNCTION(BlueprintCallable, Category = "Movement Settings")
	float GetMappedSpeed() const;

	// Movement Settings Variables
	UPROPERTY()
	uint8 bRequestMaxWalkSpeedChange : 1;

	UPROPERTY()
	UCurveVector* MovementCurve = nullptr;

	UPROPERTY()
	float LocWalkSpeed = 0.f;

	UPROPERTY()
	float LocRunSpeed = 0.f;
		
	UPROPERTY()
	float LocSprintSpeed = 0.f;

	UPROPERTY()
	float NewMaxWalkSpeed = 0.f;

	// Set Max Walking Speed (Called from the owning client)
	UFUNCTION(BlueprintCallable, Category = "Movement Settings")
	void SetMaxWalkingSpeed(float UpdateMaxWalkSpeed);

	UFUNCTION(reliable, Server, WithValidation)
	void Server_SetMaxWalkingSpeed(const float UpdateMaxWalkSpeed);

	// Set Movement Settings (Called from the owning client)
	UFUNCTION(BlueprintCallable, Category = "Movement Settings")
	void SetMovementSettings(UCurveVector* NewMovementCurve, float NewLocWalkSpeed,
							 float NewLocRunSpeed, float NewLocSprintSpeed);
};
