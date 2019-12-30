/*
 * CrÃ©Ã© le 25-02-2016 19:25:05 CET
 *
 * result_sheet.c
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "result_sheet.h"
#include "player.h"
#include "dice_engine.h"

#include <glib/gprintf.h>
#include <gtk/gtk.h>

#define TAB_SIZE(x) (sizeof(x) / sizeof(x[0])) /*macro calcul taille tableau*/
#define MIN_BONUS 63

extern Player *Players, Player_1, Player_2;

static sheetScore sheetScore_1, sheetScore_2;
static sheetScoreArray sheetScoreArray_1, sheetScoreArray_2;
static sheetScoreCell Cell_1[YAZZI_NB_CELL], Cell_2[YAZZI_NB_CELL];
int (*row_sheet_score[])(Player *) = {calcScoreSameSide,
									  calcScoreSmallStraight,
									  calcScoreLargeStraight,
									  calcScoreFullHouse,
									  calcScoreThreeOfKind,
									  calcScoreFourOfKind,
									  calcScoreYazzi,
									  calcScoreChance,
									  calcScoreYazziBonus};

char *cellName[] = {
	"empty",
	"scoreAces",		  //1
	"scoreTwos",		  //2
	"scoreThrees",		  //3
	"scoreFours",		  //4
	"scoreFives",		  //5
	"scoreSixes",		  //6
	"scoreSmallStraight", //7
	"scoreLargeStraight", //8
	"scoreFullHouse",	 //9
	"scoreThreeOfKind",   //10
	"scoreFourOfKind",	//11
	"scoreYazzi",		  //12
	"scoreChance",		  //13
	"scoreSubSum",		  //14
	"scoreBonus",		  //15
	"scoreUpperSection",  //16
	"scoreYazziBonus",	//17
	"scoreLowerSection",  //18
	"scoreGrandTotal"	 //19
};

typedef enum cellScoreName
{
	Aces,		   //indice 0 -----debut des choix 'enable'
	Twos,		   //1
	Threes,		   //2
	Fours,		   //3
	Fives,		   //4
	Sixes,		   //5
	SmallStraight, //6
	LargeStraight, //7
	FullHouse,	 //8
	ThreeOfKind,   //9
	FourOfKind,	//10
	Yazzi,		   //11
	Chance,		   //12------------fin des choix possibles 'enable'
	SubSum,		   //13
	Bonus,		   //14
	UpperSection,  //15
	YazziBonus,	//16---------------choix possible apres Yazzi validé
	LowerSection,  //17
	GrandTotal	 //18
} cellScoreName;

static int _adaptator_sheet_score();
static void _set_sheet_scoreArray_cell_enable(Player *);
static void _sheet_score_update_upper_section(Player *);
static void _sheet_score_update_lower_section(Player *);
static void _sheet_score_update_grand_total(Player *);
int _sheet_score_already(Player *, int);

int display_result_sheet(Player *self, int numMark)
{
	/**
	 * @brief empty
	 * 
	 */
	return -1;
}

void _fetch_count(Player *self, int *tab_count_value)
{
	int ind, val_max = 6, val_min = 1;
	for (int value = val_min; value <= val_max; value++)
	{
		ind = value - 1;
		tab_count_value[ind] = count_value_dice(self, value);
	}
}

void sheet_score_new(Player *newPlayer)
{
	static int sheetScoreId = 0;
	if (sheetScoreId == 0)
	{
		ptr_sheetScore[YAZZI_NB_SHEET] = malloc(YAZZI_NB_SHEET * sizeof(sheetScore *));
		if (ptr_sheetScore[YAZZI_NB_SHEET] == NULL)
		{
			exit(EXIT_FAILURE);
		};
		sheetScore_1.id = sheetScoreId + 1;
		sheetScore_1.player_id = newPlayer->id;
		ptr_sheetScore[sheetScoreId] = &sheetScore_1;
	}
	else if (sheetScoreId == 1)
	{
		sheetScore_2.id = sheetScoreId + 1;
		sheetScore_2.player_id = newPlayer->id;
		ptr_sheetScore[sheetScoreId] = &sheetScore_2;
	}
	newPlayer->score = ptr_sheetScore[sheetScoreId];
	sheetScoreId++;
}

