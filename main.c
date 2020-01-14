/*
 * main.c
 * Créé le 13 avril 2016 11:47:00
 * Copyright 2016 Le KiWi <jean@antix1>
 *
 */
//#define GETTEXT_PACKAGE "GTK30"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include </usr/include/gtk-3.0/gdk/gdkkeysyms-compat.h>
#include "board_engine.h"
#include "dice_engine.h"
#include "player.h"
#include "result_sheet.h"
#include "eprai.h"
#include <gtk/gtk.h>
#include <glib/gi18n.h>
// #include <glib/gi18n-lib.h>
#include <libintl.h>
#include <locale.h>
#include <glib/gprintf.h>
#include <glib.h>

/*      
#include "eprai.h" 
*/
#define GETTEXT_DOMAIN_NAME "yazziGtk"
#define GETTEXT_DIR_NAME "locale"

#define YAZ_APP_NAME "YazziGtk"
#define YAZ_WINDOW_MAIN_SIZE_H 140
#define YAZ_WINDOW_MAIN_SIZE_V 650
#define YAZ_WINDOW_MAIN_TITLE "Yazzi Le Kiwi :: version Beta 1.01a (GTK+3) ::"
#define YAZ_TABLE_MAIN_ROWS 15
#define YAZ_TABLE_MAIN_COLS 6
#define YAZ_REP_IMAGE "image/"
#define YAZ_NB_BOUTON_FIGURE 8	 //pRadioButton
#define _YAZ_CONCAT(s1, s2) s1##s2 //usage: _YAZ_CONCAT(jean_,_tatareau)  --> jean__tatareau (symbole pas string!)
#define YAZ_ENTRY_NAME_MAX_LENGTH 20
#define YAZ_THEME_PATH "theme/"
#define YAZ_THEME_FILE "yazzi.css"
extern Player *Players,
	Player_1, Player_2;
extern ptr_Player p;
extern int (*row_sheet_score[])(Player *);
extern int _sheet_score_already(Player *self, int numMark);
extern diceSet *_epr_get_set_dices(Player *self);
extern tabDice *_epr_factory_new(Player *self, eprTab tab);
int player_id;

typedef enum e_yaz_state
{
	YAZ_STATE_INIT,
	YAZ_STATE_DICE_SELECT,
	YAZ_STATE_SCORE_SELECT,
	YAZ_STATE_ROLL_ALL,
	YAZ_STATE_ROLL,
	YAZ_STATE_VALID,
	YAZ_STATE_NEXT_PLAYER,
	YAZ_STATE_COUNTER_MAX,
	YAZ_STATE_INVALID_YAZZI,
	YAZ_STATE_ALL_ON,
	YAZ_STATE_ALL_OFF
} yazState;

typedef enum e_yaz_eventKey
{
	YAZ_KEY_CTRL_D,
	YAZ_KEY_CTRL_R
} yazEventKey;

typedef struct s_state_button
{
	gboolean rollAll;
	gboolean roll;
	gboolean valid;
	gboolean nextPlayer;
	gboolean quit;
	gboolean radioButtonDice;
	gboolean radioButtonFigure;
	gboolean boxImageDice;
} stateButton, *pstateButton;

GtkWidget *pWindowMain = NULL, /* fenetre principale */
	*pTableMain = NULL,
		  *pGroupScore = NULL,
		  *pButtonQuit = NULL,
		  *pButtonValid = NULL,
		  *pButtonRollAll = NULL,
		  *pButtonRoll = NULL,
		  *pHButtonBox = NULL,
		  *pLabelName = NULL,
		  *pLabelCount = NULL,
		  *pLabelTurn = NULL,
		  *pRadioButtonSmallStraight = NULL,
		  *pRadioButtonLargeStraight = NULL,
		  *pRadioButtonFullHouse = NULL,
		  *pRadioButtonThreeOfKind = NULL,
		  *pRadioButtonFourOfKind = NULL,
		  *pRadioButtonYazzi = NULL,
		  *pRadioButtonChance = NULL,
		  *pRadioButtonYazziBonus = NULL,
		  *pButtonNextPlayer = NULL,
		  *pHSeparator = NULL,
		  *pVSeparator = NULL,
		  *pValueDice[6],
		  *pDice[DICE_NUMBER],
		  *eventBoxImageDice[DICE_NUMBER],
		  *pRadioButtonDice[6],
		  *pRadioButtonFigure[YAZ_NB_BOUTON_FIGURE],
		  *pValueFigure[YAZ_NB_BOUTON_FIGURE],
		  *pVBoxNames = NULL,
		  *pGridMain = NULL,
		  *pWindowAlert = NULL,
		  *pValue = NULL,
		  *pLabel = NULL,
		  *pLabelAlert = NULL;
GtkCssProvider *pCssProvider = NULL;
GtkImage *image = NULL;
static const gchar *labelCrunching[] = {N_("SubSum"), N_("Bonus"), N_("Upper"), N_("Lower"), N_("Grand Total")};
GtkWidget *pLabelCrunching[5] = {NULL, NULL, NULL, NULL, NULL};
GtkWidget *eventBoxLabelAlert = NULL;
GdkCursor *cursorDice = NULL;
GdkCursor *cursorRowScore = NULL;

void OnDestroy(GtkWidget *pWidget, gpointer pData);
void OnDelete(GtkWidget *pWidget, gpointer pData);
static gboolean
OnKeyPressWindowMain(GtkWidget *pWidget, GdkEvent *event, gpointer pData);
void OnRollAll(GtkWidget *pWidget, gpointer pData);
void OnRoll(GtkWidget *pWidget, gpointer pData);
void OnValid(GtkWidget *pWidget, gpointer pData);
void OnCloseAlert(GtkWidget *widget, gpointer pData);
void OnNextPlayer(GtkWidget *pWidget, gpointer pData);
void OnReleaseRadioButtonDice(GtkWidget *pWidget, gpointer pData);
void OnReleaseAfterRadioButtonDice(GtkWidget *pWidget, gpointer pData);
void OnLeaveRadioButtonDice(GtkWidget *pWidget, gpointer pData);
void OnToggledRadioButtonDice(GtkWidget *pWidget, gpointer pData);
void OnReleaseRadioButtonFigure(GtkWidget *pWidget, gpointer pData);
void OnReleaseAfterRadioButtonFigure(GtkWidget *pWidget, gpointer pData);
void OnToggledRadioButtonFigure(GtkWidget *pWidget, gpointer pData);
static gboolean
OnClickDice(GtkWidget *eventBoxImageDice, GdkEvent *event, gpointer pData); //GdkEventButton //GdkEventKey
static gboolean
OnKeyPressRadioButtonDice(GtkWidget *pWidget, GdkEvent *event, gpointer pData);
static gboolean
OnKeyPressRadioButtonFigure(GtkWidget *pWidget, GdkEvent *event, gpointer pData);
static gboolean
OnEnterDice(GtkWidget *pWidget, GdkEvent *event, gpointer pData);
static gboolean
OnLeaveDice(GtkWidget *pWidget, GdkEvent *event, gpointer pData);
static gboolean
OnEnterRadioButtonScore(GtkWidget *pWidget, GdkEvent *event, gpointer pData);
static gboolean
OnLeaveRadioButtonScore(GtkWidget *pWidget, GdkEvent *event, gpointer pData);

static void
_g_display_players_set_count(const int count);
static void
_g_display_players_set_name(const char *name);
static void
_g_players_set_dices_is_enable(const int name);
static void
_g_display_players_dices_is_enable(const int value, const int name, const gboolean enable);
static void
_g_players_set_dices_is_unknown();
static void
_g_display_players_value_dice_score(const int numMark, const int value);
static void
_g_display_players_value_figure_score(const int numMark, const int value);
static void
_g_mediator_widget_state();
static int
_g_radio_button_dice_num_active();
static int
_g_radio_button_figure_num_active();
static void
_g_button_set_state(stateButton *isState);
static void
_g_display_players_widget_css(GtkWidget *widget, gpointer pData);
static void
_g_display_players_entries_text(Player *self);
static void
_g_display_players_update_score_all(Player *self);
static void
_g_display_players_preliminary_score_all(Player *self);
static void
_g_display_reset_value_score_select();
static void
_g_display_winner();
static gboolean
_g_display_play_again();
static gboolean
_g_display_confirm_quit();
static void
_g_display_players_set_all_names();
static void
_g_display_alert_with_message(GtkWidget *alertMessage, const char *message);
static gboolean
_g_chain_order_focus();
static gboolean
_g_set_focus_dice(int pos);

/**
 * @brief Cé LE MAINEUH         =|8°() <\ © the-little-monkey >
 * @param argc
 * @param argv
 * @returns
 *
 *
 */
