int file_exists(char* filename)
{
  struct stat statbuf;
  if(stat(filename, &statbuf) < 0)
  {
    if(errno == ENOENT)
      return 0;
    else
      return 1;
  }
  return 1;
}


// core definitions

CORE::CORE()
{
    rockets = new list<rocket>(15);
    bombs = new list<bomb>(20);
	nukes = new list<nuke>(5);
}

CORE::~CORE()
{
    delete this->ground;
    delete this->turret;
    delete this->gun;
    delete this->sky;
    delete this->crosshair;
    delete this->rocketTrailPiece;
    delete this->bombTrailPiece;
	delete this->nukeTrailPiece;

    delete this->rockets;
    delete this->bombs;
    delete this->nukes;
}

int CORE::load_data()
{
    this->init();

    // load font
    fnt = new hgeFont("data/font.fnt");
    fnt->SetColor(0xFFFFFFFF);

    // create sky
    sky = new hgeSprite(0, 0, 0, 800, 600);
    sky->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND);

    // create textures
    txRocket = hge->Texture_Load("data/rocket.png");
    txBomb = hge->Texture_Load("data/bomb.png");
	txNuke = hge->Texture_Load("data/nuke.png");

    // create crosshair
    txCrosshair = hge->Texture_Load("data/crosshair.png");
    crosshair = new hgeSprite(txCrosshair, 0, 0, 16, 16);
    crosshair->SetHotSpot(8, 8);

    // create ground
    txGround = hge->Texture_Load("data/ground.png");
    ground = new hgeSprite(txGround, 0, 0, 800, 100);

    // create turret
    txTurret = hge->Texture_Load("data/turret.png");
    turret = new hgeSprite(txTurret, 0, 0, 128, 128);

    // create gun
    txGun = hge->Texture_Load("data/gun.png");
    gun = new hgeSprite(txGun, 0, 0, 72, 72);
    gun->SetHotSpot(36, 56);
    this->turretAngle = 0;

	// sign
	txSign = hge->Texture_Load("data/sign.png");
	sign = new hgeSprite(txSign, 0, 0, 64, 128);
	sign->SetHotSpot(32, 64);

    // create particle systems
    txRocketTrail = hge->Texture_Load("data/rockettrail.png");
    rocketTrailPiece = new hgeSprite(txRocketTrail, 0, 0, 24, 24);
    rocketTrailPiece->SetHotSpot(12, 12);
    rocketTrailPiece->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_ZWRITE);

    txBombTrail = hge->Texture_Load("data/bombtrail.png");
    bombTrailPiece = new hgeSprite(txBombTrail, 0, 0, 32, 32);
    bombTrailPiece->SetHotSpot(16, 16);
    bombTrailPiece->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_ZWRITE);

	txNukeTrail = hge->Texture_Load("data/nuketrail.png");
    nukeTrailPiece = new hgeSprite(txNukeTrail, 0, 0, 32, 32);
    nukeTrailPiece->SetHotSpot(16, 16);
    nukeTrailPiece->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_ZWRITE);

    txExplosion = hge->Texture_Load("data/explosion.png");
    explosionPiece = new hgeSprite(txExplosion, 0, 0, 48, 48);
    explosionPiece->SetHotSpot(24, 24);
    explosionPiece->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_ZWRITE);

    // now make sound!
    music = hge->Music_Load("data/bg.mod");
    if(music) channel = hge->Music_Play(music, true);
    hge->Channel_SetVolume(channel, 40);

    if(txRocket && txBomb && txCrosshair && txGround && txTurret && txSign && txGun && txRocketTrail && txBombTrail && txExplosion && txNuke && txNukeTrail && music && channel) return 1;
    return 0;
}

void CORE::init()
{
    // values
    hge->Random_Seed(0);
    this->bombInterval = this->bombTimer = this->bombSpeedup = 4;
	this->nukeTimer = 30;
    score = fired = hit = 0;
    textalpha = 0xFF;

	alive = true;
	pause = false;

    // sky & stars
    colors[0] = 0;
    colors[1] = 16;
    colors[2] = 16;
    colors[3] = 0;
    colors[4] = 21;
    colors[5] = 80;

	starcolor = 0;
	for(int idx=0; idx<100; idx++)
	{
		stars[idx].x = hge->Random_Int(0, 800);
		stars[idx].y = hge->Random_Int(0, 600);
	}

    // empty lists
    this->rockets->empty();
    this->bombs->empty();
    this->nukes->empty();
}

void CORE::rotate_turret()
{
    if(this->mouseY < 400)
        this->turretAngle = atan((this->mouseX-398)/(420-this->mouseY));
    else if(this->mouseX > 398)
        this->turretAngle = 3.14/2;
    else
        this->turretAngle = 3.14*3/2;
}

void CORE::fire_rocket()
{
    if(this->rockets->length == 15) return;

    // create and set rocket up
    rocket *r = new rocket(398 + sin(this->turretAngle)*50, 420 - cos(this->turretAngle)*50, this->turretAngle);

    // add rocket to the list
    this->rockets->add(r);
}

