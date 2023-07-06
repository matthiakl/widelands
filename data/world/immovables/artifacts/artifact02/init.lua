push_textdomain("world")

local dirname = path.dirname(__file__)

wl.Descriptions():new_immovable_type{
   name = "artifact02",
   descname = _("Artifact"),
   size = "small",
   programs = {},
   animation_directory = dirname,
   icon = dirname .. "menu.png",
   animations = {
      idle = {
         hotspot = { 14, 20 },
      },
   }
}

pop_textdomain()
