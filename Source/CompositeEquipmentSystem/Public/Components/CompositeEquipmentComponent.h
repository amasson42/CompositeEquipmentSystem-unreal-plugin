// Amasson

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structures/CompositeEquipmentSystemStructs.h"
#include "CompositeEquipmentComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipedCompositesUpdatedSignature, const UCompositeEquipmentComponent*, MeshEquipmentComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnComponentCreatedSignature, const UCompositeEquipmentComponent*, MeshEquipmentComponent, const USceneComponent*, CreatedComponent, TSet<FName>, Tags);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnComponentRemovingSignature, const UCompositeEquipmentComponent*, MeshEquipmentComponent, const USceneComponent*, DestroyedComponent, TSet<FName>, Tags);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMPOSITEEQUIPMENTSYSTEM_API UCompositeEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCompositeEquipmentComponent();

	UPROPERTY(BlueprintAssignable)
	FOnEquipedCompositesUpdatedSignature OnEquipedCompositesUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnComponentCreatedSignature OnComponentCreated;

	UPROPERTY(BlueprintAssignable)
	FOnComponentRemovingSignature OnComponentRemoving;

	UFUNCTION(BlueprintCallable, Category="Leader Pose")
	void InitializeWithLeaderPose(USkeletalMeshComponent* NewLeaderPose);

	FORCEINLINE USkeletalMeshComponent* GetLeaderPoseComponent() const { return LeaderPoseComponent; }

    UFUNCTION(BlueprintCallable, Category="Equiped Composites")
    void BroadcastCreatedComponents() const;

	UFUNCTION(BlueprintCallable, Category="Equiped Composites")
	void AddCompositeName(FName CompositeName);

	UFUNCTION(BlueprintCallable, Category="Equiped Composites")
	void RemoveCompositeName(FName CompositeName);

	UFUNCTION(BlueprintCallable, Category="Equiped Composites")
	void SetCompositeNames(const TSet<FName>& NewCompositeNames);

	UFUNCTION(BlueprintCallable, Category="Equiped Composites")
	void AddCompositeCustom(const FEquipmentComposite& Composite, FName KeyName);

protected:

	/** Table of EquipmentComposite */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Equipment Data", meta = (RequiredAssetDataTags = "RowStructure=/Script/CompositeEquipmentSystem.EquipmentComposite"))
	TArray<TObjectPtr<UDataTable>> EquipmentTables;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Equiped Composites", meta = (AllowPrivateAccess = true))
	TSet<FName> CompositeNames;

private:

	UPROPERTY(BlueprintReadOnly, Category = "Leader Pose", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> LeaderPoseComponent;

	UPROPERTY()
	TWeakObjectPtr<AActor> PoseOwner;

	UPROPERTY()
	TMap<FName, FEquipmentCompositeGeneratedObjects> GeneratedObjectsForCompositeNames;

protected:

	void UpdateCompositesObjects();

	void DestroyRemovedComposites();
	void CreateAddedComposites();

	void CreateCompositeFromTable(const FName& CompositeName);
	void CreateCompositeWithKey(const FEquipmentComposite& Composite, const FName& CompositeKey);

	void CreateCompositeObjects(const FEquipmentComposite& Composite, FEquipmentCompositeGeneratedObjects& GeneratedObjects);

	void CreateStaticMeshes(const TArray<FEquipableStaticMesh>& EquipableMeshes, FEquipmentCompositeGeneratedObjects& GeneratedObjects);
	void CreateSkeletalMeshes(const TArray<FEquipableSkeletalMesh>& EquipableMeshes, FEquipmentCompositeGeneratedObjects& GeneratedObjects);
	void CreateSpecialEffects(const TArray<FEquipableSpecialEffect>& EquipableEffects, FEquipmentCompositeGeneratedObjects& GeneratedObjects);
	void CreateChildActors(const TArray<FEquipableChildActor>& EquipableActors, FEquipmentCompositeGeneratedObjects& GeneratedObjects);

	template<class TComponent>
	TComponent* CreateAttachedComponent_Unsafe(const FEquipableAttached& Equipable)
	{
		static_assert(std::is_base_of<USceneComponent, TComponent>::value, "TComponent must be a subclass of USceneComponent.");

		UActorComponent* CreatedComponent = PoseOwner->AddComponentByClass(
			TComponent::StaticClass(),
			true,
			Equipable.RelativeTransform,
			false
		);

		if (!IsValid(CreatedComponent))
			return nullptr;

		if (TComponent* SceneComponent = Cast<TComponent>(CreatedComponent))
		{
			SceneComponent->AttachToComponent(
				LeaderPoseComponent,
				FAttachmentTransformRules::KeepRelativeTransform,
				Equipable.SocketName
			);

            for (const FName& Tag : Equipable.Tags)
            {
                SceneComponent->ComponentTags.Add(Tag);
            }

			return SceneComponent;
		}

		return nullptr;
	}

};
