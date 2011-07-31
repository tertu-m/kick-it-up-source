//Code for handling players.
#ifndef PLAYER_H
#define PLAYER_H

enum Player{
	Player_1,
	Player_2,
	Player_None
};

struct PlayerOptions{
	bool bModeMirror;
	bool bModeNonstep;
	bool bModeUnion;
	bool bModeRandom;
	bool bMode4D;
	bool bModeVanish;
	bool bModeSuddenR;
	bool bModeRandomS;
	int iHighSpeed;
	int iHighSpeed4D[5];
};

struct PlayerStats{
	int iLife;
	long iScore;
	unsigned long iCombo;
	unsigned long iMaxCombo;	
	unsigned long iJudgments[5];
	unsigned long iCurMaxCombo;
};

struct PlayerInfo{
	Player player;
	PlayerStats stats;
	PlayerOptions options;
};

//and now constructors and Clear operators.

void PlayerOptions::Clear()
{
	this->bModeMirror = false;
	this->bModeNonstep = false;
	this->bModeUnion = false;
	this->bModeRandom = false;
	this->bMode4D = false;
	this->bModeVanish = false;
	this->bModeSuddenR = false;
	this->bModeRandomS = false;
	this->iHighSpeed = 1;
	this->iHighSpeed4D = [1,1,1,1,1];
}

PlayerOptions(){ this->Clear(); }

void PlayerStats::Clear()
{
	this->iLife = 10;
	this->iScore = 0;
	this->iCombo = 0;
	this->iMaxCombo = 0;
	this->iJudgments = [0,0,0,0,0];
	this->iCurMaxCombo = 0;
}

PlayerStats(){ this->Clear(); }

void PlayerInfo::Clear()
{
	this->player = Player_None;
	this->stats.Clear();
	this->options.Clear();
}

PlayerInfo(){ this->Clear(); }

#endif
