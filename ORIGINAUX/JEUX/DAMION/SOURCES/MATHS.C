#include "maths.h"

/* table de sinus et de cosinus */

static TYPESC  TSIN[360];
static TYPESC  TCOS[360];

void initSinCos(void)
{
short i;

/* 0 � 9 */
TSIN[0]=0;
TSIN[1]=0.017452406*COEFM;
TSIN[2]=0.034899497*COEFM;
TSIN[3]=0.052335956*COEFM;
TSIN[4]=0.069756474*COEFM;
TSIN[5]=0.087155743*COEFM;
TSIN[6]=0.104528463*COEFM;
TSIN[7]=0.121869343*COEFM;
TSIN[8]=0.139173101*COEFM;
TSIN[9]=0.156434465*COEFM;

/* 10 � 19 */
TSIN[10]=0.173648178*COEFM;
TSIN[11]=0.190808995*COEFM;
TSIN[12]=0.207911691*COEFM;
TSIN[13]=0.224951054*COEFM;
TSIN[14]=0.241921896*COEFM;
TSIN[15]=0.258819045*COEFM;
TSIN[16]=0.275637356*COEFM;
TSIN[17]=0.292371705*COEFM;
TSIN[18]=0.309016994*COEFM;
TSIN[19]=0.325568154*COEFM;

/* 20 � 29 */
TSIN[20]=0.342020143*COEFM;
TSIN[21]=0.358367950*COEFM;
TSIN[22]=0.374606593*COEFM;
TSIN[23]=0.390731128*COEFM;
TSIN[24]=0.406736643*COEFM;
TSIN[25]=0.422618262*COEFM;
TSIN[26]=0.438371147*COEFM;
TSIN[27]=0.453990500*COEFM;
TSIN[28]=0.469471563*COEFM;
TSIN[29]=0.484809620*COEFM;

/* 30 � 39 */
TSIN[30]=0.500000000*COEFM;
TSIN[31]=0.515038075*COEFM;
TSIN[32]=0.529919264*COEFM;
TSIN[33]=0.544639035*COEFM;
TSIN[34]=0.559192903*COEFM;
TSIN[35]=0.573576436*COEFM;
TSIN[36]=0.587785252*COEFM;
TSIN[37]=0.601815023*COEFM;
TSIN[38]=0.615661475*COEFM;
TSIN[39]=0.629320391*COEFM;

/* 40 � 49 */
TSIN[40]=0.642787610*COEFM;
TSIN[41]=0.656059029*COEFM;
TSIN[42]=0.669130606*COEFM;
TSIN[43]=0.681998360*COEFM;
TSIN[44]=0.694658370*COEFM;
TSIN[45]=0.707106781*COEFM;
TSIN[46]=0.719339800*COEFM;
TSIN[47]=0.731353702*COEFM;
TSIN[48]=0.743144825*COEFM;
TSIN[49]=0.754709580*COEFM;

/* 50 � 59 */
TSIN[50]=0.766044443*COEFM;
TSIN[51]=0.777145961*COEFM;
TSIN[52]=0.788010754*COEFM;
TSIN[53]=0.798635510*COEFM;
TSIN[54]=0.809016994*COEFM;
TSIN[55]=0.819152044*COEFM;
TSIN[56]=0.829037573*COEFM;
TSIN[57]=0.838670568*COEFM;
TSIN[58]=0.848048096*COEFM;
TSIN[59]=0.857167301*COEFM;

/* 60 � 69 */
TSIN[60]=0.866025404*COEFM;
TSIN[61]=0.874619707*COEFM;
TSIN[62]=0.882947593*COEFM;
TSIN[63]=0.891006524*COEFM;
TSIN[64]=0.898794046*COEFM;
TSIN[65]=0.906307787*COEFM;
TSIN[66]=0.913545458*COEFM;
TSIN[67]=0.920504853*COEFM;
TSIN[68]=0.927183855*COEFM;
TSIN[69]=0.933580426*COEFM;

/* 70 � 79 */
TSIN[70]=0.939692621*COEFM;
TSIN[71]=0.945518576*COEFM;
TSIN[72]=0.951056516*COEFM;
TSIN[73]=0.956304756*COEFM;
TSIN[74]=0.961261696*COEFM;
TSIN[75]=0.965925826*COEFM;
TSIN[76]=0.970295726*COEFM;
TSIN[77]=0.974370065*COEFM;
TSIN[78]=0.978147601*COEFM;
TSIN[79]=0.981627183*COEFM;

/* 80 � 89 */
TSIN[80]=0.984807753*COEFM;
TSIN[81]=0.987688341*COEFM;
TSIN[82]=0.990268069*COEFM;
TSIN[83]=0.992546152*COEFM;
TSIN[84]=0.994521895*COEFM;
TSIN[85]=0.996194698*COEFM;
TSIN[86]=0.997564050*COEFM;
TSIN[87]=0.998629535*COEFM;
TSIN[88]=0.999390827*COEFM;
TSIN[89]=0.999847695*COEFM;

/* 90 */
TSIN[90]=COEFM;

/* le reste */
for(i=1;i<=90;i++)
	TSIN[90+i]=TSIN[90-i];

for(i=1;i<=90;i++)
	TSIN[180+i]=-TSIN[180-i];

for(i=1;i<=89;i++)
	TSIN[270+i]=TSIN[270-i];

	for(i=0;i<=90;i++)
		TCOS[i]=TSIN[90-i];

	for(i=91;i<=180;i++)
		TCOS[i]=-TSIN[i-90];

	for(i=181;i<=270;i++)
		TCOS[i]=-TSIN[i-90];

	for(i=271;i<=359;i++)
		TCOS[i]=-TSIN[i-90];
}

TYPESC sinq(short angle)
{
 if(angle>=0)
		return TSIN[angle%360];
 else
		return -TSIN[-angle%360];
}

TYPESC cosq(short angle)
{
 if(angle>=0)
		return TCOS[angle%360];
 else
		return TCOS[-angle%360];
}
