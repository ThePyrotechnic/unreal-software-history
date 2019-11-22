// Fill out your copyright notice in the Description page of Project Settings.


#include "MuseumApi.h"
#include "Json.h"
#include "JsonUtilities.h"


// Sets default values
AMuseumApi::AMuseumApi() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMuseumApi::BeginPlay() {
	Super::BeginPlay();
}

void AMuseumApi::GetNodeWithRelationships(FString Uri, ResponseDelegate Callback) {
	TSharedRef<IHttpRequest> Request = GetRequest(TEXT("node?uri=") + Uri);
	Request->OnProcessRequestComplete().BindUObject(this, &AMuseumApi::GetGraphResponse, Callback);
	Send(Request);
}

void AMuseumApi::GetClassAndSoftware(FString ClassUri, ResponseDelegate Callback) {
	TSharedRef<IHttpRequest> Request = GetRequest(TEXT("class?uri=") + ClassUri);
	Request->OnProcessRequestComplete().BindUObject(this, &AMuseumApi::GetGraphResponse, Callback);
	Send(Request);
}

void AMuseumApi::GetGraphResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, ResponseDelegate Callback) {
	if (!ResponseIsValid(Response, bWasSuccessful)) return;
	
	FString JsonString = Response->GetContentAsString();

	FMuseumGraph* Graph = new FMuseumGraph();

	if (FJsonObjectConverter::JsonObjectStringToUStruct<FMuseumGraph>(JsonString, Graph, 0, 0)) {
		Callback.Execute(Graph);
	}
}
