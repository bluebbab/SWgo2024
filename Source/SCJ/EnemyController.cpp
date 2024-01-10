// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "SCJCharacter.h"
#include "BB_Keys.h"
#include "EnemyCharacter.h"

AEnemyController::AEnemyController(FObjectInitializer const& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Blueprints/Enemy/AI/BT_PatrolEnemy.BT_PatrolEnemy'"));
	if (BTObject.Succeeded())
	{
		BehaviorTree = BTObject.Object;
	}
	BehaviorTreeComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTreeComp"));
	BlackboardtoUse = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	SetPerceptionSystem();
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	RunBehaviorTree(BehaviorTree);
}

void AEnemyController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	ASCJCharacter* Player = Cast<ASCJCharacter>(Actor);
	if (Player != nullptr)
	{
		if (Player->ActorHasTag(TEXT("UnHide")))
		{
			BlackboardtoUse->SetValueAsBool(BB_Keys::Can_See_Player, Stimulus.WasSuccessfullySensed());
		}
	}
}

UBlackboardComponent* AEnemyController::GetBlackboard() const
{
	return Blackboard;
}

void AEnemyController::SetPerceptionSystem()
{
	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception")));

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnTargetDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}