int main(int argc, char **argv)
{
	int i = 0;
	/* -------------------------------------------------------------------------- */
	/*							International Le Kiwi <Int>				  		  */
	/* 			  On internationnalise le Yazzi [Fichier .pot->.po->.mo]          */
	/* -------------------------------------------------------------------------- */
	setlocale(LC_ALL, "");
	bindtextdomain(GETTEXT_DOMAIN_NAME, GETTEXT_DIR_NAME);
	bind_textdomain_codeset(GETTEXT_DOMAIN_NAME, "UTF-8");
	textdomain(GETTEXT_DOMAIN_NAME);

	/* -------------------------------------------------------------------------- */
	/*							Init players						  	  		  */
	/* 						On initialise les joueurs, le tableau de jeu	      */
	/* -------------------------------------------------------------------------- */
	srand(time(NULL));
	player_init();
	board_init();

	/* -------------------------------------------------------------------------- */
	/*							Init  App									      */
	/* 						initialisation gtk 							          */
	/* -------------------------------------------------------------------------- */
	g_set_application_name(YAZ_APP_NAME);
	gtk_init(&argc, &argv);

	/* -------------------------------------------------------------------------- */
	/*							Main Window									  	  */
	/* 						crée la fenetre principale 							  */
	/* -------------------------------------------------------------------------- */

	pWindowMain = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	/* Définition de la position */
	gtk_window_set_position(GTK_WINDOW(pWindowMain), GTK_WIN_POS_CENTER);
	/* Définition de la taille de la fenêtre */
	gtk_window_set_default_size(GTK_WINDOW(pWindowMain), 450, 300);
	gtk_widget_set_valign(pWindowMain, GTK_ALIGN_CENTER);
	/* Titre de la fenêtre */
	gtk_window_set_title(GTK_WINDOW(pWindowMain), YAZ_WINDOW_MAIN_TITLE);

	/* -------------------------------------------------------------------------- */
	/*							Signal connect WindowMain						  */
	/* 						Connexion des evenements sur les CallBack(evt) 		  */
	/* evt: "destroy" 		-> suite bouton quitter								  */
	/*      "delete-event" 	-> suite appel fermeture fenetre principale			  */
	/*		"key-press-event"-> appui sur des touches clavier (combinées ou pas)  */
	/* -------------------------------------------------------------------------- */
	gtk_widget_set_events(GTK_WIDGET(pWindowMain), GDK_KEY_PRESS_MASK || GDK_FOCUS_CHANGE_MASK);
	g_signal_connect(pWindowMain, "destroy", G_CALLBACK(OnDestroy), NULL);
	g_signal_connect(pWindowMain, "delete-event", G_CALLBACK(OnDelete), NULL);
	g_signal_connect(pWindowMain, "key-press-event", G_CALLBACK(OnKeyPressWindowMain), NULL);

	/* -------------------------------------------------------------------------- */
	/*							Table 	==> GtkGrid reecriture pour gtk3		  */
	/* 						Grid de mise en place des widgets 	  			      */
	/* -------------------------------------------------------------------------- */
	pGridMain = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(pGridMain), 0);
	//gtk_grid_set_column_spacing(GTK_GRID(pGridMain), 0);
	gtk_grid_set_column_homogeneous(GTK_GRID(pGridMain), TRUE);
	gtk_grid_set_row_spacing(GTK_GRID(pGridMain), 0);
	gtk_container_add(GTK_CONTAINER(pWindowMain), GTK_WIDGET(pGridMain));
	gtk_container_set_border_width(GTK_CONTAINER(pWindowMain), 20);

	/* -------------------------------------------------------------------------- */
	/*							Name of player /roll count	     	  	  		  */
	/* 				Informe du nom du joueur courant et son nombre de jet 		  */
	/* -------------------------------------------------------------------------- */
	pLabelName = gtk_label_new(_("Player : Unknown"));
	pLabelCount = gtk_label_new(_("Count : Unknown"));
	pLabelTurn = gtk_label_new(_("Turn : 1"));
	gtk_grid_attach(GTK_GRID(pGridMain), pLabelName, 0, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(pGridMain), pLabelCount, 2, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(pGridMain), pLabelTurn, 4, 1, 1, 1);
	/* appel le premier joueur */
	Players = player_get_can_play();
	_g_display_players_set_name(Players->name);
	_g_display_players_set_count(Players->set->count);

	/* -------------------------------------------------------------------------- */
	/*							Radio button with label							  */
	/* 					Bouton radio pour selectionner les marques  			  */
	/* -------------------------------------------------------------------------- */
	pGroupScore = gtk_radio_button_new(NULL);
	g_signal_connect(G_OBJECT(pGroupScore), "toggled", G_CALLBACK(OnToggledRadioButtonDice), GINT_TO_POINTER(0));
	static const gchar *labelRadioButtonDice[] = {N_("DICE #1"), N_("DICE #2"), N_("DICE #3"), N_("DICE #4"), N_("DICE #5"), N_("DICE #6")};
	static const gchar *labelRadioButtonFigure[] = {N_("Small Straight"), N_("Large Straight"), N_("Full House"), N_("Three Of Kind"), N_("Four Of Kind"), N_("Yazzi"), N_("Chance"), N_("Yazzi Bonus")};
	static const gchar *toolTipRadioButtonFigure[] = {N_("30 points"), N_("40 points"), N_("25 points"), N_("Sum"), N_("Sum"), N_("50 points"), N_("Sum"), N_("50 points")};
	gchar *display;
	gint k = 0;
	for (i = 0; i <= DICE_NUMBER; i++)
	{
		pRadioButtonDice[i] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pGroupScore), _(labelRadioButtonDice[i]));
		display = g_strdup_printf(_("Sum of same dices #%d"), i + 1); //convert num to str
		gtk_widget_set_tooltip_text(pRadioButtonDice[i], display);
		g_free(display);
		// (toggled, activate, clicked, enter, leave, pressed, released).
		k = i + 1;
		g_signal_connect(G_OBJECT(pRadioButtonDice[i]), "released", G_CALLBACK(OnReleaseRadioButtonDice), GINT_TO_POINTER(k));
		g_signal_connect(G_OBJECT(pRadioButtonDice[i]), "activate", G_CALLBACK(OnReleaseRadioButtonDice), GINT_TO_POINTER(k));
		g_signal_connect(G_OBJECT(pRadioButtonDice[i]), "toggled", G_CALLBACK(OnToggledRadioButtonDice), GINT_TO_POINTER(k));
		g_signal_connect_after(G_OBJECT(pRadioButtonDice[i]), "released", G_CALLBACK(OnReleaseAfterRadioButtonDice), GINT_TO_POINTER(k));
		g_signal_connect(G_OBJECT(pRadioButtonDice[i]), "key-press-event", G_CALLBACK(OnKeyPressRadioButtonDice), GINT_TO_POINTER(k));
		//_after pour laisser le CSS "agir" :hover
		g_signal_connect_after(G_OBJECT(pRadioButtonDice[i]), "enter-notify-event", G_CALLBACK(OnEnterRadioButtonScore), GINT_TO_POINTER(k));
		g_signal_connect_after(G_OBJECT(pRadioButtonDice[i]), "leave-notify-event", G_CALLBACK(OnLeaveRadioButtonScore), GINT_TO_POINTER(k));
		gtk_grid_attach(GTK_GRID(pGridMain), pRadioButtonDice[i], 0, i + 2, 1, 1);
	}
	for (i = 0; i < YAZ_NB_BOUTON_FIGURE; i++)
	{
		pRadioButtonFigure[i] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pGroupScore),
																			_(labelRadioButtonFigure[i]));
		gtk_widget_set_tooltip_text(pRadioButtonFigure[i], _(toolTipRadioButtonFigure[i]));
		k = i + 1;
		g_signal_connect(G_OBJECT(pRadioButtonFigure[i]), "released", G_CALLBACK(OnReleaseRadioButtonFigure), GINT_TO_POINTER(k));
		g_signal_connect(G_OBJECT(pRadioButtonFigure[i]), "activate", G_CALLBACK(OnReleaseRadioButtonFigure), GINT_TO_POINTER(k));
		g_signal_connect(G_OBJECT(pRadioButtonFigure[i]), "toggled", G_CALLBACK(OnToggledRadioButtonFigure), GINT_TO_POINTER(k));
		g_signal_connect_after(G_OBJECT(pRadioButtonFigure[i]), "released", G_CALLBACK(OnReleaseAfterRadioButtonFigure), GINT_TO_POINTER(k));
		g_signal_connect(G_OBJECT(pRadioButtonFigure[i]), "key-press-event", G_CALLBACK(OnKeyPressRadioButtonFigure), GINT_TO_POINTER(k));
		//_after pour laisser le CSS "agir" :hover
		g_signal_connect_after(G_OBJECT(pRadioButtonFigure[i]), "enter-notify-event", G_CALLBACK(OnEnterRadioButtonScore), GINT_TO_POINTER(k));
		g_signal_connect_after(G_OBJECT(pRadioButtonFigure[i]), "leave-notify-event", G_CALLBACK(OnLeaveRadioButtonScore), GINT_TO_POINTER(k));
		gtk_grid_attach(GTK_GRID(pGridMain), pRadioButtonFigure[i], 3, i + 2, 1, 1);
	}

	/* -------------------------------------------------------------------------- */
	/*							Entry read only values 			     	  		  */
	/* 						Affiche le score pour chaque marque    			      */
	/* -------------------------------------------------------------------------- */
	/* Colonne gauche */
	const gchar *nameEntryScore = "rowScore";
	for (i = 0; i <= DICE_NUMBER; i++)
	{
		pValueDice[i] = gtk_entry_new();
		gtk_editable_set_editable(GTK_EDITABLE(pValueDice[i]), FALSE);
		gtk_widget_set_can_focus(pValueDice[i], FALSE);
		gtk_entry_set_max_length(GTK_ENTRY(pValueDice[i]), 2);
		gtk_entry_set_width_chars(GTK_ENTRY(pValueDice[i]), 2);
		gtk_entry_set_text(GTK_ENTRY(pValueDice[i]), "0");
		gtk_entry_set_alignment(GTK_ENTRY((pValueDice[i])), 0.5); //float: 0 <= xalign <= 1
		gtk_widget_set_halign(pValueDice[i], GTK_ALIGN_START);
		gtk_grid_attach(GTK_GRID(pGridMain), pValueDice[i], 1, i + 2, 1, 1);
		gtk_widget_set_name(pValueDice[i], nameEntryScore);
	}
	/* Colonne droite */
	for (i = 0; i < YAZ_NB_BOUTON_FIGURE; i++)
	{
		pValueFigure[i] = gtk_entry_new();
		gtk_editable_set_editable(GTK_EDITABLE(pValueFigure[i]), FALSE);
		gtk_widget_set_can_focus(pValueFigure[i], FALSE);
		gtk_entry_set_max_length(GTK_ENTRY(pValueFigure[i]), 2);
		gtk_entry_set_width_chars(GTK_ENTRY(pValueFigure[i]), 2);
		gtk_entry_set_text(GTK_ENTRY(pValueFigure[i]), "0");
		gtk_entry_set_alignment(GTK_ENTRY((pValueFigure[i])), 0.5);
		gtk_widget_set_halign(pValueFigure[i], GTK_ALIGN_START);
		gtk_grid_attach(GTK_GRID(pGridMain), pValueFigure[i], 4, i + 2, 1, 1);
		gtk_widget_set_name(pValueFigure[i], nameEntryScore);
	}

	/* -------------------------------------------------------------------------- */
	/*							SubSum Bonus Upper Lower Grand Total   	  		  */
	/* 					Sous calcul intermediaire et totaux...		  			  */
	/* -------------------------------------------------------------------------- */
	;
	for (i = 0; i < 5; i++)
	{

		pLabelCrunching[i] = gtk_label_new(_(labelCrunching[i]));
		gtk_label_set_justify(GTK_LABEL(pLabelCrunching[i]), GTK_JUSTIFY_RIGHT);
	}
	for (i = 0; i <= 2; i++)
	{

		gtk_grid_attach(GTK_GRID(pGridMain), pLabelCrunching[i], 0, 8 + i, 1, 1); //subsum bonus upper
	}
	gtk_grid_attach(GTK_GRID(pGridMain), pLabelCrunching[3], 3, 10, 1, 1);				  //lower
	gtk_grid_attach(GTK_GRID(pGridMain), pLabelCrunching[4], 0, 11, 5, 1);				  //grand total
	gtk_widget_set_tooltip_text(pLabelCrunching[1], _("SubSum > 63 points: +35 points")); // Bonus

	/* -------------------------------------------------------------------------- */
	/*							Box Dices							  	  		  */
	/* 						Boite de dés a jouer    				  			  */
	/* -------------------------------------------------------------------------- */
	/* une box event pour wrapper l'image */
	gchar *de = "diceX-64x64.png";
	for (i = 0; i < DICE_NUMBER; i++)
	{
		eventBoxImageDice[i] = gtk_event_box_new();
		gtk_event_box_set_visible_window(GTK_EVENT_BOX(eventBoxImageDice[i]), TRUE);
		pDice[i] = gtk_image_new_from_file(g_strdup_printf("%s%s", YAZ_REP_IMAGE, de));
		gtk_grid_attach(GTK_GRID(pGridMain), eventBoxImageDice[i], i, 12, 1, 1);
		gtk_container_add(GTK_CONTAINER(eventBoxImageDice[i]), GTK_WIDGET(pDice[i]));
		gtk_widget_set_can_focus(GTK_WIDGET(pDice[i]), TRUE);
		gtk_widget_set_events(GTK_WIDGET(pDice[i]), GDK_KEY_PRESS_MASK || GDK_FOCUS_CHANGE_MASK);
		g_signal_connect(eventBoxImageDice[i],
						 "button-press-event",
						 G_CALLBACK(OnClickDice),
						 GINT_TO_POINTER(i));
		//evt: set-focus-child  focus-in-event g_signal_connect GTK_CONTAINER(eventBoxImageDice[i])
		// g_signal_connect(GTK_CONTAINER(eventBoxImageDice[i]),
		// 				 "set-focus-child",
		// 				 G_CALLBACK(OnClickDice),
		// 				 GINT_TO_POINTER(i));
		g_signal_connect(GTK_WIDGET(pDice[i]),
						 "key-press-event",
						 G_CALLBACK(OnClickDice),
						 GINT_TO_POINTER(i));
		g_signal_connect(GTK_WIDGET(eventBoxImageDice[i]), "enter-notify-event", G_CALLBACK(OnEnterDice), GINT_TO_POINTER(i));
		g_signal_connect(GTK_WIDGET(eventBoxImageDice[i]), "leave-notify-event", G_CALLBACK(OnLeaveDice), GINT_TO_POINTER(i));
	}

	/* -------------------------------------------------------------------------- */
	/*							Box Button Lower    				  	  		  */
	/* 						Barre des boutons en bas							  */
	/*          |Roll All| |Roll| |Valid| |Next Player| |Quit|                    */
	/* -------------------------------------------------------------------------- */
	static const gchar *toolTipButtonLower[] = {N_("Roll all dices"), N_("Roll a dice selection"), N_("Valid a select score"), N_("Go to next player"), N_("Quit the game")};
	pHButtonBox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(pHButtonBox), GTK_BUTTONBOX_SPREAD);
	pButtonQuit = gtk_button_new_with_mnemonic(_("_Quit"));
	gtk_widget_set_tooltip_text(pButtonQuit, _(toolTipButtonLower[4]));
	pButtonValid = gtk_button_new_with_mnemonic(_("_Valid"));
	gtk_widget_set_tooltip_text(pButtonValid, _(toolTipButtonLower[2]));
	pButtonRollAll = gtk_button_new_with_mnemonic(_("Roll _All"));
	gtk_widget_set_tooltip_text(pButtonRollAll, _(toolTipButtonLower[0]));
	pButtonRoll = gtk_button_new_with_mnemonic(_("_Roll"));
	gtk_widget_set_tooltip_text(pButtonRoll, _(toolTipButtonLower[1]));
	pButtonNextPlayer = gtk_button_new_with_mnemonic(_("_Next Player"));
	gtk_widget_set_tooltip_text(pButtonNextPlayer, _(toolTipButtonLower[3]));
	gtk_grid_attach(GTK_GRID(pGridMain), GTK_WIDGET(pHButtonBox), 0, 13, 5, 1);
	gtk_box_pack_start(GTK_BOX(pHButtonBox), pButtonRollAll, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pHButtonBox), pButtonRoll, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pHButtonBox), pButtonValid, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pHButtonBox), pButtonNextPlayer, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pHButtonBox), pButtonQuit, TRUE, FALSE, 0);
	g_signal_connect(pButtonRollAll, "clicked", G_CALLBACK(OnRollAll), NULL);
	g_signal_connect(pButtonRoll, "clicked", G_CALLBACK(OnRoll), NULL);
	g_signal_connect(pButtonValid, "clicked", G_CALLBACK(OnValid), NULL);
	g_signal_connect(pButtonNextPlayer, "clicked", G_CALLBACK(OnNextPlayer), NULL);
	g_signal_connect(pButtonQuit, "clicked", G_CALLBACK(OnDestroy), NULL);
	_g_mediator_widget_state(pWindowMain, GINT_TO_POINTER(YAZ_STATE_INIT));

	/* -------------------------------------------------------------------------- */
	/*							Boite d'alerte de l'application		     	      */
	/* 	                        top level box  message alert /!\                  */
	/* -------------------------------------------------------------------------- */
	pWindowAlert = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_modal(GTK_WINDOW(pWindowAlert), TRUE);
	/* Rend dependante la fenetre alerte de la principale, utile pour la rendre modale*/
	gtk_window_set_transient_for(GTK_WINDOW(pWindowAlert), GTK_WINDOW(pWindowMain));
	/* Définition de la position */
	gtk_window_set_position(GTK_WINDOW(pWindowAlert), GTK_WIN_POS_CENTER_ON_PARENT);
	/* Définition de la taille de la fenêtre */
	gtk_window_set_default_size(GTK_WINDOW(pWindowAlert), 250, 100);
	/* Titre de la fenêtre [ inutile car sans decoration] pour un autre aspect may be!*/
	gtk_window_set_title(GTK_WINDOW(pWindowAlert), _("Message alert"));
	/* ajoute un label null , sera remplit ulterieurement*/
	pLabelAlert = gtk_label_new(NULL);
	/* Definition pour un evenement */
	eventBoxLabelAlert = gtk_event_box_new();
	gtk_event_box_set_visible_window(GTK_EVENT_BOX(eventBoxLabelAlert), FALSE);
	/* Ajoute le texte à la boite */
	gtk_container_add(GTK_CONTAINER(eventBoxLabelAlert), pLabelAlert);
	/* ajoute la boite a la fenetre */
	gtk_container_add(GTK_CONTAINER(pWindowAlert), eventBoxLabelAlert);
	/* enleve les decoration pour un aspect tool tips*/
	gtk_window_set_decorated(GTK_WINDOW(pWindowAlert), FALSE);
	/* callback de l'evenement de la boite evenement */
	g_signal_connect(eventBoxLabelAlert, "button-press-event", G_CALLBACK(OnCloseAlert), NULL);
	g_signal_connect(eventBoxLabelAlert, "delete-event", G_CALLBACK(OnCloseAlert), NULL);

	/* -------------------------------------------------------------------------- */
	/*							CSS  Computer S_hazarding S_Zone		     	  */
	/* 	                un peu de CSS pour rendre la vie plus belle               */
	/*                               Et Hop! Cascading!                           */
	/* -------------------------------------------------------------------------- */
	pCssProvider = gtk_css_provider_new();
	const gchar *themePathFile = g_strdup_printf("%s%s", YAZ_THEME_PATH, YAZ_THEME_FILE);
	if (gtk_css_provider_load_from_path(pCssProvider, themePathFile, NULL))
		gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
												  GTK_STYLE_PROVIDER(pCssProvider),
												  GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_style_context_add_class(gtk_widget_get_style_context(pLabelName), "labeltop");
	gtk_style_context_add_class(gtk_widget_get_style_context(pLabelCount), "labeltop");
	gtk_style_context_add_class(gtk_widget_get_style_context(pLabelTurn), "labeltop");
	for (gint i = 0; i < DICE_NUMBER; i++)
	{
		gtk_style_context_add_class(gtk_widget_get_style_context(pDice[i]), "dice");
	}
	gtk_style_context_add_class(gtk_widget_get_style_context(pLabelCrunching[4]), "important");
	for (int i = 0; i < 5; i++)
	{
		if (i != 4)
			gtk_style_context_add_class(gtk_widget_get_style_context(pLabelCrunching[i]), "labelcrunching");
	}
	gtk_style_context_add_class(gtk_widget_get_style_context(pWindowAlert), "windowAlert");
	gtk_widget_set_name(pWindowMain, "windowMain");

	/* -------------------------------------------------------------------------- */
	/*							VBox Names								  	  	  */
	/* 	Boite de dialogue modale pour saisir les noms des joueurs	              */
	/* -------------------------------------------------------------------------- */
	_g_display_players_set_all_names();

	/* -------------------------------------------------------------------------- */
	/*							Players update							  	  	  */
	/* 	            Mise à jour des noms des joueurs	                          */
	/* -------------------------------------------------------------------------- */
	player_id = Players->id;
	_g_display_players_update_score_all(Players);

	/* -------------------------------------------------------------------------- */
	/*							Show go on								  	  	  */
	/* 					On affiche window main et tout  		    			  */
	/* -------------------------------------------------------------------------- */
	gtk_grid_set_column_homogeneous(GTK_GRID(pGridMain), TRUE);
	gtk_widget_show_all(pWindowMain);

	/* -------------------------------------------------------------------------- */
	/*							Launch Main						  				  */
	/* 					(x)	°  Earth -===|> Moon                				  */
	/* -------------------------------------------------------------------------- */
	gtk_main();

	/* -------------------------------------------------------------------------- */
	/* 						  Fin du monde							  			  */
	/* -------------------------------------------------------------------------- */
	gtk_widget_destroy(pWindowMain);
	gtk_main_quit();
	return 0;
}

