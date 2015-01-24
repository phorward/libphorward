/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	comp.gram.tpl
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

void pp_bnf_define( ppgram* g )
{
##call ppgram2c --indent 1 bnf.syn
}
