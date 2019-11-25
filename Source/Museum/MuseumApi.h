// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HttpService.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MuseumApi.generated.h"

USTRUCT()
struct FMuseumNode {
	GENERATED_BODY()

	UPROPERTY() FString Id;
	UPROPERTY() FString Uri;
	UPROPERTY() FString Label;
	UPROPERTY() FString Type;
	UPROPERTY() int32 ReleaseYear;
	UPROPERTY() int32 ReleaseMonth;
	UPROPERTY() int32 ReleaseDay;


	UPROPERTY() TArray<FString> Parents;
	UPROPERTY() TArray<FString> Children;
	UPROPERTY() TArray<FString> Software;

	UPROPERTY() int32 Weight = 0;

	FMuseumNode() {}
};

USTRUCT()
struct FMuseumRelationship {
	GENERATED_BODY()

	UPROPERTY() FString Id;
	UPROPERTY() FString Type;
	UPROPERTY() FString StartId;
	UPROPERTY() FString EndId;

	FMuseumRelationship() {}
};

USTRUCT()
struct FMuseumGraph {
	GENERATED_BODY()

	UPROPERTY() TArray<FMuseumNode> Nodes;
	UPROPERTY() TArray<FMuseumRelationship> Relationships;

	FMuseumGraph() {}
};

DECLARE_DELEGATE_OneParam(ResponseDelegate, FMuseumGraph*)

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
	void GetGraphResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, ResponseDelegate Callback);

public:
	void GetClassAndSoftware(FString ClassUri, ResponseDelegate Callback);

	void GetGraph(ResponseDelegate Callback);
};