/**
 * @brief pattern mediator pour synchroniser les etats des widgets
 * en fonction du jeu
 *
 *
 */
static void
_g_mediator_widget_state(GtkWidget *pWidget, gpointer pState)
{
	/* stateButton = la suite des booleens suivantes :
	/       rollAll, roll, valid, nextPlayer, quit, radioButtonDice, radioButtonFigure, boxImageDice
	*/
	stateButton stateInit = {TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE};
	stateButton stateRollAll = {TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE};
	stateButton stateRoll = {FALSE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE};
	stateButton stateScoreSelect = {TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE};
	stateButton stateDiceSelect = {TRUE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE};
	stateButton stateValid = {FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE};
	stateButton stateNextPlayer = {TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE}; //EN DEBUGactive les des et figures
	stateButton stateCounterMax = {FALSE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE, FALSE};
	stateButton stateAllOff = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
	stateButton stateAllOn = {TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE};
	stateButton stateInvalidYazzi = {TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE};
	pstateButton pstate;
	pstate = (pstateButton)malloc(1 * sizeof(stateButton));
	if (pstate == NULL)
	{
		exit(EXIT_FAILURE);
	}
	switch (GPOINTER_TO_INT(pState))
	{
	case YAZ_STATE_INIT:
		pstate = &stateInit;
		break;
	case YAZ_STATE_ROLL_ALL:
		pstate = &stateRollAll;
		break;
	case YAZ_STATE_ROLL:
		pstate = &stateRoll;
		break;
	case YAZ_STATE_SCORE_SELECT:
		pstate = &stateScoreSelect;
		break;
	case YAZ_STATE_DICE_SELECT:
		pstate = &stateDiceSelect;
		break;
	case YAZ_STATE_VALID:
		pstate = &stateValid;
		break;
	case YAZ_STATE_NEXT_PLAYER:
		_g_button_set_state(&stateRollAll); //met à jour l'affichage des button dice et figure au joueur en cours
		pstate = &stateNextPlayer;
		break;
	case YAZ_STATE_COUNTER_MAX:
		pstate = &stateCounterMax;
		break;
	case YAZ_STATE_INVALID_YAZZI:
		pstate = &stateInvalidYazzi;
		break;
	case YAZ_STATE_ALL_OFF:
		pstate = &stateAllOff;
		break;
	case YAZ_STATE_ALL_ON:
		pstate = &stateAllOn;
		break;
	default:
		pstate = &stateAllOn; //debug usage
		;
	}
	_g_button_set_state(pstate);
}

