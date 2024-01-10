// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class SCJ_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:

	AEnemyController(FObjectInitializer const& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* pawn) override;

protected:



public:

	UFUNCTION()
	void SetPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);

	UBlackboardComponent* GetBlackboard() const;

private:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(EditAnywhere)
	UBlackboardComponent* BlackboardtoUse;

	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditAnywhere)
	float AISightRadius = 500.f;
	UPROPERTY(EditAnywhere)
	float AILoseSightRadius = 500.f;
	UPROPERTY(EditAnywhere)
	float AIFieldOfView = 70.f;
	UPROPERTY(EditAnywhere)
	float AISightAge = 5.f;

};
