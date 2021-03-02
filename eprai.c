/*
 * eprai.c
 *
 * Copyright 2016 Le KiWi <jean@antix1>
 * Créé le 24-05-2016 15:52:17 CEST
 * EPRAI
 * Engine Purpose Result Artificial Intelligence
 * C PA DU PIPO ;)
 */

#include "eprai.h"
#include "dice_engine.h"
#include "player.h"
#include <ctype.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>
#include <glib/gregex.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#define LEVEL_LOW 2	 // nombre de face identique basse (sensibilité)
#define LEVEL_HIGH 3 // nombre de face identique haute (sensibilité)
#define BIG_SUM 20	 // nombre considéré pour valider une somme des dés comme big sum

extern GtkWidget *pLabelBar;

typedef enum e_eprOpenPattern
{
	OP_DIFFERENT, // les des sont tres differents
	OP_SIMILAR,	  // les des ont des double ou triple
	OP_MISC,	  // divers aucun des cas
	OP_SMALL_STRAIGHT,
	OP_LARGE_STRAIGHT,
	OP_FULL_HOUSE,
	OP_THREE_OF_KIND,
	OP_FOUR_OF_KIND,
	OP_YAZZI,
	//	OP_CHANCE, // not yet implemented
	OP_YAZZI_BONUS,
	OP_BIG_SUM, // la somme des des est importante
	OP_NOP		// pas de pattern

} eprOpenPattern;

