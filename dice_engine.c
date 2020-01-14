/*
 * Créé le 25-02-2016 17:13:44 CET
 *
 * dice_engine.c 
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dice_engine.h"
#include "player.h"
#include <glib/gprintf.h>
#include <gtk/gtk.h>

#define ERR_VALUE_FACE -1
#define ERR_ENABLE_FACE -1
extern GtkWidget *pDice[5];
extern GtkWidget *eventBoxImageDice[5];
extern GtkImage *image;

static ptr_dice tab_dice;		//tableau objet de type structure
static ptr_diceSet tab_diceSet; //tableau objet de type structure
static GRand *g_rand = NULL;

int _dice_rand(void);
void dice_create(dice *newDice, diceName name, int id);
void dice_set_create(Player *self, diceSet *newDiceSet, dice *newDice, int newId);
int get_value_face(Player *self, diceName name);
void set_value_face(Player *self, int value);
int is_enables(Player *self, diceName name);

/**
 * @brief tirage d'un nombre aleatoire
 * 
 * @return int 
 */
int _dice_rand(void)
{
	// int x, max = 7, min = 1;
	// x = rand() % (max - min) + min;
	int x = g_rand_int_range(g_rand, 1, 7);
	return (x);
}

/**
 * @brief trie ASC/DESC le tirage de des pour un joueur
 * 
 * @param self 
 * @param tab_dice_sort 
 * @param orderBy 
 */
