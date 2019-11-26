// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualNode.h"


// Sets default values
AVisualNode::AVisualNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	CollisionMesh = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionMesh"));
	CollisionMesh->SetupAttachment(RootComponent);
	CollisionMesh->SetRelativeLocation(FVector(0));
	CollisionMesh->SetSphereRadius(51.f);
	// CollisionMesh->SetHiddenInGame(false);
	CollisionMesh->SetCollisionProfileName("BlockAllDynamic");


	DetectionMesh = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionMesh"));	
	DetectionMesh->SetupAttachment(RootComponent);
	DetectionMesh->SetRelativeLocation(FVector(0));
	DetectionMesh->SetSphereRadius(150.f);
	// DetectionMesh->SetHiddenInGame(false);
	DetectionMesh->ShapeColor = FColor(0, 255, 0, 255);

	DetectionMesh->OnComponentBeginOverlap.AddDynamic(this, &AVisualNode::OnOverlapBegin);
	DetectionMesh->OnComponentEndOverlap.AddDynamic(this, &AVisualNode::OnOverlapEnd);

}

// Called when the game starts or when spawned
void AVisualNode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVisualNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVisualNode::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr) {
		if (GEngine && MuseumNode)
        	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, MuseumNode->Label + TEXT(" : ") + FString::FromInt(MuseumNode->ReleaseYear));
    }  
}

void AVisualNode::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr) {}
}
