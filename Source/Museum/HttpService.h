// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Online/HTTP/Public/Http.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpService.generated.h"

UCLASS(Blueprintable, hideCategories = (Rendering, Replication, Input, Actor, "Actor Tick"))
class MUSEUM_API AHttpService : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHttpService();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	FHttpModule* Http;

	TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute);
	void SetRequestHeaders(TSharedRef<IHttpRequest>& Request);

	TSharedRef<IHttpRequest> GetRequest(FString Subroute);
	TSharedRef<IHttpRequest> PostRequest(FString Subroute, FString ContentJsonString);
	void Send(TSharedRef<IHttpRequest>& Request);

	bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);

public:
	UPROPERTY(EditAnywhere)
	FString ApiBaseUrl = "http://localhost:8080/";
};
