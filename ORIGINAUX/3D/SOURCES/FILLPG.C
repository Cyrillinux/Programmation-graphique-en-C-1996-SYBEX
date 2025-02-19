/*
 * Remplissage de triangles
 * Patrice Siracusa (1995)
 *
 * Fonctions de remplissage en phong-shading
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
static long xideb,xifin,yideb,yifin,dxi1,dxi2,dyi1,dyi2;
static long pac,pab,pbc;

/* Extern */

extern char far ZBuffer[64000]; /* -128 <=z<=+127 */
extern long RefXa,RefYa,RefXb,RefYb,RefXc,RefYc;
extern unsigned char TPhongXY[65535];

/* Fonctions */

/*
 * Remplissage sans Z-BUFFER
 */

/*
 * Trace d'une ligne en phong-shading
 */

void lignehp_C(short xd,short xid,short yid,short xf,short xif,short yif,short l,unsigned char far * page)
{
 short p,offp,dxfd;
 long  xdi,ydi,xic,yic,el,x,y;

 /* clipping haut et bas */
 if(l<0 || l>199)
    return;

 /* phong */
 if(xd == xf)
 {
   xdi = 0;
   ydi = 0;
 }
 else
 {
   dxfd = xf-xd;
   xdi = ((long)(xif-xid)<<LS)/dxfd;
   ydi = ((long)(yif-yid)<<LS)/dxfd;
 }
 xic = (long)xid<<LS;
 yic = (long)yid<<LS;

 /* clipping droit */
 if(xf>319)
    xf=319;

 offp = l*320+xd;

 for(p=xd;p<=xf;p++,offp++)
 {
   if(p>=0) /* clipping gauche */
   {
     /* L'utilisation du tableau TPhongXY[] permet d'eviter
      * le calcul suivant pour chaque pixel :
      * r = 127 - (x*x) - (y*y)
      * page[offp] = sqrt(r)*255.
      */
     x = xic>>RS;
     y = yic>>RS;
     /* developpement de : el = ((y+127)*255L)+(x+127); */
     el = (y<<8)-y+x+32512;
     page[offp] = TPhongXY[el];
   }
   xic += xdi;
   yic += ydi;
 }
}

void fillPoly3Phong_V2(long xa,long ya,long xb,long yb,long xc,long yc,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* phong */
    SWAP(RefXa,RefXb)
    SWAP(RefYa,RefYb)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* phong */
    SWAP(RefXa,RefXc)
    SWAP(RefYa,RefYc)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* phong */
    SWAP(RefXb,RefXc)
    SWAP(RefYb,RefYc)
  }


  /* Remplissage */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* phong */
    dxi1 = ((RefXa-RefXc)<<LS)/pac;
    dyi1 = ((RefYa-RefYc)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* phong */
    dxi1 = 0;
    dyi1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* phong */
    dxi2 = ((RefXb-RefXa)<<LS)/pab;
    dyi2 = ((RefYb-RefYa)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* phong */
    dxi2 = 0;
    dyi2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* phong */
  xideb = RefXa<<LS;
  xifin = xideb;
  yideb = RefYa<<LS;
  yifin = yideb;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      /* phong */
      xideb += dxi1;
      xifin += dxi2;
      yideb += dyi1;
      yifin += dyi2;

      lignehp((short)(xd>>RS),(short)(xideb>>RS),(short)(yideb>>RS),(short)(xf>>RS),(short)(xifin>>RS),(short)(yifin>>RS),l,page);
    }
  }
  else
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      /* phong */
      xideb += dxi1;
      xifin += dxi2;
      yideb += dyi1;
      yifin += dyi2;

      lignehp((short)(xf>>RS),(short)(xifin>>RS),(short)(yifin>>RS),(short)(xd>>RS),(short)(xideb>>RS),(short)(yideb>>RS),l,page);
    }
  }

  if(yb!=yc)
  {
    pbc = yb-yc;
    delta2 = ((xb-xc)<<LS)/pbc;
    xf = xb<<LS;

    /* phong */
    dxi2 = ((RefXb-RefXc)<<LS)/pbc;
    dyi2 = ((RefYb-RefYc)<<LS)/pbc;
    xifin = RefXb<<LS;
    yifin = RefYb<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        /* phong */
        xideb += dxi1;
        xifin += dxi2;
        yideb += dyi1;
         yifin += dyi2;

        lignehp((short)(xd>>RS),(short)(xideb>>RS),(short)(yideb>>RS),(short)(xf>>RS),(short)(xifin>>RS),(short)(yifin>>RS),l,page);
      }
    }
    else
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        /* phong */
        xideb += dxi1;
        xifin += dxi2;
        yideb += dyi1;
         yifin += dyi2;

        lignehp((short)(xf>>RS),(short)(xifin>>RS),(short)(yifin>>RS),(short)(xd>>RS),(short)(xideb>>RS),(short)(yideb>>RS),l,page);
      }
    }
  }
}

/* AVEC Z-BUFFER */

/*
 * Trace d'une ligne en Phong-Shading + Z-Buffer
 */

