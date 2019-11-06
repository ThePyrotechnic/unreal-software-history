// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HttpService.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MuseumApi.generated.h"

USTRUCT()
struct FSoftwareNode {
	GENERATED_BODY()

	UPROPERTY() FString Uri;
	UPROPERTY() FString Label;

	FSoftwareNode() {}
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

public:
	void GetNode(FString Uri);
};
