/*
 * player.h 
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdio.h>
#include <gtk/gtk.h>
#include "dice_engine.h"
#include "result_sheet.h"

#define NB_PLAYER 2

typedef enum e_state_Player
{
	WINNER_PLAYER, /* 0 */
	LOOSER_PLAYER, /* 1 */
	EQUAL_PLAYER,  /* 2 */
	NEW_PLAYER,	/* 3  nouveau joueur ou qui a jouer second au premier match */
	BEGIN_PLAYER   /* 4  le joueur qui a commencer la partie*/
} statePlayer;

typedef struct s_Player
{
	int id;
	gchar *name;				 /* un joueur */
	sheetScoreArray *scoreArray; /* Forme tab:: a une feuille de marque pour un jeu */
	sheetScore *score;			 /* Forme simple:: a une feuille de marque pour un jeu */
	diceSet *set;				 /* a un jeu de dés */
	gint canPlay;				 /* a son tour/ou pas de jouer */
	statePlayer status;			 /* gagnant,perdant... */
} Player, *ptr_Player;

void player_init();
Player *get_player_all();
void player_set_name();
void player_set_can_play(Player *, int);
void player_set_status(Player *, statePlayer);
Player *player_get_can_play();
Player *player_swap(Player *, Player *);
Player *player_which_winner();
void player_all_set_reset();
#endif