void sheet_score_init(Player *self)
{
	static int sheetScoreId = 0;
	if (sheetScoreId == 0)
	{
		ptr_sheetScoreArray[YAZZI_NB_SHEET] = malloc(YAZZI_NB_SHEET * sizeof(sheetScoreArray *));
		if (ptr_sheetScoreArray[YAZZI_NB_SHEET] == NULL)
		{
			exit(EXIT_FAILURE);
		};
		sheetScoreArray_1.id = sheetScoreId + 1;
		sheetScoreArray_1.player_id = self->id;
		sheetScoreArray_1.full = 0;
		sheetScoreArray_1.ptr_cell = calloc(YAZZI_NB_CELL, sizeof(sheetScoreCell));
		if (sheetScoreArray_1.ptr_cell == NULL)
		{
			exit(EXIT_FAILURE);
		}
		sheetScoreArray_1.ptr_cell = &Cell_1[0];
		ptr_sheetScoreArray[sheetScoreId] = &sheetScoreArray_1;
	}
	else if (sheetScoreId == 1)
	{
		sheetScoreArray_2.id = sheetScoreId + 1;
		sheetScoreArray_2.player_id = self->id;
		sheetScoreArray_2.full = 0;
		sheetScoreArray_2.ptr_cell = calloc(YAZZI_NB_CELL, sizeof(sheetScoreCell));
		if (sheetScoreArray_2.ptr_cell == NULL)
		{
			exit(EXIT_FAILURE);
		}
		sheetScoreArray_2.ptr_cell = &Cell_2[0];
		ptr_sheetScoreArray[sheetScoreId] = &sheetScoreArray_2;
	}
	self->scoreArray = ptr_sheetScoreArray[sheetScoreId];
	_set_sheet_scoreArray_cell_enable(self);
	sheetScoreId++;
}

sheetScore
	*
	sheet_score_get(Player *self)
{
	if (self->score != NULL)
		return self->score;
	else
		exit(EXIT_FAILURE);
}

void sheet_score_destroy()
{
	free(sheetScoreArray_2.ptr_cell);
	free(sheetScoreArray_1.ptr_cell);
	free(ptr_sheetScoreArray[YAZZI_NB_SHEET]);
	free(ptr_sheetScore[YAZZI_NB_SHEET]);
}

void sheet_score_all_reset()
{
	Player *p = NULL;
	int numMark = 1;
	cellScoreName fig;
	p = (Player *)malloc(NB_PLAYER * sizeof(Player));
	if (p == NULL)
	{
		free(p);
		exit(EXIT_FAILURE);
	}
	p = Players = &Player_1;
	for (fig = Aces; fig <= GrandTotal; fig++)
	{
		p->scoreArray->ptr_cell[fig].id = numMark;
		p->scoreArray->ptr_cell[fig].name = cellName[numMark];
		p->scoreArray->ptr_cell[fig].value = 0;
		p->scoreArray->ptr_cell[fig].enable = TRUE;
		numMark++;
	}
	numMark = 1;
	sheet_score_update_all(p);
	p = Players = &Player_2;
	for (fig = Aces; fig <= GrandTotal; fig++)
	{
		p->scoreArray->ptr_cell[fig].id = numMark;
		p->scoreArray->ptr_cell[fig].name = cellName[numMark];
		p->scoreArray->ptr_cell[fig].value = 0;
		p->scoreArray->ptr_cell[fig].enable = TRUE;
		numMark++;
	}
	numMark = 1;
	sheet_score_update_all(p);
	sheetScoreArray_1.ptr_cell = &Cell_1[0];
	sheetScoreArray_2.ptr_cell = &Cell_2[0];
	sheetScoreArray_1.full = 0;
	sheetScoreArray_2.full = 0;
	p = NULL;
	free(p);
}

