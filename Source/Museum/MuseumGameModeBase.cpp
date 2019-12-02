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

	SeededRand = FRandomStream(RandomSeed);

	Api = GetWorld()->SpawnActor<AMuseumApi>(FVector(0), FRotator(0));

	// ResponseDelegate ClassAndSoftwareDel;
	// ClassAndSoftwareDel.BindUObject(this, &AMuseumGameModeBase::ClassAndSoftwareCallback);
	// // "source code editor"
	// Api->GetClassAndSoftware("http://www.wikidata.org/entity/Q522972", ClassAndSoftwareDel);

	if (bEnableSpawning) {
		ResponseDelegate GraphDel;
		GraphDel.BindUObject(this, &AMuseumGameModeBase::GraphCallback);
		Api->GetGraph(GraphDel);
	}
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
	TSet<FString>* Edges = new TSet<FString>();
	for (auto& Node : Classes) {
		CalculateWeight(Node);
		if (Node->Children.Num() == 0) Edges->Add(Node->Id);
	}

	UE_LOG(LogTemp, Warning, TEXT("Nodes: %i | Classes: %i | Edges: %i"), NodeIdMap.Num(), Classes.Num(), Edges->Num());

	TSet<FString>* SpawnedClasses = new TSet<FString>();

	PlaceClasses(&NodeIdMap, SpawnedClasses, Edges, 0, FVector2D(0, 0));

	delete SpawnedClasses;
}

void AMuseumGameModeBase::CalculateWeight(FMuseumNode* Node) {
	Node->Weight = Node->Software.Num();
}

void AMuseumGameModeBase::PlaceClasses(TMap<FString, FMuseumNode*>* NodeIdMap, TSet<FString>* SpawnedClasses, TSet<FString>* Classes, float CurrentRadius, FVector2D CurrentCenter) {
	// MATH WARNING
	// ------------
	// Classes->Sort([](const FMuseumNode& a, const FMuseumNode& b) {
	// 	return a.Weight < b.Weight;
	// });

	TSet<FString> Ignored;
	Ignored.Add("");

	TSet<FString>* Parents = new TSet<FString>();

	float OldRadius = CurrentRadius;
	FVector2D OldCenter = CurrentCenter;

	FVector2D ClassPosition;
	FVector2D NewCenter;
	for (auto& NodeId : *Classes) {
		// FMuseumNode* Node = (*NodeIdMap)["43"];

		if (SpawnedClasses->Contains(NodeId)) continue;
		SpawnedClasses->Add(NodeId);

		FMuseumNode* Node = (*NodeIdMap)[NodeId];

		// Recurse into parents regardless of whether the node was spawned
		Parents->Append(Node->Parents);

		// Only spawn a class node with a name, and ignore empty edge nodes
		if (!Ignored.Contains(Node->Label) && (Node->Children.Num() > 0 || Node->Software.Num() > 0)) {
			
			// Temporarily spawn the class at the origin in order to spawn its software, and then move it
			AVisualNode* ClassNode = GetWorld()->SpawnActor<AVisualNode>(NodeTemplate, FVector(0), FRotator(0));
			ClassNode->MuseumNode = Node;
			// Don't forget that PlaceSoftware() also moves ClassNode to the proper height
			float ClassRadius = PlaceSoftware(*NodeIdMap, ClassNode);

			/* The following formulas are:
			**
			** (For class position)
			** XNew = |XOld| - (RNew + ROld) * XOld / (YOld * √(XOld^2 / YOld^2 + 1))
			** YNew = YOld - (RNew + ROld) * sign(YOld) / (√(XOld^2 / YOld^2 + 1))
			**
			** (For new circle position)
			** XOld - RNew * XOld / (YOld * √(XOld^2 / YOld^2 + 1))
			** YOld - RNew * sign(YOld) / (√(XOld^2 / YOld^2 + 1))
			** 
			** where the case of y = 0 is handled appropriately
			*/
			// if (OldCenter.Y != 0.f) {
			// 	float XLegUnit = FMath::Abs(OldCenter.X) / (OldCenter.Y * FMath::Sqrt((OldCenter.X * OldCenter.X) / (OldCenter.Y * OldCenter.Y) + 1));
			// 	float YLegUnit = FMath::Sign(OldCenter.Y) / (FMath::Sqrt((OldCenter.X * OldCenter.X) / (OldCenter.Y * OldCenter.Y) + 1));
				
			// 	ClassPosition.X = OldCenter.X - (ClassRadius + OldRadius) * XLegUnit;
			// 	ClassPosition.Y = OldCenter.Y - (ClassRadius + OldRadius) * YLegUnit;
			
			// 	NewCenter.X = OldCenter.X - ClassRadius * XLegUnit;
			// 	NewCenter.Y = OldCenter.Y - ClassRadius * YLegUnit;
			// }
			// // If the OldCenter is (0, 0) then pick a random direction
			// else if (OldCenter.X == 0.f) {
				float Direction = SeededRand.GetFraction() * TwoPi;

				ClassPosition.X = (ClassRadius + OldRadius) * FMath::Cos(Direction);
				ClassPosition.Y = (ClassRadius + OldRadius) * FMath::Sin(Direction);

				NewCenter.X = OldCenter.X + ClassRadius * FMath::Cos(Direction);
				NewCenter.Y = OldCenter.Y + ClassRadius * FMath::Sin(Direction);
			// }
			// // Otherwise, account for the hole in the function for YOld = 0
			// else {
			// 	ClassPosition.X = OldCenter.X - (ClassRadius + OldRadius);
			// 	ClassPosition.Y = 0.f;

			// 	NewCenter.X = OldCenter.X - ClassRadius;
			// 	NewCenter.Y = 0.f;
			// }

			// PlaceSoftware() calculates the correct Z value, so carry that over
			ClassNode->SetActorLocation(FVector(ClassPosition, ClassNode->GetActorLocation().Z));

			OldRadius = ClassRadius + OldRadius;
			OldCenter = NewCenter;

			UE_LOG(LogTemp, Warning, TEXT("Moved: %s | Position: (%f, %f) | Total radius: %f | New center: (%f, %f)"), *Node->Label, ClassPosition.X, ClassPosition.Y, OldRadius, NewCenter.X, NewCenter.Y);
		}
	}

	delete Classes;
	UE_LOG(LogTemp, Warning, TEXT("Spawning complete"));
	if (Parents->Num() > 0)
		PlaceClasses(NodeIdMap, SpawnedClasses, Parents, OldRadius, OldCenter);
}

