/*
 * eprai.c
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 * Créé le 24-05-2016 15:52:17 CEST
 * EPRAI
 * Engine Purpose Result Artificial Intelligence
 * C PA DU PIPO ;)
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "eprai.h"
#include "dice_engine.h"
#include "player.h"
#include <glib/gi18n.h>
#include <glib/gprintf.h>
#include <glib/gregex.h>
#include <gtk/gtk.h>

#define LEVEL_LOW 2  //nombre de face identique basse (sensibilité)
#define LEVEL_HIGH 3 //nombre de face identique haute (sensibilité)

typedef enum e_eprOpenPattern
{
	OP_DIFFERENT, // les des sont tres differents
	OP_SIMILAR,   // les des ont des double ou triple
	OP_BIG_SUM,   // la somme des des et importante
	OP_MISC		  // divers aucun des cas
} eprOpenPattern;

typedef enum e_eprFamilyPattern
{							// quoi: Famille des patterns à chercher (strategie)
	FA_SAME_DICE_LOW,		// dés egaux (ie plusieurs 3) #1 #2 #3 poids bas
	FA_SAME_DICE_HIGH,		// dés egaux (ie plusieurs 3) #3 #4 #5 poids hauts
	FA_STRAIGHT_DICE,		// SS LS une suite (ie 2 3 4 or 4 5 6)
	FA_FULL_HOUSE,			// FH YAZ YB
	FA_FULL_HOUSE_WITH_MIN, // ToK FoK et sum > 12
	FA_SUM_DICE,			// Sum
	FA_DIRECT_PATTERN,		// Figure entiere identifiée
	FA_OTHERS				// Aucune des precedentes
} eprFamilyPattern;

tabDice dataBase;
tabDelta deltaDB;

static void _epr_set_dices_tab();
static void _epr_sort_set_dices();
diceSet *_epr_get_set_dices();
static void _epr_search_pattern();
static void _epr_delta_pattern();

//
// typedef struct s_eprAtom{
// int id;						// identifiant
// int priority;				// degre de priorité-pertinence
// int cycle;					// degre de cycle (persistence lors des iterations-crible de selection)
// int sum;					// somme des dés
// char *scoreName; 			// nom du motif visé (purpose)
// diceSet *bitSet;			// morceaux du set qui reste a rejoué dans le tirage
// eprFamilyPattern family;	// famille de type de figure [Poids forts/suite/3Xidentiques(FH)/FH > 12 => FH+ ToK FoK]
// } eprAtom;
//
// typedef struct s_eprPattern{
// diceSet *set;				// jeu de dé où chercher
// eprFamilyPattern family; 	// quoi chercher (un nombre, une suite de nombre)
// int     count;				// combien de fois chercher ( trois fois, une fois une expr)
// int 	match;				// 0 si pas trouvé sinon 1
// diceSet *bitSet;			// morceaux du set qui reste a rejoué dans le tirage
// int 	value;				// si un nombre cherché le nombre sinon 0
// } eprPattern;
//
// static eprAtom  *ptrEprAtom[20] ;			// nbre max atom 20
// static eprPattern *ptrEprPattern[20] ;		// nbre max pattern 20
//
// static int calcScoreSameSideLow(Player *self);
// int (*epr_purpose_score[])(Player *) = {calcScoreSameSideLow
// calcScoreSameSideHigh,
// calcScoreSmallStraight,
// calcScoreLargeStraight,
// calcScoreFullHouse,
// calcScoreThreeOfKind,
// calcScoreFourOfKind,
// calcScoreYazzi,
// calcScoreChance,
// calcScoreYazziBonus
// } ;

// static gboolean epr_get_pattern() ;
// static int epr_get_sum_dices() ;
// static diceSet *_epr_get_set_dices() ;
// static void _epr_sort_set_dices() ;
// static eprAtom *epr_factory_eprAtom() ;
// static int _epr_pattern_set_remain() ;
// void epr_mediator_engine() ;

/**
 * @brief
 * @param self
 * @param pattern
 *
 *
 */
// gboolean
// epr_pattern_get_match(Player *self, eprPattern *pattern){
// int level = 0;
// pattern->set = _epr_get_set_dices(self);
// switch (pattern->family){
// case SAME_DICE_LOW:
// pattern->count = 1 ; //debug on retournera au premier motif trouvé
// level = count_value_dice(Players, pattern->value) ;
// if(level >= LEVEL_LOW)
// pattern->match = level;
// else pattern->match = 0 ;
//
// break;
// case SAME_DICE_HIGH:
// pattern->count = 1 ; //debug on retournera au premier motif trouvé
// level = count_value_dice(Players, pattern->value) ;
// if(level >= LEVEL_LOW)
// pattern->match = level;
// else pattern->match = 0 ;
// /* *
// * */
// break;
// case STRAIGHT_DICE:
// /* *
// * */
// break ;
// case FULL_HOUSE:
// /* *
// * */
// break ;
// case FULL_HOUSE_WITH_MIN:
// /* *
// * */
// break ;
// case SUM_DICE:
// /* *
// * */
// break;
// case DIRECT_PATTERN:
// /* *
// * */
// break ;
// case OTHERS:
// pattern->match = 0 ;
// /* *
// * */
// break ;
// default:
// pattern->match = 0 ;
// /* *
// * */
// ;
// }
// return pattern->match ;
// }

