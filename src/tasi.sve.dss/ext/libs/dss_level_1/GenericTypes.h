#if !defined(__GenericTypes_h) // Use file only if it's not already included.
#define __GenericTypes_h

/******************************************************************************

    PROJECT:      DSS - S/C Dynamics Simulation Software
    FILE:         GenericTypes.h
    RELEASE:      0.01
    AUTHOR:       Luigi Galvagni
    DATE:         2008/10/17

    OVERVIEW
    ========
    Header file for defining the shared custom data types

    CHANGE LOG
    ==========
   Luigi Galvagni 2013/04/26 Added "B1553_MODE_FIELD_CODE0" to provide both
                             the Mil Bus 1553 standard allowed values,
                              i.e. both "0" and "31"
   R.Cialdini     2017/07/07 introduced additional generic types for DSS
   Luigi Galvagni 2017/12/30 added binary operators  "&&",  "||",  "&="  and  "|="  for the
                             generic data types "typ_return", "typ_off_on" and "typ_en_dis".
                             Added endianess management based on "LITTLE_ENDIAN" define switch.

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

// Autonomous endianess identification ("BIG_ENDIAN" applies to SPARC and Power-PC)
// NOTE: if the "LITTLE_ENDIAN" define switch is already set outside this code (e.g. in the MAKE file)
//       then this check can be commented in order not to override the external setting.
#if defined sparc || defined __ppc__   // "BIG_ENDIAN" applies to SPARC and Power-PC
   #if defined LITTLE_ENDIAN
      #undef LITTLE_ENDIAN
   #endif
#else                                  // "LITTLE_ENDIAN" applies to Intel-based platforms
   #if !defined LITTLE_ENDIAN
      #define LITTLE_ENDIAN
   #endif
#endif

/*-------------------------------------------------------------------------
   General Purposes Macro Definitions
-------------------------------------------------------------------------*/

/** Macro to free memory and set the pointer to zero */
#define _Deallocate(pointer)    {if(pointer){delete(pointer);(pointer)=NULL;}}
/** Macro to convert string to integer: # means hexadecimal format, otherwise is decimal */
#define _AtoI(string)           ((*(string)=='#')?strtoul((string)+1,0,16):atoi(string))

/*-------------------------------------------------------------------------
   Generic Data Type Definitions
-------------------------------------------------------------------------*/

enum typ_return   /** Type used to notify the sub-program return mode */
{
   Error    = 0,  //!< Notifies sub-program ABNORMAL termination
   Nominal  = 1   //!< Notifies sub-program NOMINAL  completion
};

enum typ_off_on   /** Type used to define the HW unit power supply status */
{
   Off = 0,       //!< The HW unit is switched OFF
   On  = 1        //!< The HW unit is switched ON
};

enum typ_off_null_on_signal   /** Type used to describe three-states signals (e.g. as those used to command the PRP subsytem units) */
{
   sign_Off = -1,       
   no_signal  =  0,
   sign_On  =  1        
};

enum typ_en_dis   /** Type used to define the SW function enabling status */
{
   Disable = 0,   //!< The SW function is DISABLED
   Enable  = 1    //!< The SW function is ENABLED
};

/*-------------------------------------------------------------------------
   Dynamics Simulation SW (DSS) Data Type Definitions
-------------------------------------------------------------------------*/

enum typ_dss_mode   /** Type used to define the DSS Propagator operational mode */
{
   Dss_not_initialized   = 0, //!< DSS Model still to be built and initialized
   Dss_initialize        = 1, //!< DSS Model initialization on-going
   Dss_hold              = 2, //!< DSS Model ready to Run
   Dss_run               = 3  //!< DSS Model Running
};

enum typ_scheduler   /** Type used to define the DSS scheduler frequency */
{
    scheduler_8Hz        = 0, //!< DSS   8 Hz scheduling frequency
    scheduler_32Hz       = 1, //!< DSS  32 Hz scheduling frequency
    scheduler_128Hz      = 2, //!< DSS 128 Hz scheduling frequency
    scheduler_notDefined = 3  //!< DSS undefined scheduling frequency
};

