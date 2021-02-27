// Project:         Advanced Locomotion System V4 BP Replication
// Original Author: Haziq Fadhil

#include "ALSCharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "ALS_BaseCharacter.h"

UALSCharacterMovementComponent::UALSCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UALSCharacterMovementComponent::OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);

	if (!CharacterOwner)
	{
		return;
	}

	// Set Movement Settings
	if (bRequestMaxWalkSpeedChange)
	{
		MaxWalkSpeed = NewMaxWalkSpeed;
		MaxWalkSpeedCrouched = NewMaxWalkSpeed;

		// Ensures server Movement Settings values updates to latest
		bRequestMaxWalkSpeedChange = false;
	}
}

void UALSCharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	if (MovementCurve)
	{
		GroundFriction = MovementCurve->GetVectorValue(GetMappedSpeed()).Z;
	}
	Super::PhysWalking(deltaTime, Iterations);
}

float UALSCharacterMovementComponent::GetMaxAcceleration() const
{
	if (!IsMovingOnGround() || !MovementCurve)
	{
		return Super::GetMaxAcceleration();
	}
	return MovementCurve->GetVectorValue(GetMappedSpeed()).X;
}

float UALSCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (!IsMovingOnGround() || !MovementCurve)
	{
		return Super::GetMaxBrakingDeceleration();
	}
	return MovementCurve->GetVectorValue(GetMappedSpeed()).Y;
}

float UALSCharacterMovementComponent::GetMappedSpeed() const
{
	// Map the character's current speed to the configured movement speeds with a range of 0-3,
	// with 0 = stopped, 1 = the Walk Speed, 2 = the Run Speed, and 3 = the Sprint Speed.
	// This allows us to vary the movement speeds but still use the mapped range in calculations for consistent results

	const float Speed = Velocity.Size2D();

	if (Speed > LocRunSpeed)
	{
		return FMath::GetMappedRangeValueClamped({LocRunSpeed, LocSprintSpeed}, {2.0f, 3.0f}, Speed);
	}

	if (Speed > LocWalkSpeed)
	{
		return FMath::GetMappedRangeValueClamped({LocWalkSpeed, LocRunSpeed}, {1.0f, 2.0f}, Speed);
	}

	return FMath::GetMappedRangeValueClamped({0.0f, LocWalkSpeed}, {0.0f, 1.0f}, Speed);
}

void UALSCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags) // Client only
{
	Super::UpdateFromCompressedFlags(Flags);

	bRequestMaxWalkSpeedChange = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

class FNetworkPredictionData_Client* UALSCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UALSCharacterMovementComponent* MutableThis = const_cast<UALSCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_My(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UALSCharacterMovementComponent::FSavedMove_My::Clear()
{
	Super::Clear();

	bSavedRequestMaxWalkSpeedChange = false;
	SavedNewMaxWalkSpeed = 0.f;
}

uint8 UALSCharacterMovementComponent::FSavedMove_My::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedRequestMaxWalkSpeedChange)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

bool UALSCharacterMovementComponent::FSavedMove_My::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character,
                                                                   float MaxDelta) const
{
	// Set which moves can be combined together. This will depend on the bit flags that are used.	
	if (bSavedRequestMaxWalkSpeedChange != ((FSavedMove_My*)&NewMove)->bSavedRequestMaxWalkSpeedChange)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UALSCharacterMovementComponent::FSavedMove_My::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel,
                                                               class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UALSCharacterMovementComponent* CharacterMovement = Cast<UALSCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		bSavedRequestMaxWalkSpeedChange = CharacterMovement->bRequestMaxWalkSpeedChange;
		SavedNewMaxWalkSpeed = CharacterMovement->NewMaxWalkSpeed;
	}
}

void UALSCharacterMovementComponent::FSavedMove_My::PrepMoveFor(class ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UALSCharacterMovementComponent* CharacterMovement = Cast<UALSCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		CharacterMovement->NewMaxWalkSpeed = SavedNewMaxWalkSpeed;
	}
}

UALSCharacterMovementComponent::FNetworkPredictionData_Client_My::FNetworkPredictionData_Client_My(
	const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UALSCharacterMovementComponent::FNetworkPredictionData_Client_My::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_My());
}

// Set Movement Settings RPC to transfer the current Movement Settings from the Owning Client to the Server
bool UALSCharacterMovementComponent::Server_SetMaxWalkingSpeed_Validate(const float UpdateMaxWalkSpeed)
{
		return true;
}

void UALSCharacterMovementComponent::Server_SetMaxWalkingSpeed_Implementation(const float UpdateMaxWalkSpeed)
{
	NewMaxWalkSpeed = UpdateMaxWalkSpeed;
}

void UALSCharacterMovementComponent::SetMaxWalkingSpeed(float UpdateMaxWalkSpeed)
{
	if (UpdateMaxWalkSpeed != NewMaxWalkSpeed)
	{
		if (PawnOwner->IsLocallyControlled())
		{
			NewMaxWalkSpeed = UpdateMaxWalkSpeed;
			Server_SetMaxWalkingSpeed(UpdateMaxWalkSpeed);
			bRequestMaxWalkSpeedChange = true;
			return;
		}
		if (!PawnOwner->HasAuthority())
		{
			MaxWalkSpeed = UpdateMaxWalkSpeed;
			MaxWalkSpeedCrouched = UpdateMaxWalkSpeed;
		}
	}
}

void UALSCharacterMovementComponent::SetMovementSettings(UCurveVector* NewMovementCurve, float NewLocWalkSpeed,
														 float NewLocRunSpeed, float NewLocSprintSpeed)
{
	if (MovementCurve != NewMovementCurve)
	{
		MovementCurve = NewMovementCurve;
	}

	if (LocWalkSpeed != NewLocWalkSpeed)
	{
		LocWalkSpeed = NewLocWalkSpeed;
	}

	if (LocRunSpeed != NewLocRunSpeed)
	{
		LocRunSpeed = NewLocRunSpeed;
	}

	if (LocSprintSpeed != NewLocSprintSpeed)
	{
		LocSprintSpeed = NewLocSprintSpeed;
	}
}