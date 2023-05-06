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
#include "defiRegion.h"
#include "defiDebug.h"

/*
 * defiRegion
 */ defiRegion *
defiRegion_CreateUninitialized()
{
  defiRegion *this = (defiRegion *) malloc(sizeof(defiRegion));

  defiRegion_Init(this);
  return (this);
}

void 
defiRegion_Init(defiRegion * this)
{
  this->name_ = 0;
  this->nameLength_ = 0;
  this->type_ = 0;
  this->xl_ = 0;
  this->yl_ = 0;
  this->xh_ = 0;
  this->yh_ = 0;
  this->numProps_ = 0;
  this->propsAllocated_ = 2;
  this->propNames_ = (char **) defMalloc(sizeof(char *) * 2);
  this->propValues_ = (char **) defMalloc(sizeof(char *) * 2);
  this->propDValues_ = (double *) defMalloc(sizeof(double) * 2);
  this->propTypes_ = (char *) defMalloc(sizeof(char) * 2);
  defiRegion_clear(this);
  this->numRectangles_ = 0;
  this->rectanglesAllocated_ = 1;
  this->xl_ = (int *) defMalloc(sizeof(int) * 1);
  this->yl_ = (int *) defMalloc(sizeof(int) * 1);
  this->xh_ = (int *) defMalloc(sizeof(int) * 1);
  this->yh_ = (int *) defMalloc(sizeof(int) * 1);
}

void
defiRegion_Delete(defiRegion * this)
{
  if (this == NULL)
    return;
  defiRegion_Destroy(this);
  free(this);
}

void 
defiRegion_clear(defiRegion * this)
{
  int     i;

  for (i = 0;
       i < this->numProps_;
       i++) {
    defFree(this->propNames_[i]);
    defFree(this->propValues_[i]);
    this->propDValues_[i] = 0;
  }
  this->numProps_ = 0;
  this->numRectangles_ = 0;
  if (this->type_)
    defFree(this->type_);
  this->type_ = 0;
}

void 
defiRegion_Destroy(defiRegion * this)
{
  if (this->name_)
    defFree(this->name_);
  defiRegion_clear(this);
  this->name_ = 0;
  this->nameLength_ = 0;
  defFree((char *) (this->xl_));
  defFree((char *) (this->yl_));
  defFree((char *) (this->xh_));
  defFree((char *) (this->yh_));
  defFree((char *) (this->propNames_));
  defFree((char *) (this->propValues_));
  defFree((char *) (this->propDValues_));
  defFree((char *) (this->propTypes_));
}

void 
defiRegion_addRect(defiRegion * this,
		   int xl,
		   int yl,
		   int xh,
		   int yh)
{
  if (this->numRectangles_ == this->rectanglesAllocated_) {
    int     i;

    int     max = this->rectanglesAllocated_ = this->rectanglesAllocated_ * 2;

    int    *newxl = (int *) defMalloc(sizeof(int) * max);

    int    *newyl = (int *) defMalloc(sizeof(int) * max);

    int    *newxh = (int *) defMalloc(sizeof(int) * max);

    int    *newyh = (int *) defMalloc(sizeof(int) * max);

    for (i = 0;
	 i < this->numRectangles_;
	 i++) {
      newxl[i] = this->xl_[i];
      newyl[i] = this->yl_[i];
      newxh[i] = this->xh_[i];
      newyh[i] = this->yh_[i];
    }
    defFree((char *) (this->xl_));
    defFree((char *) (this->yl_));
    defFree((char *) (this->xh_));
    defFree((char *) (this->yh_));
    this->xl_ = newxl;
    this->yl_ = newyl;
    this->xh_ = newxh;
    this->yh_ = newyh;
  }
  this->xl_[this->numRectangles_] = xl;
  this->yl_[this->numRectangles_] = yl;
  this->xh_[this->numRectangles_] = xh;
  this->yh_[this->numRectangles_] = yh;
  this->numRectangles_ += 1;
}

