// Fill out your copyright notice in the Description page of Project Settings.


#include "MuseumGameModeBase.h"


AMuseumGameModeBase::AMuseumGameModeBase() {
	NodeDistance = 350.f;
	TreeBase = FVector(0, 0, 40);
}

void AMuseumGameModeBase::LogGraph(FMuseumGraph* Graph) {
	for (int a = 0; a < Graph->Nodes.Num(); a++) {
		UE_LOG(LogTemp, Warning, TEXT("Node Id: %s | Type: %s | Label: %s | Uri: %s"), 
			*Graph->Nodes[a].Id, *Graph->Nodes[a].Type, *Graph->Nodes[a].Label, *Graph->Nodes[a].Uri);
	}

	for (int a = 0; a < Graph->Relationships.Num(); a++) {
		UE_LOG(LogTemp, Warning, TEXT("Relationship Id: %s | Type: %s | StartId: %s | EndId: %s"), 
			*Graph->Relationships[a].Id, *Graph->Relationships[a].Type, *Graph->Relationships[a].StartId, *Graph->Relationships[a].EndId);
	}
}

void AMuseumGameModeBase::StartPlay()
{
    Super::StartPlay();

	Api = GetWorld()->SpawnActor<AMuseumApi>(FVector(0), FRotator(0));

	ResponseDelegate ClassAndSoftwareDel;
	ClassAndSoftwareDel.BindUObject(this, &AMuseumGameModeBase::ClassAndSoftwareCallback);
	// "source code editor"
	Api->GetClassAndSoftware("http://www.wikidata.org/entity/Q522972", ClassAndSoftwareDel);
}

void AMuseumGameModeBase::ClassAndSoftwareCallback(FMuseumGraph* Graph) {
	if (!NodeTemplate) return; 
	
	LogGraph(Graph);

	int32 NumSoftware = Graph->Nodes.Num() - 1;

	// The required radius for maintaining the desired NodeDistance
	float Radius = NodeDistance * NumSoftware / TwoPi;

	// Evenly space the nodes
	float Angle = TwoPi / NumSoftware;

	FVector ClassPosition = TreeBase;

	for (int a = 0; a < Graph->Nodes.Num(); a++) {
		if (Graph->Nodes[a].Type == "Class") {
			AVisualNode* ClassNode = GetWorld()->SpawnActor<AVisualNode>(NodeTemplate, ClassPosition, FRotator(0));
			ClassNode->MuseumNode = &Graph->Nodes[a];
		}
		else {
			// Calculate the next coordinates (on the unit circle)
			float SinAngle, CosAngle;
			FMath::SinCos(&SinAngle, &CosAngle, Angle * a);

			AVisualNode* NewNode = GetWorld()->SpawnActor<AVisualNode>(
				// Origin + Radius * Sin/Cos(Angle)
				NodeTemplate, ClassPosition + FVector(Radius, Radius, 1) * FVector(SinAngle, CosAngle, 1), FRotator(0)
			);
			NewNode->MuseumNode = &Graph->Nodes[a];
		}
	}
}
