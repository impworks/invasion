// rocket

rocket::rocket(float x, float y, float angle):dynamic_spr(x, y, angle)
{
	speed = 1;
	body = new hgeSprite(core->txRocket, 0, 0, 24, 24);
	body->SetHotSpot(12, 12);
	alive = true;

	dead_time = 0;

	// create trail
	trail = new hgeParticleSystem("data/rockettrail.psi", core->rocketTrailPiece);
	trail->FireAt(this->x, this->y);

	// create working thread
	this->thread = CreateThread(NULL, 0, UpdateRocket, (LPVOID)this, 0, 0);
}

void rocket::explode()
{
    this->alive = false;
    this->trail->Stop();
}



// bomb

bomb::bomb(float x, float y, float angle):dynamic_spr(x, y, angle)
{
    speed = 1;
    body = new hgeSprite(core->txBomb, 0, 0, 32, 32);
    body->SetHotSpot(16, 16);

    alive = true;
    dead_time = 0;
    fatal = false;
	curved = false;
    curve_dir = 0;
	angle_offset = 0;

    // create trail
    trail = new hgeParticleSystem("data/bombtrail.psi", core->bombTrailPiece);
    trail->FireAt(x, y);

    // prepare explosion
    explosion = new hgeParticleSystem("data/explosion.psi", core->explosionPiece);

    // create working thread
	thread = CreateThread(NULL, 0, UpdateBomb, (LPVOID)this, 0, 0);
}

void bomb::explode()
{
    this->fatal = false;
    this->alive = false;
    this->trail->Stop();
    this->explosion->FireAt(this->x, this->y);
}



// nuke 

nuke::nuke(float x, float y, float angle):dynamic_spr(x, y, angle)
{
    speed = 0.7;
    body = new hgeSprite(core->txNuke, 0, 0, 48, 48);
    body->SetHotSpot(24, 24);

    alive = true;
    dead_time = 0;
    fatal = false;
    curved = false;

    // create trail
    trail = new hgeParticleSystem("data/bombtrail.psi", core->nukeTrailPiece);
    trail->FireAt(x, y);

    // prepare explosion
    explosion = new hgeParticleSystem("data/explosion.psi", core->explosionPiece);

    // create working thread
	thread = CreateThread(NULL, 0, UpdateBomb, (LPVOID)this, 0, 0);
}

void nuke::explode()
{
    bomb *b = new bomb(this->x, this->y, 3.14-hge->Random_Float(0, 0.5));
	b->speed = 0.7;
	b->curved = true;
    b->curve_dir = (hge->Random_Int(0, 100) > 50 ? -1 : 1);
    core->bombs->add(b);

    b = new bomb(this->x, this->y, 3.14+hge->Random_Float(0, 0.5));
	b->speed = 0.7;
	b->curved = true;
    b->curve_dir = (hge->Random_Int(0, 100) > 50 ? -1 : 1);
    core->bombs->add(b);

    this->fatal = false;
    this->alive = false;
    this->trail->Stop();
    this->explosion->FireAt(this->x, this->y);
}