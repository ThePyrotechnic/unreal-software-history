// Fill out your copyright notice in the Description page of Project Settings.


#include "MuseumGameModeBase.h"
#include "Classes/Components/SphereComponent.h"


AMuseumGameModeBase::AMuseumGameModeBase() {
	NodeDistance = 350.f;
	TreeBase = FVector(0, 0, 40);
}

void AMuseumGameModeBase::LogGraph(FMuseumGraph* Graph) {
	for (FMuseumNode Node : Graph->Nodes) {
		UE_LOG(LogTemp, Warning, TEXT("Node Id: %s | Type: %s | Label: %s | Uri: %s"), 
			*Node.Id, *Node.Type, *Node.Label, *Node.Uri);
	}

	for (FMuseumRelationship Relation : Graph->Relationships) {
		UE_LOG(LogTemp, Warning, TEXT("Relationship Id: %s | Type: %s | StartId: %s | EndId: %s"), 
			*Relation.Id, *Relation.Type, *Relation.StartId, *Relation.EndId);
	}
}

void AMuseumGameModeBase::StartPlay()
{
    Super::StartPlay();

	Api = GetWorld()->SpawnActor<AMuseumApi>(FVector(0), FRotator(0));

	// ResponseDelegate ClassAndSoftwareDel;
	// ClassAndSoftwareDel.BindUObject(this, &AMuseumGameModeBase::ClassAndSoftwareCallback);
	// // "source code editor"
	// Api->GetClassAndSoftware("http://www.wikidata.org/entity/Q522972", ClassAndSoftwareDel);

	ResponseDelegate GraphDel;
	GraphDel.BindUObject(this, &AMuseumGameModeBase::GraphCallback);
	Api->GetGraph(GraphDel);
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

	int SoftwarePlaced = 0;
	for (int a = 0; a < Graph->Nodes.Num(); a++) {
		if (Graph->Nodes[a].Type == "Class") {
			AVisualNode* ClassNode = GetWorld()->SpawnActor<AVisualNode>(NodeTemplate, ClassPosition, FRotator(0));
			ClassNode->MuseumNode = &Graph->Nodes[a];
		}
		else {
			// Calculate the next coordinates (on the unit circle)
			float SinAngle, CosAngle;
			FMath::SinCos(&SinAngle, &CosAngle, Angle * SoftwarePlaced++);

			AVisualNode* NewNode = GetWorld()->SpawnActor<AVisualNode>(
				// Origin + Radius * Sin/Cos(Angle)
				NodeTemplate, ClassPosition + FVector(Radius, Radius, 1) * FVector(SinAngle, CosAngle, 1), FRotator(0)
			);
			NewNode->MuseumNode = &Graph->Nodes[a];
		}
	}
}

void AMuseumGameModeBase::GraphCallback(FMuseumGraph* Graph) {
	if (!NodeTemplate) return;

	// Create mapping of node IDs to parents
	TMap<FString, FMuseumNode*> NodeIdMap;
	TArray<FMuseumNode*> Classes;
	for (auto& Node : Graph->Nodes) {
		if (Node.Type == "Class") Classes.Add(&Node);
		
		NodeIdMap.Add(Node.Id, &Node);
	}

	// Create the tree by adding references to/from each node
	for (auto& Relation : Graph->Relationships) {
		if (Relation.Type == "INSTANCE") {
			if (NodeIdMap[Relation.EndId]->Parents.Num() <= 0)
				NodeIdMap[Relation.StartId]->Parents.Add(Relation.EndId);  // Only add 1 parent relation for Software nodes
			NodeIdMap[Relation.EndId]->Software.Add(Relation.StartId);  // Add a Software relationship to parent classes of software
		}
		else if (Relation.Type == "SUBCLASS") {
			NodeIdMap[Relation.StartId]->Parents.Add(Relation.EndId);  // Always add a Parent relationship
			NodeIdMap[Relation.EndId]->Children.Add(Relation.StartId);  // Add a Child relationship to parents of a class
		}
	}

	// Get all edge classes
	TArray<FMuseumNode*>* Edges = new TArray<FMuseumNode*>();
	for (auto& Node : Classes) {
		CalculateWeight(Node);
		if (Node->Children.Num() == 0) Edges->Add(Node);
	}

	UE_LOG(LogTemp, Warning, TEXT("Nodes: %i | Classes: %i | Edges: %i"), NodeIdMap.Num(), Classes.Num(), Edges->Num());

	PlaceClasses(&NodeIdMap, Edges, 0, FVector2D(0, 0));
}

void AMuseumGameModeBase::CalculateWeight(FMuseumNode* Node) {
	Node->Weight = Node->Software.Num();
}

