/*
 * Copyright (C) 2002-2018 by the Widelands Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef WL_MAP_IO_MAP_BUILDING_PACKET_H
#define WL_MAP_IO_MAP_BUILDING_PACKET_H

#include "map_io/map_data_packet.h"

class FileRead;
class FileWrite;

namespace Widelands {

class Building;

/*
 * This packet cares for the existence of buildings
 * on the map, the data is parsed somewhere else
 */
struct MapBuildingPacket {
	void read(FileSystem&, EditorGameBase&, bool, MapObjectLoader&);
	void write(FileSystem&, EditorGameBase&, MapObjectSaver&);

protected:
	void read_priorities(Building&, FileRead&);
	void write_priorities(const Building&, FileWrite&);
};
}

#endif  // end of include guard: WL_MAP_IO_MAP_BUILDING_PACKET_H
