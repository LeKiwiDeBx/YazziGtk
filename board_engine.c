/*
 * Créé le 18-03-2016 15:33:41 CET
 *
 * board_engine.c 
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 *
 */

#include "board_engine.h"
#include "dice_engine.h"
#include "player.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#define MAX_CAR 13

extern Player *Players;
ptr_Player p;

/** 
 * @brief
 *
 *
 */
void board_init()
{
	int x, max = 2, min = 0;
	p = NULL;
	p = (Player *)malloc(NB_PLAYER * sizeof(Player));
	if (p == NULL)
	{
		exit(EXIT_FAILURE);
	}
	p = get_player_all();
	//tirage au sort 0 ou 1 du joueur commençant le jeu
	srand(time(NULL));
	x = rand() % (max - min) + min; // rand()%2
	player_set_can_play(&Players[x], TRUE);
}
