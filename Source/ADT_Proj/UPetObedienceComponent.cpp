#include "UPetObedienceComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

UUPetObedienceComponent::UUPetObedienceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUPetObedienceComponent::BeginPlay()
{
	Super::BeginPlay();

	Will = GetOwner()->FindComponentByClass<UUPetWillComponent>();

	if (!Will)
	{
		UE_LOG(LogTemp, Warning, TEXT("PetObedience: no PetWill component on %s"), *GetOwner()->GetName());
	}
}

void UUPetObedienceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(OrderTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void UUPetObedienceComponent::GiveOrder(EPetAction NewOrder)
{
	if (!Will) return;

	CurrentOrder = NewOrder;

	OrderedScore = Will->Scores.FindRef(NewOrder);
	HighestScore = Will->HighestWillScore;
	Resistance   = FMath::Max(0.0f, HighestScore - OrderedScore);
	Margin       = Obedience - Resistance;
	bObeys       = Margin >= 0.0f;

	GetWorld()->GetTimerManager().ClearTimer(OrderTimerHandle);

	if (bObeys)
	{
		bHasOrder = true;

		GetWorld()->GetTimerManager().SetTimer(
			OrderTimerHandle,
			this,
			&UUPetObedienceComponent::ClearOrder,
			OrderDuration,
			false);
	}
	else
	{
		bHasOrder = false;
	}

	PrintDebug();

	OnOrderChanged.Broadcast();
}

void UUPetObedienceComponent::ClearOrder()
{
	GetWorld()->GetTimerManager().ClearTimer(OrderTimerHandle);

	if (!bHasOrder) return;

	bHasOrder = false;

	OnOrderChanged.Broadcast();
}

void UUPetObedienceComponent::PrintDebug() const
{
	if (!GEngine) return;

	GEngine->AddOnScreenDebugMessage(6, 3.0f, FColor::Magenta,
		FString::Printf(TEXT("Order: %s  ordered %.2f  highest %.2f  resist %.2f  margin %.2f"),
			*UEnum::GetValueAsString(CurrentOrder), OrderedScore, HighestScore, Resistance, Margin));

	GEngine->AddOnScreenDebugMessage(7, 3.0f, bObeys ? FColor::Green : FColor::Red,
		bObeys ? TEXT("OBEYS") : TEXT("REFUSES"));
}
