/*
 * Remplissage de triangles
 * Patrice Siracusa (1995)
 *
 * Fonctions de remplissage unicolore
 *
 * - sans z-buffer
 * - avec z-buffer
 *
 */

 /* Include */

#include "vga.h"
#include "maths.h"
#include "fill.h"

/* Defines & MACROS */

/*  Variables globales */

/*  Variables locales-globales */

static long SwapTmp,xd,xf,delta1,delta2,zc,dz,zd,zf,dz1,dz2,offp;
static short p,l,pz;
static unsigned char far *off,*ppage;
static char newz;
static char far *pZBuffer;
static long pac,pab,pbc;

/* Extern */

extern char far ZBuffer[64000]; /* -128 <=z<=+127 */

/* Fonctions */

/*
 * Remplissage sans Z-BUFFER
 */

/*
 * Trace d'une ligne avec une couleur donnee
 */

void ligneh_C(short xd,short xf,short l,unsigned char far * page,byte coul)
{
 if(l<0 || l>199)  return;
 if(xd<0)  xd=0;
 if(xf>319)  xf=319;
 offp = l*320L+xd;
 off = page+offp;
 for(p=xd;p<=xf;p++)  *(off++) = coul;
}

void fillPoly3_V2(long xa,long ya,long xb,long yb,long xc,long yc,unsigned char far * page,byte coul)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
  }


  /* Remplissage */

  if(ya!=yc)
    delta1 = ((xa-xc)<<LS)/(ya-yc);
  else
    delta1 = 0;

  if(ya!=yb)
    delta2 = ((xb-xa)<<LS)/(yb-ya);
  else
    delta2 = 0;

  xd = xa<<LS;
  xf = xd;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      ligneh((short)(xd>>RS),(short)(xf>>RS),(short)l,page,coul);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      ligneh((short)(xf>>RS),(short)(xd>>RS),(short)l,page,coul);
    }
  }


  if(yb!=yc)
  {
    delta2 = ((xb-xc)<<LS)/(yb-yc);
    xf = xb<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        ligneh((short)(xd>>RS),(short)(xf>>RS),(short)l,page,coul);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        ligneh((short)(xf>>RS),(short)(xd>>RS),(short)l,page,coul);
      }
    }
  }
}

/*
 * Remplissage avec Z-BUFFER
 */

/*
 * Trace d'une ligne avec une couleur donnee
 * en testant le z-buffer
 */

void lignehz_C(long xd,long zd,long xf,long zf,short l,unsigned char far * page,byte coul)
{

 if(l<0 || l>199)    return;
 if(xd<0)   xd=0;
 if(xf>319)  xf=319;
 if(xd>xf)   return;

 if(xf==xd)
   dz=0;
 else
   dz = ((zf-zd)<<LS)/(xf-xd);
 zc = zd<<LS;

 pz = l*320+xd;
 pZBuffer = ZBuffer + pz;
 ppage = page + pz;

 for(p=xd;p<=xf;p++)
 {
   newz = zc>>RS;
   if(newz > *pZBuffer)
   {
      *ppage = coul;
      *pZBuffer = newz;
   }
   pZBuffer++;
   ppage++;
   zc+=dz;
 }
}

void fillPoly3Z_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,unsigned char far * page,byte coul)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* z-buffer */
    SWAP(za,zb)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* z-buffer */
    SWAP(za,zc)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* z-buffer */
    SWAP(zb,zc)
  }


  /* Remplissage */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    dz1 = ((za-zc)<<LS)/pac; /* z-buffer */
  }
  else
  {
    delta1 = 0;
    dz1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    dz2 = ((zb-za)<<LS)/pab; /* z-buffer */
  }
  else
  {
    delta2 = 0;
    dz2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* z-buffer */
  zd = za<<LS;
  zf = zd;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      /* z-buffer */
      zd += dz1;
      zf += dz2;

      lignehz((xd>>RS),(zd>>RS),(xf>>RS),(zf>>RS),l,page,coul);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      /* z-buffer */
      zd += dz1;
      zf += dz2;

      lignehz((xf>>RS),(zf>>RS),(xd>>RS),(zd>>RS),l,page,coul);
    }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* z-buffer */
    dz2 = ((zb-zc)<<LS)/pbc;
    zf = zb<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        /* z-buffer */
        zd += dz1;
        zf += dz2;

        lignehz((xd>>RS),(zd>>RS),(xf>>RS),(zf>>RS),l,page,coul);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        /* z-buffer */
        zd += dz1;
        zf += dz2;

        lignehz((xf>>RS),(zf>>RS),(xd>>RS),(zd>>RS),l,page,coul);
      }
    }
  }
}
