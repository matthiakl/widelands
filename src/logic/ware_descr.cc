/*
 * Copyright (C) 2002-2004, 2006-2008, 2011-2012 by the Widelands Development Team
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

#include "logic/ware_descr.h"

#include <boost/format.hpp>

#include "base/i18n.h"
#include "graphic/animation.h"
#include "graphic/graphic.h"
#include "logic/tribe.h"
#include "profile/profile.h"

namespace Widelands {

WareDescr::WareDescr
	(char const * const _name,
	 char const * const _descname,
	 const std::string & directory, Profile & prof, Section & global_s)
	:
	MapObjectDescr(MapObjectType::WARE, _name, _descname),
	generic_name_   ("NOCOM generic name"),
	icon_fname_    (directory + "/menu.png"),
	icon_(g_gr->images().get("pics/but0.png"))
{
	helptexts_.insert("default", "NOCOM default helptext");
	default_target_quantities_.insert
			("default",
			 global_s.get_positive("default_target_quantity", std::numeric_limits<uint32_t>::max()));

	add_animation("idle", g_gr->animations().load(directory, prof.get_safe_section("idle")));

	// NOCOM take care of tribes
	preciousnesses_.insert("default", global_s.get_natural("preciousness", 0));
}

WareDescr::WareDescr(const LuaTable& table) :
	MapObjectDescr(MapObjectType::WARE, table.get_string("name"), table.get_string("descname")),
	generic_name_(table.get_string("genericname")),
	icon_fname_(table.get_string("menu_picture")),
	icon_(g_gr->images().get("pics/but0.png")) {

	LuaTable items_table = table.get_table("default_target_quantity");
	for (const std::string& key : items_table.keys()) {
		default_target_quantities_.insert(key, items_table.get_int(key));
	}

	items_table = table.get_table("preciousness");
	for (const std::string& key : items_table.keys()) {
		preciousnesses_.insert(key, items_table.get_int(key));
	}

	items_table = table.get_table("helptext");
	for (const std::string& key : items_table.keys()) {
		helptexts_.insert(key, items_table.get_string(key));
	}

	items_table = table.get_table("animations");
	for (const std::string& key : items_table.keys()) {
		const LuaTable anims_table = table.get_table(key);
		for (const std::string& anim_key : anims_table.keys()) {
			// NOCOM(GunChleoc): And the hotspot?
			add_animation(anim_key, g_gr->animations().load(anims_table.get_string("pictures")));
		}
	}
}

int WareDescr::preciousness(const std::string& tribename) const {
	if (preciousnesses_.count(tribename > 0)) {
		return preciousnesses_.at(tribename);
	}
	return kInvalidWare;
}


int WareDescr::default_target_quantity(const std::string& tribename) const {
	if (default_target_quantities_.count(tribename > 0)) {
		return default_target_quantities_.at(tribename);
	}
	return kInvalidWare;
}

const std::string& WareDescr::helptext(const std::string tribename) const {
	if (helptexts_.count(tribename > 0)) {
		i18n::Textdomain td("tribes");
		if (helptexts_.count("default" > 0)) {
			return (boost::format("%s %s")
					  % _(helptexts_.at("default"))
					  % _(helptexts_.at(tribename))).str();
		} else {
			return _(helptexts_.at(tribename));
		}
	} else if (helptexts_.count("default" > 0)) {
		i18n::Textdomain td("tribes");
		return _(helptexts_.at("default"));
	} else {
		return _("This ware has no help text yet.");
	}
}


/**
 * Load all static graphics
 */
void WareDescr::load_graphics()
{
	icon_ = g_gr->images().get(icon_fname_);
}

bool WareDescr::has_demand_check(const std::string& tribename) const {
	return default_target_quantity(tribename) != kInvalidWare;
}

void WareDescr::set_has_demand_check(const std::string& tribename) {
	if (default_target_quantities_.count(tribename > 0)
		 && default_target_quantities_.at(tribename) == kInvalidWare) {
		default_target_quantities_.at(tribename) = 1;
	}
}

}
