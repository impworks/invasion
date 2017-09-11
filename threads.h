DWORD WINAPI UpdateRocket(LPVOID rpointer)
{
	rocket *curr = (rocket *) rpointer;
	while(curr->dead_time < 0.5 && core->alive)
	{
		if(!core->pause)
		{
			if(curr->alive)
			{
				curr->x += sin(curr->angle)*curr->speed;
				curr->y -= cos(curr->angle)*curr->speed;
				curr->speed += 0.05;
				curr->trail->MoveTo(curr->x-sin(curr->angle)*12, curr->y+cos(curr->angle)*12);
			}

			if((curr->x < -30 || curr->x > 830 || curr->y < -30) && curr->alive)
			{
				curr->alive = false;
				curr->trail->Stop();
			}

			if(!curr->alive)
			{
				curr->dead_time += hge->Timer_GetDelta();
			}

			curr->trail->Update(hge->Timer_GetDelta());
		}

		Sleep(9);
	}
	curr->dead_time = 1;
	return 0;
}

DWORD WINAPI UpdateBomb(LPVOID bpointer)
{
	bomb *curr = (bomb *)bpointer;
	while(curr->dead_time < 0.9  && core->alive)
	{
		if(!core->pause)
		{
			if(curr->alive)
			{
				curr->x += sin(curr->angle)*curr->speed;
				curr->y -= cos(curr->angle)*curr->speed;
				if(curr->curved)
				{
					curr->angle += sin(curr->angle_offset)/100*curr->curve_dir;
					curr->angle_offset += 0.02;
					if(curr->angle_offset > 6.28) curr->angle_offset = 0;
				}

				curr->trail->MoveTo(curr->x-sin(curr->angle)*12, curr->y+cos(curr->angle)*12);
			}

			if(curr->y > 440)
			{
				core->colors[0]++;
				core->colors[1]--;
				core->colors[2]--;
				core->colors[3]++;
				core->colors[4]--;
				core->colors[5]--;
			}
        
			// explode reaching bombs
			if(curr->y >= 500 && curr->alive)
			{
				curr->explode();
				curr->fatal = true;
			}

			if(!curr->alive)
			{
				curr->dead_time += hge->Timer_GetDelta();
			}

			curr->trail->Update(hge->Timer_GetDelta());
			curr->explosion->Update(hge->Timer_GetDelta());
		}

		Sleep(9);
	}
	curr->dead_time = 1;
	return 0;
}