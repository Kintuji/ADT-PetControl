#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UPetWillComponent.h"
#include "UPetObedienceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOrderChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADT_PROJ_API UUPetObedienceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUPetObedienceComponent();

	UPROPERTY(BlueprintAssignable, Category = "Obedience|Events")
	FOnOrderChanged OnOrderChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obedience|Tuning", meta = (ClampMin = "0", ClampMax = "1"))
	float Obedience = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obedience|Tuning", meta = (ClampMin = "0.1"))
	float OrderDuration = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obedience|Decision")
	bool bHasOrder = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obedience|Decision")
	EPetAction CurrentOrder = EPetAction::Follow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obedience|Decision")
	bool bObeys = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obedience|Debug")
	float OrderedScore = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obedience|Debug")
	float HighestScore = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obedience|Debug")
	float Resistance = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obedience|Debug")
	float Margin = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "Obedience")
	void ReceiveOrder(EPetAction NewOrder);

	UFUNCTION(BlueprintCallable, Category = "Obedience")
	void ClearOrder();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TObjectPtr<UUPetWillComponent> Will;

	FTimerHandle OrderTimerHandle;

	void PrintDebug() const;
};