void 
defiRegion_setup(defiRegion * this,
		 const char *name)
{
  int     len = strlen(name) + 1;

  defiRegion_clear(this);
  if (len > this->nameLength_) {
    if (this->name_)
      defFree(this->name_);
    this->nameLength_ = len;
    this->name_ = (char *) defMalloc(len);
  }
  strcpy(this->name_, DEFCASE(name));
}

void 
defiRegion_addProperty(defiRegion * this,
		       const char *name,
		       const char *value,
		       const char type)
{
  int     len;

  if (this->numProps_ == this->propsAllocated_) {
    int     i;

    char  **nn;

    char  **nv;

    double *nd;

    char   *nt;

    this->propsAllocated_ *= 2;
    nn = (char **) defMalloc(sizeof(char *) * this->propsAllocated_);
    nv = (char **) defMalloc(sizeof(char *) * this->propsAllocated_);
    nd = (double *) defMalloc(sizeof(double) * this->propsAllocated_);
    nt = (char *) defMalloc(sizeof(char) * this->propsAllocated_);
    for (i = 0;
	 i < this->numProps_;
	 i++) {
      nn[i] = this->propNames_[i];
      nv[i] = this->propValues_[i];
      nd[i] = this->propDValues_[i];
      nt[i] = this->propTypes_[i];
    }
    defFree((char *) (this->propNames_));
    defFree((char *) (this->propValues_));
    defFree((char *) (this->propDValues_));
    defFree((char *) (this->propTypes_));
    this->propNames_ = nn;
    this->propValues_ = nv;
    this->propDValues_ = nd;
    this->propTypes_ = nt;
  }
  len = strlen(name) + 1;
  this->propNames_[this->numProps_] = (char *) defMalloc(len);
  strcpy(this->propNames_[this->numProps_], DEFCASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProps_] = (char *) defMalloc(len);
  strcpy(this->propValues_[this->numProps_], DEFCASE(value));
  this->propDValues_[this->numProps_] = 0;
  this->propTypes_[this->numProps_] = type;
  this->numProps_ += 1;
}

void 
defiRegion_addNumProperty(defiRegion * this,
			  const char *name,
			  const double d,
			  const char *value,
			  const char type)
{
  int     len;

  if (this->numProps_ == this->propsAllocated_) {
    int     i;

    char  **nn;

    char  **nv;

    double *nd;

    char   *nt;

    this->propsAllocated_ *= 2;
    nn = (char **) defMalloc(sizeof(char *) * this->propsAllocated_);
    nv = (char **) defMalloc(sizeof(char *) * this->propsAllocated_);
    nd = (double *) defMalloc(sizeof(double) * this->propsAllocated_);
    nt = (char *) defMalloc(sizeof(char) * this->propsAllocated_);
    for (i = 0;
	 i < this->numProps_;
	 i++) {
      nn[i] = this->propNames_[i];
      nv[i] = this->propValues_[i];
      nd[i] = this->propDValues_[i];
      nt[i] = this->propTypes_[i];
    }
    defFree((char *) (this->propNames_));
    defFree((char *) (this->propValues_));
    defFree((char *) (this->propDValues_));
    defFree((char *) (this->propTypes_));
    this->propNames_ = nn;
    this->propValues_ = nv;
    this->propDValues_ = nd;
    this->propTypes_ = nt;
  }
  len = strlen(name) + 1;
  this->propNames_[this->numProps_] = (char *) defMalloc(len);
  strcpy(this->propNames_[this->numProps_], DEFCASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProps_] = (char *) defMalloc(len);
  strcpy(this->propValues_[this->numProps_], DEFCASE(value));
  this->propDValues_[this->numProps_] = d;
  this->propTypes_[this->numProps_] = type;
  this->numProps_ += 1;
}

void 
defiRegion_setType(defiRegion * this,
		   const char *type)
{
  int     len;

  if (this->type_)
    defFree(this->type_);
  len = strlen(type) + 1;
  this->type_ = (char *) defMalloc(len);
  strcpy(this->type_, DEFCASE(type));
}

int 
defiRegion_hasType(const defiRegion * this)
{
  return this->type_ ? 1 : 0;
}

