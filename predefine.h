class CORE;
class dynamic_spr;
class rocket;
class bomb;
class nuke;

DWORD WINAPI UpdateRocket(LPVOID rpointer);
DWORD WINAPI UpdateBomb(LPVOID bpointer);

bool FrameFunc();
bool RenderFunc();
bool FrameFuncLost();
bool RenderFuncLost();

// common active object class

class dynamic_spr
{
  public:
    hgeSprite *body;
    hgeParticleSystem *trail, *explosion;
    float x, y;
    float angle, speed;
    float dead_time;
    dynamic_spr *next, *prev;
    bool alive;

	HANDLE thread;

    dynamic_spr(float nx, float ny, float nangle)
    {
        body = NULL;
        trail = explosion = NULL;
        next = prev = NULL;
        x = nx;
		y = ny;
		angle = nangle;
		speed = 0;
		thread = 0;
        alive = false;
    }

    int collides(dynamic_spr *obj)
    {
        if(abs(this->x - obj->x) < 16 && abs(this->y - obj->y) < 16 && obj->alive) return true;
        return false;
    }
};




// rocket class

class rocket: public dynamic_spr
{
    public:

	rocket(float x, float y, float angle);

    void explode();
};




// bomb class

class bomb: public dynamic_spr
{
    public:
    bool fatal, curved;
    float angle_offset;
    int curve_dir;

	bomb(float x, float y, float angle);

    void explode();
};



// nuke class

class nuke: public dynamic_spr
{
    public:
    bool fatal, curved;

	nuke(float x, float y, float angle);

    void explode();
};




template <class Type>
class list
{
  public:
    Type *first, *last;
    int length, max;

    list(int nmax);
    ~list();

    void add(Type *obj);
    void del(Type *obj);
    Type * get(int id);
    void empty();

    void render();
};


// core class

class CORE
{
  public:
	HMUSIC music;

    HCHANNEL channel;

    HTEXTURE
		txGround, txSky,
		txTurret, txGun,
		txRocket, txRocketTrail,
		txBomb,   txBombTrail,
		txNuke,   txNukeTrail,
		txExplosion,
		txCrosshair,
		txSign;

    hgeSprite
		*ground, *sky,
		*turret, *gun,
	    *crosshair,
		*rocketTrailPiece, *bombTrailPiece, *explosionPiece, *nukeTrailPiece,
		*sign;

    hgeFont *fnt;

    list<rocket> *rockets;
    list<bomb> *bombs;
	list<nuke> *nukes;

	struct { float x, y; } stars[100];

    float turretAngle, mouseX, mouseY;
    float bombTimer, nukeTimer, bombInterval, bombSpeedup;

    float colors[6], starcolor, textalpha;

    long score, fired, hit;
	bool alive, pause, debug;
    
    CORE();
    ~CORE();

    int load_data();
    void init();
    void rotate_turret();
    void fire_rocket();
    void drop_bomb();
	void drop_nuke();
    void explode_bombs();
    void repaint_sky();

	void debug_render();

    bool collect_garbage();
};