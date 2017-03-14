#include "BigEnemyShip.h"

BigEnemyShip::BigEnemyShip(float X, float Y, const Surface & surface, BulletManager& BulletM, AnimationFrames& ExploAnim, Sound& ExploSound, const Surface& ShipFlash)
	:
	pos(X, Y),
	shipSurface(surface),
	bulletM(BulletM),
	exploAnim(ExploAnim, 2.0f),
	exploSound(ExploSound),
	bulletTimer(1.5f),
	timer(15.0f),
	coolDownTimer(0.75f),
	shipFlash(ShipFlash),
	hitTimer(0.05f)
	
{}

void BigEnemyShip::Draw(Graphics & gfx)
{
	switch (state)
	{
	case AliveState:
		gfx.DrawSpriteKey(int(pos.x), int(pos.y), shipSurface, shipSurface.GetPixel(0, 0));
		if (isHit)
		{
			gfx.DrawSpriteKey(int(pos.x), int(pos.y), shipFlash, shipFlash.GetPixel(0, 0));
		}
		break;
		
	case DyingState:
		exploAnim.Draw(int(pos.x), int(pos.y), gfx);
		break;
	}
}

void BigEnemyShip::Update(float dt)
{
	switch (state)
	{
	case AliveState:
		Move(dt);
		if (coolDown)
		{
			if (coolDownTimer.Pause(dt))
			{
				coolDown = false;
				coolDownTimer.Reset();
			}
		}

		if (isHit)
		{
			if (hitTimer.Pause(dt))
			{
				isHit = false;
				hitTimer.Reset();
			}
		}

		if (hp <= 0)
		{
			exploSound.Play();
			state = DyingState;
		}
		break;

	case DyingState:
		exploAnim.Advance(dt);
		if (exploAnim.AnimEnd())
		{
			state = DeadState;
		}
		break;
	}
}

RectF BigEnemyShip::GetCollisionRect() const
{
	return RectF(pos, width, height);
}

int BigEnemyShip::GetCollisionDmg() const
{
	return collisionDmg;
}

int BigEnemyShip::GetBulletDmg() const
{
	return bulletDmg;
}

void BigEnemyShip::HandleCollision(int dmg)
{
	if (state == AliveState)
	{
		hp -= dmg;
		coolDown = true;
		isHit = true;
	}
}

void BigEnemyShip::Attack(float dt)
{
	if (bulletTimer.Pause(dt))
	{
		Vec2 canonPos = pos + canon;
		bulletTimer.Reset();
		bulletM.FireBullet(canonPos, bulletVel, bulletHalfWidth, bulletHalfHeight, bulletRectSize, bulletDmg, bulletPitch);
		bulletM.ResetShotsFired();
	}
}

void BigEnemyShip::Move(float dt)
{
	if (pos.y < 25.0f)
	{
		pos.y += vel.y * dt;
	}
	else if (!timer.Pause(dt))
	{
		pos.x += vel.x * dt;
		Attack(dt);

		timer.Increment(dt);
	}
	if ((pos.y < Graphics::ScreenHeight) && timer.PauseOver())
	{
		pos.y += vel.y * dt;
	}
	else if (pos.y > Graphics::ScreenHeight)
	{
		state = DeadState;
	}

	if ((pos.x <= 0 && vel.x < 0.0f) ||
		((pos.x + width) >= Graphics::ScreenWidth && vel.x > 0.0f))
	{
		vel.x = -vel.x;
	}
}

bool BigEnemyShip::IsAlive() const
{
	return state == AliveState;
}

bool BigEnemyShip::GetCoolDown() const
{
	return coolDown;
}