int sheet_score_write(Player *self, int numMark)
{
	/* ---> DEBUG     <--- */
	g_printf("\nVotre choix est: [%d] \n", numMark);
	/* ---> FIN DEBUG <--- */
	int numFunct = numMark - 1, num_cell;
	if (numFunct < 6)
		numFunct = 0;
	else
		numFunct -= 5;
	if (numMark == 14)
	{ /* double ::YAZZI BONUS */
		num_cell = 16;
		numFunct = 8;
	}
	else
		num_cell = numMark - 1;
	if (self->scoreArray->ptr_cell[num_cell].enable == FALSE)
		return -1;
	// int value = _adaptator_sheet_score(self, row_sheet_score[numFunct], numMark);
	int value = _adaptator_sheet_score(self, numMark);
	value = (value != -1) ? value : 0;
	self->scoreArray->ptr_cell[num_cell].id = numMark;
	self->scoreArray->ptr_cell[num_cell].name = cellName[numMark];
	self->scoreArray->ptr_cell[num_cell].value = value;
	self->scoreArray->ptr_cell[num_cell].enable = FALSE;
	return (!numMark) ? numFunct : numMark; //ret -1 si numMark 0 == n'existe pas
}

int _sheet_score_already(Player *self, int numMark)
{
	int numFunct = numMark - 1, num_cell;
	if (numFunct < 6)
		numFunct = 0;
	else
		numFunct -= 5;
	if (numMark == 14)
	{
		num_cell = 16;
		numFunct = 8;
	}
	else
		num_cell = numMark - 1;
	if (self->scoreArray->ptr_cell[num_cell].enable == FALSE)
		return 0;
	else
		return (numMark);
}

void sheet_score_update_all(Player *self)
{
	self->score->scoreAces = self->scoreArray->ptr_cell[Aces].value;
	self->score->scoreTwos = self->scoreArray->ptr_cell[Twos].value;
	self->score->scoreThrees = self->scoreArray->ptr_cell[Threes].value;
	self->score->scoreFours = self->scoreArray->ptr_cell[Fours].value;
	self->score->scoreFives = self->scoreArray->ptr_cell[Fives].value;
	self->score->scoreSixes = self->scoreArray->ptr_cell[Sixes].value;
	_sheet_score_update_upper_section(self);
	self->score->scoreSubSum = self->scoreArray->ptr_cell[SubSum].value;
	self->score->scoreBonus = self->scoreArray->ptr_cell[Bonus].value;
	self->score->scoreUpperSection = self->scoreArray->ptr_cell[UpperSection].value;
	self->score->scoreSmallStraight = self->scoreArray->ptr_cell[SmallStraight].value;
	self->score->scoreLargeStraight = self->scoreArray->ptr_cell[LargeStraight].value;
	self->score->scoreFullHouse = self->scoreArray->ptr_cell[FullHouse].value;
	self->score->scoreThreeOfKind = self->scoreArray->ptr_cell[ThreeOfKind].value;
	self->score->scoreFourOfKind = self->scoreArray->ptr_cell[FourOfKind].value;
	self->score->scoreYazzi = self->scoreArray->ptr_cell[Yazzi].value;
	self->score->scoreChance = self->scoreArray->ptr_cell[Chance].value;
	self->score->scoreYazziBonus = self->scoreArray->ptr_cell[YazziBonus].value;
	_sheet_score_update_lower_section(self);
	self->score->scoreLowerSection = self->scoreArray->ptr_cell[LowerSection].value;
	_sheet_score_update_grand_total(self);
	self->score->scoreGrandTotal = self->scoreArray->ptr_cell[GrandTotal].value;
}

