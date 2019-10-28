#include "stdint.h"
#include "main.h"

#define ARED 0x42
#define AGRN 0x32
#define ABLU 0x52
#define BRED 0x7C
#define BGRN 0x6C
#define BBLU 0x8C
#define CRED 0x78
#define CGRN 0x68
#define CBLU 0x88
#define DRED 0x46
#define DGRN 0x36
#define DBLU 0x56
#define ERED 0x14
#define EGRN 0x04
#define EBLU 0x24
#define FRED 0x48
#define FGRN 0x38
#define FBLU 0x58
#define GRED 0x4A
#define GGRN 0x3A
#define GBLU 0x5A
#define HRED 0x4C
#define HGRN 0x3C
#define HBLU 0x5C
#define IRED 0x1E
#define IGRN 0x0E
#define IBLU 0x2E
#define JRED 0x4E
#define JGRN 0x3E
#define JBLU 0x5E
#define KRED 0x70
#define KGRN 0x60
#define KBLU 0x80
#define LRED 0xA2
#define LGRN 0x92
#define LBLU 0xB2
#define MRED 0xA0
#define MGRN 0x90
#define MBLU 0xB0
#define NRED 0x7E
#define NGRN 0x6E
#define NBLU 0x8E
#define ORED 0x40
#define OGRN 0x30
#define OBLU 0x50
#define PRED 0x72
#define PGRN 0x62
#define PBLU 0x82
#define QRED 0x10
#define QGRN 0x00
#define QBLU 0x20
#define RRED 0x16
#define RGRN 0x06
#define RBLU 0x26
#define SRED 0x44
#define SGRN 0x34
#define SBLU 0x54
#define TRED 0x18
#define TGRN 0x08
#define TBLU 0x28
#define URED 0x1C
#define UGRN 0x0C
#define UBLU 0x2C
#define VRED 0x7A
#define VGRN 0x6A
#define VBLU 0x8A
#define WRED 0x12
#define WGRN 0x02
#define WBLU 0x22
#define XRED 0x76
#define XGRN 0x66
#define XBLU 0x86
#define YRED 0x74
#define YGRN 0x64
#define YBLU 0x84
#define ZRED 0x1A
#define ZGRN 0x0A
#define ZBLU 0x2A



void lampboard_setLetter(char letter, RGB color);

void lampboard_setLamp(uint8_t lamp, RGB color);

void lampboard_fill(RGB color);

void lampboard_update();

void lampboard_clear();
