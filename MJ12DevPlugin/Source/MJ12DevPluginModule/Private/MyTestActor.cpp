#include "MyTestActor.h"
#include "Logging/LogMacros.h" // Required for UE_LOG

// Sets default values
AMyTestActor::AMyTestActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    TestFloatProperty = 42.0f;
}

// Called when the game starts or when spawned
void AMyTestActor::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("AMyTestActor::BeginPlay() called. TestFloatProperty: %f"), TestFloatProperty);
}

// Called every frame
void AMyTestActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMyTestActor::TestFunction(int32 Parameter)
{
    UE_LOG(LogTemp, Warning, TEXT("AMyTestActor::TestFunction(%d) called."), Parameter);
}
