/*
 * scoreDB.c
 * 
 * Copyright 2021 LeKiwiDeBx [°}
 *
 */

#include "scoreDB.h"

static int scoreDB_init();
static int scoreDB_add(ptr_ScoreDB, int);
static gint _scoreDB_gcompare(gconstpointer p1, gconstpointer p2, gpointer data);

gconstpointer ptr_ScoreDB p1, p2 ;


gint _scoreDB_gcompare(gconstpointer item1, gconstpointer item2, gpointer data){
    (item1->value == item2->value)? return 0:;
    return (item1->value > item2->value)? 1:-1;
}
/*
*
*
*
*/
int 
scoreDB_init(){
    pScoreDB = fopen(YAZZI_FILE_DB, "rb");
    gpointer data;
    if(!pScoreDB) {
        printf("le fichier %s n'existe pas!", YAZZI_FILE_DB) ;
        return 0;

    }else{
        pScoreDB = fopen(YAZZI_FILE_DB, "rb+");
        while(fread()){
            //lit la liste des struct et met dans l'ordre
            listScore = g_slist_insert_sorted_with_data (listScore,data,(GCompareDataFunc)_scoreDB_gcompare( p1,  p2,NULL), NULL);
        }
        
        return 1;
    }

}

/*
*
*
*
*/
int 
scoreDB_add(ptr_ScoreDB pScore, int pos){
    pScoreDB = fopen(YAZZI_FILE_DB, "rb+");
    // sauve apres la position
    // fseek
    // completer avec le reste jusqu'a 10 record max
    fseek(pScoreDB, sizeof(ScoreDB)*pos, SEEK_SET);
    fwrite(pScore,sizeof(ScoreDB),1,pScoreDB);

}