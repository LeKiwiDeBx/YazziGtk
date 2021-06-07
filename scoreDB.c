/*
 * scoreDB.c
 * 
 * Copyright 2021 LeKiwiDeBx [°}
 *
 */

#include "scoreDB.h"

static void scoreDB_init();



void scoreDB_init(){
    pScoreDB = fopen(YAZZI_FILE_DB, "rb");
    if(!pScoreDB) {
        printf("le fichier %s n'existe pas!", YAZZI_FILE_DB) ;

    }else{
        pScoreDB = fopen(YAZZI_FILE_DB, "rb+");
    }

}