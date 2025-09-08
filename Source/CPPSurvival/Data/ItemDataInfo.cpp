#include "Data/ItemDataInfo.h"

FPrimaryAssetId UItemDataInfo::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType("Item"), GetFName());
}