typedef enum e_eprFamilyPattern
{							// quoi: Famille des patterns à chercher
							// (strategie)
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
static eprOpenPattern _epr_search_pattern();
static void _epr_delta_pattern();
char *_epr_do_message_bar();

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
			for (int op = OP_DIFFERENT; op <= OP_BIG_SUM; op++)
			{
				_epr_search_pattern(op);
			}
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
diceSet *_epr_get_set_dices(Player *self)
{
	dice *p = (dice *)g_try_malloc(DICE_NUMBER * sizeof(dice));
	if (p == NULL)
		exit(EXIT_FAILURE);
	else
		p = self->set->dices;
	for (int i = 0; i < DICE_NUMBER; i++)
	{
		/* display debug */
		// g_printf("dé #%d: %d\n", i + 1, p->value);
		p++;
	}

	return self->set;
}

void _epr_fetch_succ(Player *self, int *tab) {}

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
/**
 * @brief recherche un pattern ou un type de pattern approchant
 *
 * @param op
 * @return eprOpenPattern
 */
static eprOpenPattern _epr_search_pattern(eprOpenPattern op)
{
	GRegex *regex = NULL;
	GMatchInfo *match_info;
	static const gchar *typeOP[] = {N_("Different [start Straight]"),
									N_("Similar [start Full,Yazzi]"),
									N_("Misc [neither good or bad]"),
									N_("Small Straight"),
									N_("Large Straight"),
									N_("Full House"),
									N_("Three of Kind"),
									N_("Four of Kind"),
									N_("Yazzi"),
									N_("Yazzi Bonus"),
									N_("Big Sum"),
									N_("Pattern Not Found")
	};
	gint sum = 0;
	//char *typeOP = "not yet implemented";
	switch (op)
	{
	case OP_SIMILAR:
		g_printf("# (H) search similar: (C)");
		g_printf("chaine %s ", deltaDB);
		//typeOP = "similar (like start full or Yazzi)";
		regex =
			g_regex_new("0+.*0+", 0, 0, NULL); // au moins deux double(ou triple)
		break;
	case OP_DIFFERENT:
		g_printf("# (H) search different: (C)");
		g_printf("chaine %s ", deltaDB);
		//typeOP = "different (like start straight)";
		regex =
			g_regex_new("1+0*1+", 0, 0,
						NULL); // au moins trois qui se succede (deux delta de 1)
		break;
	case OP_SMALL_STRAIGHT:
		//typeOP = "small straight ";
		g_printf("# (H) search :%s (C)", typeOP[op]);
		g_printf("chaine %s ", deltaDB);
		regex = g_regex_new("^(1)?(?(1)(011|101|11)($|[^1])|[^1]111)", 0, 0,
							NULL); // sens strict: pas grande suite
		break;
	case OP_LARGE_STRAIGHT:
		//typeOP = "Large straight ";
		g_printf("# (H) search :%s (C)", typeOP[op]);
		g_printf("chaine %s ", deltaDB);
		regex = g_regex_new("1{4}", 0, 0, NULL);
		break;
	case OP_FULL_HOUSE:
		//typeOP = "Full House ";
		g_printf("# (H) search :%s (C)", typeOP[op]);
		g_printf("chaine %s ", deltaDB);
		regex = g_regex_new("^0(0[^0]|[^0]0)0$", 0, 0, NULL);
		break;
	case OP_THREE_OF_KIND:
		//typeOP = "Three of kind ";
		g_printf("# (H) search :%s (C)", typeOP[op]);
		g_printf("chaine %s ", deltaDB);
		regex = g_regex_new("([^0]|^)(0{2})([^0]|$)", 0, 0,
							NULL); // au sens strict: pas de 4 à la suite
		break;
	case OP_FOUR_OF_KIND:
		//typeOP = "Four of kind ";
		g_printf("# (H) search :%s (C)", typeOP[op]);
		g_printf("chaine %s ", deltaDB);
		regex = g_regex_new("([^0]|^)(0{3})([^0]|$)", 0, 0, NULL);
		break;
	case OP_YAZZI:
		//typeOP = "Yazzi ";
		g_printf("# (H) search :%s (C)", typeOP[op]);
		g_printf("chaine %s ", deltaDB);
		regex = g_regex_new("0{4}", 0, 0, NULL);
		break;
	case OP_YAZZI_BONUS:
		//typeOP = "Yazzi Bonus ";
		g_printf("# (H) search :%s (C)", typeOP[op]);
		g_printf("chaine %s ", deltaDB);
		regex = g_regex_new("0{4}", 0, 0, NULL);
		break;
	case OP_MISC:
		//typeOP = "misc (not good/not bad :°) )";
		g_printf("# (H) search misc: (C)");
		g_printf("chaine %s ", deltaDB);
		regex = g_regex_new("^(?!.*(10?1|0.*0)).*", 0, 0,
							NULL); // exclut tous ce qui est suite et double/triple
		break;
	case OP_BIG_SUM:
		//typeOP = "Big Sum";
		// test si la somme des dés est grande
		
		for (int i = 0; i < DICE_NUMBER; i++)
		{
			sum += dataBase[i];
			//g_printf("chaine val du dé %d\n", dataBase[i]);
		}
		if (sum > 20)
			_epr_do_message_bar(_(typeOP[op]), FALSE);
		return op;
	case OP_NOP:
		return op;
	default:
		regex = g_regex_new(".*", 0, 0, NULL);
		
	}

	if (g_regex_match(regex, deltaDB, 0, &match_info))
	{
		g_printf("OK %s ", typeOP[op]);
		_epr_do_message_bar(_(typeOP[op]), FALSE);
		while (g_match_info_matches(match_info))
		{
			gchar *pattern = g_match_info_fetch(match_info, 0);
			g_print(" pour le Pattern: %s\n", pattern);
			g_free(pattern);
			g_match_info_next(match_info, NULL);
		}
	}
	g_match_info_free(match_info);
	g_regex_unref(regex);
	return op;
}
/**
 * @brief fait une ligne des messages à afficher
 * 
 * @param sOP le message à concatener
 * @param reset true/false si on efface le message
 * @return char* le message à afficher dans le label
 */
char *_epr_do_message_bar(const gchar *sOP, gboolean reset)
{
	// g_printf("concat %s\n", sOP);
	static char messageBar[255*sizeof(char)];
	if (reset)
	{
		memset(messageBar, '\0', sizeof(messageBar));
	}
	else{
		g_strlcat(messageBar, " | ", sizeof(messageBar));
		g_strlcat(messageBar, sOP, sizeof(messageBar));
	}
	return messageBar;
}
