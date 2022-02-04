#include <glib.h>

#include <cmath>
#include <memory>
#include <vector>

class GameObject
{
public:
	enum class UpdateInfo
	{
		None,
		Remove,
		Win,
		Lose
	};

	enum class CollisionMask
	{
		Any,
		Wall,
		None
	};

	enum class Type
	{
		Player,
		Finish,
		Enemy,
		Wall
	};

	GameObject(float x, float y) : x(x), y(y), px(x), py(y) { }
	virtual ~GameObject() = default;

	bool collides(const GameObject& other) const
	{
		return top() + 0.05f < other.bottom() && bottom() - 0.05f > other.top() && left() + 0.05f < other.right() && right() - 0.05f > other.left();
	}

	void draw(glib& glib) const
	{
		glib.rect(left(), top(), 10.0f, 10.0f, color());
	}

	UpdateInfo update(const std::vector<std::unique_ptr<GameObject>>& gos)
	{
		UpdateInfo info = UpdateInfo::None;
		onUpdate(info);

		if (cmask() == CollisionMask::None) { return info; }

		std::vector<int> collided;

		for (int i = 0; i < gos.size(); i++)
		{
			if (this == gos[i].get()) { continue; }

			if (passesCMaskTest(*gos[i]) && collides(*gos[i]))
			{
				onCollide(*gos[i], info);
				collided.push_back(i);
			}
		}

		if (info != UpdateInfo::Lose && collided.size() > 0)
		{
			if (collides(gos, collided))
			{
				float tmpX = x;

				float stepX = (px - x) / 20.0f;

				for (int j = 0; j < 20 && collides(gos, collided); j++)
				{
					x += stepX;
				}

				float resultX = x;

				if (collides(gos, collided))
				{
					x = tmpX;

					float stepY = (py - y) / 20.0f;

					for (int j = 0; j < 20 && collides(gos, collided); j++)
					{
						y += stepY;
					}

					if (collides(gos, collided))
					{
						x = resultX;
					}
				}
			}
		}

		return info;
	}

	virtual Type type() const = 0;

protected:
	float getX() const { return x; }
	float getY() const { return y; }
	void setX(float nx) { px = x; x = nx; }
	void setY(float ny) { py = y; y = ny; }

	float top() const { return getY(); }
	float bottom() const { return getY() + 10.0f; }
	float left() const { return getX(); }
	float right() const { return getX() + 10.0f; }

	virtual glib::color color() const = 0;
	virtual CollisionMask cmask() const = 0;

	virtual void onUpdate(UpdateInfo& info) {}
	virtual void onCollide(const GameObject& go, UpdateInfo& info) {}

private:
	float x;
	float y;
	float px;
	float py;

	bool passesCMaskTest(const GameObject& other) const
	{
		switch (cmask())
		{
		case CollisionMask::Any:
			return true;

		case CollisionMask::Wall:
			return other.type() == Type::Wall;

		default:
			return false;
		}
	}

	bool collides(const std::vector<std::unique_ptr<GameObject>>& gos, const std::vector<int>& check)
	{
		for (int i = 0; i < check.size(); i++)
		{
			if (collides(*gos[check[i]])) { return true; }
		}
		return false;
	}
};

class Wall final : public GameObject
{
public:
	using GameObject::GameObject;

	virtual Type type() const override { return Type::Wall; }

protected:
	virtual glib::color color() const override { return { 0, 0, 0, 255 }; }
	virtual CollisionMask cmask() const override { return CollisionMask::None; }
};

class Finish final : public GameObject
{
public:
	using GameObject::GameObject;

	virtual Type type() const override { return Type::Finish; }

protected:
	virtual glib::color color() const override { return { 0, 255, 0, 255 }; }
	virtual CollisionMask cmask() const override { return CollisionMask::None; }
};

class Player final : public GameObject
{
public:
	using GameObject::GameObject;