/**
 * @brief Appeler par WindowMain lors de sa destruction fin de prog
 * @param pWidget
 * @param pData
 *
 *
 */
void OnDestroy(GtkWidget *pWidget, gpointer pData)
{
	if (_g_display_confirm_quit() == TRUE)
	{
		exit(EXIT_SUCCESS);
	}
}

/**
 * @brief Appeler par Windowmain lorsque clique barre de fenetre haut droit [X]
 * @param pWidget
 * @param pData
 * 
 */
void OnDelete(GtkWidget *pWidget, gpointer pData)
{
	if (_g_display_confirm_quit() == TRUE)
	{
		exit(EXIT_SUCCESS);
	}
	else if (gtk_widget_hide_on_delete(pWindowMain))
		gtk_widget_show_all(pWindowMain);
}

/**
 * @brief Gestion des evenements clavier sur la fenetre principale
 * 
 * @param pWidget la fenetre principale
 * @param event   evenement que l'on transtype clavier pour etre plus souple
 * @param pData   un data, peut etre NULL
 */
static gboolean
OnKeyPressWindowMain(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
	yazEventKey eventKey;
	GdkModifierType modifiers = gtk_accelerator_get_default_mod_mask(); //evite interference caps lock/num lock
	// g_printf("CALLBACK: OnKeyPressWindowMain calc keyval 0x%03x state %d\n",
	// 		 ((GdkEventKey *)event)->keyval,
	// 		 (((GdkEventKey *)event)->state & modifiers));
	// g_printf("CALLBACK: OnKeyPressWindowMain refe keyval 0x%03x state %d\n", GDK_d, GDK_CONTROL_MASK);
	if (((GdkEventKey *)event)->keyval == GDK_d && (((GdkEventKey *)event)->state & modifiers) == GDK_CONTROL_MASK)
	{
		eventKey = YAZ_KEY_CTRL_D;
	}
	if (((GdkEventKey *)event)->keyval == GDK_r && (((GdkEventKey *)event)->state & modifiers) == GDK_CONTROL_MASK)
	{
		eventKey = YAZ_KEY_CTRL_R;
	}

	switch (eventKey)
	{
	case YAZ_KEY_CTRL_D:
		return _g_set_focus_dice(1);
		break;
	case YAZ_KEY_CTRL_R:
		return _g_chain_order_focus();
		break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

/**
 * @brief met le focus sur un dé à jouer
 * 
 * @param pos position du dé {1..DICE_NUMBER}
 * @return gboolean a pu ou pas
 */
static gboolean
_g_set_focus_dice(int pos)
{
	const int ind = pos - 1;
	if (ind >= 0 && ind <= DICE_NUMBER)
	{
		if (gtk_widget_get_can_focus(pDice[ind]) && gtk_widget_is_sensitive(pDice[ind]))
		{
			gtk_widget_set_can_focus(pDice[ind], TRUE);
			gtk_widget_grab_focus(pDice[ind]);
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * @brief On demande de jouer tous les dés
 * @param pWidget
 * @param pData
 *
 *
 */
void OnRollAll(GtkWidget *pWidget, gpointer pData)
{
	if (Players->set->count < DICE_SET_MAX_COUNT)
	{
		if (Players->set->count < DICE_SET_MAX_COUNT - 1)
			_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_ROLL_ALL));
		else
			_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_COUNTER_MAX));
		Players->set->count++;
		_g_display_players_set_count(Players->set->count);
		_g_display_reset_value_score_select();
		roll_all(Players);
	}
	_g_chain_order_focus();
	/*
	epr AI
	*/
	//
	int *p = (int *)g_malloc(DICE_NUMBER * sizeof(int));
	if (p != NULL)
		p = _epr_factory_new(Players, TAB_SORT_ASC)[0];
	for (int i = 0; i < DICE_NUMBER; i++)
	{
		g_printf("dices in order : %d\n", *p);
		p++;
	}
}

/**
 * @brief ordonne la chaine des focus sur le premier widget des scores sensible au focus
 * 
 * @return gboolean retouve un focus ou pas
 */

static gboolean
_g_chain_order_focus()
{
	for (int i = 0; i < 6; i++)
	{
		if (gtk_widget_get_can_focus(pRadioButtonDice[i]) &&
			gtk_widget_is_sensitive(pRadioButtonDice[i]))
		{
			gtk_widget_set_can_focus(GTK_WIDGET(pRadioButtonDice[i]), TRUE);
			gtk_widget_grab_focus(pRadioButtonDice[i]);
			return (TRUE);
		}
	}
	for (int i = 0; i < 8; i++)
	{
		if (gtk_widget_get_can_focus(pRadioButtonFigure[i]) &&
			gtk_widget_is_sensitive(pRadioButtonFigure[i]))
		{
			gtk_widget_set_can_focus(GTK_WIDGET(pRadioButtonFigure[i]), TRUE);
			gtk_widget_grab_focus(pRadioButtonFigure[i]);
			return (TRUE);
		}
	}
	return (FALSE);
}

/**
 * @brief On lance les des selectionnes
 * @param pWidget
 * @param pData
 *
 *
 */
void OnRoll(GtkWidget *pWidget, gpointer pData)
{
	diceName name = DICE_1;
	if (Players->set->count > DICE_SET_MAX_COUNT - 2)
		_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_COUNTER_MAX));
	Players->set->count++;
	_g_display_players_set_count(Players->set->count);
	_g_display_reset_value_score_select();
	while (name <= DICE_5)
	{
		if (Players->set->dices[name].enable == FALSE)
		{
			Players->set->dices[name].enable = TRUE;
			roll_a_dice(Players, name);
			_g_display_players_dices_is_enable(Players->set->dices[name].value, name, TRUE);
		}
		name++;
	};
	/**************************************************************************************************/
	//test test test test test test test test test test test test test test test test test test test test
	int numMarkDice = _g_radio_button_dice_num_active();
	int numMarkFigure = _g_radio_button_figure_num_active();
	if (numMarkDice != -1)
		gtk_widget_set_state_flags(GTK_WIDGET(pRadioButtonDice[numMarkDice - 1]), GTK_STATE_FLAG_NORMAL, TRUE);
	else if (numMarkFigure != -1)
		gtk_widget_set_state_flags(GTK_WIDGET(pRadioButtonFigure[numMarkDice - 7]), GTK_STATE_FLAG_NORMAL, TRUE);

	numMarkDice = _g_radio_button_dice_num_active();
	g_printf("numMark FLAGS: %d\n", numMarkDice);
	// fin test  fin test  fin test  fin test  fin test  fin test  fin test  fin test  fin test  fin test
	/**************************************************************************************************/
	_g_chain_order_focus();
	/*
	epr AI
	*/
	//
	int *p = (int *)g_malloc(DICE_NUMBER * sizeof(int));
	if (p != NULL)
		p = _epr_factory_new(Players, TAB_SORT_ASC)[0];
	for (int i = 0; i < DICE_NUMBER; i++)
	{
		g_printf("dices in order : %d\n", *p);
		p++;
	}
}

