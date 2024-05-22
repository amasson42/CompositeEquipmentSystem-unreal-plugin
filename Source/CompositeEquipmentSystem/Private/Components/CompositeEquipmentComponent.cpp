// Amasson


#include "Components/CompositeEquipmentComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

UCompositeEquipmentComponent::UCompositeEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCompositeEquipmentComponent::InitializeWithLeaderPose(USkeletalMeshComponent* NewLeaderPose)
{
	LeaderPoseComponent = NewLeaderPose;
	PoseOwner = IsValid(LeaderPoseComponent) ? LeaderPoseComponent->GetOwner() : nullptr;

	UpdateCompositesObjects();
}

void UCompositeEquipmentComponent::BroadcastCreatedComponents() const
{
    for (const TPair<FName, FEquipmentCompositeGeneratedObjects>& Composite : GeneratedObjectsForCompositeNames)
    {
        for (const USceneComponent* Component : Composite.Value.Components)
        {
            TSet<FName> Tags;
            for (const FName& Tag : Component->ComponentTags)
            {
                Tags.Add(Tag);
            }
            OnComponentCreated.Broadcast(this, Component, Tags);
        }
    }
}

void UCompositeEquipmentComponent::AddCompositeName(FName CompositeName)
{
	CompositeNames.Add(CompositeName);
	UpdateCompositesObjects();
}

void UCompositeEquipmentComponent::RemoveCompositeName(FName CompositeName)
{
	CompositeNames.Remove(CompositeName);
	UpdateCompositesObjects();
}

void UCompositeEquipmentComponent::SetCompositeNames(const TSet<FName>& NewCompositeNames)
{
	CompositeNames = NewCompositeNames;
	UpdateCompositesObjects();
}

void UCompositeEquipmentComponent::AddCompositeCustom(const FEquipmentComposite& Composite, FName KeyName)
{
	if (CompositeNames.Remove(KeyName))
		DestroyRemovedComposites();

	CompositeNames.Add(KeyName);

	CreateCompositeWithKey(Composite, KeyName);

	UpdateCompositesObjects();
}


/** Private */

void UCompositeEquipmentComponent::UpdateCompositesObjects()
{
	DestroyRemovedComposites();

	CreateAddedComposites();

	OnEquipedCompositesUpdated.Broadcast(this);
}

void UCompositeEquipmentComponent::DestroyRemovedComposites()
{
	TArray<FName> KeysToRemove;

    for (TPair<FName, FEquipmentCompositeGeneratedObjects>& NameAndGeneratedObjects : GeneratedObjectsForCompositeNames)
    {
		const FName& CompositeName(NameAndGeneratedObjects.Key);
		FEquipmentCompositeGeneratedObjects& GeneratedObjects(NameAndGeneratedObjects.Value);

		bool bShouldRemoveKey = !CompositeNames.Contains(CompositeName);

		if (bShouldRemoveKey)
		{
            for (USceneComponent* Component : GeneratedObjects.Components)
            {
                TSet<FName> Tags;
                for (const FName& Tag : Component->ComponentTags)
                {
                    Tags.Add(Tag);
                }
                OnComponentRemoving.Broadcast(this, Component, Tags);
            }
			GeneratedObjects.Destroy();
			KeysToRemove.Add(CompositeName);
		}
    }

    for (const FName& Key : KeysToRemove)
    {
        GeneratedObjectsForCompositeNames.Remove(Key);
    }
}

void UCompositeEquipmentComponent::CreateAddedComposites()
{
	for (const FName& CompositeName : CompositeNames)
	{
		if (!GeneratedObjectsForCompositeNames.Contains(CompositeName))
			CreateCompositeFromTable(CompositeName);
	}
}

void UCompositeEquipmentComponent::CreateCompositeFromTable(const FName& CompositeName)
{
	if (IsValid(EquipmentTable))
	{
		FEquipmentComposite* CompositeRow = EquipmentTable->FindRow<FEquipmentComposite>(CompositeName, "Fetch Equipment Data");
		if (CompositeRow)
		{
			CreateCompositeWithKey(*CompositeRow, CompositeName);
			return;
		}
	}
	/**
	 * In case of invalid name, we still create an empty object in our
	 * GeneratedObjectsForCompositeNames array to prevent future lookup
	 * that would waste performances
	*/
	CreateCompositeWithKey(FEquipmentComposite(), CompositeName);
}

void UCompositeEquipmentComponent::CreateCompositeWithKey(const FEquipmentComposite& Composite, const FName& CompositeKey)
{
	FEquipmentCompositeGeneratedObjects ComponentsArray;
	CreateCompositeObjects(Composite, ComponentsArray);
	GeneratedObjectsForCompositeNames.Add(CompositeKey, ComponentsArray);
}

