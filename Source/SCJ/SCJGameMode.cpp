// Copyright Epic Games, Inc. All Rights Reserved.

#include "SCJGameMode.h"
#include "SCJCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASCJGameMode::ASCJGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
