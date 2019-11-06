// Fill out your copyright notice in the Description page of Project Settings.


#include "MuseumApi.h"


// Sets default values
AMuseumApi::AMuseumApi() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMuseumApi::BeginPlay() {
	Super::BeginPlay();
	
	GetNode()
}

void AMuseumApi::GetNode(FString Uri) {
	TSharedRef<IHttpRequest> Request = GetRequest(TEXT("node?uri=") + Uri);
	Request->OnProcessRequestComplete().BindUObject(this, &AMuseumApi::GetNodeResponse);
}

void AMuseumApi::GetNodeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!ResponseIsValid(Response, bWasSuccessful)) return;

	FSoftwareNode Node;
	GetStructFromJsonString<FSoftwareNode>(Response, Node);

	UE_LOG(LogTemp, Warning, TEXT("uri: %s | label: %s"), Node.Uri, Node.Label);
}