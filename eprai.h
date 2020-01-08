/*
 * eprai.h
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */
#ifndef _EPRAI_H_
#define _EPRAI_H_

#include <stdio.h>
#include <gtk/gtk.h>
#include "dice_engine.h"

//public
// void epr_mediator_engine() ;
//diceSet *_epr_get_set_dices();
typedef enum e_eprTab
{
	TAB_SORT_ASC,
	TAB_SORT_DESC,
	TAB_RAW
} eprTab;

typedef int tabDice[DICE_NUMBER];
typedef char tabDelta[DICE_NUMBER-1];
tabDice * _epr_factory_new();
#endif
