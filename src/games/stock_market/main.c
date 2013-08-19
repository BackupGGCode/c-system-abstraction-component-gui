
#include <stdhdrs.h>
#include <stdio.h>
#include <logging.h>
#include <sharemem.h>
#include <sack_types.h>

#include "global.h"
#include "stocks.h"
#include "board.h"
#include "player.h"
#include "input.h"

void CPROC ButtonRollDice( PTRSZVAL psv, PCONTROL pc )
{
	char number[4];
	if( psv )
		snprintf( number, sizeof( number ), "%ld\n", psv );
	else
		snprintf( number, sizeof( number ), "0" );
	EnqueStrokes( number );
}

int Init( void )
{
	int players, n;
	g.Players = CreateList();
	g.Board = CreateList();
	g.scale = 28; // assuming 800x600 at least.
	g.pImg = GetImageInterface();
	g.pRend = GetDisplayInterface();
	ReadStockDefinitions( "Stocks.Data" );
	ReadBoardDefinitions( "Board.Data" );
	if( !g.board )
		return FALSE;
	if( !g.flags.bRandomRoll )
	{
		AddSheet( g.Panel, g.RollDice = CreateFrame( "Dice Cup"
																 , 0, 0
																 , g.PanelWidth, g.PanelHeight
																 , BORDER_NOCAPTION|BORDER_WITHIN|BORDER_NONE, NULL ) );
		SetControlID( g.RollDice, PANEL_ROLL );
		DisableSheet( g.Panel, PANEL_ROLL, FALSE );
		MakeButton( g.RollDice, 5, 5, 50, 20, -1, "2", 0, ButtonRollDice, 2 );
		MakeButton( g.RollDice, 5, 27, 50, 20, -1, "3", 0, ButtonRollDice, 3 );
		MakeButton( g.RollDice, 5, 49, 50, 20, -1, "4", 0, ButtonRollDice, 4 );
		MakeButton( g.RollDice, 5, 71, 50, 20, -1, "5", 0, ButtonRollDice, 5 );
		MakeButton( g.RollDice, 5, 93, 50, 20, -1, "6", 0, ButtonRollDice, 6 );
		MakeButton( g.RollDice, 5, 115, 50, 20, -1, "7", 0, ButtonRollDice, 7 );
		MakeButton( g.RollDice, 57, 5, 50, 20, -1, "8", 0, ButtonRollDice, 8 );
		MakeButton( g.RollDice, 57, 27, 50, 20, -1, "9", 0, ButtonRollDice, 9 );
		MakeButton( g.RollDice, 57, 49, 50, 20, -1, "10", 0, ButtonRollDice, 10 );
		MakeButton( g.RollDice, 57, 71, 50, 20, -1, "11", 0, ButtonRollDice, 11 );
		MakeButton( g.RollDice, 57,  93, 50, 20, -1, "12", 0, ButtonRollDice, 12 );
		MakeButton( g.RollDice, 57, 115, 50, 20, -1, "Random", 0, ButtonRollDice, 0 );
	}
	InitPlayer();
	InitStockDialogs();

	//UpdateControl( g.board );
	do
	{
		printf( "How many players? " );
		players = GetANumber();
	} while( players < 2 || players > 8 );

	for( n = 0; n < players; n++ )
	{
		PPLAYER pPlayer;
		AddLink( &g.Players, pPlayer = New( PLAYER ) );
		pPlayer->nHistory = 0;
		pPlayer->nHistoryAvail = 0;
		pPlayer->History = NULL;
		pPlayer->me = NULL;
		pPlayer->next = NULL;
		pPlayer->pPlayerToken = NULL;
		pPlayer->id = n + 1;
		pPlayer->portfolio = NULL;
		pPlayer->Cash = 0;
		pPlayer->NetValue = 0;
		pPlayer->pCurrentSpace = NULL;
		ChoosePlayerColor( pPlayer );
		g.pCurrentPlayer = pPlayer;
	}

	return 1;
}


int Deinit( void )
{
    INDEX idx;
    PPLAYER pPlayer;
    LIST_FORALL( g.Players, idx, PPLAYER, pPlayer )
    {
        Release( pPlayer );
    }
    ResetPlayers();
	return 1;
}


void setattr( int attr )
{
#ifndef __LINUX__
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), attr );
#endif
}

void EnterDiceRoll( void )
{
	int accum;
	if( !g.flags.bRandomRoll )
	{
		printf( "Enter Die Roll (0 = roll):" );
		accum = GetANumber();
	}
	else
		accum = 0;
    if( !accum )
    {
		int roll1, roll2;
		roll1 = ( ( rand() >> 8 ) % 6 ) + 1;
		roll2 = ( ( rand() >> 8 ) % 6 ) + 1;
		accum = roll1 + roll2;
		printf( "Automatic roll: %d (%d + %d)\n", accum, roll1, roll2 );
	}
	g.nCurrentRoll = accum;
}




int main( int argc, char **argv )
{
	int first;
	//SetSystemLog( SYSLOG_FILE, fopen( "stockmarket.log", "wt" ) );
	if( argc > 1 )
		SetInputFile( argv[1] );
	if( argc > 2 )
		SetOutputFile( argv[2] );
	//printf( "Shall we play a game?\n" );

	if( Init() )
	{
		first = 1;
		SetFirstPlayer();
		ChoosePlayerProfessions();
		while( 1 )
		{
			EvaluatePlayerValue( TRUE );
			ShowCurrentPlayer();
			if( !first )
				ChoosePlayerProfessions();

			AllowCurrentPlayerSell();

			EnterDiceRoll();
			PayWageSlaves();
			ChoosePlayerDestination();

			ProcessCurrentSpace();

			EvaluatePlayerValue( FALSE );
			first = 0;
			StepNextPlayer();
		}
	}
	return 0;
}