void lignehpz_C(short xd,short zd,short xid,short yid,short xf,short zf,short xif,short yif,short l,unsigned char far * page)
{
 short p,offp,dxfd;
 long  xdi,ydi,xic,yic,el,x,y;

 /* clipping haut et bas */
 if(l<0 || l>199)
    return;

 /* phong */
 if(xd == xf)
 {
   xdi = 0;
   ydi = 0;
 }
 else
 {
   dxfd = xf-xd;
   xdi = ((long)(xif-xid)<<LS)/dxfd;
   ydi = ((long)(yif-yid)<<LS)/dxfd;
 }
 xic = (long)xid<<LS;
 yic = (long)yid<<LS;

 /* z-buffer */
 if(xf==xd)
   dz=0;
 else
  dz = ((long)(zf-zd)<<LS)/dxfd;
 zc = (long)zd<<LS;

 /* clipping droit */
 if(xf>319)
    xf=319;

 offp = l*320+xd;
 for(p=xd;p<=xf;p++,offp++)
 {
   if(p>=0) /* clipping gauche */
   {
     if((zc>>RS) > ZBuffer[offp])
     {
       /* L'utilisation du tableau TPhongXY[] permet d'eviter
        * le calcul suivant pour chaque pixel :
        * r = 127 - (x*x) - (y*y)
        * page[offp] = sqrt(r)*255.
        */
        x = xic>>RS;
        y = yic>>RS;
        /* dev. de : el = ((y+127)*255L)+(x+127); */
        el = (y<<8)-y+x+32512;
        page[offp] = TPhongXY[el];
        /* z-buffer */
        ZBuffer[offp] = zc>>RS;
     }
   }
   xic += xdi;
   yic += ydi;
   zc  += dz; /* z-buffer */
 }
}

void fillPoly3PhongZ_V2(long xa,long ya,long za,long xb,long yb,long zb,long xc,long yc,long zc,unsigned char far * page)
{
  /* Trie des trois points de haut en bas */

  if(ya>yb)
  {
    SWAP(xa,xb)
    SWAP(ya,yb)
    /* z-buffer */
    SWAP(za,zb)
    /* phong */
    SWAP(RefXa,RefXb)
    SWAP(RefYa,RefYb)
  }

  if(ya>yc)
  {
    SWAP(xa,xc)
    SWAP(ya,yc)
    /* z-buffer */
    SWAP(za,zc)
    /* phong */
    SWAP(RefXa,RefXc)
    SWAP(RefYa,RefYc)
  }

  if(yb>yc)
  {
    SWAP(xb,xc)
    SWAP(yb,yc)
    /* z-buffer */
    SWAP(zb,zc)
    /* phong */
    SWAP(RefXb,RefXc)
    SWAP(RefYb,RefYc)
  }


  /* Remplissage */

  if(ya!=yc)
  {
    pac = ya-yc;
    delta1 = ((xa-xc)<<LS)/pac;
    /* z-buffer */
    dz1 = ((za-zc)<<LS)/pac;
    /* phong */
    dxi1 = ((RefXa-RefXc)<<LS)/pac;
    dyi1 = ((RefYa-RefYc)<<LS)/pac;
  }
  else
  {
    delta1 = 0;
    /* z-buffer */
    dz1 = 0;
    /* phong */
    dxi1 = 0;
    dyi1 = 0;
  }

  if(ya!=yb)
  {
    pab = yb-ya;
    delta2 = ((xb-xa)<<LS)/pab;
    /* z-buffer */
    dz2 = ((zb-za)<<LS)/pab;
    /* phong */
    dxi2 = ((RefXb-RefXa)<<LS)/pab;
    dyi2 = ((RefYb-RefYa)<<LS)/pab;
  }
  else
  {
    delta2 = 0;
    /* z-buffer */
    dz2 = 0;
    /* phong */
    dxi2 = 0;
    dyi2 = 0;
  }

  xd = xa<<LS;
  xf = xd;

  /* z-buffer */
  zd = za<<LS;
  zf = zd;

  /* phong */
  xideb = RefXa<<LS;
  xifin = xideb;
  yideb = RefYa<<LS;
  yifin = yideb;

  if(delta1<delta2)
  {
    for(l = ya+1; l <= yb; l++)
    {
      xd += delta1;
      xf += delta2;
      /* z-buffer */
      zd += dz1;
      zf += dz2;
      /* phong */
      xideb += dxi1;
      xifin += dxi2;
      yideb += dyi1;
      yifin += dyi2;

      lignehpz((short)(xd>>RS),(short)(zd>>RS),(short)(xideb>>RS),(short)(yideb>>RS),(short)(xf>>RS),(short)(zf>>RS),(short)(xifin>>RS),(short)(yifin>>RS),l,page);
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
      /* phong */
      xideb += dxi1;
      xifin += dxi2;
      yideb += dyi1;
      yifin += dyi2;

      lignehpz((short)(xf>>RS),(short)(zf>>RS),(short)(xifin>>RS),(short)(yifin>>RS),(short)(xd>>RS),(short)(zd>>RS),(short)(xideb>>RS),(short)(yideb>>RS),l,page);
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

    /* phong */
    dxi2 = ((RefXb-RefXc)<<LS)/pbc;
    dyi2 = ((RefYb-RefYc)<<LS)/pbc;
    xifin = RefXb<<LS;
    yifin = RefYb<<LS;

    if(delta2<delta1)
    {
      for(l = yb+1; l <= yc; l++)
      {
        xd += delta1;
        xf += delta2;
        /* z-buffer */
        zd += dz1;
        zf += dz2;
        /* phong */
        xideb += dxi1;
        xifin += dxi2;
        yideb += dyi1;
        yifin += dyi2;

        lignehpz((short)(xd>>RS),(short)(zd>>RS),(short)(xideb>>RS),(short)(yideb>>RS),(short)(xf>>RS),(short)(zf>>RS),(short)(xifin>>RS),(short)(yifin>>RS),l,page);
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
        /* phong */
        xideb += dxi1;
        xifin += dxi2;
        yideb += dyi1;
        yifin += dyi2;

        lignehpz((short)(xf>>RS),(short)(zf>>RS),(short)(xifin>>RS),(short)(yifin>>RS),(short)(xd>>RS),(short)(zd>>RS),(short)(xideb>>RS),(short)(yideb>>RS),l,page);
      }
    }
  }
}