const char *
defiRegion_type(const defiRegion * this)
{
  return this->type_;
}

int 
defiRegion_numRectangles(const defiRegion * this)
{
  return this->numRectangles_;
}

int 
defiRegion_numProps(const defiRegion * this)
{
  return this->numProps_;
}

const char *
defiRegion_propName(const defiRegion * this,
		    int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numProps_) {
    sprintf(msg, "ERROR (DEFPARS-6130): The index number %d given for the REGION PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    defiError(msg);
    return 0;
  }
  return this->propNames_[index];
}

const char *
defiRegion_propValue(const defiRegion * this,
		     int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numProps_) {
    sprintf(msg, "ERROR (DEFPARS-6130): The index number %d given for the REGION PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    defiError(msg);
    return 0;
  }
  return this->propValues_[index];
}

double 
defiRegion_propNumber(const defiRegion * this,
		      int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numProps_) {
    sprintf(msg, "ERROR (DEFPARS-6130): The index number %d given for the REGION PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    defiError(msg);
    return 0;
  }
  return this->propDValues_[index];
}

const char 
defiRegion_propType(const defiRegion * this,
		    int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numProps_) {
    sprintf(msg, "ERROR (DEFPARS-6130): The index number %d given for the REGION PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    defiError(msg);
    return 0;
  }
  return this->propTypes_[index];
}

int 
defiRegion_propIsNumber(const defiRegion * this,
			int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numProps_) {
    sprintf(msg, "ERROR (DEFPARS-6130): The index number %d given for the REGION PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    defiError(msg);
    return 0;
  }
  return this->propDValues_[index] ? 1 : 0;
}

int 
defiRegion_propIsString(const defiRegion * this,
			int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numProps_) {
    sprintf(msg, "ERROR (DEFPARS-6130): The index number %d given for the REGION PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    defiError(msg);
    return 0;
  }
  return this->propDValues_[index] ? 0 : 1;
}

const char *
defiRegion_name(const defiRegion * this)
{
  return this->name_;
}

int 
defiRegion_xl(const defiRegion * this,
	      int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numRectangles_) {
    sprintf(msg, "ERROR (DEFPARS-6131): The index number %d given for the REGION RECTANGLE is invalid.\nValid index is from 0 to %d", index, this->numRectangles_);
    defiError(msg);
    return 0;
  }
  return this->xl_[index];
}

int 
defiRegion_yl(const defiRegion * this,
	      int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numRectangles_) {
    sprintf(msg, "ERROR (DEFPARS-6131): The index number %d given for the REGION RECTANGLE is invalid.\nValid index is from 0 to %d", index, this->numRectangles_);
    defiError(msg);
    return 0;
  }
  return this->yl_[index];
}

int 
defiRegion_xh(const defiRegion * this,
	      int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numRectangles_) {
    sprintf(msg, "ERROR (DEFPARS-6131): The index number %d given for the REGION RECTANGLE is invalid.\nValid index is from 0 to %d", index, this->numRectangles_);
    defiError(msg);
    return 0;
  }
  return this->xh_[index];
}

int 
defiRegion_yh(const defiRegion * this,
	      int index)
{
  char    msg[160];

  if (index < 0 || index >= this->numRectangles_) {
    sprintf(msg, "ERROR (DEFPARS-6131): The index number %d given for the REGION RECTANGLE is invalid.\nValid index is from 0 to %d", index, this->numRectangles_);
    defiError(msg);
    return 0;
  }
  return this->yh_[index];
}

void 
defiRegion_print(const defiRegion * this,
		 FILE * f)
{
  int     i;

  fprintf(f, "Region '%s'", defiRegion_name(this));
  for (i = 0;
       i < defiRegion_numRectangles(this);
       i++) {
    fprintf(f, " %d %d %d %d", defiRegion_xl(this,
					     i), defiRegion_yl(this,
						     i), defiRegion_xh(this,
						     i), defiRegion_yh(this,
								       i));
  }
  fprintf(f, "\n");
}
