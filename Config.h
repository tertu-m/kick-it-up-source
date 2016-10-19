#ifndef __INCLUDE_CONFIG__
#define	__INCLUDE_CONFIG__

#define MAX_LEGACY_SIZE 56U

#define KIUEDIT_MAGIC "KIUEdit"

#define	NORMAL		0
#define	FUSION		1

#define	DDR			0
#define	KOINS		1

typedef struct
{
	BOOL	bcDead;
	BOOL	auto1_1p;
	BOOL	auto3_1p;
	BOOL	auto5_1p;
	BOOL	auto7_1p;
	BOOL	auto9_1p;
	BOOL	auto1_2p;
	BOOL	auto3_2p;
	BOOL    auto5_2p;
	BOOL    auto7_2p;
	BOOL    auto9_2p;

	int		KeySet;
	BOOL	OptJudge;
	int		JoySet;

} KIUCONFIG;

void Configuration(void);
void CFGInitialize();
void CFGWrite();

#endif