void UCompositeEquipmentComponent::CreateCompositeObjects(const FEquipmentComposite& Composite, FEquipmentCompositeGeneratedObjects& GeneratedObjects)
{
	if (!(IsValid(LeaderPoseComponent) && PoseOwner.IsValid()))
		return;

	CreateStaticMeshes(Composite.StaticMeshes, GeneratedObjects);
	CreateSkeletalMeshes(Composite.SkeletalMeshes, GeneratedObjects);
	CreateSpecialEffects(Composite.SpecialEffects, GeneratedObjects);
	CreateChildActors(Composite.ChildActors, GeneratedObjects);
}

void UCompositeEquipmentComponent::CreateStaticMeshes(const TArray<FEquipableStaticMesh>& EquipableMeshes, FEquipmentCompositeGeneratedObjects& GeneratedObjects)
{
	for (const FEquipableStaticMesh& Equipable : EquipableMeshes)
	{
		UStaticMeshComponent* MeshComponent = CreateAttachedComponent_Unsafe<UStaticMeshComponent>(Equipable);

		if (IsValid(MeshComponent))
		{
			MeshComponent->SetStaticMesh(Equipable.StaticMesh);

			for (int32 i = 0; i < Equipable.OverrideMaterials.Num(); i++)
				MeshComponent->SetMaterial(i, Equipable.OverrideMaterials[i]);

            MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			GeneratedObjects.AddComponent(MeshComponent);
            OnComponentCreated.Broadcast(this, MeshComponent, Equipable.Tags);
		}
	}
}

void UCompositeEquipmentComponent::CreateSkeletalMeshes(const TArray<FEquipableSkeletalMesh>& EquipableMeshes, FEquipmentCompositeGeneratedObjects& GeneratedObjects)
{
	AActor* Owner = LeaderPoseComponent->GetOwner();
	if (!IsValid(Owner)) return;

	for (const FEquipableSkeletalMesh& Equipable : EquipableMeshes)
	{
		USkeletalMeshComponent* SkeletalComponent = CreateAttachedComponent_Unsafe<USkeletalMeshComponent>(Equipable);

		if (IsValid(SkeletalComponent))
		{
			SkeletalComponent->SetSkeletalMesh(Equipable.SkeletalMesh);

			for (int32 i = 0; i < Equipable.OverrideMaterials.Num(); i++)
				SkeletalComponent->SetMaterial(i, Equipable.OverrideMaterials[i]);

			for (const FName& BoneName : Equipable.HiddenBones)
				SkeletalComponent->HideBoneByName(BoneName, EPhysBodyOp::PBO_None);

			if (Equipable.bUseLeaderPose)
				SkeletalComponent->SetLeaderPoseComponent(LeaderPoseComponent);

			if (IsValid(Equipable.AnimInstanceClass))
				SkeletalComponent->SetAnimInstanceClass(Equipable.AnimInstanceClass);

            SkeletalComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			GeneratedObjects.AddComponent(SkeletalComponent);
            OnComponentCreated.Broadcast(this, SkeletalComponent, Equipable.Tags);
		}
	}
}

void UCompositeEquipmentComponent::CreateSpecialEffects(const TArray<FEquipableSpecialEffect>& EquipableEffects, FEquipmentCompositeGeneratedObjects& GeneratedObjects)
{
	for (const FEquipableSpecialEffect& Equipable : EquipableEffects)
	{
		if (IsValid(Equipable.CascadeEffect))
		{
			UParticleSystemComponent* ParticleComponent = UGameplayStatics::SpawnEmitterAttached(
				Equipable.CascadeEffect,
				LeaderPoseComponent,
				Equipable.SocketName,
				Equipable.RelativeTransform.GetLocation(),
				Equipable.RelativeTransform.GetRotation().Rotator(),
				EAttachLocation::Type::KeepRelativeOffset
			);
			
			GeneratedObjects.AddComponent(ParticleComponent);
            OnComponentCreated.Broadcast(this, ParticleComponent, Equipable.Tags);
		}

		if (IsValid(Equipable.NiagaraEffect))
		{
			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				Equipable.NiagaraEffect,
				LeaderPoseComponent,
				Equipable.SocketName,
				Equipable.RelativeTransform.GetLocation(),
				Equipable.RelativeTransform.GetRotation().Rotator(),
				EAttachLocation::Type::KeepRelativeOffset,
				true
			);

			GeneratedObjects.AddComponent(NiagaraComponent);
            OnComponentCreated.Broadcast(this, NiagaraComponent, Equipable.Tags);
		}
	}
}

void UCompositeEquipmentComponent::CreateChildActors(const TArray<FEquipableChildActor>& EquipableActors, FEquipmentCompositeGeneratedObjects& GeneratedObjects)
{
	AActor* Owner = LeaderPoseComponent->GetOwner();
	if (!IsValid(Owner)) return;

	for (const FEquipableChildActor& Equipable : EquipableActors)
	{
		UChildActorComponent* ChildActorComponent = CreateAttachedComponent_Unsafe<UChildActorComponent>(Equipable);

		if (IsValid(ChildActorComponent))
		{
			ChildActorComponent->SetChildActorClass(Equipable.ActorClass);

			GeneratedObjects.AddComponent(ChildActorComponent);
            OnComponentCreated.Broadcast(this, ChildActorComponent, Equipable.Tags);
		}
	}
}
