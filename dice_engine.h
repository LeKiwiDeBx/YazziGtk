/*
 * dice_engine.h
 *
 * Copyright 2016 Le KiWi <jean@antix1> 
 *
 */
#ifndef _DICE_ENGINE_H_
#define _DICE_ENGINE_H_

#include <stdio.h>
#include <gtk/gtk.h>  
 
#define DICE_NUMBER 5
#define DICE_SET_MAX_COUNT 3
#define NB_DICE_SET 2
#define DICE_ALL -1  //rejouer tous les des

typedef enum e_diceOrderBy{
	ASC, 
	DESC
	} diceOrderBy; 

typedef enum e_diceName {
	DICE_1,
	DICE_2,
	DICE_3,
	DICE_4,
	DICE_5
	} diceName;

typedef struct s_dice{
	int id;
	diceName	name;		/* identifiant	*/
	int  		value;
	int     	enable; 	/* jouable - non jouable */
	} dice, *ptr_dice;

typedef enum e_nameDiceSet{
	DICE_SET_1,
	DICE_SET_2
	} nameDiceSet;

typedef struct s_diceSet{
	int id;
	nameDiceSet name; 
	int player_id;
	dice *dices;
	int count; 		/* nombre de fois jouÃ© <= DICE_SET_MAX_COUNT*/
	} diceSet,*ptr_diceSet ;

void dice_set_new() ;
void dice_set_destroy() ;
int count_value_dice() ;
int get_value_face() ;
void sort_dice_value_order_by() ;
void roll_all() ;
void roll_a_dice();
void roll_a_dice_slice() ;
void set_raz_value_face();
#endif
