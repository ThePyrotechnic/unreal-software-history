// Fill out your copyright notice in the Description page of Project Settings.


#include "Json.h"
#include "JsonUtilities.h"
#include "MuseumApi.h"


// Sets default values
AMuseumApi::AMuseumApi() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMuseumApi::BeginPlay() {
	Super::BeginPlay();
	
	GetNodeWithRelationships("http://www.wikidata.org/entity/Q307441");
}

void AMuseumApi::GetNodeWithRelationships(FString Uri) {
	TSharedRef<IHttpRequest> Request = GetRequest(TEXT("node?uri=") + Uri + "&include_relationships=true");
	Request->OnProcessRequestComplete().BindUObject(this, &AMuseumApi::GetGraphResponse);
	Send(Request);
}

void AMuseumApi::GetGraphResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!ResponseIsValid(Response, bWasSuccessful)) return;

	FMuseumGraph Graph;
	FString JsonString = Response->GetContentAsString();
	UE_LOG(LogTemp, Warning, TEXT("NEW STRING"), *JsonString);
	if (FJsonObjectConverter::JsonObjectStringToUStruct<FMuseumGraph>(JsonString, &Graph, 0, 0)) {
		for (int a = 0; a < Graph.Nodes.Num(); a++)
			UE_LOG(LogTemp, Warning, TEXT("Node Id: %s | Type: %s | Label: %s | Uri: %s"), 
				*Graph.Nodes[a].Id, *Graph.Nodes[a].Type, *Graph.Nodes[a].Label, *Graph.Nodes[a].Uri);
		for (int a = 0; a < Graph.Relationships.Num(); a++)
			UE_LOG(LogTemp, Warning, TEXT("Relationship Id: %s | Type: %s | StartId: %s | EndId: %s"), 
				*Graph.Relationships[a].Id, *Graph.Relationships[a].Type, *Graph.Relationships[a].StartId, *Graph.Relationships[a].EndId);
	}
}