enum typ_1553_status      /** Type used to define the DSS BUS 1553 FEE management status */
{
    waiting              = 0, //!< VRFM2G_SUCCESS and two consecutive tsc7 and tsc0 is still not arrived or VRFM2G != VRFM2G_SUCCESS
    init                 = 1, //!< VRFM2G_SUCCESS and is arrived the first tsc7 and tsc0 passage
    running              = 2, //!< VRFM2G_SUCCESS and tsc_new  = tsc_old +1
    frozen               = 3  //!< VRFM2G_SUCCESS and tsc_new != tsc_old +1
};

enum typ_model       /** Type used to define the DSS HW Unit model in use */
{
    SIMULATED = 0,      //!< DSS using HW Unit simulation model
    REAL      = 1       //!< DSS using Real HW Unit
};

enum typ_atb         /** Type used to define the DSS Avionics Test Bench (ATB) environment in use */
{
    ATB_SHORT = 0,      //!< Reduced ATB (ATB-R)
    ATB_FULL  = 1       //!< Full ATB (ATB-Full)
};

enum typ_test_environment  /** Type used to define the DSS Avionics Test Bench (ATB) HW unit using mode */
{
    ATB_REDUCED           = 1,   //!< Reduced ATB (ATB-R)
    ATB_FULL_WITHOUT_UNIT = 2,   //!< ATB Full with HW Unit simulated at BUS 1553 level
    ATB_FULL_WITH_UNIT    = 3,   //!< ATB Full with real HW Unit on the bench
    ATB_FULL_TEST_PORT    = 4    //!< ATB Full with HW Unit simulated at Test Port level
};

enum typ_scheduler_time    /** Type used to define the DSS scheduler real-time performance status */
{
   REAL_TIME = 1,    //!< Real time mode is kept
   TIME_OUT  = 2     //!< Timeout expired
};

/*-------------------------------------------------------------------------
   MIL BUS 1553 Data Type Definitions
-------------------------------------------------------------------------*/

#define  B1553_MODE_FIELD_CODE0      0    //!< Subaddress/Mode field value 00 to identify ModeCode transactions
#define  B1553_MODE_FIELD_CODE      31    //!< Subaddress/Mode field value 31 to identify ModeCode transactions
#define  B1553_BROADCAST_ADDRES     31    //!< Broadcast Message RT-ADDRESS (i.e. for all the RT's)

enum typ_B1553_TxRx                       /** Type to identify Transmit or Recieve (ert RT) transactions on Bus 1553 */
{
   DSS_RX = 0,                                //!< Receive  Transaction, i.e. [Bus ontroller] ---> [RT]
   DSS_TX = 1                                 //!< Transmit Transaction, i.e. [RT] ---> [Bus ontroller]
};

enum typ_B1553_ModeCode                   /** Type providing the Bus 1553 standard ModeCode transaction tipologies */
{
   mcSynchronise                           =  1,     //!< MIL BUS 1553 Standard Mode Code
   mcTransmitStatusWord                    =  2,     //!< MIL BUS 1553 Standard Mode Code
   mcInitiateSelftest                      =  3,     //!< MIL BUS 1553 Standard Mode Code
   mcTransmitterShutdown                   =  4,     //!< MIL BUS 1553 Standard Mode Code
   mcOverrideTransmitterShutdown           =  5,     //!< MIL BUS 1553 Standard Mode Code
   mcInhibitTerminalFlagBit                =  6,     //!< MIL BUS 1553 Standard Mode Code
   mcOverrideInhibitTerminalFlagBit        =  7,     //!< MIL BUS 1553 Standard Mode Code
   mcResetRemoteTerminal                   =  8,     //!< MIL BUS 1553 Standard Mode Code
   mcTransmitVectorWord                    = 16,     //!< MIL BUS 1553 Standard Mode Code
   mcSynchroniseWithDataWord               = 17,     //!< MIL BUS 1553 Standard Mode Code
   mcTransmitLastCommandWord               = 18,     //!< MIL BUS 1553 Standard Mode Code
   mcTransmitBuiltInTestWord               = 19,     //!< MIL BUS 1553 Standard Mode Code
   mcSelectedTransmitterShutdown           = 20,     //!< MIL BUS 1553 Standard Mode Code
   mcOverrideSelectedTransmitterShutdown   = 21      //!< MIL BUS 1553 Standard Mode Code
};