/**
 * @brief
 * @param self
 * @param pattern
 * @returns
 *
 *
 */
// int
// _epr_pattern_set_remain(Player *self, eprPattern *pattern){
// /*
// * dans le cas SAME_DICE_LOW: SAME_DICE_HIGH:
// * 		positionne les des a jouer sauf la paire(LEVEL_LOW) ou trio(LEVEL_HIGH)
// * */
// /* ---> DEBUG     <--- */
// int i ;
// switch(pattern->family)
// {
// case SAME_DICE_LOW:
// epr_purpose_score[0](Players) ; // ici direct sinon implementer Strategy fonction(Purpose) qui trouve les des à relancer (remain)
// // a inscrire dans le bitSet du Pattern pour aller ensuite enrichir la generation d'eprAtom
// for(i=0 ; i<5; i++){
// //g_printf("%d\n",self->set->dices[i].value);
// }
// }
// /* ---> FIN DEBUG <--- */
// return 0;
// }

// int calcScoreSameSideLow(Player *self){
// /* ---> DEBUG     <--- */
// g_printf("\ncalcScoreSameSideLow\n");
// /* ---> FIN DEBUG <--- */
// int i = 0 ;
// for(i=0 ; i<5; i++){
// if(self->set->dices[i].value!=1 && self->set->dices[i].value!=2 && self->set->dices[i].value!=3)
// g_printf("%d\n",self->set->dices[i].value);
// }
// return 1 ;
// }

/**
 * @brief
 * @param self
 * @returns
 *
 *
 */
// int
// epr_get_sum_dices(Player *self){
// int sum = 0 ;
// return sum ;
// }

/**
 * @brief fabrique la base de donnée
 * 
 * @param tab 
 * @return dataBase* 
 */
