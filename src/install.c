/* 

        Copyright (C) 1994-
        Free Software Foundation, Inc.

   This file is part of GNU cfengine - written and maintained 
   by Mark Burgess, Dept of Computing and Engineering, Oslo College,
   Dept. of Theoretical physics, University of Oslo
 
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

*/

/*****************************************************************************/
/*                                                                           */
/* File: listmechanics.c                                                     */
/*                                                                           */
/*****************************************************************************/

#include "cf3.defs.h"
#include "cf3.extern.h"

/*******************************************************************/

struct Bundle *AppendBundle(struct Bundle **start,char *name, char *type, struct Rlist *args)

{ struct Bundle *bp,*lp;
  char *sp;
  struct Rlist *rp;

if (INSTALL_SKIP)
   {
   return NULL;
   }
  
Debug("Appending new bundle %s %s (",type,name);

if (DEBUG)
   {
   ShowRlist(FOUT,args);
   }
Debug(")\n");

CheckBundle(name,type);

if ((bp = (struct Bundle *)malloc(sizeof(struct Bundle))) == NULL)
   {
   CfLog(cferror,"Unable to alloc Bundle","malloc");
   FatalError("");
   }

if ((sp = strdup(name)) == NULL)
   {
   CfLog(cferror,"Unable to allocate Bundle","malloc");
   FatalError("");
   }

if (*start == NULL)
   {
   *start = bp;
   }
else
   {
   for (lp = *start; lp->next != NULL; lp=lp->next)
      {
      }

   lp->next = bp;
   }

bp->name = sp;
bp->next = NULL;
bp->type = type;
bp->args = args;
bp->subtypes = NULL;
return bp;
}

/*******************************************************************/

struct Body *AppendBody(struct Body **start,char *name, char *type, struct Rlist *args)

{ struct Body *bp,*lp;
  char *sp;
  struct Rlist *rp;

Debug("Appending new promise body %s %s(",type,name);

CheckBody(name,type);

for (rp = args; rp!= NULL; rp=rp->next)
   {
   Debug("%s,",(char *)rp->item);
   }
Debug(")\n");

if ((bp = (struct Body *)malloc(sizeof(struct Body))) == NULL)
   {
   CfLog(cferror,"Unable to allocate Body","malloc");
   FatalError("");
   }

if ((sp = strdup(name)) == NULL)
   {
   CfLog(cferror,"Unable to allocate Body","malloc");
   FatalError("");
   }

if (*start == NULL)
   {
   *start = bp;
   }
else
   {
   for (lp = *start; lp->next != NULL; lp=lp->next)
      {
      }

   lp->next = bp;
   }

bp->name = sp;
bp->next = NULL;
bp->type = type;
bp->args = args;
bp->conlist = NULL;
return bp;
}

/*******************************************************************/

struct SubType *AppendSubType(struct Bundle *bundle,char *typename)

{ struct SubType *tp,*lp;
  char *sp;

if (INSTALL_SKIP)
   {
   return NULL;
   }
  
Debug("Appending new type section %s\n",typename);

if (bundle == NULL)
   {
   yyerror("Software error. Attempt to add a type without a bundle\n");
   FatalError("Stopped");
   }
 
if ((tp = (struct SubType *)malloc(sizeof(struct SubType))) == NULL)
   {
   CfLog(cferror,"Unable to allocate SubType","malloc");
   FatalError("");
   }

if ((sp = strdup(typename)) == NULL)
   {
   CfLog(cferror,"Unable to allocate SubType","malloc");
   FatalError("");
   }

if (bundle->subtypes == NULL)
   {
   bundle->subtypes = tp;
   }
else
   {
   for (lp = bundle->subtypes; lp->next != NULL; lp=lp->next)
      {
      }

   lp->next = tp;
   }

tp->promiselist = NULL;
tp->name = sp;
tp->next = NULL;
return tp;
}

/*******************************************************************/

struct Promise *AppendPromise(struct SubType *type,char *promiser, void *promisee,char petype,char *classes,char *bundle)

{ struct Promise *pp,*lp;
 char *sp = NULL,*spe = NULL;

if (INSTALL_SKIP)
   {
   return NULL;
   }
 
if (type == NULL)
   {
   yyerror("Software error. Attempt to add a promise without a type\n");
   FatalError("Stopped");
   }

/* Check here for broken promises - or later with more info? */

Debug("Appending Promise from bundle %s %s if context %s\n",bundle,promiser,classes);

if ((pp = (struct Promise *)malloc(sizeof(struct Promise))) == NULL)
   {
   CfLog(cferror,"Unable to allocate Promise","malloc");
   FatalError("");
   }

if ((sp = strdup(promiser)) == NULL)
   {
   CfLog(cferror,"Unable to allocate Promise","malloc");
   FatalError("");
   }

if ((spe = strdup(classes)) == NULL)
   {
   CfLog(cferror,"Unable to allocate Promise","malloc");
   FatalError("");
   }

if (type->promiselist == NULL)
   {
   type->promiselist = pp;
   }
else
   {
   for (lp = type->promiselist; lp->next != NULL; lp=lp->next)
      {
      }

   lp->next = pp;
   }

pp->audit = AUDITPTR;
pp->lineno = P.line_no;
pp->bundle =  strdup(bundle);

pp->promiser = sp;
pp->promisee = promisee;
pp->petype = petype;      /* rtype of promisee - list or scalar recipient? */
pp->classes = spe;
pp->conlist = NULL;
pp->next = NULL;
return pp;
}


