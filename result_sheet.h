/*
 * CrÃ©Ã© le 25-02-2016 19:38:29 CET
 *
 * result_sheet.h
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 */
#ifndef _RESULT_SHEET_H_
#define _RESULT_SHEET_H_

#include <stdio.h>
#include <gtk/gtk.h>
 
#define YAZZI_VALUE 50 
#define	BONUS 35
#define YAZZI_MIN_BONUS 63
#define YAZZI_SMALL_STRAIGHT 30
#define YAZZI_LARGE_STRAIGHT 40
#define YAZZI_FULL_HOUSE 25
#define YAZZI_MAX_ACES	5
#define YAZZI_MAX_TWOS	10
#define YAZZI_MAX_THREES 15
#define YAZZI_MAX_FOURS	20
#define YAZZI_MAX_FIVES	25
#define YAZZI_MAX_SIXES	30
#define YAZZI_NB_SHEET 2 /* nombre de feuilles pour une partie de 2 joueurs */
#define YAZZI_NB_CELL 19 /* cellule par feuille */

typedef struct s_sheet_Score{
	int id;
	int player_id;
	int scoreAces;
	int scoreTwos;
	int scoreThrees;
	int scoreFours;
	int scoreFives;
	int scoreSixes;
	int scoreSubSum;
	int scoreBonus;
	int scoreUpperSection;
	int scoreSmallStraight;
	int scoreLargeStraight;
	int scoreFullHouse;
	int scoreThreeOfKind;
	int scoreFourOfKind;
	int scoreYazzi;
	int scoreChance;
	int scoreYazziBonus;
	int scoreLowerSection;
	int scoreGrandTotal;
	} sheetScore ;

sheetScore *ptr_sheetScore[YAZZI_NB_SHEET] ; /* carnet de score */

typedef struct s_sheet_Score_Cell{
	int id;
	char *name;
	int value;
	int enable;
} sheetScoreCell ;

typedef struct s_sheet_Score_Array{
	int id;
	int player_id;
	int full;
	sheetScoreCell *ptr_cell;
} sheetScoreArray;

sheetScoreArray *ptr_sheetScoreArray[YAZZI_NB_SHEET] ;

sheetScore *sheet_score_get();
void sheet_score_new();
void sheet_score_init();
void sheet_score_destroy();
int sheet_score_write();
void sheet_score_update_all() ;
void sheet_score_all_reset();
int full_sheet_score() ;
int calcScoreSameSide();
int calcScoreChance();
int calcScoreFullHouse();
int calcScoreThreeOfKind();
int calcScoreFourOfKind();
int calcScoreYazzi();
int calcScoreYazziBonus();
int calcScoreLargeStraight();
int calcScoreSmallStraight();
int display_result_sheet() ;
#endif
