/******************************************************************************
*
*   FILE:         SUM.h
*   AUTHOR:       Marco Anania
*   DATE:         29/10/2014
*
*   OVERVIEW
*   ========
*   Header file for the SUM Model
*   The model send in output the linear sum of inputs multiplied for an 
*   optional gain 
*
*   CHANGE LOG
*   ==========
*
******************************************************************************/
#if !defined(__SUM_h) // Sentry, use file only if it's not already included.
#define __SUM_h

#include "../dss_level_1/GenericObject.h"

/******************************************************************************
* Macros definitions
******************************************************************************/
#define MAX_ADD   20    //!< Maximum number of block inputs

/******************************************************************************
* Class definitions
******************************************************************************/
class SUM : public GenericObject
{

   /******************************************************************************
   * Data Type definitions
   ******************************************************************************/
   struct typ_status /** Type STATUS shall be mandatory implemented (eventually empty)*/
   {
   };
   
   struct typ_input /** Type INPUT shall be mandatory implemented (eventually empty) */
   {
		double addends[MAX_ADD];      // Inputs addends
   };

   struct typ_output /** Type OUTPUT shall be mandatory implemented (eventually empty) */
   {
      double sum;                   // Output SUM
   };
   
   struct typ_parameters  /** Type PARAMETER shall be mandatory implemented (eventually empty) */
	{
      double gains[MAX_ADD];        //!< Gain of each addend
      int   nInputs;                //!< Number of addends
	};
   
   /******************************************************************************
   * Data definitions
   ******************************************************************************/
   public:
      typ_status     X;             //!< STATUS X shall be mandatory instanciated at Level 2 (HW Model)
      typ_status     Xdot;          //!< STATUS DERIVATIVE Xdot shall be mandatory instanciated at Level 2 (HW Model)
      typ_input      U;             //!< INPUT  U shall be mandatory instanciated at Level 2 HW Model
      typ_output     Y;             //!< OUTPUT Y shall be mandatory instanciated at Level 2 HW Model
      typ_parameters P;             //!< PARAMETER P shall be mandatory instanciated at Level 2 HW Model
      
   /******************************************************************************
   * Function declarations
   ******************************************************************************/
   public:

      SUM (const char* ModelName, int nInputs); 
      ~SUM ();

      void   Initialize     (typ_return &result);                                                             //!< Shall be mandatory instanciated at Level 2 HW Model
      void   Update         (typ_return &result, double time, bool is_the_final_updating = true);             //!< Shall be mandatory instanciated at Level 2 HW Model
      
};

#endif // __SUM_h sentry.