/**
 * @brief On valide la marque choisie
 * @param pWidget
 * @param pData
 *
 *
 */
void OnValid(GtkWidget *pWidget, gpointer pData)
{
	int numMarkDice = -1, numMarkFigure = -1, numMark = 0;
	numMarkDice = _g_radio_button_dice_num_active();
	numMarkFigure = _g_radio_button_figure_num_active();

	if (numMarkDice != -1)
	{
		numMark = numMarkDice;
	}
	else if (numMarkFigure != -1)
	{
		numMark = numMarkFigure;
	}
	else
		numMark = 0;
	if (sheet_score_write(Players, numMark) != -1 && numMark)
	{
		sheet_score_update_all(Players);
		_g_display_players_update_score_all(Players);
		_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_VALID));
	}
	else
	{
		_g_display_alert_with_message(pWindowAlert, _("You must select a choice !"));
		_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_COUNTER_MAX));
	}
	_g_chain_order_focus();
}

/**
 * @brief Affiche la boite d'alerte avec un message
 * 
 * @param alertMessage la boite d'alerte
 * @param message 
 */
static void
_g_display_alert_with_message(GtkWidget *alertMessage, const char *message)
{
	const gchar *markupAlert = g_strdup_printf("%s\n<span style=\"italic\">%s</span>", message, _("clicked to close."));
	GValue valOpacity = {
		0,
	};
	//g_value_init(&valOpacity,G_TYPE_DOUBLE);
	gtk_style_context_get_property(gtk_widget_get_style_context(pWindowAlert), "opacity", GTK_STATE_FLAG_NORMAL, &valOpacity);
	gtk_label_set_markup(GTK_LABEL(pLabelAlert), markupAlert);
	gtk_widget_show_all(alertMessage);
	gtk_widget_set_opacity(GTK_WIDGET(pWindowAlert), g_value_get_double(&valOpacity));
	//g_printf("opacity: %s\n", g_strdup_value_contents(&valOpacity));
}

/**
 * @brief ferme le message d'alerte
 * 
 * @param widget la fenetre du message
 * @param pData NULL
 */
void OnCloseAlert(GtkWidget *widget, gpointer pData)
{
	gtk_widget_hide(pWindowAlert);
}

/**
 * @brief On passe la main au joueur suivant
 * @param pWidget
 * @param pData
 *
 *
 */
void OnNextPlayer(GtkWidget *pWidget, gpointer pData)
{
	int static turn = 1;
	Players = player_swap(Players, p);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pGroupScore), TRUE);
	_g_players_set_dices_is_unknown();

	if (Players->canPlay && !full_sheet_score(Players))
	{
		if (player_id == Players->id)
		{
			turn++;
			gchar *display = g_strdup_printf(_("Turn : %d"), turn);
			gtk_label_set_text(GTK_LABEL(pLabelTurn), display);
			g_free(display);
		}
		_g_display_players_set_name(Players->name);
		_g_display_players_set_count(Players->set->count);
		_g_display_players_entries_text(Players);
		_g_display_players_update_score_all(Players);
		_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_NEXT_PLAYER));
	}
	else
	{
		_g_display_winner();
		if (!_g_display_play_again())
		{ /* Y'en a marre ! :( */
			exit(EXIT_SUCCESS);
		}
		sheet_score_all_reset();
		player_all_set_reset();
		board_init();
		Players = player_get_can_play();
		_g_display_players_entries_text(Players);
		_g_display_players_update_score_all(Players);
		_g_display_players_set_name(Players->name);
		_g_display_players_set_count(Players->set->count);
		_g_mediator_widget_state(pWindowMain, GINT_TO_POINTER(YAZ_STATE_INIT));
		turn = 1;
		player_id = Players->id;
		gchar *display = g_strdup_printf(_("Turn : %d"), turn);
		gtk_label_set_text(GTK_LABEL(pLabelTurn), display);
		g_free(display);
	}
}

/**
 * @brief on clique sur une image de dé
 * @param eventBoxImageDice
 * @param event
 * @param pData
 * @returns return vrai si clique
 *
 *
 */
static gboolean
OnClickDice(GtkWidget *eventBoxImageDice, GdkEvent *event, gpointer pData) //GdkEventButton //GdkEventKey
{
	const int name = GPOINTER_TO_INT(pData);
	//g_printf("onClick keyval %d hardware keycode %d\n", ((GdkEventKey *)event)->keyval, ((GdkEventKey *)event)->hardware_keycode);
	if (Players->set->count > 0 && Players->set->count <= DICE_SET_MAX_COUNT && (((GdkEventKey *)event)->keyval == GDK_KEY_space || ((GdkEventButton *)event)->type == GDK_BUTTON_PRESS))
	{
		_g_mediator_widget_state(eventBoxImageDice, GINT_TO_POINTER(YAZ_STATE_DICE_SELECT));
		_g_players_set_dices_is_enable(name);
	}
	else
		return FALSE;
	return TRUE;
}

/**
 * @brief une touche clavier sur le radio button des dés
 * 
 * @param pWidget le widget appelant
 * @param event evenement clavier
 * @param pData la position du radio bouton
 * @return gboolean 
 */
static gboolean
OnKeyPressRadioButtonDice(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
	if (Players->set->count > 0 && Players->set->count <= DICE_SET_MAX_COUNT && (((GdkEventKey *)event)->keyval == GDK_KEY_Return || ((GdkEventButton *)event)->type == GDK_BUTTON_PRESS))
	{
		OnReleaseRadioButtonDice(pWidget, pData);
		_g_display_players_preliminary_score_all(Players);
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief une touche clavier sur le radio button des figure
 * 
 * @param pWidget le widget appelant
 * @param event evenement clavier
 * @param pData la position du radio bouton
 * @return gboolean 
 */
static gboolean
OnKeyPressRadioButtonFigure(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
	if (Players->set->count > 0 && Players->set->count <= DICE_SET_MAX_COUNT && (((GdkEventKey *)event)->keyval == GDK_KEY_Return || ((GdkEventButton *)event)->type == GDK_BUTTON_PRESS))
	{
		OnReleaseRadioButtonFigure(pWidget, pData);
		_g_display_players_preliminary_score_all(Players);
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief Clique sur le bouton radio du choix des dés
 * @param pWidget
 * @param pData
 *
 *
 */
void OnReleaseRadioButtonDice(GtkWidget *pWidget, gpointer pData)
{
	int value = 0;
	int numMark = GPOINTER_TO_INT(pData);
	if (numMark < 7)
	{
		value = calcScoreSameSide(Players, numMark);
	}
	if (Players->set->count > DICE_SET_MAX_COUNT - 1)
		_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_COUNTER_MAX));
	else
		_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_SCORE_SELECT));

	_g_display_players_value_dice_score(numMark, value);
	/* ---> DEBUG     <--- */
	// g_printf("eventRadioButtonDice numMark: %d\n", numMark);
	// g_printf("Score crunch: %d\n", value);
	/* ---> FIN DEBUG <--- */
	;
}

/**
 * @brief Calcul temporaire coté Dice pour evaluation par le joueur des points totaux qu'il peut marquer
 * 
 * @param pWidget 
 * @param pData 
 */
