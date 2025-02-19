/*************************************************************/
// TITRE   : BIBLIOTHEQUE GRAPHIQUE VGA STANDARD
// AUTEUR  : SIRACUSA WALTER/wiSdom
// FICHIER : VGA.C
/*************************************************************/

#include <vga.h>

#define BUF_PCX 100     //Taille du tampon (en octet)
byte _buf_pcx[BUF_PCX]; //Tampon pour LoadPCX() et SavePCX()
char _fch_pcx[45];      //Chemin des fichier PCX

void read_buf_pcx(int handle,byte *tampon,word *comp)
{
  if(*comp==BUF_PCX) //Tampon plein ?
  {
    *comp=0;
    read(handle,tampon,BUF_PCX);
  }
}

byte LoadPCX(char *path,bytef *scr,byte *pal)
{
  int canal;
  byte octet;
  word dim=64000,
       i,rep,cpt_buf=0;

  sprintf(_fch_pcx,"%s.pcx",path); //Ajoute l'extension .PCX
  canal=open(_fch_pcx,O_RDONLY|O_BINARY); //Ouverture du fichier en lecture

  if(canal!=-1)
  {
    //En-tete
    lseek(canal,128,SEEK_SET);

    //Image
    read(canal,_buf_pcx,BUF_PCX); //Remplit le tampon

    while(dim)
    {
      octet=_buf_pcx[cpt_buf++]; //Lecture du tampon : compteur ou encre ?
      read_buf_pcx(canal,_buf_pcx,&cpt_buf); //Remplit le tampon

      if(octet>191) // -> Compteur
      {
        rep=octet-192; //Nombre de r�p�tition de l'encre
        octet=_buf_pcx[cpt_buf++]; //Lecure du tampon : encre � r�p�ter
        read_buf_pcx(canal,_buf_pcx,&cpt_buf); //Remplit le tampon

        for(i=0;i<rep;i++) //R�p�tition de l'encre
        {
          *(scr++)=octet; //Ecriture de l'encre
          dim--;
        }
      }
      else          // -> Encre
      {
        *(scr++)=octet; //Ecriture de l'encre
        dim--;
      }
    }

    //Palette
    lseek(canal,-768L,SEEK_END);

    for(i=0;i<768;i++)
    {
      read(canal,&pal[i],1);
      pal[i]>>=2;
    }
    close(canal);
    return 1;
  }
  close(canal);
  return 0;
}

/*byte LoadPCX(char *path,bytef *scr,byte *pal)
{
  int canal;
  byte octet;
  word dim=64000,
       i,rep;

  sprintf(_fch_pcx,"%s.pcx",path); //Ajoute l'extension .PCX
  canal=open(_fch_pcx,O_RDONLY|O_BINARY); //Ouverture du fichier en lecture

  if(canal!=-1)
  {
    //En-tete
    lseek(canal,128,SEEK_SET);

    //Image
    while(dim)
    {
      read(canal,&octet,1); //Compteur ou encre ?
      if(octet>191) // -> Compteur
      {
        rep=octet-192; //Nombre de r�p�tition de l'encre
        read(canal,&octet,1); //Encre � r�p�ter
        for(i=0;i<rep;i++) //R�p�tition de l'encre
        {
          *(scr++)=octet; //Ecriture de l'encre
          dim--;
        }
      }
      else          // -> Encre
      {
        *(scr++)=octet; //Ecriture de l'encre
        dim--;
      }
    }

    //Palette
    lseek(canal,-768L,SEEK_END);

    for(i=0;i<768;i++)
    {
      read(canal,&pal[i],1);
      pal[i]>>=2;
    }
    close(canal);
    return 1;
  }
  close(canal);
  return 0;
}*/

void write_buf_pcx(int handle,byte *tampon,word *comp)
{
  if(*comp==BUF_PCX) //Tampon plein ?
  {
    *comp=0;
    write(handle,tampon,BUF_PCX);
  }
}