tabDice *_epr_factory_new(Player *self, eprTab tab)
{
	dice *p = (dice *)g_try_malloc(DICE_NUMBER * sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	else
	{
		p = self->set->dices;
		_epr_set_dices_tab(self, dataBase);
		switch (tab)
		{
		case TAB_SORT_ASC:
			_epr_sort_set_dices(self, dataBase);
			_epr_delta_pattern(dataBase);
			/* test debug*/
			//auparavent analyse du database en chaine Delta :: ecart entre les des
			_epr_search_pattern(OP_DIFFERENT);
			_epr_search_pattern(OP_SIMILAR);
			_epr_search_pattern(OP_MISC);
			/*fin test debug*/
			break;

		default:
			break;
		}
	}
	return &dataBase;
}

/**
 * @brief recupere le tirage de dés pour le joueur en cours
 * @param self
 * @returns
 */
diceSet
	*
	_epr_get_set_dices(Player *self)
{
	dice *p = (dice *)g_try_malloc(DICE_NUMBER * sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	else
		p = self->set->dices;
	for (int i = 0; i < DICE_NUMBER; i++)
	{
		/* display debug */
		//g_printf("dé #%d: %d\n", i + 1, p->value);
		p++;
	}

	return self->set;
}

void _epr_fetch_succ(Player *self, int *tab)
{
}

/**
 * @brief ordonne de facon croisante le tirage de dés
 * pour le joueur en cours
 * @param self
 * @param tab_dice_sort
 */
void _epr_sort_set_dices(Player *self, int *tab_dice_sort)
{
	sort_dice_value_order_by(self, tab_dice_sort, ASC);
}

/**
 * @brief met dans le tableau d'analyse le tirage des dés
 * 
 * @param self 
 * @param tab_dice_sort 
 */
void _epr_set_dices_tab(Player *self, int *tab_dice_sort)
{
	dice *p = (dice *)g_try_malloc(DICE_NUMBER * sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	else
		p = self->set->dices;
	for (int i = 0; i < DICE_NUMBER; i++)
	{
		tab_dice_sort[i] = p->value;
		p++;
	}
}
static void _epr_delta_pattern(const int *DB)
{
	char *p = (char *)g_try_malloc(4 * sizeof(char));
	p = deltaDB;
	g_printf("delta: ");
	for (int i = 0; i < (DICE_NUMBER - 1); i++)
	{
		p = g_strdup_printf("%d", (DB[i + 1] - DB[i]));
		deltaDB[i] = *p;
		p++;
	}
	g_printf("%s\n", deltaDB);
}

static void _epr_search_pattern(eprOpenPattern op)
{
	GRegex *regex = NULL;
	GMatchInfo *match_info;
	char *typeOP = "nothing";
	g_printf("search pattern...\n");
	switch (op)
	{
	case OP_SIMILAR:
		g_printf("# (H) search similar: (C)");
		g_printf("chaine %s ", deltaDB);
		typeOP = "similar (like start full or Yazzi)";
		regex = g_regex_new("0+.*0+", 0, 0, NULL); // au moins deux double(ou triple)
		break;
	case OP_DIFFERENT:
		g_printf("# (H) search different: (C)");
		g_printf("chaine %s ", deltaDB);
		typeOP = "different (like start straight)";
		regex = g_regex_new("1+0*1+", 0, 0, NULL); // au moins trois qui se succede (deux delta de 1)
		break;
	case OP_MISC:
		g_printf("# (H) search misc: (C)");
		g_printf("chaine %s ", deltaDB);
		typeOP = "misc (not wrong/ not good)";
		regex = g_regex_new("^(?!.*(10?1|0.*0)).*", 0, 0, NULL); //exclut tous ce qui est suite et double/triple

		//regex = g_regex_new("", 0, 0, NULL); // 1 double au plus et/ou moins de trois qui se succede
	}
	if (g_regex_match(regex, deltaDB, 0, &match_info))
		g_printf("OK %s ", typeOP);
	else
	{
		g_printf("NOP\n");
	}

	while (g_match_info_matches(match_info))
	{
		gchar *pattern = g_match_info_fetch(match_info, 0);
		g_print(" pour le Pattern: %s\n", pattern);
		g_free(pattern);
		g_match_info_next(match_info, NULL);
	}
	g_match_info_free(match_info);
	g_regex_unref(regex);
}

/**
 * @brief
 * @param pAtom
 *
 *
 */
// eprAtom
// *epr_factory_eprAtom(eprAtom *pAtom, eprPattern *pattern){
// static int id = 0 ;
// ptrEprAtom[id] = pAtom ;
// pAtom->id = id++;
// pAtom->priority = -1;
// pAtom->cycle = 0 ;
// pAtom->sum = epr_get_sum_dices(Players);
// pAtom->scoreName = NULL; 			// nom du motif visé (purpose)
// if(_epr_set_remain_pattern(Players, pattern))
// pAtom->bitSet = pattern->bitSet; //morceaux du set à rejouer
// else pAtom->bitSet = NULL ; //motif complet
// pAtom->family = pattern->family;
// epr_Atom_set_list(pAtom);
// return pAtom ;
//
// }

/**
 * @brief
 * @returns pattern
 *
 *
 */
// eprPattern
// *epr_prototype_eprPattern(eprPattern *pattern){
// if(epr_pattern_get_match(pattern->family)){
// epr_pattern_clone(pattern);
// epr_pattern_set_list(pattern);
// epr_pattern_change(pattern, TRUE);
// }
// //epr_pattern_change(pattern, FALSE);
// }

/**
 * @brief duplique le pattern en choisissant les figures visées dans la famille
 *
 *
 */
// void
// epr_pattern_clone(eprPattern *pattern ){
// // duplique le pattern en choisissant les figures visées dans la famille
// epr_purpose_score[i](Players) ;
// }

/**
 * @brief
 * @param pattern 
 *
 *
 */
// void
// epr_pattern_set_list(eprPattern *pattern){
// static iPattern = 0 ;
// ptrEprPattern[iPattern++] = pattern;
// }

/**
 * @brief informe epr_engine(mediator) du changement
 * @param pattern
 *
 *
 */
// void
// epr_pattern_change(eprPattern *pattern, gboolean change){
// if(change) epr_mediator_engine(Players, change);
// }

/**	MEDIATOR
 * @brief Engine le E de E.P.R.A.I.
 *
 *
 */
// void
// epr_mediator_engine(Player *self, gboolean change){
// eprPattern *pattern = NULL;
// eprAtom *pAtom = NULL;
// eprFamilyPattern family ;
// int iAtom = 0 , iPattern = 0 , value = 0;
//
// pattern = (eprPattern *)malloc(20 * sizeof(eprPattern) );
// if(pattern == NULL ){ exit( EXIT_FAILURE );}
//
// pAtom = (eprAtom *)malloc(20 * sizeof(eprAtom) );
// if(pAtom == NULL ){ exit( EXIT_FAILURE );}
//
// for(family = SAME_DICE_LOW; family <= OTHERS; family++){ // pour tous les motifs
// pattern->family = family ;
// epr_prototype_eprPattern(pattern) ;
// if(family == SAME_DICE_LOW || family == SAME_DICE_HIGH){ // pour les faces identiques
// for(value = 0; value <= 6; value++){
// pattern->value = value ;
// }
// }
// }
// ptrEprPattern[iPattern] = pattern ;
// pattern++ ;
// if(change_){
// ptrEprAtom[iAtom] = epr_factory_eprAtom(pAtom, pattern) ;
// iAtom++ ;
// }
// }
