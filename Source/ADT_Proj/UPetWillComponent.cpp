#include "UPetWillComponent.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HealthInfo.h"

UUPetWillComponent::UUPetWillComponent()
{
	// No Tick: the timer below does the work every EvaluateInterval seconds
	PrimaryComponentTick.bCanEverTick = false;
}


void UUPetWillComponent::BeginPlay()
{
	Super::BeginPlay();

	RunWill();
}

void UUPetWillComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(EvaluateTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void UUPetWillComponent::RunWill()
{
	GetWorld()->GetTimerManager().SetTimer(
		EvaluateTimerHandle,
		this,
		&UUPetWillComponent::EvaluateWill,
		EvaluateInterval,
		true);
}

void UUPetWillComponent::EvaluateWill()
{
	GatherContext();
	
	AttackScore  = CalculateAttackScore();
	ProtectScore = CalculateProtectScore();
	SearchScore  = CalculateSearchScore();
	FollowScore  = CalculateFollowScore();
	
	HighestWillAction = CalculateAction();
	
	SwitchAction(HighestWillAction);

	PrintDebug();
}

EPetAction UUPetWillComponent::CalculateAction()
{
	EPetAction HighestAction = EPetAction::Follow;
	float HighestScore = 0.0f;
	
	UpdateScoreList();
	
	for (const TPair<EPetAction, float>& Pair : Scores)
	{
		if (Pair.Value > HighestScore)
		{
			HighestScore = Pair.Value;
			HighestAction = Pair.Key;
			
			HighestWillScore = HighestScore;
		}
	}
	
	return HighestAction;
}

void UUPetWillComponent::SwitchAction(EPetAction PetAction)
{
	const float Now = GetWorld()->GetTimeSeconds();
	const float Elapsed = Now - LastSwitchTime;
	const float T = FMath::Clamp(Elapsed / SwitchTime, 0.0f, 1.0f);

	CurrentMargin = SwitchMargin * (1.0f - T);

	if (PetAction == CurrentAction) return;

	const float NewScore = Scores.FindRef(PetAction);
	const float CurrentScore = Scores.FindRef(CurrentAction);

	if (NewScore > CurrentScore + CurrentMargin)
	{
		CurrentAction = PetAction;
		LastSwitchTime = Now;
		OnActionChanged.Broadcast(CurrentAction);
	}
}

void UUPetWillComponent::GatherContext()
{
	const AController* Controller = Cast<AController>(GetOwner());
	if (!Controller) return;

	APawn* Pet = Controller->GetPawn();
	if (!Pet) return;

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player) return;
	
	if (Player->Implements<UHealthInfo>())
	{
		PlayerHealth = IHealthInfo::Execute_GetHealthPercent(Player);
	}
	if (Pet->Implements<UHealthInfo>())
	{
		PetHealth = IHealthInfo::Execute_GetHealthPercent(Pet);
	}

	FindMostThreateningEnemy(Pet, Player);
}

void UUPetWillComponent::FindMostThreateningEnemy(APawn* Pet, APawn* Player)
{
	TargetEnemy = nullptr;
	BestThreat = 0.0f;
	bHasEnemy = false;
	EnemyDistanceToPlayer = 0.0f;
	EnemyDistanceToPet = 0.0f;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Pet);
	ActorsToIgnore.Add(Player);

	TArray<AActor*> FoundActors;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		Player->GetActorLocation(),
		DetectionRadius,
		ObjectTypes,
		nullptr,
		ActorsToIgnore,
		FoundActors);
	
	for (AActor* Actor : FoundActors)
	{
		if (!Actor || !Actor->ActorHasTag("Enemy")) continue;

		const float DistToPlayer = FVector::Distance(Actor->GetActorLocation(), Player->GetActorLocation());
		const float DistToPet    = FVector::Distance(Actor->GetActorLocation(), Pet->GetActorLocation());

		const float Threat = FMath::Max(
			DistanceToProximity(DistToPlayer),
			DistanceToProximity(DistToPet) * PetSelfWeight);

		if (Threat > BestThreat)
		{
			BestThreat = Threat;
			TargetEnemy = Actor;
			EnemyDistanceToPlayer = DistToPlayer;
			EnemyDistanceToPet = DistToPet;
		}
	}

	bHasEnemy = (TargetEnemy != nullptr);
}

void UUPetWillComponent::UpdateScoreList()
{
	Scores.Add(EPetAction::Attack, AttackScore);
	Scores.Add(EPetAction::Protect, ProtectScore);
	Scores.Add(EPetAction::Search, SearchScore);
	Scores.Add(EPetAction::Follow, FollowScore);
}

float UUPetWillComponent::DistanceToProximity(float Distance) const
{
	if (DetectionRadius <= 0.0f) return 0.0f;  

	return 1.0f - FMath::Clamp(Distance / DetectionRadius, 0.0f, 1.0f);
}

float UUPetWillComponent::CalculateAttackScore() 
{
	if (!bHasEnemy) return 0.0f;

	return  BestThreat * PlayerHealth;
}

float UUPetWillComponent::CalculateProtectScore() 
{
	if (!bHasEnemy) return 0.0f;
	
	return (1.0f - PlayerHealth);
}

float UUPetWillComponent::CalculateSearchScore()
{
	return bHasEnemy ? 0.0f : SearchBaseScore;
}

float UUPetWillComponent::CalculateFollowScore()
{
	return FollowBaseScore;
}


void UUPetWillComponent::PrintDebug() const
{
	if (!GEngine) return;

	const float Time = EvaluateInterval + 0.1f;

	GEngine->AddOnScreenDebugMessage(1, Time, FColor::Green,
		FString::Printf(TEXT("Will | Atk %.2f  Prot %.2f  Search %.2f  Follow %.2f"),
			AttackScore, ProtectScore, SearchScore, FollowScore));

	GEngine->AddOnScreenDebugMessage(2, Time, FColor::Green,
		FString::Printf(TEXT("HP | Player %.2f  Pet %.2f"), PlayerHealth, PetHealth));

	GEngine->AddOnScreenDebugMessage(3, Time, FColor::Yellow,
		FString::Printf(TEXT("Target: %s  threat %.2f  dPlayer %.0f  dPet %.0f"),
			TargetEnemy ? *TargetEnemy->GetName() : TEXT("none"),
			BestThreat, EnemyDistanceToPlayer, EnemyDistanceToPet));

	FString ActionName = UEnum::GetValueAsString(CurrentAction);

	GEngine->AddOnScreenDebugMessage(4, Time, FColor::Cyan,
		FString::Printf(TEXT("Current Action: %s"), *ActionName));

	GEngine->AddOnScreenDebugMessage(5, Time, FColor::Orange,
		FString::Printf(TEXT("Want: %s  Margin %.3f"),
			*UEnum::GetValueAsString(HighestWillAction), CurrentMargin));
}
