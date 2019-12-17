/*
 * Créé le 26-02-2016 12:02:05 CET
*/
/*
 * player.c
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "player.h"
#include "board_engine.h"
#include <glib/gprintf.h>
#include <gtk/gtk.h>

Player Player_1 =
	{0,
	 NULL,
	 NULL,
	 NULL,
	 NULL,
	 FALSE,
	 NEW_PLAYER};

Player Player_2 =
	{0,
	 NULL,
	 NULL,
	 NULL,
	 NULL,
	 FALSE,
	 NEW_PLAYER};

Player *Players;
extern ptr_Player p;

/**
 * @brief retourne l'id du joueur
 * 
 * @param player lejoueur
 * @return int id unique
 */
static int
_player_get_new_id(Player *player)
{
	static int playerId = 0;
	playerId++;
	return (playerId);
}

/**
 * @brief Retourne le un pointeur sur le joueur
 * 
 * @return Player* 
 */
Player *
get_player_all()
{
	if (Players == NULL)
	{
		free(Players);
		exit(EXIT_FAILURE);
	}
	return Players;
}

/**
 * @brief initialise les joueurs
 * 
 */
void player_init()
{
	gint i = 0;
	Player *p = NULL;
	Players = (Player *)malloc(NB_PLAYER * sizeof(Player));
	if (Players == NULL)
	{
		free(Players);
		exit(EXIT_FAILURE);
	}
	p = (Player *)malloc(NB_PLAYER * sizeof(Player));
	if (p == NULL)
	{
		free(p);
		exit(EXIT_FAILURE);
	}
	gchar *name[] = {"Byron", "Cendrillon"};

	Players = &Player_1;
	Players->id = _player_get_new_id(Players);
	player_set_name(Players, name[i]);
	player_set_can_play(Players, FALSE);
	player_set_status(Players, NEW_PLAYER);
	sheet_score_new(Players); /* creation de la feuille de score */
	sheet_score_init(Players);
	dice_set_new(Players); /* creation du jeu de dé */

	i++;
	Players++;

	Players = &Player_2;
	Players->id = _player_get_new_id(Players);
	player_set_name(Players, name[i]);
	player_set_can_play(Players, FALSE);
	player_set_status(Players, NEW_PLAYER);
	sheet_score_new(Players); /* creation de la feuille de score */
	sheet_score_init(Players);
	dice_set_new(Players); /* creation du jeu de dé */
	srand(time(NULL));
	int x, max = 2, min = 0;
	x = rand() % (max - min) + min;
	if (x)
	{
		player_set_can_play(&Player_1, TRUE);
		player_set_status(&Player_1, BEGIN_PLAYER);
		player_set_status(&Player_2, NEW_PLAYER);
	}
	else
	{
		player_set_can_play(&Player_2, TRUE);
		player_set_status(&Player_2, BEGIN_PLAYER);
		player_set_status(&Player_1, NEW_PLAYER);
	}
	free(p);
}

/**
 * @brief reinitialise les joueurs
 * 
 */
void player_all_set_reset()
{
	Players = &Player_1;
	Players->set->count = 0;
	if (Players->status == BEGIN_PLAYER)
	{
		player_set_can_play(Players, FALSE);
		player_set_status(Players, NEW_PLAYER);
	}
	else
	{
		player_set_status(Players, BEGIN_PLAYER);
		player_set_can_play(Players, TRUE);
	}
	Players++;
	Players = &Player_2;
	Players->set->count = 0;
	if (Players->status == BEGIN_PLAYER)
	{
		player_set_can_play(Players, FALSE);
		player_set_status(Players, NEW_PLAYER);
	}
	else
	{
		player_set_status(Players, BEGIN_PLAYER);
		player_set_can_play(Players, TRUE);
	}
}

/**
 * @brief donne un nom au joueur
 * 
 * @param player 
 * @param name 
 */
void player_set_name(Player *player, gchar *name)
{
	if (name == NULL || strlen(name) == 0)
		player->name = _("Anonymous");
	else
		player->name = name;
}

/**
 * @brief donne la main au joueur
 * 
 * @param player 
 * @param canPlay 
 */
void player_set_can_play(Player *player, int canPlay)
{
	player->canPlay = canPlay;
}

/**
 * @brief retourne le joueur qui peut jouer
 * 
 * @return Player* 
 */
Player
	*
	player_get_can_play()
{
	if (Player_1.canPlay)
	{
		return &Player_1;
	}
	else
	{
		return &Player_2;
	}
}

/**
 * @brief indique le statut du joueur (gagnant, perdant)
 * 
 * @param player 
 * @param status 
 */
void player_set_status(Player *player, statePlayer status)
{
	player->status = status;
}

/**
 * @brief echange le joueur en cours
 * 
 * @param playerCurrent 
 * @param p // NOT USE
 * @return Player* le pointeur sur le joueur a jouer
 */
Player
	*
	player_swap(Player *playerCurrent, Player *p)
{
	if (playerCurrent == &Player_1)
	{
		player_set_can_play(playerCurrent, FALSE);
		playerCurrent = &Player_2;
	}
	else
	{
		playerCurrent = &Player_1;
	}
	player_set_can_play(playerCurrent, TRUE);
	playerCurrent->set->count = 0;
	return playerCurrent;
}

/**
 * @brief Retourne le joueur gagnant perdant et ex aequo
 * 
 * @return Player* 
 */
Player *
player_which_winner()
{
	Player *winner = (Player *)g_try_malloc(sizeof(Player));
	if (winner == NULL)
	{
		fprintf(stderr, "\nAllocation impossible player_which_winner() !\n");
		exit(EXIT_FAILURE);
	};
	Players = &Player_1;
	int p0 = Player_1.score->scoreGrandTotal;
	Players = &Player_2;
	int p1 = Player_2.score->scoreGrandTotal;

	if (p0 == p1)
	{
		Player_1.status = Player_2.status = EQUAL_PLAYER;
		return Players;
	}
	else if (p0 > p1)
	{
		Player_1.status = WINNER_PLAYER;
		Player_2.status = LOOSER_PLAYER;
		return &Player_1;
	}
	else
	{
		Player_1.status = LOOSER_PLAYER;
		Player_2.status = WINNER_PLAYER;
		return &Player_2;
	}
}