void AMuseumGameModeBase::PlaceClasses(TMap<FString, FMuseumNode*>* NodeIdMap, TArray<FMuseumNode*>* Classes, float CurrentRadius, FVector2D CurrentCenter) {
	Classes->Sort([](const FMuseumNode& a, const FMuseumNode& b) {
		return a.Weight < b.Weight;
	});

	TArray<FMuseumNode*>* Parents = new TArray<FMuseumNode*>();

	for (auto& Node : *Classes) {
		// FMuseumNode* Node = (*NodeIdMap)["43"];
		// Temporarily spawn the class at the origin in order to spawn its software, and then move it
		AVisualNode* ClassNode = GetWorld()->SpawnActor<AVisualNode>(NodeTemplate, FVector(0), FRotator(0));
		ClassNode->MuseumNode = Node;
		float ClassRadius = PlaceSoftware(*NodeIdMap, ClassNode);
		
		
		// TODO move class based on total used width so far
	}

	delete Classes;

	// if (Parents->Num() > 0)
	// 	PlaceClasses(NodeIdMap, Parents, CurrentRadius, CurrentCenter);
}

int32 AMuseumGameModeBase::PlaceSoftware(const TMap<FString, FMuseumNode*>& NodeIdMap, AVisualNode* VisualClassNode) {
	FMuseumNode& ClassNode = *VisualClassNode->MuseumNode;  // For convenience

	TMap<int32, TArray<FMuseumNode*>> NodesByYear;

	int32 LowestYear = 3000000; // Y2K etc...
	for (auto& Node : ClassNode.Software) {
		UE_LOG(LogTemp, Warning, TEXT("Node Id: %s | Type: %s | Label: %s | Uri: %s | ReleaseYear: %i"), 
			*NodeIdMap[Node]->Id, *NodeIdMap[Node]->Type, *NodeIdMap[Node]->Label, *NodeIdMap[Node]->Uri,
			NodeIdMap[Node]->ReleaseYear);
		
		int32 ReleaseYear = NodeIdMap[Node]->ReleaseYear;

		if (ReleaseYear == 0) continue;

		if (ReleaseYear < LowestYear) LowestYear = ReleaseYear;

		if (NodesByYear.Contains(ReleaseYear)) {
			NodesByYear[ReleaseYear].Add(NodeIdMap[Node]);
		}
		else {
			NodesByYear.Add(ReleaseYear, TArray<FMuseumNode*>());
			NodesByYear[ReleaseYear].Add(NodeIdMap[Node]);
		}
	}

	VisualClassNode->SetActorLocation(FVector(0, 0, (LowestYear - 3) * YearToUnits));
	#if WITH_EDITOR
		VisualClassNode->SetActorLabel(ClassNode.Label);
	#endif  // WITH_EDITOR

	float MinRadius = ((USphereComponent*)NodeTemplate->GetDefaultSubobjectByName(TEXT("DetectionMesh")))->GetScaledSphereRadius();
	// UE_LOG(LogTemp, Warning, TEXT("NodeDistance: %f | MinRadius: %f | LowestYear: %i | YearToUnits: %f"),
	// 	NodeDistance, MinRadius, LowestYear, YearToUnits);

	float MaxGeneratedRadius = 0.f;

	for (auto& Element : NodesByYear) {
		int32 CurrentYear = Element.Key;
		int32 SoftwarePlaced = 0;

		FVector Center = {0, 0, CurrentYear * YearToUnits};
		FMath::SinCos(&Center.X, &Center.Y, CurrentYear);
		Center *= FVector(HelixRadius, HelixRadius, 1);

		float NumSoftware = Element.Value.Num();

		// The required radius for maintaining the desired NodeDistance
		// https://mathopenref.com/polygonsides.html
		float Radius = (NumSoftware == 1) ? 0 :  NodeDistance / (2 * FMath::Sin(Pi/NumSoftware));

		// This Radius plus the helix radius is the total radius of this class' "column" in space
		if (Radius > MaxGeneratedRadius) MaxGeneratedRadius = Radius;

		// Evenly space the nodes
		float Angle = TwoPi / NumSoftware;

		// UE_LOG(LogTemp, Warning, TEXT("Current center: (%f, %f, %f) | Angle: %f | Radius: %f | NumSoftware: %f"), 
		// 	Center.X, Center.Y, Center.Z, Angle, Radius, NumSoftware);

		float SinAngle, CosAngle;
		for (auto& SoftwareNode : Element.Value) {
			FMath::SinCos(&SinAngle, &CosAngle, Angle * SoftwarePlaced++);

			AVisualNode* VisualSoftwareNode = GetWorld()->SpawnActor<AVisualNode>(NodeTemplate, Center + FVector(Radius, Radius, 1) * FVector(SinAngle, CosAngle, 1), FRotator(0));
			VisualSoftwareNode->MuseumNode = SoftwareNode;
			VisualSoftwareNode->AttachToActor(VisualClassNode, FAttachmentTransformRules::KeepWorldTransform);
			#if WITH_EDITOR
				VisualSoftwareNode->SetActorLabel(SoftwareNode->Label);
			#endif // WITH_EDITOR
		}
	}
	return HelixRadius + MaxGeneratedRadius;
}
