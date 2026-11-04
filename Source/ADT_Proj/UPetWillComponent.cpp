
#include "UPetWillComponent.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "HealthInfo.h"
#include  "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UUPetWillComponent::UUPetWillComponent()
{
	// No Tick: the timer below does the work every EvaluateInterval seconds
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UUPetWillComponent::BeginPlay()
{
	Super::BeginPlay();

	// Like InvokeRepeating("EvaluateWill", 0, EvaluateInterval) in Unity
	GetWorld()->GetTimerManager().SetTimer(
		EvaluateTimerHandle,               // filled with the timer's ID
		this,                              // object that owns the function
		&UUPetWillComponent::EvaluateWill, // function to call
		EvaluateInterval,                  // every X seconds
		true);                           // loop
	

	
}


// Called when the game ends or the component is destroyed
void UUPetWillComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Like CancelInvoke() in Unity
	GetWorld()->GetTimerManager().ClearTimer(EvaluateTimerHandle);

	Super::EndPlay(EndPlayReason);
}


void UUPetWillComponent::GatherContext()
{
	AController* Controller = Cast<AController>(GetOwner());
	if (!Controller) return;
	
	APawn* Pawn = Controller->GetPawn();
	if (!Pawn) return;

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player) return;

	if (Player->Implements<UHealthInfo>())
	{
		playerHealth = IHealthInfo::Execute_GetHealthPercent(Player);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(2, EvaluateInterval + 0.1f, FColor::Green,
				FString::Printf(TEXT("playerHealth %.2f"), playerHealth));
		}
	}
	if (Pawn->Implements<UHealthInfo>())
	{
		petHealth = IHealthInfo::Execute_GetHealthPercent(Pawn);
	}
	
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Pawn);
	ActorsToIgnore.Add(Player);
	
	TArray<AActor*> FoundActors;
	
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		Player->GetActorLocation(),
		DetectionRadius,
		ObjectTypes,
		nullptr,
		ActorsToIgnore,
		FoundActors
		);
	
	if (FoundActors.Num() > 0)
	{
		for(int i = 0; i < FoundActors.Num(); i++)
		{
			if (!FoundActors[i]->ActorHasTag("Enemy")) return;
			
			//AActor* NearestEnemy = nullptr;
			AActor* NextEnemy =  FoundActors[i];
			
			float DistanceToPlayer = FVector::Distance(Pawn->GetActorLocation(), NextEnemy->GetActorLocation());
			
			//float Distance = FVector::Distance()
			//if (NextEnemy.)
			AActor* NearestEnemy = FoundActors[i];
			
			
		}
	}
	else
	{
		
	}
}

void UUPetWillComponent::EvaluateWill()
{
	GatherContext();
	
	attackScore = HandleAttackScore();
	protectScore = HandleProtectScore();
	searchScore = HandleSearchScore();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, EvaluateInterval + 0.1f, FColor::Green,
			FString::Printf(TEXT("Will | Atk %.0f  Prot %.0f  Search %.0f  Follow %.0f"),
				attackScore, protectScore, searchScore, followScore));
	}
}


float UUPetWillComponent::HandleAttackScore()
{
	//float attackScore = 0.0f;
	
	//attackScore =   
	
	return 0.0f;
}

float UUPetWillComponent::HandleProtectScore()
{
	return 0.0f;
}

float UUPetWillComponent::HandleSearchScore()
{
	return 0.0f;
}