static void
_sheet_score_update_upper_section(Player *self)
{
	int fig;
	self->scoreArray->ptr_cell[SubSum].value = 0;
	self->scoreArray->ptr_cell[Bonus].value = 0;
	self->scoreArray->ptr_cell[UpperSection].value = 0;
	for (fig = Aces; fig <= Sixes; fig++)
	{
		self->scoreArray->ptr_cell[SubSum].value += self->scoreArray->ptr_cell[fig].value;
	}
	self->scoreArray->ptr_cell[UpperSection].value = self->scoreArray->ptr_cell[SubSum].value;
	if (self->scoreArray->ptr_cell[SubSum].value > YAZZI_MIN_BONUS)
	{
		self->scoreArray->ptr_cell[Bonus].value = BONUS;
		self->scoreArray->ptr_cell[UpperSection].value += BONUS;
	}
}

static void
_sheet_score_update_lower_section(Player *self)
{
	int fig;
	self->scoreArray->ptr_cell[LowerSection].value = 0;
	for (fig = SmallStraight; fig <= Chance; fig++)
	{
		self->scoreArray->ptr_cell[LowerSection].value += self->scoreArray->ptr_cell[fig].value;
	}
	self->scoreArray->ptr_cell[LowerSection].value += self->scoreArray->ptr_cell[YazziBonus].value;
}

static void
_sheet_score_update_grand_total(Player *self)
{
	self->scoreArray->ptr_cell[GrandTotal].value =
		self->scoreArray->ptr_cell[UpperSection].value + self->scoreArray->ptr_cell[LowerSection].value;
}

/**
 * @brief Adapteur de parametre fonction de calcul
 * @param funct
 * @param num
 * @returns valeur de la ligne de score
 *
 *
 */
static int
// _adaptator_sheet_score(Player *self, int (*funct)(Player *self), int num)
_adaptator_sheet_score(Player *self, int numMark)
{
	int value = 0;
	switch (numMark)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		return calcScoreSameSide(self, numMark);
	case 7:
		value = calcScoreSmallStraight(self);
		break;
	case 8:
		value = calcScoreLargeStraight(self);
		break;
	case 9:
		value = calcScoreFullHouse(self);
		break;
	case 10:
		value = calcScoreThreeOfKind(self);
		break;
	case 11:
		value = calcScoreFourOfKind(self);
		break;
	case 12:
		value = calcScoreYazzi(self);
		break;
	case 13:
		value = calcScoreChance(self);
		break;
	case 14:
		value = calcScoreYazziBonus(self);
		break;
	default:
		//return (*funct)(self);
		value = -1;
	}
	return value;
}

static void
_set_sheet_scoreArray_cell_enable(Player *self)
{
	cellScoreName fig;
	for (fig = Aces; fig <= GrandTotal; fig++)
	{
		self->scoreArray->ptr_cell[fig].enable = TRUE;
	}
}

int full_sheet_score(Player *self)
{
	cellScoreName fig;
	if (self->scoreArray->ptr_cell[YazziBonus].enable)
		return FALSE;
	for (fig = Aces; fig <= Chance; fig++)
	{
		if (self->scoreArray->ptr_cell[fig].enable)
			return FALSE;
	}
	return TRUE;
}

int calcScoreSameSide(Player *self, int value)
{
	int ret = count_value_dice(self, value) * value;
	return ret;
}

int calcScoreFullHouse(Player *self)
{
	int ret = -1;
	int val_max = 6;
	gboolean yenAdeux = FALSE, yenAtrois = FALSE;
	int tab_count_value[6] = {0, 0, 0, 0, 0, 0};

	_fetch_count(self, tab_count_value);
	for (int ind = 0; ind < val_max; ind++)
	{
		if (tab_count_value[ind] == 2)
			yenAdeux = TRUE;
		if (tab_count_value[ind] == 3)
			yenAtrois = TRUE;
	}
	if (yenAdeux && yenAtrois)
		return YAZZI_FULL_HOUSE;
	return ret;
}

int calcScoreThreeOfKind(Player *self)
{
	int ret = -1, ind = 0, i, val_max = 6;
	int tab_count_value[val_max];
	_fetch_count(self, tab_count_value);
	for (ind = 0; ind < val_max; ind++)
	{
		if (tab_count_value[ind] >= 3)
		{
			ret = 0;
			for (i = 0; i < val_max; i++)
			{
				ret += tab_count_value[i] * (i + 1);
			}
			return ret;
		}
	}
	return ret;
}

