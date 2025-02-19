/*
 * Remplissage de triangles
 * Patrice Siracusa (1995)
 *
 * Fonctions de remplissage en gouraud-shading
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

/* sans Z-BUFFER */

/* avec Z-BUFFER */

/*  Variables globales */

/*  Variables locales-globales */

static long SwapTmp;
static short p;
static long xd,xf;
static long delta1,delta2;
static short l;
static long zc,dz;
static short pz;
static long zd,zf,dz1,dz2;
static long ideb,ifin,di1,di2;
static long pac,pab,pbc;

/* Extern */

extern char far ZBuffer[64000]; /* -128 <=z<=+127 */

/* Fonctions */

/*
 * Remplissage sans Z-BUFFER
 */

/*
 * Trace d'une ligne en gouraud-shading
 */

void lignehg_C(short xd,short ideb,short xf,short ifin,short l,unsigned char far * page)
{
 short p;
 long di,ic;
 short offp;

 /* clipping haut et bas */
 if(l<0 || l>199)
    return;

 /* gouraud */
 if(xd == xf)
   di = 0;
 else
   di = ((long)(ifin-ideb)<<LS)/(xf-xd);
 ic = (long)ideb<<LS;

 /* clipping droit */
 if(xf>319)
    xf=319;

 offp = l*320+xd;
 for(p=xd;p<=xf;p++,offp++)
 {
   if(p>=0) /* clipping gauche */
     page[offp] = ic>>RS;
   ic += di;
 }
}

void fillPoly3Gouraud_V2(long xa,long ya,long ia,long xb,long yb,long ib,long xc,long yc,long ic,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* gouraud */
    SWAP(ia,ib)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* gouraud */
    SWAP(ia,ic)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* gouraud */
    SWAP(ib,ic)
  }


  /* Remplissage */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* gouraud */
    di1 = ((ia-ic)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* gouraud */
    di1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* gouraud */
    di2 = ((ib-ia)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* gouraud */
    di2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* gouraud */
  ideb = ia<<LS;
  ifin = ideb;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      /* gouraud */
      ideb += di1;
      ifin += di2;

      lignehg((short)(xd>>RS),(short)(ideb>>RS),(short)(xf>>RS),(short)(ifin>>RS),l,page);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      /* gouraud */
      ideb += di1;
      ifin += di2;

      lignehg((short)(xf>>RS),(short)(ifin>>RS),(short)(xd>>RS),(short)(ideb>>RS),l,page);
    }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* gouraud */
    di2 = ((ib-ic)<<LS)/pbc;
    ifin = ib<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        /* gouraud */
        ideb += di1;
        ifin += di2;

        lignehg((short)(xd>>RS),(short)(ideb>>RS),(short)(xf>>RS),(short)(ifin>>RS),l,page);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        /* gouraud */
        ideb += di1;
        ifin += di2;

        lignehg((short)(xf>>RS),(short)(ifin>>RS),(short)(xd>>RS),(short)(ideb>>RS),l,page);
      }
    }
  }
}

/*
 * Remplissage avec Z-BUFFER
 */

/*
 * Trace d'une ligne en gouraud-shading en testant le z-buffer
 */

void lignehgz_C(short xd,short zd,short ideb,short xf,short zf,short ifin,short l,unsigned char far * page)
{
 short p;
 long di,ic;
 long zc,dz;
 short pz,dxfd;

 /* ligne dest trop haute ou trop basse */
 if(l<0 || l>199)
    return;


 /* gouraud */
 if(xd == xf)
   di = 0;
 else
 {
   dxfd = xf-xd;
   di = ((long)(ifin-ideb)<<LS)/dxfd;
 }
 ic = (long)ideb<<LS;

 /* z-buffer */
 if(xf==xd)
   dz=0;
 else
  dz = ((long)(zf-zd)<<LS)/dxfd;
 zc = (long)zd<<LS;

 /* clipping droit */
 if(xf>319)
    xf=319;

 pz = l*320+xd;
 for(p=xd;p<=xf;p++)
 {
   if((zc>>RS) > ZBuffer[pz])
   {
      if(p>=0)  /* clipping gauche */
        page[pz] = ic>>RS;
      ic += di; /* gouraud */

      /* z-buffer */
      ZBuffer[pz] = zc>>RS;
   }
   pz++;
   zc+=dz; /* z-buffer */
 }
}

void fillPoly3GouraudZ_V2(long xa,long ya,long za,long ia,long xb,long yb,long zb,long ib,long xc,long yc,long zc,long ic,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* z-buffer */
    SWAP(za,zb)
    /* gouraud */
    SWAP(ia,ib)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* z-buffer */
    SWAP(za,zc)
    /* gouraud */
    SWAP(ia,ic)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* z-buffer */
    SWAP(zb,zc)
    /* gouraud */
    SWAP(ib,ic)
  }


  /* Remplissage */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* gouraud */
    di1 = ((ia-ic)<<LS)/pac;
    /* z-buffer */
    dz1 = ((za-zc)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* gouraud */
    di1 = 0;
    /* z-buffer */
    dz1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* gouraud */
    di2 = ((ib-ia)<<LS)/pab;
    /* z-buffer */
    dz2 = ((zb-za)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* gouraud */
    di2 = 0;
    /* z-buffer */
    dz2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* gouraud */
  ideb = ia<<LS;
  ifin = ideb;

  /* z-buffer */
  zd = za<<LS;
  zf = zd;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      /* gouraud */
      ideb += di1;
      ifin += di2;
      /* z-buffer */
      zd += dz1;
      zf += dz2;

      lignehgz((short)(xd>>RS),(short)(zd>>RS),(short)(ideb>>RS),(short)(xf>>RS),(short)(zf>>RS),(short)(ifin>>RS),l,page);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      /* gouraud */
      ideb += di1;
      ifin += di2;
      /* z-buffer */
      zd += dz1;
      zf += dz2;

      lignehgz((short)(xf>>RS),(short)(zf>>RS),(short)(ifin>>RS),(short)(xd>>RS),(short)(zd>>RS),(short)(ideb>>RS),l,page);
    }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* gouraud */
    di2 = ((ib-ic)<<LS)/pbc;
    ifin = ib<<LS;

    /* z-buffer */
    dz2 = ((zb-zc)<<LS)/pbc;
    zf = zb<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        /* gouraud */
        ideb += di1;
        ifin += di2;
        /* z-buffer */
        zd += dz1;
        zf += dz2;

        lignehgz((short)(xd>>RS),(short)(zd>>RS),(short)(ideb>>RS),(short)(xf>>RS),(short)(zf>>RS),(short)(ifin>>RS),l,page);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        /* gouraud */
        ideb += di1;
        ifin += di2;
        /* z-buffer */
        zd += dz1;
        zf += dz2;

        lignehgz((short)(xf>>RS),(short)(zf>>RS),(short)(ifin>>RS),(short)(xd>>RS),(short)(zd>>RS),(short)(ideb>>RS),l,page);
      }
    }
  }
}