void CORE::drop_bomb()
{
    // create and set bomb up
	float x = hge->Random_Int(20, 780);
    bomb *b = new bomb(x, -32, 3.14 + atan((hge->Random_Int(200, 600)-x)/(-532)));

    // hard mode ?
    if(hge->Timer_GetTime() > 40 && hge->Random_Int(0, 100) > 50)
    {
        b->curved = true;
        b->curve_dir = (hge->Random_Int(0, 100) > 50 ? -1 : 1);
    }
	
    // add bomb to list
    this->bombs->add(b);
}

void CORE::drop_nuke()
{
    // create and set nuke up
	float x = hge->Random_Int(20, 780);
    nuke *n = new nuke(x, -32, 3.14 + atan((hge->Random_Int(200, 600)-x)/(-532)));
	
    // add bomb to list
    this->nukes->add(n);
}

void CORE::explode_bombs()
{
    rocket *curr = this->rockets->first;
    for(int idx=0; idx<this->rockets->length; idx++)
    {
        bomb *currb = this->bombs->first;
        for(int idx2=0; idx2<this->bombs->length; idx2++)
        {
            if(curr->collides(currb) && curr->alive)
            {
                curr->explode();
                currb->explode();
                this->score += (100-currb->y/5);
            }

            if(currb->x < -10 || currb->x > 810) currb->explode();
            currb = (bomb *)currb->next;
        }

		nuke *currn = this->nukes->first;
        for(idx2=0; idx2<this->nukes->length; idx2++)
        {
            if(curr->collides(currn) && curr->alive)
            {
                curr->explode();
                currn->explode();
                this->score += (150-currn->y/7);
            }
            currn = (nuke *)currn->next;
        }		

        curr = (rocket *)curr->next;
    }
}

void CORE::repaint_sky()
{
    for(int idx=0; idx<6; idx++)
    {
        if(colors[idx]<0) colors[idx] = 0;
        if(colors[idx]>255) colors[idx] = 255;
    }

    long col1 = ARGB(0xFF, ((char)colors[0]), ((char)colors[1]), ((char)colors[2]));
    long col2 = ARGB(0xFF, ((char)colors[3]), ((char)colors[4]), ((char)colors[5]));

    sky->SetColor(col1, 0);
    sky->SetColor(col1, 1);
    sky->SetColor(col2, 2);
    sky->SetColor(col2, 3);
}

bool CORE::collect_garbage()
{
    // rockets
    rocket *tmp, *curr = this->rockets->first;
    for(int idx=0; idx<this->rockets->length; idx++)
    {
        if(curr->dead_time == 1)
        {
			this->fired++;
			CloseHandle(curr->thread);
            this->rockets->del(curr);
            tmp = (rocket *)curr->next;
            delete curr->body;
            delete curr->trail;
            delete curr;
            curr = tmp;
        }
    }

    // bombs
    bomb *tmpb, *currb = this->bombs->first;
    for(idx=0; idx<this->bombs->length; idx++)
    {
        if(currb->dead_time == 1)
        {
			CloseHandle(currb->thread);

            if(currb->fatal)
			{
				alive = false;
				return true;
			}

			this->hit++;
            this->bombs->del(currb);
            tmpb = (bomb *)currb->next;
            delete currb->body;
            delete currb->trail;
            delete currb->explosion;
            delete currb;
            currb = tmpb;
        }
    }

    // nukes
    nuke *tmpn, *currn = this->nukes->first;
    for(idx=0; idx<this->nukes->length; idx++)
    {
        if(currn->dead_time == 1)
        {
			CloseHandle(currn->thread);

            if(currn->fatal)
			{
				alive = false;
				return true;
			}

			this->hit++;
            this->nukes->del(currn);
            tmpn = (nuke *)currn->next;
            delete currn->body;
            delete currn->trail;
            delete currn->explosion;
            delete currn;
            currn = tmpn;
        }
    }

    return false;
}


void CORE::debug_render()
{
	fnt->printf(5, 20, HGETEXT_LEFT, "Threads working: %i", bombs->length + rockets->length + nukes->length);
	int y=32;
	dynamic_spr *curr = rockets->first;
	for(int idx=0; idx<rockets->length; idx++)
	{
		fnt->printf(5, y, HGETEXT_LEFT, "[addr %p, handle %i] rocket", curr, curr->thread);
		curr = curr->next;
		y+=12;
	}

	curr = bombs->first;
	for(idx=0; idx<bombs->length; idx++)
	{
		fnt->printf(5, y, HGETEXT_LEFT, "[addr %p, handle %i] bomb", curr, curr->thread);
		curr = curr->next;
		y+=12;
	}

	curr = nukes->first;
	for(idx=0; idx<nukes->length; idx++)
	{
		fnt->printf(5, y, HGETEXT_LEFT, "[addr %p, handle %i] nuke", curr, curr->thread);
		curr = curr->next;
		y+=12;
	}
}