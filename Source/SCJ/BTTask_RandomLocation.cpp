// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RandomLocation.h"
#include "EnemyController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BB_Keys.h"

UBTTask_RandomLocation::UBTTask_RandomLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("RandomLocation");
}

EBTNodeResult::Type UBTTask_RandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	auto Enemy = Controller->GetPawn();

	if (Enemy == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	UNavigationSystemV1* const NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector Origin = Enemy->GetActorLocation();
	FNavLocation NextPatrol;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, SearchRadius, NextPatrol, nullptr))
	{
		Controller->GetBlackboard()->SetValueAsVector(BB_Keys::Target_Location, NextPatrol.Location);
	}
	else
	{
		Controller->GetBlackboard()->SetValueAsVector(BB_Keys::Target_Location, Enemy->GetActorLocation());
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
