// Amasson

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CompositeEquipmentSystemStructs.generated.h"


USTRUCT(BlueprintType)
struct COMPOSITEEQUIPMENTSYSTEM_API FEquipableAttached
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Position")
    FTransform RelativeTransform;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Position")
    FName SocketName;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tag")
    TSet<FName> Tags;

};

USTRUCT(BlueprintType)
struct COMPOSITEEQUIPMENTSYSTEM_API FEquipableStaticMesh : public FEquipableAttached
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
    TObjectPtr<UStaticMesh> StaticMesh;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
    TArray<UMaterialInterface*> OverrideMaterials;

};


USTRUCT(BlueprintType)
struct COMPOSITEEQUIPMENTSYSTEM_API FEquipableSkeletalMesh : public FEquipableAttached
{
    GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TArray<UMaterialInterface*> OverrideMaterials;

    /** Incompatible with UseLeaderPose */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TArray<FName> HiddenBones;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	bool bUseLeaderPose = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TSubclassOf<UAnimInstance> AnimInstanceClass;

};


class UNiagaraSystem;
class UParticleSystem;

USTRUCT(BlueprintType)
struct COMPOSITEEQUIPMENTSYSTEM_API FEquipableSpecialEffect : public FEquipableAttached
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VFX")
	TObjectPtr<UNiagaraSystem> NiagaraEffect;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VFX")
	TObjectPtr<UParticleSystem> CascadeEffect;

};


USTRUCT(BlueprintType)
struct COMPOSITEEQUIPMENTSYSTEM_API FEquipableChildActor : public FEquipableAttached
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Actor")
	TSubclassOf<AActor> ActorClass;

};


USTRUCT(BlueprintType, Blueprintable)
struct COMPOSITEEQUIPMENTSYSTEM_API FEquipmentComposite : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Composites")
    TArray<FEquipableStaticMesh> StaticMeshes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Composites")
    TArray<FEquipableSkeletalMesh> SkeletalMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Composites")
    TArray<FEquipableSpecialEffect> SpecialEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Composites")
    TArray<FEquipableChildActor> ChildActors;
};


USTRUCT(BlueprintType)
struct FEquipmentCompositeGeneratedObjects
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    TArray<USceneComponent*> Components;

    void AddComponent(USceneComponent* NewComponent);

    void Destroy();
};
