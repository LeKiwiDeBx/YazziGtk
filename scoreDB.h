/* 
 * score.h
 * 
 * Copyright 2021 LeKiwiDeBx [°}
 *
 */

#ifndef _SCOREDB_H_
#define _SCOREDB_H_

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#define YAZZI_NB_SCORE_DB 10
#define YAZZI_FILE_DB "yazziScore.db"

FILE *pScoreDB ;
typedef struct s_ScoreDB
{
    /* data */
    gint id;
    gchar *name; 
    gint value; /* somme des scores gagnant */
    gint victory_number; /* nombre de victoire */
    gint value_avg; /* moyenne des points de la victoire */
    gint party_number; /* nombre de partie */

} ScoreDB, *ptr_ScoreDB; 

void scoreDB_init();
void scoreDB_add();
void scoreDB_remove();
void scoreDB_read();
void scoreDB_sort();
void scoreDB_write(); 
#endif // !_SCOREDB_H