void OnReleaseAfterRadioButtonDice(GtkWidget *pWidget, gpointer pData)
{
	_g_display_players_preliminary_score_all(Players);
}

/**
 * @brief  Calcul temporaire coté Figure pour evaluation par le joueur des points totaux qu'il peut marquer
 * 
 * @param pWidget 
 * @param pData 
 */
void OnReleaseAfterRadioButtonFigure(GtkWidget *pWidget, gpointer pData)
{
	_g_display_players_preliminary_score_all(Players);
}

/**
 * @brief Clique sur le bouton radio du choix des figures
 * @param pWidget
 * @param pData
 *
 *
 */
void OnReleaseRadioButtonFigure(GtkWidget *pWidget, gpointer pData)
{
	int value = 0;
	int numMark = GPOINTER_TO_INT(pData);
	//DEBUG:: fuite memoire ???: sur Full House
	// 	value = (row_sheet_score[numMark])(Players);
	//FIN DEBUGS
	switch (numMark)
	{
	case 1:
		value = calcScoreSmallStraight(Players);
		break;
	case 2:
		value = calcScoreLargeStraight(Players);
		break;
	case 3:
		value = calcScoreFullHouse(Players);
		break;
	case 4:
		value = calcScoreThreeOfKind(Players);
		break;
	case 5:
		value = calcScoreFourOfKind(Players);
		break;
	case 6:
		value = calcScoreYazzi(Players);
		break;
	case 7:
		value = calcScoreChance(Players);
		break;
	case 8:
		value = calcScoreYazziBonus(Players);
		break;
	default:
		break;
		value = -1;
	}
	value = (value != -1) ? value : 0;
	// si on veut invalider le Yazzi il faut auparavant avoir invalide le YazziBonus
	g_printf("\nPlayers->set->count:%d\n", Players->set->count);
	if (value == 0 && numMark == 6 && Players->set->count <= DICE_SET_MAX_COUNT && Players->scoreArray->ptr_cell[16].enable == TRUE) //Yazzi select et yazziBonus libre
	{
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pRadioButtonFigure[5])))
		{
			g_printf("yazzi selectionné\n");
			int name = 0;
			while (name <= DICE_5) //deselectionne les des
			{
				if (!Players->set->dices[name].enable)
					_g_players_set_dices_is_enable(name);
				name++;
			}
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pRadioButtonFigure[7]), TRUE); //[7]Yazzi Bonus
		}
		if (Players->set->count == DICE_SET_MAX_COUNT)
			_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_COUNTER_MAX));
		else
			_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_INVALID_YAZZI));
		_g_display_alert_with_message(pWindowAlert, _("Disabled Yazzi Bonus before this!"));
	}
	else
	{
		if (Players->set->count == DICE_SET_MAX_COUNT)
			_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_COUNTER_MAX));
		else
			_g_mediator_widget_state(pWidget, GINT_TO_POINTER(YAZ_STATE_SCORE_SELECT));
	}
	_g_display_players_value_figure_score(numMark, value);
	// /* ---> DEBUG     <--- */
	// g_printf("eventRadioButtonFigure numMark: %d\n", numMark);
	// g_printf("Score crunch: %d\n", value);
	// /* ---> FIN DEBUG <--- */
}

/**
 * @brief Quitte le survol radiobutton apres evaluation sans validation des dés
 * @param pWidget
 * @param pData
 *
 *
 */
void OnLeaveRadioButtonDice(GtkWidget *pWidget, gpointer pData)
{
	/**
	 * @brief fonction vide
	 * 
	 * DUMMY
	 */
}

/**
 * @brief Au basculement du bouton radio Dice efface le score du radio bouton precedent
 * @param pWidget
 * @param pData
 *
 *
 */
void OnToggledRadioButtonDice(GtkWidget *pWidget, gpointer pData)
{
	if (GPOINTER_TO_INT(pData) > 0 && GPOINTER_TO_INT(pData) < 7)
		if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pWidget)))
			gtk_entry_set_text(GTK_ENTRY(pValueDice[GPOINTER_TO_INT(pData) - 1]), "0");
}

/**
 * @brief Au basculement du bouton radio Figure efface le score du radio bouton precedent
 * @param pWidget
 * @param pData
 *
 *
 */
void OnToggledRadioButtonFigure(GtkWidget *pWidget, gpointer pData)

{
	if (GPOINTER_TO_INT(pData) > 0 && GPOINTER_TO_INT(pData) < 9)
		if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pWidget)) && pData != 0)
			gtk_entry_set_text(GTK_ENTRY(pValueFigure[GPOINTER_TO_INT(pData) - 1]), "0");
}

/**
 * @brief quand la souris entre sur le Dé:
 * - change la forme du curseur
 * 
 * @param pWidget le dé en question
 * @param event enter-notify-event
 * @param pData indice du dé
 * @return gboolean 
 */
static gboolean
OnEnterDice(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
	if (gdk_event_get_event_type(event) == GDK_ENTER_NOTIFY)
	{
		GdkWindow *win = NULL;
		cursorDice = gdk_cursor_new_from_name(gdk_display_get_default(), "grabbing");
		win = gtk_widget_get_parent_window(pWidget);
		if (win != NULL && cursorDice != NULL)
		{
			gdk_window_set_cursor(win, cursorDice);
			// g_printf("OnEnterDice\n");
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * @brief quand la souris sort du Dé:
 * - change la forme du curseur
 * 
 * @param pWidget le dé en question
 * @param event enter-notify-event
 * @param pData indice du dé
 * @return gboolean 
 */
static gboolean
OnLeaveDice(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
	if (gdk_event_get_event_type(event) == GDK_LEAVE_NOTIFY)
	{
		GdkWindow *win = NULL;
		cursorDice = gdk_cursor_new_from_name(gdk_display_get_default(), "default");
		win = gtk_widget_get_parent_window(pWidget);
		if (win != NULL && cursorDice != NULL)
		{
			gdk_window_set_cursor(win, cursorDice);
			// g_printf("OnLeaveDice\n");
			return TRUE;
		}
	}
	return FALSE;
}
/**
 * @brief Quand la souris entre sur radiobutton
 * 
 * @param pWidget le radio button
 * @param event enter-notify-event
 * @param pData indice du radiobutton
 * @return gboolean 
 */
static gboolean
OnEnterRadioButtonScore(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
	if (gdk_event_get_event_type(event) == GDK_ENTER_NOTIFY)
	{
		GdkWindow *win = NULL;
		cursorRowScore = gdk_cursor_new_from_name(gdk_display_get_default(), "pointer");
		win = gtk_widget_get_parent_window(pWidget);
		if (win != NULL && cursorRowScore != NULL)
		{
			gdk_window_set_cursor(win, cursorRowScore);
			// g_printf("OnEnterRadioButtonScore\n");
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * @brief Quand la souris sort du radio button
 * 
 * @param pWidget le radiobutton en cours
 * @param event leave-notify-event
 * @param pData l'indice du radiobutton
 * @return gboolean 
 */
static gboolean
OnLeaveRadioButtonScore(GtkWidget *pWidget, GdkEvent *event, gpointer pData)
{
	if (gdk_event_get_event_type(event) == GDK_LEAVE_NOTIFY)
	{
		GdkWindow *win = NULL;
		cursorRowScore = gdk_cursor_new_from_name(gdk_display_get_default(), "default");
		win = gtk_widget_get_parent_window(pWidget);
		if (win != NULL && cursorRowScore != NULL)
		{
			gdk_window_set_cursor(win, cursorRowScore);
			// g_printf("OnLeaveRadioButtonScore\n");
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * @brief Ecrit le score des dès dans la zone de saisie
 * 
 * @param numMark indice de la gtk_entry
 * @param value   valeur à inscrire
 */
static void
_g_display_players_value_dice_score(const int numMark, const int value)
{
	gtk_entry_set_text(GTK_ENTRY(pValueDice[numMark - 1]), g_strdup_printf("%d", value));
}

/**
 * @brief Ecrit le score des figure dans la zone de saisie correspondant
 * 
 * @param numMark indice de la gtk_entry
 * @param value  valeur a inscrire
 */
static void
_g_display_players_value_figure_score(const int numMark, const int value)
{
	gtk_entry_set_text(GTK_ENTRY(pValueFigure[numMark - 1]), g_strdup_printf("%d", value));
}

/**
 * @brief Affiche le nombre de jet
 * @param count nombre de jet
 *
 *
 */
static void
_g_display_players_set_count(const int count)
{
	gchar *display;
	if (count)
		display = g_strdup_printf(_("Count : %d"), count);
	else
		display = g_strdup_printf(_("Count : %s"), _("none"));
	gtk_label_set_text(GTK_LABEL(pLabelCount), display);
	g_free(display);
}

/**
 * @brief Affiche le nom du joueur courant
 * @param count
 *
 *
 */
static void
_g_display_players_set_name(const char *name)
{
	gtk_label_set_text(GTK_LABEL(pLabelName), g_strdup_printf(_("Player : %s"), name));
}

/**
 * @brief met l'affichage a jour des scores intermediaires et finaux
 * @param self le joueur en cours
 *
 *
 */
static void
_g_display_players_update_score_all(Player *self)
{
	enum e_label
	{
		SubSum = 13,	   //13
		Bonus = 14,		   //14
		UpperSection = 15, //15
		LowerSection = 17, //17
		GrandTotal = 18	//18
	};
	gchar *display;
	gint minusBonus;

	display = g_strdup_printf("%-10s %2d", _(labelCrunching[0]), self->scoreArray->ptr_cell[SubSum].value);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[0]), display);

	minusBonus = self->scoreArray->ptr_cell[SubSum].value - YAZZI_MIN_BONUS - 1;
	display = g_strdup_printf("%-6s %+2d %2d", _(labelCrunching[1]), minusBonus, self->scoreArray->ptr_cell[Bonus].value);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[1]), display);

	display = g_strdup_printf("%-10s %2d", _(labelCrunching[2]), self->scoreArray->ptr_cell[UpperSection].value);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[2]), display);

	display = g_strdup_printf("%-10s %2d", _(labelCrunching[3]), self->scoreArray->ptr_cell[LowerSection].value);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[3]), display);

	display = g_strdup_printf("%s %d", _(labelCrunching[4]), self->scoreArray->ptr_cell[GrandTotal].value);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[4]), display);

	display = g_strdup_printf("%-20s%3d\n%-20s%3d", Player_1.name, Player_1.scoreArray->ptr_cell[GrandTotal].value, Player_2.name, Player_2.scoreArray->ptr_cell[GrandTotal].value);
	gtk_widget_set_tooltip_text(pLabelCrunching[4], display);

	g_free(display);
}