byte SavePCX(char *path,bytef *scr,byte *pal)
{
  int canal;
  byte octet;
  short entier;
  word dim=64000,i,
       ligne,rep,cpt_buf=0;

  sprintf(_fch_pcx,"%s.pcx",path); //Ajoute l'extension .PCX
  _fmode=O_BINARY;
  canal=creat(_fch_pcx,S_IREAD|S_IWRITE); //Ouverture du fichier en �criture

  if(canal!=-1)
  {
    //En-tete
    octet=10;write(canal,&octet,1);
    octet=5;write(canal,&octet,1);
    octet=1;write(canal,&octet,1);
    octet=8;write(canal,&octet,1);
    entier=0;write(canal,&entier,2);    //Xmin
    entier=0;write(canal,&entier,2);    //Ymin
    entier=319;write(canal,&entier,2);  //Xmax
    entier=199;write(canal,&entier,2);  //Ymax
    entier=320;write(canal,&entier,2);  //Largeur
    entier=200;write(canal,&entier,2);  //Hauteur
    entier=0;for(i=0;i<48/2;i++) write(canal,&entier,2);
    octet=0;write(canal,&octet,1);
    octet=1;write(canal,&octet,1);
    entier=320;write(canal,&entier,2);  //Nombre de pixels par ligne
    entier=1;write(canal,&entier,2);
    entier=0;for(i=0;i<58/2;i++) write(canal,&entier,2);

    //Image
    octet=*scr;
    rep=0;
    ligne=0;

    while(dim)
    {
      do
      {
        scr++;   //Encre suivante
        rep++;   //Nombre de repetition
        ligne++; //Traite un ligne
        dim--;
      }while(*scr==octet && rep<63 && ligne<320 && dim);

      if(ligne==320) ligne=0;

      if(rep==1)
      {
        if(octet<192)
        {
          _buf_pcx[cpt_buf++]=octet; //Ecriture de l'encre
          write_buf_pcx(canal,_buf_pcx,&cpt_buf);
        }
        else
        {
          _buf_pcx[cpt_buf++]=193; //Ecriture du nombre de repetition = 1
          write_buf_pcx(canal,_buf_pcx,&cpt_buf);

          _buf_pcx[cpt_buf++]=octet; //Ecriture de l'encre
          write_buf_pcx(canal,_buf_pcx,&cpt_buf);
        }
      }
      else
      {
        rep+=192;
        _buf_pcx[cpt_buf++]=rep; //Ecriture du nombre de repetition
        write_buf_pcx(canal,_buf_pcx,&cpt_buf);

        _buf_pcx[cpt_buf++]=octet; //Ecriture de l'encre
        write_buf_pcx(canal,_buf_pcx,&cpt_buf);
      }

      octet=*scr;
      rep=0;
    }

    if(cpt_buf>0) write(canal,_buf_pcx,cpt_buf);

    octet=0x0c;write(canal,&octet,1); //Octet de s�paration

    //Palette
    for(i=0;i<768;i++)
    {
      octet=pal[i];
      octet<<=2;
      write(canal,&octet,1);
    }
    close(canal);
    return 1;
  }
  close(canal);
  return 0;
}

/*byte SavePCX(char *path,bytef *scr,byte *pal)
{
  int canal;
  byte octet;
  short entier;
  word dim=64000,i,
       ligne,rep;

  sprintf(_fch_pcx,"%s.pcx",path); //Ajoute l'extension .PCX
  _fmode=O_BINARY;
  canal=creat(_fch_pcx,S_IREAD|S_IWRITE); //Ouverture du fichier en �criture

  if(canal!=-1)
  {
    //En-tete
    octet=10;write(canal,&octet,1);
    octet=5;write(canal,&octet,1);
    octet=1;write(canal,&octet,1);
    octet=8;write(canal,&octet,1);
    entier=0;write(canal,&entier,2);    //Xmin
    entier=0;write(canal,&entier,2);    //Ymin
    entier=319;write(canal,&entier,2);  //Xmax
    entier=199;write(canal,&entier,2);  //Ymax
    entier=320;write(canal,&entier,2);  //Largeur
    entier=200;write(canal,&entier,2);  //Hauteur
    entier=0;for(i=0;i<48/2;i++) write(canal,&entier,2);
    octet=0;write(canal,&octet,1);
    octet=1;write(canal,&octet,1);
    entier=320;write(canal,&entier,2);  //Nombre de pixels par ligne
    entier=1;write(canal,&entier,2);
    entier=0;for(i=0;i<58/2;i++) write(canal,&entier,2);

    //Image
    octet=*scr;
    rep=0;
    ligne=0;

    while(dim)
    {
      do
      {
        scr++;   //Encre suivante
        rep++;   //Nombre de repetition
        ligne++; //Traite un ligne
        dim--;
      }while(*scr==octet && rep<63 && ligne<320 && dim);

      if(ligne==320) ligne=0;

      if(rep==1)
      {
        if(octet<192)
          write(canal,&octet,1); //Ecriture de l'encre
        else
        {
          rep=193;
          write(canal,&rep,1);   //Ecriture du nombre de repetition = 1
          write(canal,&octet,1); //Ecriture de l'encre
        }
      }
      else
      {
        rep+=192;
        write(canal,&rep,1);   //Ecriture du nombre de repetition
        write(canal,&octet,1); //Ecriture de l'encre
      }

      octet=*scr;
      rep=0;
    }

    octet=0x0c;write(canal,&octet,1); //Octet de s�paration

    //Palette
    for(i=0;i<768;i++)
    {
      octet=pal[i];
      octet<<=2;
      write(canal,&octet,1);
    }
    close(canal);
    return 1;
  }
  close(canal);
  return 0;
}*/