// - - - - - - - - - - - - - - - -
#ifdef LITTLE_ENDIAN // "LITTLE_ENDIAN", i.e. Intel-like endianess switch case
// - - - - - - - - - - - - - - - -

   union typ_B1553_CommandWord               /** Type bit-mapping the Bus 1553 Command Word for Data or ModeCode transactions and as a raw 16-bit word */
   {
      struct typ_Data                        /** Type bit-mapping the Bus 1553 Command Word for Data transactions */
      {
         unsigned short DataWordCount  : 5;           //!< MIL BUS 1553 Command Word for Data transactions : Data Words Counter bit field
         unsigned short Subaddress     : 5;           //!< MIL BUS 1553 Command Word for Data transactions : Sub-Address bit field
         unsigned short Transmit       : 1;           //!< MIL BUS 1553 Command Word for Data transactions : Receive/Transmit bit field
         unsigned short RtAddress      : 5;           //!< MIL BUS 1553 Command Word for Data transactions : Remote Terminal Address bit field
      } Data;
      struct typ_ModeCode                    /** Type bit-mapping the Bus 1553 Command Word for ModeCode transactions */
      {
         unsigned short ModeCode       : 5;           //!< MIL BUS 1553 Command Word for ModeCode transactions : Mode Code type bit field
         unsigned short ModeField      : 5;           //!< MIL BUS 1553 Command Word for ModeCode transactions : Mode Code flag bit field
         unsigned short Transmit       : 1;           //!< MIL BUS 1553 Command Word for ModeCode transactions : Receive/Transmit bit field
         unsigned short RtAddress      : 5;           //!< MIL BUS 1553 Command Word for ModeCode transactions : Remote Terminal Address bit field
      } ModeCode;
      unsigned short Word;
   };

   union typ_B1553_StatusWord                /** Type bit-mapping the Bus 1553 Status Word as a structured record and as a raw 16-bit word */
   {
      struct typ_Field                       /** Type bit-mapping the Bus 1553 Status Word */
      {
         unsigned short TerminalFlagBit         : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short                         : 1;
         unsigned short SubsystemFlagBit        : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short BusyBit                 : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short BroadcastCmdReceivedBit : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short                         : 3;
         unsigned short ServiceRequestBit       : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short                         : 1;
         unsigned short MessageErrorBit         : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short RtAddress               : 5;  //!< MIL BUS 1553 Status Word standard bit field
      } Field;
      unsigned short Word;
   };

// - - - - - - - - - - - - - - - -
#else // "BIG_ENDIAN", i.e. SPARC and Power-PC like endianess switch case
// - - - - - - - - - - - - - - - -

   union typ_B1553_CommandWord               /** Type bit-mapping the Bus 1553 Command Word for Data or ModeCode transactions and as a raw 16-bit word */
   {
      struct typ_Data                        /** Type bit-mapping the Bus 1553 Command Word for Data transactions */
      {
         unsigned short RtAddress      : 5;           //!< MIL BUS 1553 Command Word for Data transactions : Remote Terminal Address bit field
         unsigned short Transmit       : 1;           //!< MIL BUS 1553 Command Word for Data transactions : Receive/Transmit bit field
         unsigned short Subaddress     : 5;           //!< MIL BUS 1553 Command Word for Data transactions : Sub-Address bit field
         unsigned short DataWordCount  : 5;           //!< MIL BUS 1553 Command Word for Data transactions : Data Words Counter bit field
      } Data;
      struct typ_ModeCode                    /** Type bit-mapping the Bus 1553 Command Word for ModeCode transactions */
      {
         unsigned short RtAddress      : 5;           //!< MIL BUS 1553 Command Word for ModeCode transactions : Remote Terminal Address bit field
         unsigned short Transmit       : 1;           //!< MIL BUS 1553 Command Word for ModeCode transactions : Receive/Transmit bit field
         unsigned short ModeField      : 5;           //!< MIL BUS 1553 Command Word for ModeCode transactions : Mode Code flag bit field
         unsigned short ModeCode       : 5;           //!< MIL BUS 1553 Command Word for ModeCode transactions : Mode Code type bit field
      } ModeCode;
      unsigned short Word;
   };

   union typ_B1553_StatusWord                /** Type bit-mapping the Bus 1553 Status Word as a structured record and as a raw 16-bit word */
   {
      struct typ_Field                       /** Type bit-mapping the Bus 1553 Status Word */
      {
         unsigned short RtAddress               : 5;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short MessageErrorBit         : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short                         : 1;
         unsigned short ServiceRequestBit       : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short                         : 3;
         unsigned short BroadcastCmdReceivedBit : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short BusyBit                 : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short SubsystemFlagBit        : 1;  //!< MIL BUS 1553 Status Word standard bit field
         unsigned short                         : 1;
         unsigned short TerminalFlagBit         : 1;  //!< MIL BUS 1553 Status Word standard bit field
      } Field;
      unsigned short Word;
   };

