/*
 * The information in this file is
 * Copyright(c) 2007 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef IMPORT_AGENT_ADAPTER_H
#define IMPORT_AGENT_ADAPTER_H

#include "ImportAgent.h"
#include "ImportAgentImp.h"

class ImportAgentAdapter : public ImportAgentCommon, public ImportAgentImp
{
public:
   virtual ~ImportAgentAdapter()
   {
   }

   IMPORTAGENTADAPTER_METHODS(ImportAgentImp)
};

#endif