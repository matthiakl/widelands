/*
 * Copyright (C) 2002, 2006-2015 by the Widelands Development Team
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
 */

#include "wui/mapdetails.h"

#include <cstdio>
#include <memory>

#include <boost/format.hpp>

#include "base/i18n.h"
#include "base/log.h"
#include "base/wexception.h"
#include "graphic/graphic.h"
#include "graphic/text_constants.h"
#include "io/filesystem/layered_filesystem.h"
#include "logic/game_controller.h"
#include "logic/game_settings.h"
#include "map_io/widelands_map_loader.h"
#include "ui_basic/box.h"

MapDetails::MapDetails
		(Panel* parent, int32_t x, int32_t y, int32_t max_x, int32_t max_y) :
	UI::Panel(parent, x, y, max_x, max_y),

	padding_(4),
	indent_(10),
	labelh_(20),
	max_w_(max_x),
	max_h_(max_y),
	// Subtract for main box and author box
	descr_box_height_(max_y - 4 * labelh_ - 5 * padding_),

	main_box_(this, 0, 0, UI::Box::Vertical,
		  max_w_, max_h_, 0),

	name_box_(&main_box_, 0, 0, UI::Box::Horizontal,
		  max_w_, 3 * labelh_ + padding_, padding_ / 2),
	name_label_(&main_box_, 0, 0, max_w_ - padding_, labelh_, ""),
	name_(&name_box_, 0, 0, max_w_ - indent_, 2 * labelh_, ""),

	author_box_(&main_box_, 0, 0, UI::Box::Horizontal,
		  max_w_, 3 * labelh_ + padding_, padding_ / 2),
	author_label_(&main_box_, 0, 0, max_w_ - padding_, labelh_, ""),
	author_(&author_box_, 0, 0, max_w_ - indent_, labelh_, ""),

	descr_box_(&main_box_, 0, 0, UI::Box::Horizontal,
		  max_w_, descr_box_height_, padding_ / 2),
	descr_label_(&main_box_, 0, 0, max_w_, labelh_, ""),
	descr_(&descr_box_, 0, 0, max_w_ - indent_, descr_box_height_ - labelh_ - padding_, "")
{
	suggested_teams_box_ = new UI::SuggestedTeamsBox(this, 0, 0, UI::Box::Vertical,
																	 padding_, indent_, labelh_, max_w_, 4 * labelh_);

	main_box_.add(&name_label_, UI::Align::kLeft);
	name_box_.add_space(indent_);
	name_box_.add(&name_, UI::Align::kLeft);
	main_box_.add(&name_box_, UI::Align::kLeft);
	main_box_.add_space(padding_);

	main_box_.add(&author_label_, UI::Align::kLeft);
	author_box_.add_space(indent_);
	author_box_.add(&author_, UI::Align::kLeft);
	main_box_.add(&author_box_, UI::Align::kLeft);
	main_box_.add_space(padding_);

	main_box_.add(&descr_label_, UI::Align::kLeft);
	descr_box_.add_space(indent_);
	descr_box_.add(&descr_, UI::Align::kLeft);
	main_box_.add(&descr_box_, UI::Align::kLeft);
	main_box_.add_space(padding_);
}


void MapDetails::clear() {
	name_label_.set_text("");
	author_label_.set_text("");
	descr_label_.set_text("");
	name_.set_text("");
	author_.set_text("");
	descr_.set_text("");
	suggested_teams_box_->hide();
}

void MapDetails::set_max_height(int new_height) {
	max_h_ = new_height;
	descr_box_height_ = max_h_ - 4 * labelh_ - 5 * padding_;
	update_layout();
}

void MapDetails::update_layout() {
	// Adjust sizes for show / hide suggested teams
	if (suggested_teams_box_->is_visible()) {
		suggested_teams_box_->set_pos(Point(0, max_h_ - suggested_teams_box_->get_h()));
		main_box_.set_size(max_w_, max_h_ - suggested_teams_box_->get_h());
		descr_box_.set_size(
					descr_box_.get_w(),
					descr_box_height_ - suggested_teams_box_->get_h() - padding_);
	} else {
		main_box_.set_size(max_w_, max_h_);
		descr_box_.set_size(descr_box_.get_w(), descr_box_height_);
	}
	descr_.set_size(descr_.get_w(), descr_box_.get_h() - descr_label_.get_h() - padding_);
	descr_.scroll_to_top();
}

void MapDetails::update(const MapData& mapdata, bool localize_mapname) {
	clear();
	if (mapdata.maptype == MapData::MapType::kDirectory) {
		// Show directory information
		name_label_.set_text(_("Directory:"));
		name_.set_text(mapdata.localized_name);
		name_.set_tooltip(_("The name of this directory"));
		main_box_.set_size(max_w_, max_h_);
	} else {
		// Show map information
		if (mapdata.maptype == MapData::MapType::kScenario) {
			name_label_.set_text(_("Scenario:"));
		} else {
			name_label_.set_text(_("Map:"));
		}
		name_.set_text(localize_mapname ? mapdata.localized_name : mapdata.name);
		if (mapdata.localized_name != mapdata.name) {
			if (localize_mapname) {
				name_.set_tooltip
				/** TRANSLATORS: Tooltip in map description when translated map names are being displayed. */
				/** TRANSLATORS: %s is the English name of the map. */
						((boost::format(_("The original name of this map: %s"))
						  % mapdata.name).str());
			} else {
				name_.set_tooltip
				/** TRANSLATORS: Tooltip in map description when map names are being displayed in English. */
				/** TRANSLATORS: %s is the localized name of the map. */
						((boost::format(_("The name of this map in your language: %s"))
						  % mapdata.localized_name).str());
			}
		} else {
			name_.set_tooltip(_("The name of this map"));
		}
		author_label_.set_text(ngettext("Author:", "Authors:", mapdata.authors.get_number()));
		author_.set_text(mapdata.authors.get_names());
		descr_label_.set_text(_("Description:"));
		descr_.set_text(
					mapdata.description +
					/** TRANSLATORS: Map hint header when selecting a map. */
					(mapdata.hint.empty() ? "" : (std::string("\n\n") + _("HINT:") + "\n" + mapdata.hint)));

		// Show / hide suggested teams
		if (mapdata.suggested_teams.empty()) {
			suggested_teams_box_->hide();
		} else {
			suggested_teams_box_->show(mapdata.suggested_teams);
		}
	}
	update_layout();
}
