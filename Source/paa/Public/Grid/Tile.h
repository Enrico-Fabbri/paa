#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

/**
 * @brief Represents a grid tile that can serve as either a walkable area or an obstacle.
 *
 * The ATile actor contains a static mesh (a plane by default) and a dynamic material instance.
 * Material parameters such as obstacle status, base color, and texture index are exposed for
 * editing and can be updated at construction time.
 */
UCLASS()
class PAA_API ATile : public AActor
{
    GENERATED_BODY()
    
public:
    /// Constructor that sets default values for this actor's properties.
    ATile();

    /** @brief Returns whether this tile is considered an obstacle.
     *  @return true if the tile is an obstacle; false otherwise.
     */
    UFUNCTION()
    bool IsObstacle() const;

    /** @brief Returns the base color applied to the tile material.
     *  @return The FLinearColor representing the base color.
     */
	UFUNCTION()
    FLinearColor GetBaseColor() const;

    /** @brief Returns the texture index used by the tile material.
     *  @return An integer representing the texture index.
     */
	UFUNCTION()
    int32 GetTextureIndex() const;

    /** @brief Sets whether this tile should be treated as an obstacle.
     *  @param bIsObstacle New value for obstacle flag.
     */
	UFUNCTION()
    void SetIsObstacle(const bool bIsObstacle);

    /** @brief Sets the base color of the tile.
     *  @param BaseColor New base color value.
     */
	UFUNCTION()
    void SetBaseColor(const FLinearColor& BaseColor);

    /** @brief Sets the texture index for the tile.
     *  @param TextureIndex New texture index value.
     */
	UFUNCTION()
    void SetTextureIndex(const int32 TextureIndex);

    /** 
     * @brief Updates the dynamic material parameters.
     *
     * This function sets the scalar and vector parameters on the dynamic material instance
     * to reflect the current property values. Call this whenever one of the material parameters changes.
     */
	UFUNCTION()
    void UpdateMaterial() const;

protected:
    /**
     * @brief Called when the actor is constructed or modified in the editor.
     * @param Transform The transform for this actor.
     *
     * This override ensures that material parameters are updated whenever properties change.
     */
	UFUNCTION()
    virtual void OnConstruction(const FTransform& Transform) override;

private:
    /**
     * @brief The static mesh component representing the visual tile.
     *
     * This component is created in the constructor using CreateDefaultSubobject.
     */
    UPROPERTY(VisibleAnywhere, Category = "Tile|Components")
    UStaticMeshComponent* TileMesh;

    /**
     * @brief The dynamic material instance used to display material parameters.
     *
     * A dynamic instance is created from a base material to allow per-tile customization.
     */
    UPROPERTY(VisibleAnywhere, Category = "Tile|Components")
    UMaterialInstanceDynamic* TileMaterial;

    /// Determines whether this tile is an obstacle.
    UPROPERTY(EditAnywhere, Category = "Tile|Material")
    bool bIsObstacle = false;
    
    /// The base color applied to the tile material.
    UPROPERTY(EditAnywhere, Category = "Tile|Material")
    FLinearColor BaseColor = FLinearColor::White;
    
    /// The index of the texture used in the tile material.
    UPROPERTY(EditAnywhere, Category = "Tile|Material")
    int32 TextureIndex = 0;
};
