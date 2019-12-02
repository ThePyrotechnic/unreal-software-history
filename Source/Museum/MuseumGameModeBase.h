// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VisualNode.h"
#include "MuseumApi.h"
#include "MuseumGameModeBase.generated.h"

UCLASS()
class MUSEUM_API AMuseumGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	

	virtual void StartPlay() override;

public:
	AMuseumGameModeBase();

private:
	AMuseumApi* Api;

	FRandomStream SeededRand;

	void LogGraph(FMuseumGraph* Graph);

	void CalculateWeight(FMuseumNode* Node);

	void PlaceClasses(TMap<FString, FMuseumNode*>* NodeIdMap, TSet<FString>* SpawnedClasses, TSet<FString>* Classes, float CurrentRadius, FVector2D CurrentCenter);

	float PlaceSoftware(const TMap<FString, FMuseumNode*>& NodeIdMap, AVisualNode* VisualClassNode);


	const float Pi = 3.14159;
	const float TwoPi = Pi * 2;

public:
	UPROPERTY(EditAnywhere)
	int32 RandomSeed;

	UPROPERTY(EditAnywhere)
	bool bEnableSpawning = false;

	UPROPERTY(EditAnywhere)
	float YearToUnits = 350.f;

	UPROPERTY(EditAnywhere)
	float HelixRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float NodeDistance = 350.f;

	UPROPERTY(EditAnywhere)
	FVector TreeBase;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AVisualNode> NodeTemplate;

	void ClassAndSoftwareCallback(FMuseumGraph* Graph);

	void GraphCallback(FMuseumGraph* Graph);
};
