// Fill out your copyright notice in the Description page of Project Settings.



#include "User.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Controller.h"


// Sets default values
AUser::AUser()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCamera->SetupAttachment((USceneComponent*)GetCapsuleComponent());
	FPSCamera->SetRelativeLocation(FVector(0.f, 0.f, 50.f + BaseEyeHeight));
	FPSCamera->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);

	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FPSMesh->SetOnlyOwnerSee(true);
	FPSMesh->SetupAttachment(FPSCamera);
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;
}

// Called when the game starts or when spawned
void AUser::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Using custom character"));
    }
	
}

// Called every frame
void AUser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUser::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUser::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUser::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AUser::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AUser::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AUser::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AUser::StopJump);
}

void AUser::MoveForward(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AUser::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AUser::StartJump()
{
	bPressedJump = true;
}

void AUser::StopJump()
{
	bPressedJump = false;
}