/**
 * @brief Affichage temporaire des scores avant validation
 * 
 * @param self 
 */
static void
_g_display_players_preliminary_score_all(Player *self)
{
	typedef struct s_resume_score
	{
		int subSum;
		int minusBonus;
		int bonus;
		int upperSection;
		int lowerSection;
		int grandTotal;

	} resumeScore, *pResumeScore;
	gchar *display;

	pResumeScore pScore = malloc(sizeof(resumeScore));
	if (pScore == NULL)
		exit(EXIT_FAILURE);
	pScore->subSum = 0;
	pScore->minusBonus = 0;
	pScore->bonus = 0;
	pScore->upperSection = 0;
	pScore->lowerSection = 0;
	pScore->grandTotal = 0;

	for (int i = 0; i <= DICE_NUMBER; i++)
	{
		pScore->subSum += atoi(gtk_entry_get_text(GTK_ENTRY(pValueDice[i])));
	}
	pScore->minusBonus = pScore->subSum - YAZZI_MIN_BONUS - 1;
	if (pScore->subSum > YAZZI_MIN_BONUS)
	{
		pScore->bonus = BONUS;
		pScore->upperSection = pScore->subSum + BONUS;
	}
	else
		pScore->upperSection = pScore->subSum;
	for (int i = 0; i < YAZ_NB_BOUTON_FIGURE; i++)
	{
		pScore->lowerSection += atoi(gtk_entry_get_text(GTK_ENTRY(pValueFigure[i])));
	}
	pScore->grandTotal = pScore->upperSection + pScore->lowerSection;
	display = g_strdup_printf("%-10s %2d", _(labelCrunching[0]), pScore->subSum);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[0]), display);
	display = g_strdup_printf("%-6s %+2d %2d", _(labelCrunching[1]), pScore->minusBonus, pScore->bonus);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[1]), display);
	display = g_strdup_printf("%-10s %2d", _(labelCrunching[2]), pScore->upperSection);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[2]), display);
	display = g_strdup_printf("%-10s %2d", _(labelCrunching[3]), pScore->lowerSection);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[3]), display);
	display = g_strdup_printf("%s %d", _(labelCrunching[4]), pScore->grandTotal);
	gtk_label_set_text(GTK_LABEL(pLabelCrunching[4]), display);
	g_free(display);
}
/**
 * @brief on remet a zero la derniere marque cliquée par l'evaluation pour les dés ou pour les figures
 * et efface la selection de la marque evaluée.
 * @brief ne fonctionne qu'au tour 2 car au tour 3 la validation totale est obligatoire
 * appelé par Roll
 * @param none
 */
static void
_g_display_reset_value_score_select()
{
	int numMark = 0;
	pLabel = NULL;
	pValue = NULL;
	if ((numMark = _g_radio_button_dice_num_active()) != -1)
	{
		pValue = pValueDice[numMark - 1];
		pLabel = pRadioButtonDice[numMark - 1];
	}
	else if ((numMark = _g_radio_button_figure_num_active()) != -1)
	{
		pValue = pValueFigure[numMark - 7];
		pLabel = pRadioButtonFigure[numMark - 7];
	}
	if (numMark != -1)
	{
		_g_display_players_widget_css(GTK_WIDGET(pLabel), "grey");
		gtk_entry_set_text(GTK_ENTRY(pValue), "0");
	}
	_g_display_players_preliminary_score_all(Players);
}

/**
 * @brief Affiche l'image du dé soit actif(selectionne) ou non
 * @param value valeur du dé
 * @param name  nom du dé
 * @param enable si actif ou pas
 *
 *
 */
static void
_g_display_players_dices_is_enable(const int value, const int name, const gboolean enable)
{
	image = GTK_IMAGE(pDice[name]);
	if (enable)
		gtk_image_set_from_file(GTK_IMAGE(image),
								g_strdup_printf("image/dice%d-64x64.png", value));
	else
		gtk_image_set_from_file(GTK_IMAGE(image),
								g_strdup_printf("image/dice%d-select64x64.png", value));
}

/**
 * @brief Met l'image de la valeur du dé selectionné/deselectionne
 * @param name nom du dé
 *
 *
 */
static void
_g_players_set_dices_is_enable(const int name)
{
	Players->set->dices[name].enable = !Players->set->dices[name].enable;
	_g_display_players_dices_is_enable(Players->set->dices[name].value, name, Players->set->dices[name].enable);
}

/**
 * @brief Met l'image de la valeur du dé à inconnu avant tirage 
 *        ainsi qu'une remise à zero du tirage.
 *
 */
static void
_g_players_set_dices_is_unknown()
{
	int i;
	for (i = 0; i < DICE_NUMBER; i++)
	{
		gtk_image_set_from_file(GTK_IMAGE(pDice[i]), g_strdup_printf("image/diceX-64x64.png"));
	}
	set_raz_value_face(Players);
}

/**
 * @brief Fixe l'etat des boutons actif/inactif
 * @param isState pointeur sur structure de l'etat des boutons
 *
 *
 */
static void
_g_button_set_state(stateButton *isState)
{
	int i;
	gchar *pStyle;

	/* pButtonX ::boutons bas */
	if (Players->set->count == DICE_SET_MAX_COUNT)
		gtk_widget_set_sensitive(pButtonRollAll, FALSE);
	else
		gtk_widget_set_sensitive(pButtonRollAll, isState->rollAll);

	gtk_widget_set_sensitive(pButtonRoll, isState->roll);
	gtk_widget_set_sensitive(pButtonValid, isState->valid);
	gtk_widget_set_sensitive(pButtonNextPlayer, isState->nextPlayer);
	gtk_widget_set_sensitive(pButtonQuit, isState->quit);

	/* pRadioButtonDice[X] ::boutons radio gauche*/
	for (i = 0; i <= DICE_NUMBER; i++)
	{
		if (_sheet_score_already(Players, i + 1)) //si cellule non validée (modifiable)
		{
			if (isState->radioButtonDice) //acces en ecriture (active)
			{
				pStyle = g_strdup_printf("dark");
			}
			else //interdite en ecriture (non active)
			{
				pStyle = g_strdup_printf("red");
			}
			if (isState->valid || Players->set->count < DICE_SET_MAX_COUNT) //Players->set->count < DICE_SET_MAX_COUNT
				gtk_widget_set_sensitive(GTK_WIDGET(pRadioButtonDice[i]), isState->radioButtonDice);
			else
				gtk_widget_set_sensitive(GTK_WIDGET(pRadioButtonDice[i]), FALSE);
		}
		else //si cellule validée (non modifiable)
		{
			if (isState->radioButtonDice) //etat interdite en ecriture
			{
				pStyle = g_strdup_printf("violet");
			}
			else // etat selectionné et interdite en ecriture (permet de voir la derniere validation)
			{
				pStyle = g_strdup_printf("yellow");
			}
			gtk_widget_set_sensitive(GTK_WIDGET(pRadioButtonDice[i]), FALSE);
		}
		_g_display_players_widget_css(pRadioButtonDice[i], pStyle);
	}

	/* pRadioButtonFigure[X] ::boutons radio droite*/
	for (i = 0; i < YAZ_NB_BOUTON_FIGURE; i++)
	{
		if (_sheet_score_already(Players, i + 7)) //si cellule non validée (modifiable)
		{
			if (isState->radioButtonFigure) //acces en ecriture (active)
			{
				pStyle = g_strdup_printf("dark");
			}
			else //interdite en ecriture (non active)
			{
				pStyle = g_strdup_printf("red");
			}
			if (isState->valid || Players->set->count < DICE_SET_MAX_COUNT) //Players->set->count < DICE_SET_MAX_COUNT
				gtk_widget_set_sensitive(GTK_WIDGET(pRadioButtonFigure[i]), isState->radioButtonFigure);
			else
				gtk_widget_set_sensitive(GTK_WIDGET(pRadioButtonFigure[i]), FALSE);
		}
		else //si cellule validée (non modifiable)
		{
			if (isState->radioButtonFigure) //etat interdite en ecriture
			{
				pStyle = g_strdup_printf("violet");
			}
			else // etat selectionné et interdite en ecriture (permet de voir la derniere validation)
			{
				pStyle = g_strdup_printf("yellow");
			}
			gtk_widget_set_sensitive(GTK_WIDGET(pRadioButtonFigure[i]), FALSE);
		}
		_g_display_players_widget_css(pRadioButtonFigure[i], pStyle);
	}
	/* eventBoxImageDice[X] ::Image des dés */
	for (i = 0; i < DICE_NUMBER; i++)
	{
		if (Players->set->count < DICE_SET_MAX_COUNT)
			gtk_widget_set_sensitive(GTK_WIDGET(eventBoxImageDice[i]), isState->boxImageDice);
		else
			gtk_widget_set_sensitive(GTK_WIDGET(eventBoxImageDice[i]), FALSE);
	}
}

