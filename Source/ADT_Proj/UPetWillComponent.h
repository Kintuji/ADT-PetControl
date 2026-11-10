#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UPetWillComponent.generated.h"

UENUM(BlueprintType)
enum class EPetAction : uint8
{
	Follow UMETA(DisplayName = "Follow"),
	Attack UMETA(DisplayName = "Attack"),
	Protect UMETA(DisplayName = "Protect"),
	Search UMETA(DisplayName = "Search")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADT_PROJ_API UUPetWillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUPetWillComponent();

	//ENUM
	UPROPERTY(BlueprintReadOnly, Category = "ENUM")
	EPetAction CurrentAction = EPetAction::Follow;
	
	UPROPERTY(VisibleAnywhere, Category = "ENUM")
	EPetAction HighestWillAction = EPetAction::Follow;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ENUU")
	TMap<EPetAction, float> Scores;
	
	//Tunables

	UPROPERTY(EditAnywhere, Category = "Will|Tuning")
	float EvaluateInterval = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Will|Tuning")
	float DetectionRadius = 1500.0f;
	
	UPROPERTY(EditAnywhere, Category = "Will|Tuning", meta = (ClampMin = "0", ClampMax = "1"))
	float PetSelfWeight = 0.8f;
	
	UPROPERTY(EditAnywhere, Category = "Will|Tuning")
	float SearchBaseScore = 0.45f;

	UPROPERTY(EditAnywhere, Category = "Will|Tuning")
	float FollowBaseScore = 0.55f;

	//Context

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Context")
	bool bHasEnemy = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Context")
	TObjectPtr<AActor> TargetEnemy = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Context")
	float BestThreat = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Context")
	float EnemyDistanceToPlayer = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Context")
	float EnemyDistanceToPet = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Context")
	float PlayerHealth = 1.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Context")
	float PetHealth = 1.0f;

	// Scores

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Scores")
	float AttackScore = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Scores")
	float ProtectScore = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Scores")
	float SearchScore = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Will|Scores")
	float FollowScore = 0.0f;
	
	// Anti Flick
	UPROPERTY(EditAnywhere, Category = "AntiFlick")
	float SwitchMargin = 0.1f;
	
	UPROPERTY(EditAnywhere, Category = "AntiFlick", meta = (ClampMin = "0.1"))
	float SwitchTime = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "AntiFlick")
	float CurrentMargin = 0.0f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FTimerHandle EvaluateTimerHandle;

	void RunWill();
	void EvaluateWill();
	
	EPetAction CalculateAction();
	float LastSwitchTime = 0.0f;
	void SwitchAction(EPetAction PetAction);
	
	void GatherContext();
	void FindMostThreateningEnemy(APawn* Pet, APawn* Player);
	
	void UpdateScoreList();
	
	float CalculateAttackScore();
	float CalculateProtectScore();
	float CalculateSearchScore();
	float CalculateFollowScore();
	
	float DistanceToProximity(float Distance) const;

	void PrintDebug() const;
};


