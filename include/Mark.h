#ifndef LAB_MARK_H
#define LAB_MARK_H

#include "Cell.h"
#include "Environnement.h"

/**
 * @brief Possible mark types
 */
enum class MarkType {
	normal,
	heal,
	teleporter
};

/**
 * @brief Classe representing marks with no effects.
 */
struct Mark : public Box {
public:
	// mark default textures
	inline static int _empty_texture;
	inline static int _healing_texture;
	inline static int _mark_texture;

	Mark(int x, int y) : Mark(x, y, _empty_texture) {}
	Mark(int x, int y, int texture_id) : Box{x, y, texture_id} {}
	virtual ~Mark() {}

	/**
	 * @brief Returns the type of this mark
	 */
	virtual MarkType type() const { return MarkType::normal; }
};

/**
 * @brief Class for marks which heal for a certain amount when stepped on.
 */
struct HealingMark : public Mark {
	int _heal_amount = 20;

	HealingMark(int x, int y, int amount) : HealingMark(x, y, _healing_texture, amount) {}
	HealingMark(int x, int y, int texture_id, int amount) : Mark(x, y, texture_id), _heal_amount(amount) {}

	MarkType type() const override { return MarkType::heal; }

	int heal_amount() const { return _heal_amount; }
};

/**
 * @brief Class for marks which teleport the player to another mark.
 */
struct TeleporterMark : public Mark {
	TeleporterMark* _target = nullptr; // this mark's teleport target

	TeleporterMark(int x, int y, TeleporterMark* target) : TeleporterMark(x, y, _mark_texture, target) {}
	TeleporterMark(int x, int y, int texture_id, TeleporterMark* target) : Mark(x, y, texture_id), _target(target) {}

	MarkType type() const override { return MarkType::teleporter; }
};

#endif
