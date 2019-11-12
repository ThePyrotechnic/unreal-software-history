// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HttpService.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MuseumApi.generated.h"

USTRUCT()
struct FGraphNode {
	GENERATED_BODY()

	UPROPERTY() FString Id;
	UPROPERTY() FString Uri;
	UPROPERTY() FString Label;
	UPROPERTY() FString Type;

	FGraphNode() {}
};

USTRUCT()
struct FRelationship {
	GENERATED_BODY()

	UPROPERTY() FString Id;
	UPROPERTY() FString Label;
	UPROPERTY() FString StartId;
	UPROPERTY() FString EndId;

	FRelationship() {}	
};

USTRUCT()
struct FMuseumGraph {
	GENERATED_BODY()

	UPROPERTY() TArray<FGraphNode> Nodes;
	UPROPERTY() TArray<FRelationship> Relationships;

	FMuseumGraph() {}
};

UCLASS()
class MUSEUM_API AMuseumApi : public AHttpService
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMuseumApi();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void GetNodeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void GetGraphResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
public:
	void GetNode(FString Uri);

	void GetNodeWithRelationships(FString Uri);
};