void sort_dice_value_order_by(Player *self, int *tab_dice_sort, diceOrderBy orderBy)
{
	int i, j, temp = 0;
	dice *p = (dice *)malloc(sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	p = self->set->dices;
	for (i = 0; i < DICE_NUMBER; i++)
	{
		tab_dice_sort[i] = p->value;
		p++;
	}
	for (j = 0; j < DICE_NUMBER; j++)
	{ /* orderBy == ASC */
		temp = 0;
		for (i = 0; i < DICE_NUMBER - 1; i++)
		{
			if (tab_dice_sort[i] > tab_dice_sort[i + 1])
			{
				temp = tab_dice_sort[i];
				tab_dice_sort[i] = tab_dice_sort[i + 1];
				tab_dice_sort[i + 1] = temp;
			}
		}
	}
	if (orderBy == DESC)
	{
		for (i = 0; i <= (DICE_NUMBER / 2); i++)
		{
			temp = tab_dice_sort[i];
			tab_dice_sort[i] = tab_dice_sort[DICE_NUMBER - 1 - i];
			tab_dice_sort[DICE_NUMBER - 1 - i] = temp;
		}
	}
}

/**
 * @brief creation d'un dé
 * 
 * @param newDice 
 * @param name 
 * @param id 
 */
void dice_create(dice *newDice, diceName name, int id)
{
	newDice->id = id;
	newDice->name = name;
	newDice->value = 0;
	newDice->enable = TRUE;
}

/**
 * @brief cre un nouveau jeu de de pour un joueur
 * 
 * @param self 
 * @param newDiceSet 
 * @param newDice 
 * @param newId 
 */
void dice_set_create(Player *self, diceSet *newDiceSet, dice *newDice, int newId)
{
	newDiceSet->id = newId + 1;
	newDiceSet->name = newId;
	newDiceSet->player_id = self->id;
	newDiceSet->dices = newDice;
	newDiceSet->count = 0;
}

/**
 * @brief affecte les valeurs au nouveau set de dés
 * 
 * @param newPlayer 
 */
void dice_set_new(Player *newPlayer)
{
	static int diceSetId = 0;
	int i, j;
	diceName dicename;

	//srand(time(NULL));
	g_rand = g_rand_new();
	if (diceSetId == 0)
	{
		tab_dice = (ptr_dice)malloc(DICE_NUMBER * NB_DICE_SET * sizeof(dice));
		if (tab_dice == NULL)
		{
			exit(EXIT_FAILURE);
		}
		tab_diceSet = (ptr_diceSet)malloc(NB_DICE_SET * sizeof(diceSet));
		if (tab_diceSet == NULL)
		{
			exit(EXIT_FAILURE);
		}
		for (i = 0; i < DICE_NUMBER * NB_DICE_SET; i++)
		{
			j = i % 5;
			dicename = j;
			dice_create(&tab_dice[i], dicename, i + 1);
		}
	}
	dice_set_create(newPlayer, &tab_diceSet[diceSetId], &tab_dice[diceSetId * DICE_NUMBER], diceSetId);
	newPlayer->set = &tab_diceSet[diceSetId];
	diceSetId++;
}

/**
 * @brief libere les des
 * 
 */
void dice_set_destroy()
{
	free(tab_dice);
	free(tab_diceSet);
}

/**
 * @brief lance tous les des
 * 
 * @param self 
 */
void roll_all(Player *self)
{
	int i;
	dice *p = (dice *)malloc(sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	p = self->set->dices;
	//srand(time(NULL));
	for (i = 0; i < DICE_NUMBER; i++)
	{
		set_value_face(self, _dice_rand());
		image = GTK_IMAGE(pDice[i]);
		gtk_image_set_from_file(GTK_IMAGE(image),
								g_strdup_printf("image/dice%d-64x64.png", self->set->dices->value));
		self->set->dices++;
	}
	self->set->dices = p;
}

/**
 * @brief lance un dé selectionné
 * 
 * @param self 
 * @param name 
 */
void roll_a_dice(Player *self, diceName name)
{
	diceName de;
	dice *p = (dice *)malloc(sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	p = self->set->dices;
	//srand(time(NULL));
	for (de = DICE_1; de <= DICE_5; de++)
	{
		if (de == name)
			set_value_face(self, _dice_rand());
		self->set->dices++;
	}
	self->set->dices = p;
}

/**
 * @brief  modifie les dés choisis
 * 
 * @param self 
 * @param slice 
 */
void roll_a_dice_slice(Player *self, int *slice)
{
	diceName de;
	int i;
	for (i = 0; i < DICE_NUMBER; i++)
	{
		for (de = DICE_1; de <= DICE_5; de++)
		{
			if (de == slice[i] - 1)
				roll_a_dice(self, de);
		}
	}
}

/**
 * @brief est lequel
 * 
 */
void is_which()
{
	//empty
}

/**
 * @brief si le de est actif
 * 
 * @param self 
 * @param name 
 * @return int le numero du des (pas sa valeur)
 */
int is_enables(Player *self, diceName name)
{
	diceName de;
	dice *p = (dice *)malloc(sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	p = self->set->dices;
	for (de = DICE_1; de <= DICE_5; de++)
	{
		if (de == name)
			return p->enable;
		p++;
	}
	return ERR_ENABLE_FACE;
}

/**
 * @brief compte le nombre de des a la valeur
 * 
 * @param self 
 * @param value 
 * @return int le nbre de des selon le critere de valeur
 */
int count_value_dice(Player *self, int value)
{
	int count = 0;
	diceName de;
	dice *p = (dice *)malloc(sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	p = self->set->dices;
	if (value >= 0 && value < 7)
	{
		for (de = DICE_1; de <= DICE_5; de++)
		{
			if (p->value == value)
			{
				count++;
			}
			p++;
		}
		return count;
	}
	else
		return ERR_VALUE_FACE;
}

/**
 * @brief retourne la valeur du dé
 * 
 * @param self 
 * @param name 
 * @return int valeur du dé
 */
int get_value_face(Player *self, diceName name)
{
	diceName de;
	dice *p = (dice *)malloc(sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	p = self->set->dices;
	for (de = DICE_1; de <= DICE_5; de++)
	{
		if (de == name)
			return p->value;
		p++;
	}
	return ERR_VALUE_FACE;
}

/**
 * @brief Retourne un pointeur sur le jeu de dés
 * 
 * @param self 
 * @return dice* 
 */
dice
	*
	get_all_dices(Player *self)
{
	if (self->set->dices == NULL)
	{
		exit(EXIT_FAILURE);
	}
	return self->set->dices;
}

/**
 * @brief donne une valeur a la face du dé
 * 
 * @param self 
 * @param value 
 */
void set_value_face(Player *self, int value)
{
	if (value >= 0 && value <= 6)
		self->set->dices->value = value;
	else
		self->set->dices->value = ERR_VALUE_FACE;
}

/**
 * @brief Remise à zero du jeu de dés
 * 
 * @param self le joueur en cours
 */
void set_raz_value_face(Player *self)
{
	dice *p = (dice *)malloc(sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	p = self->set->dices;
	for (int i = 0; i < DICE_NUMBER; i++)
	{
		set_value_face(self, 0);
		self->set->dices++;
	}
	self->set->dices = p;
}