	virtual Type type() const override { return Type::Player; }

protected:
	virtual glib::color color() const override { return { 0, 0, 255, 255 }; }
	virtual CollisionMask cmask() const override { return CollisionMask::Any; }

	virtual void onUpdate(UpdateInfo& info) override
	{
		circlePosition pos;
		hidCircleRead(&pos);
		if (pos.dx > CIRCLE_CUTOFF || pos.dx < -CIRCLE_CUTOFF) { setX(getX() + pos.dx * SPEED); }
		if (pos.dy > CIRCLE_CUTOFF || pos.dy < -CIRCLE_CUTOFF) { setY(getY() + pos.dy * SPEED); }
	}

	virtual void onCollide(const GameObject& go, UpdateInfo& info) override
	{
		switch (go.type())
		{
		case Type::Enemy:
			info = UpdateInfo::Lose;
			break;

		case Type::Finish:
			info = UpdateInfo::Win;
			break;
		}
	}

private:
	static constexpr float SPEED = 1.0f / 50.0f;
	static constexpr s16 CIRCLE_CUTOFF = 24;
};

class Enemy : public GameObject
{
public:
	using GameObject::GameObject;

	virtual Type type() const final override { return Type::Enemy; }

protected:
	virtual glib::color color() const final override { return { 255, 0, 0, 255 }; }
	virtual CollisionMask cmask() const final override { return CollisionMask::Wall; }

	virtual void onUpdate(UpdateInfo& info) override = 0;
};

class BouncingEnemy final : public Enemy
{
public:
	BouncingEnemy(float x, float y, float speed, bool axis) : Enemy(x, y), speed(speed), axis(axis), collided(false) { }

protected:
	virtual void onUpdate(UpdateInfo& info) override
	{
		if (axis)
		{
			setY(getY() + speed);
		}
		else
		{
			setX(getX() + speed);
		}
		collided = false;
	}

	virtual void onCollide(const GameObject& go, UpdateInfo& info) override
	{
		if (!collided)
		{
			speed = -speed;
			collided = true;
		}
	}

private:
	float speed;
	bool axis;
	bool collided;
};

void level1(std::vector<std::unique_ptr<GameObject>>& gos)
{
	gos.emplace_back(std::make_unique<Player>(30.0f, 30.0f));
	gos.emplace_back(std::make_unique<Finish>(370.0f, 210.0f));

	for (int i = 0; i < 34; i++)
	{
		gos.emplace_back(std::make_unique<Wall>(i * 10.0f, 60.0f));
	}

	for (int i = 6; i < 40; i++)
	{
		gos.emplace_back(std::make_unique<Wall>(i * 10.0f, 180.0f));
	}

	gos.emplace_back(std::make_unique<BouncingEnemy>(90.0f, 120.0f, -2.0f, true));
	gos.emplace_back(std::make_unique<BouncingEnemy>(120.0f, 120.0f, 2.0f, true));
	gos.emplace_back(std::make_unique<BouncingEnemy>(150.0f, 120.0f, -2.0f, true));
	gos.emplace_back(std::make_unique<BouncingEnemy>(180.0f, 120.0f, 2.0f, true));
	gos.emplace_back(std::make_unique<BouncingEnemy>(220.0f, 120.0f, -2.0f, true));
	gos.emplace_back(std::make_unique<BouncingEnemy>(250.0f, 120.0f, 2.0f, true));
	gos.emplace_back(std::make_unique<BouncingEnemy>(280.0f, 120.0f, -2.0f, true));
	gos.emplace_back(std::make_unique<BouncingEnemy>(310.0f, 120.0f, 2.0f, true));
}

