/*************************************************************/
// TITRE  : AFFICHAGE D'UN TORE ET D'UN CUBE EN Z-BUFFER
// AUTEUR : Patrice SIRACUSA/wiSdom
// PROJET : EXE5.C, S3D3.C, S3DREAD.C, FILLMAP.C, FILLUNI.C
//          MATHS.C, R3D.ASM, FILLGS.C, FILLPG.C, VGA.C
//          VGA_ASM.ASM
/*************************************************************/

#include <conio.h>

#include "maths.h"
#include "vga.h"
#include "s3d3.h"

/* Recuperer la page de dessin */
extern unsigned char FAR EcranV1[65535];
/* Recuperer l'adresse video */
static bytef *Video = MK_FP(0xA000,0);
/* Recuperer la palette de couleurs */
extern unsigned char FAR Pal256[256*3];

/* Declarer les objet 3D */
obj3D_t *Objet1,*Objet2;

/* Declarer les param. globaux de la 3D */
param3D_t param3D;


/* Procedure permettant d'affecter une couleur a une face */
void initCoul(obj3D_t *obj)
{
  unsigned short f;
  for(f=0;f<obj->nbFaces;f++)
    obj->indPal[f] = 5+8*f;
}

void main(void)
{
  /* Passer en mode graphique */
  ModeVGA();

  /* On va remplir les faces de l'objet sans utiliser de mapping */
  param3D.render = no_mapping;
  /* On n'utilise aucune methode d'assombrissement */
  param3D.shading = none;
  /* On affiche les faces en utilisant le z-buffer */
  param3D.affMethod = z_buffer;

  /* init. la lib 3D */
  if(!initLib_3D(&param3D))
    return;

  /* lire 2 objets */
  if(!(Objet1 = readObjet_3D("tor.asc")))
    return;

  if(!(Objet2 = readObjet_3D("cube.asc")))
    return;

  zoom_3D(Objet1,0.5);
  zoom_3D(Objet2,2.);

  /* Initialiser les couleurs des faces */
  initCoul(Objet1);
  initCoul(Objet2);

  /* lire la palette */
  if(!loadPal256("deg.PAL"))
    return;

  while(!kbhit())
  {
    /* Faire tourner l'objet en angle relatif */
    relRotX_3D(Objet1,-4);
    relRotY_3D(Objet1,4);
    relRotZ_3D(Objet1,-4);

    relRotX_3D(Objet2,4);
    relRotY_3D(Objet2,-4);
    relRotZ_3D(Objet2,4);

    /* Afficher l'objet dans la page EcranV1 */
    affScene_3D(EcranV1);

    VBL;  /* Attendre un VBL */

    /* Copier a l'ecran */
    CpyScr(EcranV1,Video);

    /* Effacer la page EcranV1 */
    ClrScr(EcranV1);
  }

  /* Liberer la lib 3D */
  endLib_3D();

  /* Retour au mode texte */
  ModeTxt();
}
