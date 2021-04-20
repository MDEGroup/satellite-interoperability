/******************************************************************************
*
*   FILE:         SUM.cpp
*   AUTHOR:       Marco Anania
*   DATE:         29/10/2014
*
*   OVERVIEW
*   ========
*   Source file for the SUM Model
*   The model send in output the linear sum of inputs multiplied for an 
*   optional gain 
*
*   CHANGE LOG
*   ==========
*
******************************************************************************/
#include "SUM.h"
#include <algorithm>
#include <cstdlib>

//-------------------------------------------------------------------------
SUM::SUM (const char* ModelName, int nIn)
      :GenericObject (ModelName, sizeof(SUM) , sizeof(X), sizeof(U), sizeof(Y), &X, &Xdot, &U, &Y)
//-------------------------------------------------------------------------
{
   /************************************************************
   * Check maximum allowed size
   ************************************************************/
   if( nIn > MAX_ADD ){
      nIn = MAX_ADD;
      Log_Error("Maximum number of allowed inputs exceeded (%d)", MAX_ADD);
   }
   nu = nIn;
   
   /************************************************************
   * Initialize parameters
   ************************************************************/
   P.nInputs = nIn;
   for( int i=0; i<MAX_ADD; i++)
      P.gains[i] = 1.0;
   
   // Publish the parameters
   Publish(&P.nInputs, 1, "P.nInputs", "");
   Publish(P.gains, P.nInputs, "P.Gains", "");
   
   // Publish the INPUT array data
   Publish (U.addends,        nu,    "U.addends",   "");

   // Publish the OUTPUT array data
   Publish (&Y.sum,         1,   "Y.sum", "");

}

//-------------------------------------------------------------------------
SUM::~SUM ()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void   SUM::Initialize     (typ_return &result)
//-------------------------------------------------------------------------
{
   result = Nominal;
   input_file->load (&P.gains[0], Name,  "GAINS",  P.nInputs, false);
}

//-------------------------------------------------------------------------
void   SUM::Update        (typ_return &result, double time, bool is_the_final_updating)
//-------------------------------------------------------------------------
{
   result = Nominal;  
   Y.sum = 0;
   for( int i=0; i<P.nInputs; i++){
      Y.sum += P.gains[i] * U.addends[i];
   }
}



