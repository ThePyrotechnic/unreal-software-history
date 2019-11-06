// Fill out your copyright notice in the Description page of Project Settings.


#include "MuseumGameModeBase.h"


void AMuseumGameModeBase::StartPlay()
{
    Super::StartPlay();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Hello, world!"));
    }
}
