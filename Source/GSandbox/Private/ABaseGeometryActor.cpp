// Fill out your copyright notice in the Description page of Project Settings.

#include "ABaseGeometryActor.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseGeometry, All, All)

// Sets default values
ABaseGeometryActor::ABaseGeometryActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
    SetRootComponent(BaseMesh);
}

void ABaseGeometryActor::SetGeometryData(const FGeometryData& Data)
{
    GeometryData = Data;
}

// Called when the game starts or when spawned
void ABaseGeometryActor::BeginPlay()
{
    Super::BeginPlay();

    InitialLocation = GetActorLocation();

    // PrintTransform();
    // PrintTypes();
    // PrintStringTypes();

    SetColor(GeometryData.Color);

    GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseGeometryActor::OnTimerFired, GeometryData.TimerRate, true);
}

void ABaseGeometryActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    UE_LOG(LogBaseGeometry, Error, TEXT("Actor is destroyed: %s"), *GetName());
}

// Called every frame
void ABaseGeometryActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    HandleMovement();
}

void ABaseGeometryActor::HandleMovement()
{
    switch (GeometryData.MoveType)
    {
        case EMovementType::Sin:
        {
            FVector CurrentLocation = GetActorLocation();
            float time = GetWorld()->GetTimeSeconds();
            CurrentLocation.Z = InitialLocation.Z + GeometryData.Amplitude * FMath::Sin(GeometryData.Frequency * time);
            SetActorLocation(CurrentLocation);
        }
        break;

        case EMovementType::Static: break;
        default: break;
    }
}

void ABaseGeometryActor::PrintTypes()
{
    UE_LOG(LogBaseGeometry, Warning, TEXT("Actor name %s"), *GetName());
    UE_LOG(LogBaseGeometry, Warning, TEXT("Weapons num: %d, kills num: %i"), WeaponsNum, KillsNum);
    UE_LOG(LogBaseGeometry, Warning, TEXT("Health: %f"), Health);
    UE_LOG(LogBaseGeometry, Warning, TEXT("IdDead: %d"), IsDead);
    UE_LOG(LogBaseGeometry, Warning, TEXT("HasWeapon: %d"), static_cast<int>(HasWeapon));
}

void ABaseGeometryActor::PrintStringTypes()
{
    FString Name = "John Doe";
    UE_LOG(LogBaseGeometry, Warning, TEXT("Name: %s"), *Name);

    FString WeaponsNumStr = "Weapons num = " + FString::FromInt(WeaponsNum);
    FString HealthStr = "Health = " + FString::SanitizeFloat(Health);
    FString IsDeadStr = "Is dead = " + FString(IsDead ? "true" : "false");

    FString Stat = FString::Printf(TEXT(" \n == All Stats == \n %s \n %s \n %s"), *WeaponsNumStr, *HealthStr, *IsDeadStr);
    UE_LOG(LogBaseGeometry, Warning, TEXT("%s"), *Stat);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, Name);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Stat, true, FVector2D(1.5f, 1.5f));
    }
}

void ABaseGeometryActor::PrintTransform()
{
    FTransform Transform = GetActorTransform();
    FVector Location = Transform.GetLocation();
    FVector Translation = Transform.GetTranslation();
    FRotator Rotator = Transform.Rotator();
    FQuat Rotation = Transform.GetRotation();
    FVector Scale3D = Transform.GetScale3D();

    UE_LOG(LogBaseGeometry, Warning, TEXT("Actor name %s"), *GetName());
    UE_LOG(LogBaseGeometry, Warning, TEXT("Transform %s"), *Transform.ToString());
    UE_LOG(LogBaseGeometry, Warning, TEXT("Location %s"), *Location.ToString());
    UE_LOG(LogBaseGeometry, Warning, TEXT("Translation %s"), *Translation.ToString());
    UE_LOG(LogBaseGeometry, Warning, TEXT("Rotator %s"), *Rotator.ToString());
    UE_LOG(LogBaseGeometry, Warning, TEXT("Rotation %s"), *Rotation.ToString());
    UE_LOG(LogBaseGeometry, Warning, TEXT("Scale3D %s"), *Scale3D.ToString());

    UE_LOG(LogBaseGeometry, Error, TEXT("Human transform %s"), *Transform.ToHumanReadableString());
}

void ABaseGeometryActor::SetColor(const FLinearColor& Color)
{
    if (!BaseMesh) return;

    UMaterialInstanceDynamic* DinMaterial = BaseMesh->CreateAndSetMaterialInstanceDynamic(0);
    if (DinMaterial)
    {
        DinMaterial->SetVectorParameterValue("Color", Color);
    }
}

void ABaseGeometryActor::OnTimerFired()
{
    if (++TimerCount <= MaxTimerCount)
    {
        const FLinearColor NewColor = FLinearColor::MakeRandomColor();
        UE_LOG(LogBaseGeometry, Display, TEXT("TimerCount: %i, Color to set up: %s"), TimerCount, *NewColor.ToString());
        SetColor(NewColor);
        OnColorChanged.Broadcast(NewColor, GetName());
    }
    else
    {
        GetWorldTimerManager().ClearTimer(TimerHandle);
        UE_LOG(LogBaseGeometry, Warning, TEXT("Timer has been stopped!"));
        OnTimerFinished.Broadcast(this);
    }
}
