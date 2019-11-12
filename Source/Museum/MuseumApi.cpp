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
	
	GetNodeWithRelationships("http://www.wikidata.org/entity/Q307441");
}

void AMuseumApi::GetNode(FString Uri) {
	TSharedRef<IHttpRequest> Request = GetRequest(TEXT("node?uri=") + Uri);
	Request->OnProcessRequestComplete().BindUObject(this, &AMuseumApi::GetNodeResponse);
	Send(Request);
}

void AMuseumApi::GetNodeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!ResponseIsValid(Response, bWasSuccessful)) return;

	FGraphNode Node;
	GetStructFromJsonString<FGraphNode>(Response, Node);

	UE_LOG(LogTemp, Warning, TEXT("Test complete"));
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, 
			TEXT("uri: ") + Node.Uri + TEXT(" | Label: ") + Node.Label + TEXT(" | Id: ") + Node.Id + TEXT(" | Type: ") + Node.Type);
	}
}

void AMuseumApi::GetNodeWithRelationships(FString Uri) {
	TSharedRef<IHttpRequest> Request = GetRequest(TEXT("node?uri=") + Uri + "&include_relationships=true");
	Request->OnProcessRequestComplete().BindUObject(this, &AMuseumApi::GetGraphResponse);
	Send(Request);
}

void AMuseumApi::GetGraphResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!ResponseIsValid(Response, bWasSuccessful)) return;

	FMuseumGraph Graph;
	GetStructFromJsonString<FMuseumGraph>(Response, Graph);
	// TODO Need to explicity parse arrays
	UE_LOG(LogTemp, Warning, TEXT("Test complete"));
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, 
			TEXT("node count: ") + FString::FromInt(Graph.Relationships.Num()) + TEXT("relation count:") + FString::FromInt(Graph.Nodes.Num()));
	}
}
