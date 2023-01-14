#include "HUD.h"
#include "Character.h"
#include "Labyrinthe.h"
#include <fmt/ranges.h>

Labyrinthe* HUD::_labyrinth = nullptr;
std::string HUD::_data;

std::list<std::string> HUD::_messages;
std::multimap<HUD::time_point, std::list<std::string>::iterator> HUD::_temporary_messages;

Character* HUD::_focused_guard = nullptr;
HUD::time_point HUD::_focused_guard_timeout = time_point::max();

HUD::Cache HUD::_cache;
bool HUD::_redraw = true;


void HUD::init(Labyrinthe* l) {
	_labyrinth = l;
}

void HUD::refresh_cache() {
	Character* player = dynamic_cast<Character*>(_labyrinth->_guards[0]);

	if (int current_hp = player->hp(); _cache.player_health != current_hp) {
		_redraw = true;
		_cache.player_health = current_hp;
	}

	if (int max_hp = player->max_hp(); _cache.player_max_health != max_hp) {
		_redraw = true;
		_cache.player_max_health = max_hp;
	}

	if (_cache.focused_guard != _focused_guard) {
		_redraw = true;
		_cache.focused_guard = _focused_guard;
	}

	if (_focused_guard != nullptr) {
		if (int guard_hp = _focused_guard->hp(); _cache.focused_guard_health != guard_hp) {
			_redraw = true;
			_cache.focused_guard_health = guard_hp;
		}

		if (int guard_max_hp = _focused_guard->max_hp(); _cache.focused_guard_max_health != guard_max_hp) {
			_redraw = true;
			_cache.focused_guard_max_health = guard_max_hp;
		}
	} else {
		_cache.focused_guard_health = -1;
		_cache.focused_guard_max_health = -1;
	}
}

void HUD::update() {
	// we erase the messages planned for deletion
	const auto now = time_point::clock::now();
	auto it = _temporary_messages.begin();
	while (it != _temporary_messages.end() && it->first < now) {
		_redraw = true;
		_messages.erase(it->second);
		it = _temporary_messages.erase(it);
	}

	// we erase the guard's hp
	if (_focused_guard_timeout < now) {
		_focused_guard = nullptr;
	}

	refresh_cache();

	if (_redraw) {
		_data = fmt::format("HP: {}/{}", _cache.player_health, _cache.player_max_health);
		if (_cache.focused_guard != nullptr) {
			_data += fmt::format(" | guard HP: {}/{}", _cache.focused_guard_health, _cache.focused_guard_max_health);
		}
		if (!_messages.empty()) {
			_data += fmt::format(" | {}", fmt::join(_messages, " | "));
		}
		message(_data.c_str());
		_redraw = false;
	}
}

void HUD::add_message(const std::string& msg, duration timeout) {
	auto it = _messages.insert(_messages.end(), msg);
	if (timeout != duration::zero()) {
		time_point delete_time_point = time_point::clock::now() + timeout;
		_temporary_messages.insert(std::make_pair(delete_time_point, it));
	}

	_redraw = true;
	update();
}

void HUD::focus_guard(Character* guard) {
	using namespace std::chrono_literals;
	_focused_guard = guard;
	_focused_guard_timeout = time_point::clock::now() + 4s;
}

void HUD::clear() {
	_temporary_messages.clear();
	_messages.clear();
	_redraw = true;
}
