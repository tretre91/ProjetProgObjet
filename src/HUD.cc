#include "HUD.h"
#include "Character.h"
#include "Labyrinthe.h"
#include <fmt/ranges.h>

Labyrinthe* HUD::labyrinth = nullptr;
std::string HUD::data;

std::list<std::string> HUD::messages;
std::multimap<HUD::time_point, std::list<std::string>::iterator> HUD::temporary_messages;

Character* HUD::focused_guard = nullptr;
HUD::time_point HUD::focused_guard_timeout = time_point::max();

HUD::Cache HUD::cache;
bool HUD::redraw = true;


void HUD::init(Labyrinthe* l) {
	labyrinth = l;
}

void HUD::refresh_cache() {
	Character* player = dynamic_cast<Character*>(labyrinth->_guards[0]);

	if (int current_hp = player->current_hp(); cache.player_health != current_hp) {
		redraw = true;
		cache.player_health = current_hp;
	}

	if (int max_hp = player->max_hp(); cache.player_max_health != max_hp) {
		redraw = true;
		cache.player_max_health = max_hp;
	}

	if (cache.focused_guard != focused_guard) {
		redraw = true;
		cache.focused_guard = focused_guard;
	}

	if (focused_guard != nullptr) {
		if (int guard_hp = focused_guard->current_hp(); cache.focused_guard_health != guard_hp) {
			redraw = true;
			cache.focused_guard_health = guard_hp;
		}

		if (int guard_max_hp = focused_guard->max_hp(); cache.focused_guard_max_health != guard_max_hp) {
			redraw = true;
			cache.focused_guard_max_health = guard_max_hp;
		}
	} else {
		cache.focused_guard_health = -1;
		cache.focused_guard_max_health = -1;
	}
}

void HUD::update() {
	// we erase the messages planned for deletion
	const auto now = time_point::clock::now();
	auto it = temporary_messages.begin();
	while (it != temporary_messages.end() && it->first < now) {
		redraw = true;
		messages.erase(it->second);
		it = temporary_messages.erase(it);
	}

	// we erase the guard's hp
	if (focused_guard_timeout < now) {
		focused_guard = nullptr;
	}

	refresh_cache();

	if (redraw) {
		data = fmt::format("HP: {}/{}", cache.player_health, cache.player_max_health);
		if (cache.focused_guard != nullptr) {
			data += fmt::format(" | guard HP: {}/{}", cache.focused_guard_health, cache.focused_guard_max_health);
		}
		if (!messages.empty()) {
			data += fmt::format(" | {}", fmt::join(messages, "\n"));
		}
		message(data.c_str());
		redraw = false;
	}
}

void HUD::add_message(const std::string& msg, duration timeout) {
	auto it = messages.insert(messages.end(), msg);
	if (timeout != duration::zero()) {
		time_point delete_time_point = time_point::clock::now() + timeout;
		temporary_messages.insert(std::make_pair(delete_time_point, it));
	}

	redraw = true;
	update();
}

void HUD::focus_guard(Character* guard) {
	using namespace std::chrono_literals;
	focused_guard = guard;
	focused_guard_timeout = time_point::clock::now() + 4s;
}

void HUD::clear() {
	temporary_messages.clear();
	messages.clear();
	redraw = true;
}
