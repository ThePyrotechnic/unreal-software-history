// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VisualNode.h"
#include "MuseumApi.h"
#include "MuseumGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MUSEUM_API AMuseumGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	

	virtual void StartPlay() override;

public:
	AMuseumGameModeBase();

private:
	AMuseumApi* Api;

	void LogGraph(FMuseumGraph* Graph);

	const float TwoPi = 3.14159 * 2;

public:
	UPROPERTY(EditAnywhere)
	float NodeDistance;

	UPROPERTY(EditAnywhere)
	FVector TreeBase;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AVisualNode> NodeTemplate;

	void ClassAndSoftwareCallback(FMuseumGraph* Graph);
};