float AMuseumGameModeBase::PlaceSoftware(const TMap<FString, FMuseumNode*>& NodeIdMap, AVisualNode* VisualClassNode) {
	FMuseumNode& ClassNode = *VisualClassNode->MuseumNode;  // For convenience

	TMap<int32, TArray<FMuseumNode*>> NodesByYear;

	int32 LowestYear = 30000; // Y2K etc...
	for (auto& Node : ClassNode.Software) {
		// UE_LOG(LogTemp, Warning, TEXT("Node Id: %s | Type: %s | Label: %s | Uri: %s | ReleaseYear: %i"), 
		// 	*NodeIdMap[Node]->Id, *NodeIdMap[Node]->Type, *NodeIdMap[Node]->Label, *NodeIdMap[Node]->Uri,
		// 	NodeIdMap[Node]->ReleaseYear);
		
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

	if (LowestYear == 30000) return 1.f;

	VisualClassNode->SetActorLocation(FVector(0, 0, (LowestYear - 1940 - 3) * YearToUnits));
	#if WITH_EDITOR
		VisualClassNode->SetActorLabel(ClassNode.Label);
	#endif  // WITH_EDITOR

	float MinRadius = ((USphereComponent*)NodeTemplate->GetDefaultSubobjectByName(TEXT("DetectionMesh")))->GetScaledSphereRadius();

	float MaxGeneratedRadius = 0.f;

	for (auto& Element : NodesByYear) {
		int32 CurrentYear = Element.Key;
		int32 SoftwarePlaced = 0;

		FVector Center = {0, 0, (CurrentYear - 1940) * YearToUnits};
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
	float TotalRadius = HelixRadius + MaxGeneratedRadius;
	// UE_LOG(LogTemp, Warning, TEXT("Spawned: %s | Start year: %i | Radius: %f"), *ClassNode.Label, LowestYear, TotalRadius);
	return TotalRadius;
}
