/*
 * This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 * Distribution,  Product Version 5.7, and is subject to the Cadence LEF/DEF
 * Open Source License Agreement.   Your  continued  use  of this file
 * constitutes your acceptance of the terms of the LEF/DEF Open Source
 * License and an agreement to abide by its  terms.   If you  don't  agree
 * with  this, you must remove this and any other files which are part of the
 * distribution and  destroy any  copies made.
 * 
 * For updates, support, or to become part of the LEF/DEF Community, check
 * www.openeda.org for details.
 */
#include <string.h>
#include <stdlib.h>
#include "lex.h"
#include "defiSite.h"
#include "defiDebug.h"
#include "defiUtil.h"

/*
 * defiSite
 */ defiSite *
defiSite_CreateUninitialized()
{
  defiSite *this = (defiSite *) malloc(sizeof(defiSite));

  defiSite_Init(this);
  return (this);
}

void
defiSite_Delete(defiSite * this)
{
  if (this == NULL)
    return;
  defiSite_Destroy(this);
  free(this);
}

void 
defiSite_Init(defiSite * this)
{
  this->siteName_ = (char *) defMalloc(32);
  this->nameSize_ = 32;
  defiSite_clear(this);
}

void 
defiSite_Destroy(defiSite * this)
{
  defFree(this->siteName_);
}

void 
defiSite_clear(defiSite * this)
{
  if (this->siteName_)
    *this->siteName_ = '\0';
  this->x_num_ = 0.0;
  this->y_num_ = 0.0;
  this->x_step_ = 0.0;
  this->y_step_ = 0.0;
  this->orient_ = 0;
}

void 
defiSite_setName(defiSite * this,
		 const char *name)
{
  int     len = 1;

  char   *from = (char *) name;

  defiSite_clear(this);
  while (*from++)
    len++;
  if (this->nameSize_ < len)
    defiSite_bumpName(this,
		      len);
  strcpy(this->siteName_, DEFCASE(name));
}

void 
defiSite_setLocation(defiSite * this,
		     double xorg,
		     double yorg)
{
  this->x_orig_ = xorg;
  this->y_orig_ = yorg;
}

void 
defiSite_setOrient(defiSite * this,
		   int orient)
{
  this->orient_ = orient;
}

void 
defiSite_setDo(defiSite * this,
	       double x_num,
	       double y_num,
	       double x_step,
	       double y_step)
{
  this->x_num_ = x_num;
  this->y_num_ = y_num;
  this->x_step_ = x_step;
  this->y_step_ = y_step;
}

double 
defiSite_x_num(const defiSite * this)
{
  return this->x_num_;
}

double 
defiSite_y_num(const defiSite * this)
{
  return this->y_num_;
}

double 
defiSite_x_step(const defiSite * this)
{
  return this->x_step_;
}

double 
defiSite_y_step(const defiSite * this)
{
  return this->y_step_;
}

double 
defiSite_x_orig(const defiSite * this)
{
  return this->x_orig_;
}

double 
defiSite_y_orig(const defiSite * this)
{
  return this->y_orig_;
}

int 
defiSite_orient(const defiSite * this)
{
  return this->orient_;
}

const char *
defiSite_orientStr(const defiSite * this)
{
  return (defiOrientStr(this->orient_));
}

const char *
defiSite_name(const defiSite * this)
{
  return this->siteName_;
}

void 
defiSite_bumpName(defiSite * this,
		  int size)
{
  defFree(this->siteName_);
  this->siteName_ = (char *) defMalloc(size);
  this->nameSize_ = size;
  *this->siteName_ = '\0';
}

void 
defiSite_print(const defiSite * this,
	       FILE * f)
{
  fprintf(f, "Site '%p' %s\n", defiSite_name(this), defiSite_orientStr(this));
  fprintf(f, " DO X %g %g BY %g\n", defiSite_x_orig(this), defiSite_x_num(this), defiSite_x_step(this));
  fprintf(f, " DO Y %g %g BY %g\n", defiSite_y_orig(this), defiSite_y_num(this), defiSite_y_step(this));
}

/*
 * defiBox
 */ defiBox *
defiBox_CreateUninitialized()
{
  defiBox *this = (defiBox *) malloc(sizeof(defiBox));

  defiBox_Init(this);
  return (this);
}

void 
defiBox_Init(defiBox * this)
{
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->points_ = 0;
}

void
defiBox_Delete(defiBox * this)
{
  if (this == NULL)
    return;
  defiBox_Destroy(this);
  free(this);
}

void 
defiBox_Destroy(defiBox * this)
{
  struct defiPoints *p;

  p = this->points_;
  if (p) {
    defFree((char *) (p->x));
    defFree((char *) (p->y));
    defFree((char *) (this->points_));
  }
}

void 
defiBox_addPoint(defiBox * this,
		 defiGeometries * geom)
{
  struct defiPoints *p;

  struct defiPoints *tp;

  int     x, y;

  int     i;

  p = (struct defiPoints *) defMalloc(sizeof(struct defiPoints));
  p->numPoints = defiGeometries_numPoints(geom);
  p->x = (int *) defMalloc(sizeof(int) * p->numPoints);
  p->y = (int *) defMalloc(sizeof(int) * p->numPoints);
  for (i = 0;
       i < p->numPoints;
       i++) {
    defiGeometries_points(geom,
			  i,
			  &x,
			  &y);
    p->x[i] = x;
    p->y[i] = y;
    if (i == 0) {
      this->xl_ = x;
      this->yl_ = y;
    }
    else if (i == 1) {
      this->xh_ = x;
      this->yh_ = y;
    }
  }
  if (this->points_) {
    tp = this->points_;
    defFree((char *) (tp->x));
    defFree((char *) (tp->y));
    defFree((char *) (tp));
  }
  this->points_ = p;
}

int 
defiBox_xl(const defiBox * this)
{
  return this->xl_;
}

int 
defiBox_yl(const defiBox * this)
{
  return this->yl_;
}

int 
defiBox_xh(const defiBox * this)
{
  return this->xh_;
}

int 
defiBox_yh(const defiBox * this)
{
  return this->yh_;
}

struct defiPoints 
defiBox_getPoint(const defiBox * this)
{
  return *(this->points_);
}

void 
defiBox_print(const defiBox * this,
	      FILE * f)
{
  fprintf(f, "Box %d,%d %d %d\n", defiBox_xl(this), defiBox_yl(this), defiBox_xh(this), defiBox_yh(this));
}
