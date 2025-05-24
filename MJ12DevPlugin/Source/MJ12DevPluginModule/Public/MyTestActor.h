#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyTestActor.generated.h" // This must be the last include

UCLASS(Blueprintable, BlueprintType)
class MJ12DEVPLUGINMODULE_API AMyTestActor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AMyTestActor();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Test Actor")
    float TestFloatProperty;

    UFUNCTION(BlueprintCallable, Category="Test Actor")
    void TestFunction(int32 Parameter);
};
