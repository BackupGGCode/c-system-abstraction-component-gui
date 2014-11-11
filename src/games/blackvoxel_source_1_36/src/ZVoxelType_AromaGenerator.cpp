/*
 * This file is part of Blackvoxel.
 *
 * Copyright 2010-2014 Laurent Thiebaut & Olivia Merle
 *
 * Blackvoxel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Blackvoxel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * ZVoxelType_AromaGenerator.cpp
 *
 *  Created on: 4 févr. 2013
 *      Author: laurent
 */

#ifndef Z_VOXELTYPE_AROMAGENERATOR_H
#  include "ZVoxelType_AromaGenerator.h"
#endif

#ifndef Z_ZVOXELEXTENSION_AROMAGENERATOR_H
#  include "ZVoxelExtension_AromaGenerator.h"
#endif


ZVoxelExtension * ZVoxelType_AromaGenerator::CreateVoxelExtension(bool IsLoadingPhase)
{
  ZVoxelExtension_AromaGenerator * NewVoxelExtension = 0;

  NewVoxelExtension = new ZVoxelExtension_AromaGenerator;
  NewVoxelExtension->time_since_spawn = ( ( -rand() * 2000.0 ) / RAND_MAX );
  return (NewVoxelExtension);
}

void ZVoxelType_AromaGenerator::React( const ZVoxelRef &self, double tick )
{
	ZVoxelExtension_AromaGenerator *instance = (ZVoxelExtension_AromaGenerator *)self.VoxelExtension;
	instance->time_since_spawn += tick;
	if( instance->time_since_spawn > 1000 )
	{
		    Long RSx = self.Sector->Pos_x << ZVOXELBLOCSHIFT_X;
			Long RSy = self.Sector->Pos_y << ZVOXELBLOCSHIFT_Y;
			Long RSz = self.Sector->Pos_z << ZVOXELBLOCSHIFT_Z;

		UShort above = self.World->GetVoxel( RSx +self.x, RSy +self.y+1, RSz +self.z );
		if( above == 0 )
		{
			// set some green plant life.
			self.World->SetVoxel_WithCullingUpdate(RSx + self.x , RSy + self.y + 1, RSz + self.z , 236, ZVoxelSector::CHANGE_CRITICAL);                                         
		}
		instance->time_since_spawn = 0;
	}
	//lprintf( "Ground react at %g", tick );
}