void GetAllPal(byte *pal)
{
  register word i;

  for(i=0;i<256;i++)
  {
    outportb(0x3c7,i);
    *pal=inportb(0x3c9);
    *(pal+1)=inportb(0x3c9);
    *(pal+2)=inportb(0x3c9);
    pal+=3;
  }
}

void SetAllPal(byte *pal)
{
  register word i;

  for(i=0;i<256;i++)
  {
    outportb(0x3c8,i);
    outportb(0x3c9,*pal);
    outportb(0x3c9,*(pal+1));
    outportb(0x3c9,*(pal+2));
    pal+=3;
  }
}

byte GetPixel(short x,short y,bytef *scr)
{
  scr+=(y<<8)+(y<<6)+x;  //src = src+y*320+x
  return *scr;
}

void PutPixel(short x,short y,bytef *scr,byte e)
{
  scr+=(y<<8)+(y<<6)+x;
  *scr=e;
}

void Box(short x,short y,short l,short h,bytef *scr,byte e)
{
  register word i,off1,off2,off3,off4;

  l--; //Largeur > 0
  h--; //Hauteur > 0

  off1=(y<<8)+(y<<6)+x;
  off2=off1+l;
  for(i=off1;i<off2;i++) scr[i]=e;

  off3=(h<<8)+(h<<6)+off1;
  off4=off3+l;
  for(i=off3;i<off4;i++) scr[i]=e;

  for(i=off1;i<off3;i+=320) scr[i]=e; //Colonne gauche

  for(i=off2;i<=off4;i+=320) scr[i]=e; //Colonne droite
}

void FBox(short x,short y,word l,word h,bytef *scr,byte e)
{
  register word i,j,off1,off2;

  off1=(y<<8)+(y<<6)+x;
  off2=off1+l;

  for(j=0;j<h;j++)
  {
    for(i=off1;i<off2;i++) scr[i]=e;
    off1+=320;
    off2+=320;
  }
}

static char signe(short v1,short v2)
{
  if(v1<v2)
    return 1;
  else
  {
    if(v1>v2)
      return -1;
    else
      return 0;
  }
}

void Line(short x1,short y1,short x2,short y2,bytef *scr,byte e)
{
  register word i;
  char s1,s2,ech,sgn;
  short x=x1,y=y1,dx,dy,tmp,v;

  scr[y1*320+x1]=e; //Premier point
  scr[y2*320+x2]=e; //Deuxieme point

  dx=abs(x2-x1);
  dy=abs(y2-y1);
  s1=signe(x1,x2);
  s2=signe(y1,y2);

  if(dy>dx)
  {
    tmp=dx;
    dx=dy;
    dy=tmp;
    ech=1;
  }
  else ech=0;

  v=2*dy-dx;

  for(i=1;i<=dx;i++) //Algorithme de Bresenham
  {
    while(v>=0)
    {
      if(ech) x+=s1;
      else y+=s2;
      v=v-2*dx;
    }
    if(ech) y+=s2;
    else x+=s1;
    v=v+2*dy;

    *(scr+(y<<8)+(y<<6)+x)=e;
  }
}

void Circle(short x,short y,word r,bytef *scr,byte e)
{
  short xi,yi,di,limite,eta,etap;

  xi=0; yi=r; di=2-2*r;   //Algorithme de Bresenham
  e1:
    scr[(y-yi)*320+(x+xi)]=e;  //1er cadran
    scr[(y-yi)*320+(x-xi)]=e;  //2ie cadran
    scr[(y+yi)*320+(x-xi)]=e;  //3ie cadran
    scr[(y+yi)*320+(x+xi)]=e;  //4ie cadran

    if(yi<=0) goto e4;
    if(di<0) goto e2;
    if(di>0) goto e3;
    if(di==0) goto e20;
  e2:
    eta=2*di+2*yi-1;
    if(eta<=0) goto e10;
    if(eta>0) goto e20;
  e3:
    etap=2*di-2*xi-1;
    if(etap<=0) goto e20;
    if(etap>0) goto e30;
  e10:
    xi++;
    di=di+2*xi+1;
    goto e1;
  e20:
    xi++;
    yi--;
    di=di+2*xi-2*yi+2;
    goto e1;
  e30:
    yi--;
    di=di-2*yi+1;
    goto e1;
  e4:;
}