int calcScoreFourOfKind(Player *self)
{
	int ret = -1, ind = 0, i, val_max = 6;
	int tab_count_value[val_max];
	_fetch_count(self, tab_count_value);
	for (ind = 0; ind < val_max; ind++)
	{
		if (tab_count_value[ind] >= 4)
		{
			ret = 0;
			for (i = 0; i < val_max; i++)
			{
				ret += tab_count_value[i] * (i + 1);
			}
			return ret;
		}
	}
	return ret;
}

int calcScoreYazzi(Player *self)
{
	int ret = -1, ind = 0, val_max = 6;
	int tab_count_value[val_max];
	_fetch_count(self, tab_count_value);
	for (ind = 0; ind < val_max; ind++)
	{
		if (tab_count_value[ind] == 5)
			return YAZZI_VALUE;
	}
	return ret;
}

int calcScoreYazziBonus(Player *self)
{
	int ret = -1; /* si yazzi deja  valide et avec 50 points  sinon -1 */
	return (self->scoreArray->ptr_cell[Yazzi].enable == FALSE &&
			self->scoreArray->ptr_cell[Yazzi].value != 0)
			   ? calcScoreYazzi(self)
			   : ret;
}

int calcScoreLargeStraight(Player *self)
{
	int ret = -1, ind = 0, count = 0, k, j;
	int tab_dice_sort[DICE_NUMBER];
	sort_dice_value_order_by(self, tab_dice_sort, ASC);
	for (ind = 0; ind <= 1; ind++)
	{
		if (tab_dice_sort[ind] == 1 || tab_dice_sort[ind] == 2)
		{
			k = ind;
			for (j = 0; j < 4; j++)
			{
				if (tab_dice_sort[k] == (tab_dice_sort[k + 1] - 1))
					count++;
				else
					return ret;
				if (count == 4)
					return YAZZI_LARGE_STRAIGHT;
				k++;
			}
		}
	}
	return ret;
}
/**
 * @brief Evalue si les des forment une petite suite
 * 
 * @param self le joueur en cours
 * @return int la valeur YAZZI_SMALL_STRAIGHT
 */
int calcScoreSmallStraight(Player *self)
{
	int ret = -1, ind = 0, count = 0, k, j;
	int tab_dice_sort[DICE_NUMBER];
	sort_dice_value_order_by(self, tab_dice_sort, ASC);
	for (ind = 0; ind < 3; ind++)
	{ //traiter les doubles, petite suite commence indice 0 ou 1 ou 2
		if (tab_dice_sort[ind] == 1 ||
			tab_dice_sort[ind] == 2 ||
			tab_dice_sort[ind] == 3)
		{
			count = 0;
			k = ind;
			for (j = 0; j < 4; j++)
			{
				if (tab_dice_sort[k] == tab_dice_sort[k + 1] - 1)
				{
					count++;
					k++;
				}
				else
				{
					if (tab_dice_sort[k] == tab_dice_sort[k + 1])
					{ //les doubles
						k++;
						continue;
					}
					if (tab_dice_sort[k] != tab_dice_sort[k + 1] - 1 && k != 0)
						return ret; //trou au milieu des succ(3|5) sauf au premier ie: 1 2 3 5 6
				}
				if (count == 3)
					return YAZZI_SMALL_STRAIGHT;
			}
		}
	}
	return ret;
}
/**
 * @brief Calcul la somme de tous les des
 * 
 * @param self le joueur en cours
 * @return int valeur de la somme
 */
int calcScoreChance(Player *self)
{
	int i, ret = 0;
	dice *p = (dice *)g_try_malloc(sizeof(dice));
	if (p == NULL)
	{
		fprintf(stderr, "\nAllocation impossible calcScoreChance() !\n");
		exit(EXIT_FAILURE);
	}
	p = self->set->dices;
	for (i = 0; i < DICE_NUMBER; i++)
	{
		ret += p->value;
		p++;
	}
	return ret;
}
