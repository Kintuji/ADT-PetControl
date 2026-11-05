#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UPetWillComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADT_PROJ_API UUPetWillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUPetWillComponent();

	// Tunables

	UPROPERTY(EditAnywhere, Category = "Will|Tuning")
	float EvaluateInterval = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Will|Tuning")
	float DetectionRadius = 1500.0f;
	
	UPROPERTY(EditAnywhere, Category = "Will|Tuning", meta = (ClampMin = "0", ClampMax = "1"))
	float PetSelfWeight = 0.8f;
	
	UPROPERTY(EditAnywhere, Category = "Will|Tuning")
	float SearchBaseScore = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Will|Tuning")
	float FollowBaseScore = 0.3f;

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

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FTimerHandle EvaluateTimerHandle;

	void RunWill();
	void EvaluateWill();

	void GatherContext();
	void FindMostThreateningEnemy(APawn* Pet, APawn* Player);
	
	float CalculateAttackScore();
	float CalculateProtectScore();
	float CalculateSearchScore();
	float CalculateFollowScore();
	
	float DistanceToProximity(float Distance) const;

	void PrintDebug() const;
};
