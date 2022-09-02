// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Array.h"
#include "Containers/EnumAsByte.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ProceduralFoliageSpawner.h"
#include "Templates/UnrealTypeTraits.h"
#include "UObject/NoExportTypes.h"

#include "RegionOfInterest.generated.h"


UENUM(BlueprintType)
enum ERegionOfInterestType
{
  NONE,
  TERRAIN_REGION,      
  WATERBODIES_REGION,  // Not Supported yet 
  VEGETATION_REGION
};

USTRUCT(BlueprintType)
struct CARLATOOLS_API FRoiTile
{
  GENERATED_BODY()

  UPROPERTY(BlueprintReadWrite)
  int X;
  UPROPERTY(BlueprintReadWrite)
  int Y;

public:
  FRoiTile() : X(-1), Y(-1) 
  {};

  FRoiTile(int X, int Y)
  {
    this->X = X;
    this->Y = Y;
  };

  FRoiTile(const FRoiTile& Other)
    : FRoiTile(Other.X, Other.Y)
  {}

  bool operator==(const FRoiTile& Other) const
  {
    return Equals(Other);
  }

  bool Equals(const FRoiTile& Other) const
  {
    return (this->X == Other.X) && (this->Y == Other.Y);
  }

  /// A function that returns the tile that is above the current tile.
  FORCEINLINE FRoiTile Up()
  {
    // return FRoiTile(X, Y-1);
    return FRoiTile(X-1, Y);
  }

  /// A function that returns the tile that is below the current tile.
  FORCEINLINE FRoiTile Down()
  {
    // return FRoiTile(X, Y+1);
    return FRoiTile(X+1, Y);
  }

  /// A function that returns the tile that is to the left of the current tile.
  FORCEINLINE FRoiTile Left()
  {
    // return FRoiTile(X-1, Y);
    return FRoiTile(X, Y+1);
  }

  /// A function that returns the tile that is to the right of the current tile.
  FORCEINLINE FRoiTile Right()
  {
    // return FRoiTile(X+1, Y);
    return FRoiTile(X, Y-1);
  }
};

/// A function that is used to hash the FRoiTile struct.
/// It is used to hash the struct so that it can be used as a key in a TMap.
FORCEINLINE uint32 GetTypeHash(const FRoiTile& Thing)
{
  uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FRoiTile));
  return Hash;
}

/**
 * 
 */
USTRUCT(BlueprintType)
struct CARLATOOLS_API FRegionOfInterest
{
	GENERATED_BODY()

  UPROPERTY(BlueprintReadWrite)
  TArray<FRoiTile> TilesList;

  UPROPERTY(BlueprintReadWrite)
  TEnumAsByte<ERegionOfInterestType> RegionType = ERegionOfInterestType::NONE;

  FRegionOfInterest()
  {
    TilesList.Empty();
  }

  void AddTile(int X, int Y)
  {
    FRoiTile Tile(X,Y);
    TilesList.Add(Tile);
  }

  TEnumAsByte<ERegionOfInterestType> GetRegionType()
  {
    return this->RegionType;
  }

  // A template function that checks if a tile is in a map of regions.
  template <typename R>
  static FORCEINLINE bool IsTileInRegionsSet(FRoiTile RoiTile, TMap<FRoiTile, R> RoisMap)
  {
    static_assert(TIsDerivedFrom<R, FRegionOfInterest>::IsDerived, 
        "ROIs Map Value type is not an URegionOfInterest derived type.");   
    return RoisMap.Contains(RoiTile);
  }

  

};

USTRUCT(BlueprintType)
struct CARLATOOLS_API FVegetationROI : public FRegionOfInterest
{
  GENERATED_BODY()

  UPROPERTY(BlueprintReadWrite)
  TArray<UProceduralFoliageSpawner*> FoliageSpawners;
  
  FVegetationROI() : FRegionOfInterest()
  {
    this->FoliageSpawners.Empty();
  }

  void AddFoliageSpawner(UProceduralFoliageSpawner* Spawner)
  {
    FoliageSpawners.Add(Spawner);
  }

  // A function that adds a list of spawners to the list of spawners of the ROI.
  void AddFoliageSpawners(TArray<UProceduralFoliageSpawner*> Spawners)
  {
    for(UProceduralFoliageSpawner* Spawner : Spawners)
    {
      AddFoliageSpawner(Spawner);
    }
  }

  TArray<UProceduralFoliageSpawner*> GetFoliageSpawners()
  {
    return this->FoliageSpawners;
  }
};

USTRUCT(BlueprintType)
struct CARLATOOLS_API FTerrainROI : public FRegionOfInterest
{
  GENERATED_BODY()

  // A pointer to a material instance that is used to change the heightmap material of the ROI.
  UPROPERTY(BlueprintReadWrite)
  UMaterialInstanceDynamic* RoiMaterialInstance;

  // A render target that is used to store the heightmap of the ROI.
  UPROPERTY(BlueprintReadWrite)
  UTextureRenderTarget2D* RoiHeightmapRenderTarget;

  FTerrainROI() : FRegionOfInterest(), RoiMaterialInstance()
  {}

  /**
 * This function checks if a tile is on the boundary of a region of interest
 * 
 * @param RoiTile The tile we're checking
 * @param RoisMap The map of RoiTiles to Rois.
 * @param OutUp Is there a tile above this one?
 * @param OutRight Is there a ROI to the right of this tile?
 * @param OutDown Is there a ROI tile below this one?
 * @param OutLeft Is the tile to the left of the current tile in the RoiMap?
 *
 * return true if the tile is in a boundary
 */
  template <typename R>
  static bool IsTileInRoiBoundary(FRoiTile RoiTile, TMap<FRoiTile, R> RoisMap, bool& OutUp, bool& OutRight, bool& OutDown, bool& OutLeft)
  {
    OutUp   = RoisMap.Contains(RoiTile.Up());
    OutDown = RoisMap.Contains(RoiTile.Down());
    OutLeft = RoisMap.Contains(RoiTile.Left());
    OutRight = RoisMap.Contains(RoiTile.Right());

    return !OutUp || !OutDown || !OutLeft || !OutRight;
  }
};