/**
 * @brief mis a jour pour un joueur du CSS refletant l'etat des radiobutton
 * 
 * @param widget le radiobutton concerné
 * @param pData  la classe CSS à appliquer
 */
static void
_g_display_players_widget_css(GtkWidget *widget, gpointer pData)
{
	const char *pCSS[] = {"dark", "yellow", "violet", "red", "grey"};
	int sizeTabCSS = (int)(sizeof(pCSS) / sizeof(char *));
	gchar *className;
	for (int i = 0; i < sizeTabCSS; i++)
	{
		className = g_strdup_printf("%s", pCSS[i]);
		if (gtk_style_context_has_class(gtk_widget_get_style_context(widget), className))
			gtk_style_context_remove_class(gtk_widget_get_style_context(widget), className);
	}
	gtk_style_context_add_class(gtk_widget_get_style_context(widget), (char *)pData);
	gtk_widget_reset_style(widget);
}

/**
 * @brief mis a jour des cases entry au score du joueur
 * @param self le joueur en cours
 *
 *
 */
static void
_g_display_players_entries_text(Player *self)
{
	int i;
	for (i = 0; i < 6; i++) /* Colonne gauche */
		_g_display_players_value_dice_score(i + 1, self->scoreArray->ptr_cell[i].value);
	for (i = 6; i < 13; i++) /* Colonne droite */
		_g_display_players_value_figure_score(i - 5, self->scoreArray->ptr_cell[i].value);
	/*particularité du YazziBonus */
	_g_display_players_value_figure_score(8, self->scoreArray->ptr_cell[16].value);
}

/**
 * @brief cherche radio button dice actif
 * @returns retourne le numMark
 *
 *
 */
static int
_g_radio_button_dice_num_active()
{
	int i;
	// is left radio button ?
	for (i = 0; i < 6; i++)
	{
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pRadioButtonDice[i])))
			return i + 1;
	}
	return -1;
}

/**
 * @brief cherche radio button figure actif
 * @returns retourne le numMark
 *
 *
 */
static int
_g_radio_button_figure_num_active()
{
	int i;
	// is right radio button ?
	for (i = 0; i < 8; i++)
	{
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pRadioButtonFigure[i])))
			return i + 7;
	}
	return -1;
}

/**
 * @brief Affiche le... gagnant ou egal
 *
 *
 */
static void
_g_display_winner()
{
	Player *winner;
	GtkWidget *pShow;
	gchar *msg;
	winner = (Player *)malloc(1 * sizeof(Player));
	if (winner == NULL)
	{
		exit(EXIT_FAILURE);
	}
	statePlayer Player1Status = Player_1.status; // BEGIN_PLAYER || NEW_PLAYER
	statePlayer Player2Status = Player_2.status; // BEGIN_PLAYER || NEW_PLAYER
	winner = player_which_winner();
	if (winner->status == EQUAL_PLAYER)
	{
		winner = &Player_1;
		msg = g_strdup_printf(_("The two players are level!\n\n\t::: score   %3d points :::"),
							  winner->score->scoreGrandTotal); //
	}
	else
	{
		msg = g_strdup_printf(_("And the winner is %s!\n\n#\t%-20s\tscore:\t%03d points\n#\t%-20s\tscore:\t%03d points\n"),
							  winner->name, Player_1.name, Player_1.score->scoreGrandTotal,
							  Player_2.name, Player_2.score->scoreGrandTotal);
	}
	pShow = gtk_message_dialog_new(GTK_WINDOW(pWindowMain),
								   GTK_DIALOG_MODAL,
								   GTK_MESSAGE_INFO,
								   GTK_BUTTONS_CLOSE, "%s", msg); //ajout de "%s" evite warning securite gcc
	gtk_dialog_run(GTK_DIALOG(pShow));
	gtk_widget_destroy(pShow);
	/* recharge le status   BEGIN_PLAYER || NEW_PLAYER */
	Player_1.status = Player1Status;
	Player_2.status = Player2Status;
	g_free(msg);
}

/**
 * @brief affiche la demande de jouer a nouveau
 * @returns vrai/faux
 *
 *
 */
static gboolean
_g_display_play_again()
{
	GtkWidget *pShow;
	gboolean response;
	pShow = gtk_message_dialog_new(GTK_WINDOW(pWindowMain),
								   GTK_DIALOG_MODAL,
								   GTK_MESSAGE_QUESTION,
								   GTK_BUTTONS_YES_NO,
								   _("Play again ?"));
	response = (gtk_dialog_run(GTK_DIALOG(pShow)) == GTK_RESPONSE_YES) ? TRUE : FALSE;
	gtk_widget_destroy(pShow);
	return response;
}

/**
 * @brief Demande confirmation pour quitter
 * @returns vrai/faux
 *
 *
 */
static gboolean
_g_display_confirm_quit()
{
	GtkWidget *pShow;
	gboolean response;
	pShow = gtk_message_dialog_new(GTK_WINDOW(pWindowMain),
								   GTK_DIALOG_MODAL,
								   GTK_MESSAGE_QUESTION,
								   GTK_BUTTONS_YES_NO,
								   _("Confirm to quit ?"));
	response = (gtk_dialog_run(GTK_DIALOG(pShow)) == GTK_RESPONSE_YES) ? TRUE : FALSE;
	gtk_widget_destroy(pShow);
	return response;
}

/**
 * @brief Demande le nom des joueurs (autres que par défaut)
 *
 *
 */
void _g_display_players_set_all_names()
{
	GtkWidget *pContent_area = NULL;
	GtkWidget *pDialogSetUp = NULL;
	GtkWidget *pLabelPlayerName = NULL;
	GtkWidget *pEntryPlayer1Name = NULL;
	GtkWidget *pEntryPlayer2Name = NULL;
	const gchar *sPlayer1Name;
	const gchar *sPlayer2Name;

	pDialogSetUp = gtk_dialog_new_with_buttons(_(":::YaZZi setup:::"),
											   GTK_WINDOW(pWindowMain),
											   GTK_DIALOG_MODAL,
											   _("_OK"), GTK_RESPONSE_OK,
											   _("_Cancel"), GTK_RESPONSE_CANCEL,
											   NULL);

	pContent_area = gtk_dialog_get_content_area(GTK_DIALOG(pDialogSetUp));
	/* Label d'invite */
	pLabelPlayerName = gtk_label_new(_("Enter your player name"));
	gtk_box_pack_start(GTK_BOX(pContent_area), pLabelPlayerName, TRUE, FALSE, 10);
	/* Création de la zone de saisie */
	g_printf(_("\nPlayer_1: %s  \n"), Player_1.name);
	g_printf(_("Player_2: %s  \n"), Player_2.name);

	pEntryPlayer1Name = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(pEntryPlayer1Name), YAZ_ENTRY_NAME_MAX_LENGTH);
	gtk_entry_set_text(GTK_ENTRY(pEntryPlayer1Name), Player_1.name); //Player_1.name
	gtk_editable_set_position(GTK_EDITABLE(pEntryPlayer1Name), -1);

	pEntryPlayer2Name = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(pEntryPlayer2Name), YAZ_ENTRY_NAME_MAX_LENGTH);
	gtk_entry_set_text(GTK_ENTRY(pEntryPlayer2Name), Player_2.name); //Player_2.name
	gtk_editable_set_position(GTK_EDITABLE(pEntryPlayer2Name), -1);

	/* Insertion de la zone de saisie dans la boite de dialogue */
	/* Rappel : paramètre 1 de gtk_box_pack_start de type GtkBox */
	gtk_box_pack_start(GTK_BOX(pContent_area), GTK_WIDGET(pEntryPlayer1Name), TRUE, FALSE, 15);
	gtk_box_pack_start(GTK_BOX(pContent_area), GTK_WIDGET(pEntryPlayer2Name), TRUE, FALSE, 15);

	/* Affichage des éléments de la boite de dialogue et la centre */
	gtk_widget_show_all(GTK_WIDGET(pDialogSetUp));

	//gtk_window_set_position (GTK_WINDOW(pDialogSetUp), GTK_WIN_POS_CENTER);
	/* On lance la boite de dialogue et on récupére la réponse */
	switch (gtk_dialog_run(GTK_DIALOG(pDialogSetUp)))
	{
	/* L utilisateur valide */
	case GTK_RESPONSE_OK:
		sPlayer1Name = gtk_entry_get_text(GTK_ENTRY(pEntryPlayer1Name));
		sPlayer2Name = gtk_entry_get_text(GTK_ENTRY(pEntryPlayer2Name));
		player_set_name(&Player_1, g_strdup(sPlayer1Name));
		player_set_name(&Player_2, g_strdup(sPlayer2Name));
		_g_display_players_set_name((Players->id == Player_1.id) ? sPlayer1Name : sPlayer2Name);
		break;
	/* L utilisateur annule */
	case GTK_RESPONSE_CANCEL:
	case GTK_RESPONSE_NONE:
	default:
		//~ gtk_label_set_text(GTK_LABEL(pLabel), "Vous n'avez rien saisi !");
		break;
	}
	gtk_widget_destroy(pDialogSetUp);
}
