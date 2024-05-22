// Amasson


#include "Structures/CompositeEquipmentSystemStructs.h"


void FEquipmentCompositeGeneratedObjects::AddComponent(USceneComponent* NewComponent)
{
    Components.Add(NewComponent);
}

void FEquipmentCompositeGeneratedObjects::Destroy()
{
    for (USceneComponent* Component : Components)
	{
		Component->DestroyComponent();
	}
}
