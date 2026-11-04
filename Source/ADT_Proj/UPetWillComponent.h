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

	// How often (seconds) the dog re-thinks what it wants
	UPROPERTY(EditAnywhere, Category = "Will")
	float EvaluateInterval = 0.2f;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score System")
	float attackScore = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score System")
	float protectScore = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score System")
	float searchScore = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score System")
	float followScore = 30.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
	float enemyDistanceToPlayer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
	float enemyDistanceToPet = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
	float playerHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
	float petHealth = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
	float DetectionRadius = 100.0f;

	//scapePaths ? Stamina ?

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// "Ticket" of our repeating timer, so we can stop it later
	FTimerHandle EvaluateTimerHandle;

	void GatherContext();
	// Runs every EvaluateInterval seconds (our "slow Tick")
	void EvaluateWill();

	float HandleAttackScore();
	float HandleProtectScore();
	float HandleSearchScore();
};