void level2(std::vector<std::unique_ptr<GameObject>>& gos)
{
	gos.emplace_back(std::make_unique<Player>(30.0f, 30.0f));
	gos.emplace_back(std::make_unique<Finish>(370.0f, 30.0f));

	for (int i = 0; i < 19; i++)
	{
		gos.emplace_back(std::make_unique<Wall>(70.0f, i * 10.0f));
	}

	for (int i = 0; i < 19; i++)
	{
		gos.emplace_back(std::make_unique<Wall>(330.0f, i * 10.0f));
	}

	for (int i = 5; i < 24; i++)
	{
		gos.emplace_back(std::make_unique<Wall>(200.0f, i * 10.0f));
	}

	for (int i = 0; i < 5; i++)
	{
		gos.emplace_back(std::make_unique<BouncingEnemy>(135.0f, 140.0f + i * 10.0f, 2.0f, false));
		gos.emplace_back(std::make_unique<BouncingEnemy>(135.0f, 140.0f + i * 10.0f, -2.0f, false));
		gos.emplace_back(std::make_unique<BouncingEnemy>(265.0f, 140.0f + i * 10.0f, 2.0f, false));
		gos.emplace_back(std::make_unique<BouncingEnemy>(265.0f, 140.0f + i * 10.0f, -2.0f, false));
	}
}

typedef void (*levelSetupFn)(std::vector<std::unique_ptr<GameObject>>&);

const levelSetupFn levels[]
{
	level1,
	level2
};

bool loadLevel(int level, std::vector<std::unique_ptr<GameObject>>& gos)
{
	gos.clear();

	for (int i = -1; i < 41; i++)
	{
		gos.emplace_back(std::make_unique<Wall>(i * 10.0f, 240.0f));
		gos.emplace_back(std::make_unique<Wall>(i * 10.0f, -10.0f));
	}
	for (int i = -1; i < 24; i++)
	{
		gos.emplace_back(std::make_unique<Wall>(-10.0f, (i + 1) * 10.0f));
		gos.emplace_back(std::make_unique<Wall>(400.0f, (i + 1) * 10.0f));
	}

	if (level >= 0 && level < sizeof(levels) / sizeof(*levels))
	{
		levels[level](gos);
		return true;
	}
	else
	{
		return false;
	}
}

int main()
{
	{
		glib glib;
		glib.setClearColor({ 255, 255, 255, 255 });

		int level = 0;
		int delay = 0;
		bool lost = false;
		bool won = false;
		std::vector<std::unique_ptr<GameObject>> gos;
		
		loadLevel(level, gos);

		while (aptMainLoop())
		{
			hidScanInput();

			u32 kDown = hidKeysDown();
			if (kDown & KEY_START) { break; }
			if (kDown & KEY_SELECT) { loadLevel(level, gos); }
#ifdef DEBUG
			if (kDown & KEY_A) 
			{
				level++;
				if (!loadLevel(level, gos))
				{
					level--;
					loadLevel(level, gos);
				}
			}
			if (kDown & KEY_B)
			{
				level--;
				if (!loadLevel(level, gos))
				{
					level++;
					loadLevel(level, gos);
				}
			}
#endif // DEBUG

			if (delay > 0)
			{
				delay--;
			}
			else
			{
				if (lost)
				{
					loadLevel(level, gos);
					lost = false;
				}
				if (won)
				{
					level++;
					if (!loadLevel(level, gos))
					{
						// could not load level -- probably done with game
						// just go back to start for now
						level = 0;
						loadLevel(level, gos);
					}
					won = false;
				}

				for (int i = gos.size() - 1; i >= 0; i--)
				{
					switch (gos[i]->update(gos))
					{
					case GameObject::UpdateInfo::Remove:
						gos.erase(gos.begin() + i);
						break;

					case GameObject::UpdateInfo::Win:
						won = true;
						delay = 30;
						break;

					case GameObject::UpdateInfo::Lose:
						lost = true;
						delay = 30;
						break;
					}
				}
			}

			glib.beginFrame();

			for (int i = 0; i < gos.size(); i++)
			{
				gos[i]->draw(glib);
			}

			glib.endFrame();
		}
	}

	return 0;
}