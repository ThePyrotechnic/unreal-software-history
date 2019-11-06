// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MuseumGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MUSEUM_API AMuseumGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	

	virtual void StartPlay() override;
};
