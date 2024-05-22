// Copyright Amasson

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/Anchors.h"
#include "SpriteComposite.generated.h"

/**
 * An element to describe modifications of a drawn image.
 */
USTRUCT(BlueprintType)
struct COMPOSITEEQUIPMENTSYSTEM_API FSpriteCompositeElement
{
	GENERATED_BODY()
public:

	/**
	 * The image to render for this brush, can be a UTexture or UMaterialInterface or an object implementing 
	 * the AtlasedTextureInterface.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Content, meta=(DisplayThumbnail="true", DisplayName="Image", AllowedClasses="/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses = "/Script/MediaAssets.MediaTexture"))
	TObjectPtr<UObject> ResourceObject;

	/** Tinting applied to the image. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Content)
	FSlateColor TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

	/** Anchors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Position)
	FAnchors Anchors = FAnchors(0.0f, 0.0f, 1.0f, 1.0f);

	/** Rotation angle of the texture */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Position)
	float RotationAngle = 0.0f;

    /** Post processing effect material applied to the texture */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
    TObjectPtr<UMaterialInterface> EffectMaterial;

    /** The name of the texture parameter to use in the effect material */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
    FName EffectMaterialParameterName = FName(TEXT("Texture"));

    /** Should the tint color occur after the effect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
    bool bApplyTintColorAfterEffect = false;

};

/**
 * A Composition of multiple transformed 2D elements to be drawn sequentially.
 */
USTRUCT(BlueprintType)
struct FSpriteComposite
{
	GENERATED_BODY()
public:

    /** Background color of the composition */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor BackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /** The elements that make up the composite. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSpriteCompositeElement> Elements;

    /** Final tint color applied to the composition */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

    /** Fix the composition. Caching its result but preventing animated materials */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFixedComposition = true;

};