// - - - - - - - - - - - - - - - -
#endif // end endianess switch case
// - - - - - - - - - - - - - - - -

typedef enum                              /** Type used to identify internal status of scheduler process */
{
    SCHED_INIT    = 0,    //!< Scheduler is doing initialisation phase of internals; it goes into STOP phase after init completion
    SCHED_RUN     = 1,    //!< Scheduler is normally running (started or resumed from a previous STOP state)
    SCHED_HOLD    = 2,    //!< Scheduler was stopped by using STOP() function
    SCHED_TERM    = 3,    //!< Scheduler received a terminate command and it is completeting terminate operations
    SCHED_FORCE   = 254,  //!< Scheduler is forced to start also without any sync event
    SCHED_FAIL    = 255   //!< Scheduler is in error status
} typ_sched_status;

typedef enum                              /** Type used to identify scheduler syncronisation status */
{
    EV_SUCCESS = 0,        //!< Scheduler event syncronisation, brodcast received.
    EV_TIMEOUT = 1,        //!< Scheduler event syncronisation returned a timeout state condition.
    EV_STOPPED = 2,        //!< Scheduler event syncronisation returned a timeout state condition.
    EV_ERROR   = 255       //!< Scheduler event syncronisation returned an error condition.
} typ_event_status;

/*-------------------------------------------------------------------------
   Generic Data Type Operators Definitions
-------------------------------------------------------------------------*/

// - - - - - - - - - - - - - - - -
// typ_return '&&' binary Operator
inline typ_return operator && (typ_return r1, typ_return r2)
   { return typ_return ((int)r1 & (int)r2); };

// typ_return '||' binary Operator
inline typ_return operator || (typ_return r1, typ_return r2)
   { return typ_return ((int)r1 | (int)r2); };

// typ_return '&=' binary Operator
inline typ_return operator &= (typ_return &r1, typ_return r2)
   { return r1 = typ_return ((int)r1 & (int)r2); };

// typ_return '|=' binary Operator
inline typ_return operator |= (typ_return &r1, typ_return r2)
   { return r1 = typ_return ((int)r1 | (int)r2); };

// - - - - - - - - - - - - - - - -
// typ_off_on '&&' binary Operator
inline typ_off_on operator && (typ_off_on r1, typ_off_on r2)
   { return typ_off_on ((int)r1 & (int)r2); };

// typ_off_on '||' binary Operator
inline typ_off_on operator || (typ_off_on r1, typ_off_on r2)
   { return typ_off_on ((int)r1 | (int)r2); };

// typ_off_on '&=' binary Operator
inline typ_off_on operator &= (typ_off_on &r1, typ_off_on r2)
   { return r1 = typ_off_on ((int)r1 & (int)r2); };

// typ_off_on '|=' binary Operator
inline typ_off_on operator |= (typ_off_on &r1, typ_off_on r2)
   { return r1 = typ_off_on ((int)r1 | (int)r2); };

// - - - - - - - - - - - - - - - -
// typ_en_dis '&&' binary Operator
inline typ_en_dis operator && (typ_en_dis r1, typ_en_dis r2)
   { return typ_en_dis ((int)r1 & (int)r2); };

// typ_en_dis '||' binary Operator
inline typ_en_dis operator || (typ_en_dis r1, typ_en_dis r2)
   { return typ_en_dis ((int)r1 | (int)r2); };

// typ_en_dis '&=' binary Operator
inline typ_en_dis operator &= (typ_en_dis &r1, typ_en_dis r2)
   { return r1 = typ_en_dis ((int)r1 & (int)r2); };

// typ_en_dis '|=' binary Operator
inline typ_en_dis operator |= (typ_en_dis &r1, typ_en_dis r2)
   { return r1 = typ_en_dis ((int)r1 | (int)r2); };

#endif // __GenericTypes_h end
