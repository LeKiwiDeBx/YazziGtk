/*
 * scoreDB.c
 * 
 * Copyright 2021 LeKiwiDeBx [°}
 *
 */

#include "scoreDB.h"

static int scoreDB_init();
static int scoreDB_add(ptr_ScoreDB, int);

/*
*
*
*
*/
int 
scoreDB_init(){
    pScoreDB = fopen(YAZZI_FILE_DB, "rb");
    if(!pScoreDB) {
        printf("le fichier %s n'existe pas!", YAZZI_FILE_DB) ;
        return 0;

    }else{
        pScoreDB = fopen(YAZZI_FILE_DB, "rb+");
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
    fprintf(pScoreDB,"",);

}