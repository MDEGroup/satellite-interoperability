#if !defined(__GenericObject_h) // Use file only if it's not already included.
#define __GenericObject_h

/******************************************************************************

    PROJECT:      DSS - S/C Dynamics Simulation Software
    FILE:         GenericObject.h
    RELEASE:      0.01
    AUTHOR:       Luigi Galvagni
    DATE:         2008/10/17

    OVERVIEW
    ========
    Header file for the HW Model Generic Object abstract class

    CHANGE LOG
    ==========
   Luigi Galvagni 2011/01/18 added C++ Data Publishing management
   Luigi Galvagni 2011/02/25 generic model public methods defined at Level 1
   Luigi Galvagni 2011/07/29 Added the time-scheduled Commands Queue management ("MODEL_COMMAND_STACK").
                             Time description modified to "since simulation beginning"; it was "since
                             2000/01/01 00:00:00",  but that is not implemented by the schedulers.
   Luigi Galvagni 2012/05/07 Modified "Set_Rt1553_Address" to allow declaring more objects sharing the
                             same Rt1553_Address (S1 PDHT need).
   Luigi Galvagni 2013/04/02 "typ_DataType" and "typ_DataPublish" definitions moved to the public section.
                             Removed "Save_Breakpoint_Data", data registration will be used for this purpose.
                             Added public method "Get_DataPublish"
   Luigi Galvagni 2013/09/16 Removed all the "ifdef" compiler swithes. Defined the "User_Message"
                             function-pointer to allow the User defining his own static function
                             to manage the GenericObject messages, to be assigned to "User_Message".
   LG/D.Pascucci  2013/09/26 Added empty defaults for methods "Initialize", "Get_Input", "Status" and "Local_Parser"
                             to make them simple virtual methods, instead of pure virtual ones as they were before.
                             "Update" remains pure virtual, since it shall be implemented (always).
                             Removed method: "Initialize(const char* Input_File_Name)" since was misleading
                             with respect to the virtul "Initialize" method. The capability to
                             mange the input file has been added to the "Initialize_Model" method.
   Luigi Galvagni 2013/09/30 Modified destructor to manage the registration updating when destroying a single object.
                             Fixed the order of the descructor activities leaving last the LogFile closure.
   LG/T.DiCocco   2013/10/11 Added "Enable_SubAddress_flag[2][32]" and "Enable_ModeCode_flag[32]" boolean arrays
                             to store the BUS1553 Transactions enabling/disabling status.
                             Modified the "Rt1553_Set_SA_Status" and "Rt1553_Set_MC_Status" default implementation
                             in order to set the added boolean flags.
   LG/D.Pascucci  2013/10/16 Added "typ_serial_link_channel" data type and "N_serial_links" and "serial_link" private attribute.
                             Added "Serial_CreateLinks", "Serial_TransmitData" (master side) and "Serial_ReceiveData" (master side)
                             protected methods to manage the serial links.
                             Added "Serial_ProcessData" (slave side) and "Serial_ProduceData" (slave side) virtual methods, with
                             empty default implementation, for answering to serial link transaction requests coming from a master.
                             Added "Registry_Serial_Link_Connection" public static method for setting Inter-Object serial link connections.
   LG/M.Anania    2013/10/23 Added "Get_Object_by_Name" static public method to retrive the pointer of a GenericObject matching the user provided name.
   M.Anania       2015/05/25 Added "UpdateStepTime" private variable, modified "Update_Model" method to call update on the base of model step
                             Added "Get_model_discretization" public method to get "UpdateStepTime" private variable
   L.Galvagni     2015/07/16 "typ_DataPublish" modified by adding the pointer to the "GenericObject" parent class.
                             Added methods "GetPublishedDataPointer" and "GetPublishedDataName" to get the published data properties
                             from the data RAM address (pointer).
   M.Anania       2015/07/31 Added "Get_input_connection()" to return the pointer to the output connected to the model i-th input
                             Added "Get_output_connection()" to return the pointer to the input connected to the model i-th output
   M.Anania       2015/10/19 Introduced method DeleteAll() to delete all the instantiated object
                             GenericObject destructur is now virtual in order to be sure the upper-level destructor is called
   L.Galvagni     2015/12/04 Added the "delay_flag" to the "typ_objects_link" data structure to allow acquiring on the link "old data"
                             (one propagation cycle old i.e. not updated yet) in order to open a toplogical connection loop in a user-controlled way.
                             "Registry_Output2Input_Connection" function modified accordingly, the default flag value is "false".
   M.Anania       2016/05/13  Introduced public static method "Static_Local_Parser" to manage global commands
                              Introduced public static methods"Bc1553_ReceiveData", "Bc1553_TransmitData" and "Bc1553_ReceiveModeCommand" to
                              emulate 1553 bus controller functions
                              Corrected error in "Set_switch_status" inline function
   M.Anania       2017/03/07  Inverted change logic in "Set_switch_status" inline function
                              Introduced overload of "Serial_***Data" functions for 8-bit data communication
                              Introduced static function Input_Connections_Checks()
   D.Pagnozzi
   D.Andreetti    13/11/2017  Added pubblic method to publish the Generic Object properties relevant for breakpoints
                              ( As public can be invoked from external classes such as SDVE_Level3Template)
   L.Galvagni     2018/01/20  Added static publig "Publish_All_Flag" boolean flag to allow disabling the automatic data publishing (enable by default)

******************************************************************************/

#include <ctype.h>
#include <math.h>
#include <string.h>
#include "GenericTypes.h"
#include "Input_file.h"

#define  STD_SHORT_STRING_SIZE            63                            //!< Maximum allowed length for the <b>Model Unique Name</b> and short text strings
#define  MAX_COMMAND_PARAMETERS_NUMBER    512                           //!< Maximum allowed number of parameters in a model <b>Text String Command</b>
#define  GENERIC_OBJECT_LOG_FILE_NAME     "%sdss_log_%04i%02i%02i_%02i%02i%02i.txt" //!< Standard name for the <b>Messages Log File</b>, shared by all the program models
#define  GENERIC_OBJECT_LEN_LOG_FILE_NAME  1024                         //!< Maximum allowed length for the <b>LoG File Name</b> text strings (including path)

/**
* ABSTRACT Class definition for the Generic Object model <b>(Level 1)</b>.
* Specific HW Unit Models <b>(Level 2)</b> have to inherit from this class common methods and data;
* STATUS (X, Xdot), INPUT (U) and OUTPUT (Y) data structures shall be defined and instanciated at this Level 2 and they shall contain <b>double</b> data type only.
* The HW Unit Models to be implemented in each specific environment <b>(Level 3)</b> have to implement environment-custom interface methods and data.
*/

class GenericObject
{

/*-------------------------------------------------------------------------
   Data Type declarations
-------------------------------------------------------------------------*/

public:

   enum typ_processing_mode   /** Type used to identify when the Model has to be processed in the frame of a single Dynamics Propagation step*/
   {
      Unconstrained_always_updated           = 0,       //!< The Model shall be always processed
      Static_before_propagation_processing   = 1,       //!< The Model is <b>Static  (nx = 0)</b> and shall be processed <b>once only before</b> the Dynamics Propagation steps
      Dynamic_to_be_propagated               = 2,       //!< The Model is <b>Dynamic (nx > 0)</b> or is Static Between Dynamics and shall be propagated trough a Propagation step
      Static_after_propagation_processing    = 3        //!< The Model is <b>Static  (nx = 0)</b> and shall be processed <b>once only after</b> the Dynamics Propagation step
   };

     enum typ_DataType   /** Type used to identify the C++ type of registered data for publishing purposes*/
   {
      typ_bool    =  1,       //!< bool            C++ data type
      typ_char    =  2,       //!< char            C++ data type
      typ_uchar   =  3,       //!< unsigned char   C++ data type
      typ_short   =  4,       //!< short           C++ data type
      typ_ushort  =  5,       //!< unsigned short  C++ data type
      typ_int     =  6,       //!< int             C++ data type
      typ_uint    =  7,       //!< unsigned int    C++ data type
      typ_float   =  8,       //!< float           C++ data type
      typ_double  =  9        //!< double          C++ data type
   };

   struct typ_DataPublish    /** Data structure to store information about the C++ data registered for publishing purposes*/
   {
      GenericObject     *pParent;   //!< Pointer to the GenericObject Parent class
      void              *p;         //!< Pointer to the C++ data to be published
      typ_DataType      type;       //!< C++ data type identifier
      int               size;       //!< C++ data-unit size in byte
      int               number;     //!< C++ number of data-units (i.e. array size)
      char              name[33];   //!< C++ Data identifier text string
      char              unit[15];   //!< C++ Data units descriptor text string
   };

protected:

   struct typ_Rt1553_word    /** Type used to force the value of the BUS-1553 transction words */
   {
      unsigned short word_value[32];  //!< Word value to be forced
      unsigned short forced_flag[32]; //!< Flag to say if the Word has to be forced to the provided value
   };

   struct typ_forced_value    /** Type used to force the value of the BUS-1553 transction words */
   {
      double         actual_value;  //!< Double floating stored actual value
      double         forced_value;  //!< Double floating value to be forced
      unsigned short forced_flag;   //!< Flag to say if the double floating has to be forced to the provided value
   };

   struct typ_serial_link_channel   /** Data structure used to implement the Inter-Object digital data serial links */
   {
      GenericObject  *p_Other;   //!< Pointer to the <b>GenericObject</b> connected to the other side of this Inter-Object digital data serial link (Partner-Object)
      int            ch_Other;   //!< Serial link channel of this connection, as seen by the GenericObject on the other side of the link (Partner-Channel)
   };

private:

   struct typ_objects_link    /** Data structure type to store the output-to-input inter Objects link information*/
   {
      GenericObject  *pObject;   //!< Pointer to the <b>source-Object</b> generating the output linked to an input of this one (target-Object)
      double         *pValue;    //!< Pointer to the source-Object output <b>double</b> value linked to an input of this target-Object
      bool           delay_flag; //!< Flag to say if data acquired on this link can be one propagation cycle old (used to open topological loops)
   };

   struct typ_CommandData  /** Data structure type to store the data for the <b>time-scheduled Objects commands</b> */
   {
      double            elapsed_time;        //!< Command dispatching <b>Time from Simulation Beginning</b> [s]
      char              *pCommand_String;    //!< Pointer to the <b>Command String</b> (separated by <b>NEW LINE</b>) to be dispatched
   };

/*-------------------------------------------------------------------------
   Data declarations
-------------------------------------------------------------------------*/

public:

   static bool                Publish_All_Flag;                       //!<  Static public boolean flag to allow disabling the automatic data publishing (enable by default)

protected:

   char                       Name[STD_SHORT_STRING_SIZE+1];          //!<  <b>Unique Object Name identifier</b> (text string, shall not contain blanks or dots), used for logging and reporting. It can be used to identify the <b>model input parameters</b> and identifies the <b>model command strings</b>.
   static char                Path[GENERIC_OBJECT_LEN_LOG_FILE_NAME]; //!<  Static Protected Variable to store the DSS Log file path

// ----- Standard Dynamics Model parameters

   int                        nx;                              //!<  Object parameter: <b>Dynamics STATUS double-array X (Xdot)</b> number of items (instance dependent)
   int                        nu;                              //!<  Object parameter: <b>INPUT  double-array U</b> number of items (instance dependent)
   int                        ny;                              //!<  Object parameter: <b>OUTPUT double-array Y</b> number of items (instance dependent)
   double                    *pX;                              //!<  Object parameter: <b>Pointer to the model Dynamics STATUS double-array X</b> (instance dependent)
   double                    *pXdot;                           //!<  Object parameter: <b>Pointer to the model Dynamics STATUS DERIVATIVE double-array Xdot</b> (instance dependent)
   double                    *pU;                              //!<  Object parameter: <b>Pointer to the model INPUT  double-array U</b> (instance dependent)
   double                    *pY;                              //!<  Object parameter: <b>Pointer to the model OUTPUT double-array Y</b> (instance dependent)
   double                     time_at_last_update;             //!<  Object parameter: <b>Time at last model updating [s] since Simulation Beginning</b> (instance dependent)
   double                     delta_time_at_last_update;       //!<  Object parameter: <b>Time increment at last model updating [s]</b> (instance dependent)
   typ_forced_value           *pU_Forced;                      //!<  Object parameter: <b>User-forced INPUT U</b> information array
   typ_forced_value           *pY_Forced;                      //!<  Object parameter: <b>User-forced OUTPUT Y</b> information array

// ----- Power Standard Model parameters

   typ_off_on                 OFF_ON_Switch_Status;            //!<  HW Model parameter: <b>OFF/ON unit switch status</b>, switched by ON/OFF commands (instance dependent)
   bool                       Power_Supplied_Flag;             //!<  HW Model parameter: flag indicating the unit correctly receives <b>the Power Supply [nominally TRUE]</b> (instance dependent)
   double                     PowerLoad;                       //!<  HW Model parameter: <b>current power consumption [W]</b> (instance dependent)
   double                     PowerLoad_at_Switch_ON;          //!<  HW Model parameter: <b>power consumption [W] at unit Switch-ON</b>, it can be assigned or read from the setup file during the initialization (instance dependent)

// ----- MIL BUS 1553 Remote Terminal parameters

   unsigned char              Rt1553_RemoteTerminalAddress;    //!<  HW Model parameter: MIL BUS 1553 Model <b>Remote Terminal Address</b>
   bool                       Enable_SubAddress_flag[2][32];   //!<  HW Model parameter: MIL BUS 1553 Model <b>SubAddress Transaction Enabling</b> array ([RX/TX][SAid])
   bool                       Enable_ModeCode_flag[32];        //!<  HW Model parameter: MIL BUS 1553 Model <b>ModeCode Transaction Enabling</b> array ([MCid])
   typ_Rt1553_word            TxModifier[32];                  //!<  HW Model parameter: MIL BUS 1553 Model <b>Forced SubAddres Tx-Transactions</b> data ([SAid])
   typ_Rt1553_word            RxModifier[32];                  //!<  HW Model parameter: MIL BUS 1553 Model <b>Forced SubAddres Rx-Transactions</b> data ([SAid])
   typ_Rt1553_word            McModifier[32];                  //!<  HW Model parameter: MIL BUS 1553 Model <b>Forced ModeCode Transactions</b> data ([MCid])

// ----- Models initialization and input data logging stream

   static InputFile*          input_file;                      //!<  Models Setup file, <b>used for initialization</b> (shared by all Object instances)
          FILE*               Debug_File;                      //!<  Models Debug file, <b>used for debug data logging</b> (instance dependent)

private:

   static FILE*               Log_File;                        //!<  Models Log file, <b>used for message logging</b> (shared by all Object instances)
   static FILE*               Log_1553_File;                   //!<  1553 Log file, <b>used for 1553 bus</b> (shared by all Object instances)
   static unsigned long       N_Warnings;                      //!<  Number of program logged <b>WARNING Messages</b> (shared by all Object instances)
   static unsigned long       N_Errors;                        //!<  Number of program logged <b> ERROR  Messages</b> (shared by all Object instances)
   static double              Current_Epoch;                   //!<  <b>Time at last overall updating</b> [s] since Simulation Beginning (shared by all Object instances)
   static typ_CommandData*    pCommandQueueBuffer;             //!<  Pointer to the RAM Buffer to allocate the Commad Descriptors queue
   static typ_CommandData*    pCurrentCommand;                 //!<  Pointer to the current Command Descriptor
   static char*               pCommandBuffer;                  //!<  Pointer to the RAM Buffer to allocate the Commads text strings

// ----- Object variable for Update_Model scheduling
          double              LastUpdateExecTime;              //!<  Time at last Update execution
          double              UpdateStepTime;                  //!<  Time step for Update function execution <b>if <=0 execute ALWAYS</b>

// ----- Object access and linking parameters

   static unsigned long       Number_Of_Objects;               //!<  Number of the instanciated objects, <b>used for list iterator purposes</b> (shared by all Object instances)
   static GenericObject       *pFirst_Object;                  //!<  Objects list link: <b>pointer to the first object registered in the list</b> (shared by all Object instances)
   static GenericObject       *pLast_Object;                   //!<  Objects list link: <b>pointer to the last object registered in the list</b> (shared by all Object instances)
          GenericObject       *pNext_Object;                   //!<  Objects list link: <b>pointer to the next object registered in the list</b> (instance dependent)
          GenericObject       *pPrevious_Object;               //!<  Objects list link: <b>pointer to the previous object registered in the list</b> (instance dependent)
          unsigned long       Id;                              //!<  Object unique numerical identifier, it is initialized as per the instanciation order and then re-ordered by the topolgy analyzer based on the execution priority (instance dependent)
          typ_objects_link    *pY2U;                           //!<  <b>output-to-input connection pointer</b> to implement inter-object links (instance dependent)
          typ_processing_mode processing_mode;                 //!<  Object property to identify when the Model has to be processed in the frame of a single Dynamics Propagation step
          unsigned long       nesting_level;                   //!<  Object property to define the output-input linked Models processing order
   static bool                topology_solved_flag;            //!<  Flag to keep memory the <b>TOPOLOGY has been already analyzed</b> (shared by all Object instances)

// ----- C++ data publishing parameters

   typ_DataPublish            *P_PublishData;                  //!<  Pointer to the RAM buffer allocated for storing C++ data publishing information
   int                        N_PublishData;                   //!<  Number of the C++ data published by this object

// ----- Inter-Object digital data Serial Links parameters

   int                        N_serial_links;                  //!<  Number of digital data Serial Links
   typ_serial_link_channel    *serial_link;                    //!<  Digital data Serial Links array

/*-------------------------------------------------------------------------
   Function declarations
-------------------------------------------------------------------------*/

public:

// ----- Class Constructor and Destructor

/** Class <b>Constructor</b> (unique), it takes nine parameters*/
   GenericObject (const char*    UniqueName   /*! Text string <b>Unique Object Name identifier</b>, to be used for logging, reporting and data/commands assignement (shall not contain blanks or dots, max length 63 characters)*/,
                  const int      ClassSizeof  /*! Sizeof (in bytes) of the derived class, <b>used to reset the whole RAM allocated for it</b>*/,
                  const int      StatusSizeof /*! Sizeof (in bytes) of the <b>STATUS Data Structure (X and Xdot)</b> (it shall built by <b>double</b> data only)*/,
                  const int      InputSizeof  /*! Sizeof (in bytes) of the <b>INPUT  Data Structure (U)</b> (it shall built by <b>double</b> data only)*/,
                  const int      OutputSizeof /*! Sizeof (in bytes) of the <b>OUTPUT Data Structure (Y)</b> (it shall built by <b>double</b> data only)*/,
                  const void*    pointer_X    /*! Pointer to the <b>STATUS Data Structure (X)</b> (it shall built by <b>double</b> data only)*/,
                  const void*    pointer_Xdot /*! Pointer to the <b>STATUS DERIVATIVE Data Structure (Xdot)</b> (it shall built by <b>double</b> data only)*/,
                  const void*    pointer_U    /*! Pointer to the <b>INPUT Data Structure (U)</b> (it shall built by <b>double</b> data only)*/,
                  const void*    pointer_Y    /*! Pointer to the <b>OUTPUT Data Structure (Y)</b> (it shall built by <b>double</b> data only)*/);

/** Class <b>Destructor</b>*/
   virtual ~GenericObject ();

// ----- Public Generic Methods, common for all the Models

/** Public Generic Method <b> to initialize the Model</b>*/
   void                          Initialize_Model   (typ_return &result /*! Function <b>Return code</b>*/, char* Input_File_Name = NULL /*! Optional Model Initialization <b>Input File Name</b>*/);
/** Public Generic Method <b> to get the Model U INPUT data</b> at given Time*/
   void                          Get_Model_Input    (typ_return &result /*! Function <b>Return code</b>*/, double time /*! <b>Time</b> [s] since Simulation Beginning*/, bool is_the_first_getinput = true /*! Logical flag to identify wether the inputs are acquired for the first time (i.e. also external signals shall be acquired) in a numerical integration step or is an intermediate step */);
/** Public Generic Method <b> to compute the Dynamic Model Dynamics STATUS derivative Xdot</b> based on the input data U and Time*/
   void                          Model_Status       (typ_return &result /*! Function <b>Return code</b>*/, double time /*! <b>Time</b> [s] since Simulation Beginning*/);
/** Public Generic Method <b> to compute the Model Y OUTPUT data</b> for the current Dynamics STATUS X and Time*/
   void                          Update_Model       (typ_return &result /*! Function <b>Return code</b>*/, double time /*! <b>Time</b> [s] since Simulation Beginning*/, bool is_the_final_updating = true /*! Logical flag to identify wether the updating is for an intermediate numerical integration step or for the final one*/);
/** Public Generic Method <b> to parse the command strings sent to the Model</b> in order to perform actions on it (called by Execute_Command)*/
   void                          Model_Local_Parser (typ_return &result /*! Function <b>Return code</b>*/, char* model_command /*! <b>Model specific</b> command string*/, int n_parameters /*! <b>Number of Parameters</b> parsed after the Model specific Command string*/, char* parameters[] /*! Array of <b>Pointers to the Text-Parameters</b> parsed after the Model specific Command string*/);

protected:

// ----- MAIN VIRTUAL Protected Methods to be customized for the HW Unit Models (Level 2) and for the implementation Environment (Level 3) to represent the specific equipment

/** <b>Virtual</b> Protected Method <b>to initialize the Model</b>. By default is empty, it can be customized at Equipment Model level (Level 2, or higher)*/
   virtual void                  Initialize   (typ_return &result /*! Function <b>Return code</b>*/) {};
/** <b>Virtual</b> Protected Method <b>to acquire the Model INPUT U </b> at current Time. By default is empty, it can be customized at implementation Environment Level 3. <b>NOTE:</b> it shall only acquire U data, no other action has to be done here not to bypass the FORCE_INPUT feature*/
   virtual void                  Get_Input    (typ_return &result /*! Function <b>Return code</b>*/, double time /*! <b>Time</b> [s] since Simulation Beginning*/, bool is_the_first_getinput = true /*! Logical flag to identify wether the inputs are acquired for the first time (i.e. also external signals shall be acquired) in a numerical integration step or is an intermediate step */) {};
/** <b>Virtual</b> Protected Method <b>to compute the Model Dynamics STATUS Derivative Xdot</b> based on the current INPUT U and Time. By default is empty, it shall be customized at Equipment Model level (Level 2, or higher) when the STATUS X is not empty*/
   virtual void                  Status       (typ_return &result /*! Function <b>Return code</b>*/, double time /*! <b>Time</b> [s] since Simulation Beginning*/) {};
/** <b>PURE Virtual</b> Protected Method <b>to compute the Model OUTPUT Y</b> based on the current Dynamics STATUS X, INPUT U and Time. It shall be implemented at Equipment Model level (Level 2, or higher)*/
   virtual void                  Update       (typ_return &result /*! Function <b>Return code</b>*/, double time /*! <b>Time</b> [s] since Simulation Beginning*/, bool is_the_final_updating = true /*! Logical flag to identify wether the updating is for an intermediate numerical integration step or for the final one*/) = 0;
/** <b>Virtual</b> Protected Method <b>to parse the command strings sent to the model</b> in order to allow performing actions on it (called by Execute_Command). By default is empty, it can be customized at Equipment Model level (Level 2, or higher)*/
   virtual void                  Local_Parser (typ_return &result /*! Function <b>Return code</b>*/, char* model_command /*! <b>Model specific</b> command string*/, int n_parameters /*! <b>Number of Parameters</b> parsed after the Model specific Command string*/, char* parameters[] /*! Array of <b>Pointers to the Text-Parameters</b> parsed after the Model specific Command string*/) {};

// ----- VIRTUAL Protected Methods to be customized for the HW Unit Models (Level 2) for implementing the unit answer (slave) to digital data Serial Link transactions requested by a master

/** <b>Virtual</b> Protected Method [to be customized at Equipment Model level or higher] <b>for Processing unsolicited data</b> (i.e. receive as slave) sent via digital 16-bit Serial Link by a master */
   virtual  typ_return  Serial_ProcessData (const int channel /*! Serial Link local identifier*/, const int DataWordCount /*! Number of 16-bit data word*/, unsigned short *pDataWords /*! 16-bit data word buffer*/) {return Nominal;};
/** <b>Virtual</b> Protected Method [to be customized at Equipment Model level or higher] <b>for Processing unsolicited data</b> (i.e. receive as slave) sent via digital 8-bit Serial Link by a master */
   virtual  typ_return  Serial_ProcessData (const int channel /*! Serial Link local identifier*/, const int DataByteCount /*! Number of 8-bit data*/, char *pDataBytes /*! 8-bit data word buffer*/) {return Nominal;};
/** <b>Virtual</b> Protected Method [to be customized at Equipment Model level or higher] <b>for Generating solicited data</b>  (i.e. transmit as slave) requested via digital 16-bit  Serial Link by a master */
   virtual  typ_return  Serial_ProduceData (const int channel /*! Serial Link local identifier*/, const int DataWordCount /*! Number of 16-bit data word*/, unsigned short *pDataWords /*! 16-bit data word buffer*/) {return Nominal;};
/** <b>Virtual</b> Protected Method [to be customized at Equipment Model level or higher] <b>for Generating solicited data</b>  (i.e. transmit as slave) requested via digital 8-bit  Serial Link by a master */
   virtual  typ_return  Serial_ProduceData (const int channel /*! Serial Link local identifier*/, const int DataByteCount /*! Number of 8-bit data word*/, char *pDataBytes /*! 8-bit data word buffer*/) {return Nominal;};

public:

// ----- VIRTUAL Public Methods to be customized (at higher Levels) to implement the unit interface on MIL BUS 1553

/** <b>Virtual</b> Public Method [to be customized at environment level-3] <b>to enable/disble the RT model interface on MIL BUS 1553</b>*/
   virtual typ_return            Rt1553_Set_RT_Status (typ_en_dis Enabling_Status = Enable) {return Nominal;};
/** <b>Virtual</b> Public Method [to be customized at environment level-3] <b>to enable/disble the Sub-Address transactions on RT model interface on MIL BUS 1553</b>*/
   virtual typ_return            Rt1553_Set_SA_Status (int RT_SubAddress, typ_B1553_TxRx Transaction_Direction, typ_en_dis Enabling_Status = Enable) {Enable_SubAddress_flag[Transaction_Direction][RT_SubAddress] = Enabling_Status; return Nominal;};
/** <b>Virtual</b> Public Method [to be customized at environment level-3] <b>to enable/disble the Mode-Code transactions on RT model interface on MIL BUS 1553</b>*/
   virtual typ_return            Rt1553_Set_MC_Status (int RT_ModeCode, typ_en_dis Enabling_Status = Enable) {Enable_ModeCode_flag[RT_ModeCode] = Enabling_Status; return Nominal;};

/** <b>Virtual</b> Public Method [to be customized at Equipment Model level or higher] <b>to handle a Receive Data transaction on MIL BUS 1553</b>*/
   virtual typ_return            Rt1553_ReceiveData  (const unsigned char Subaddress,  const unsigned char DataWordCount,       unsigned short pDataWords[32]) {return Nominal;};
/** <b>Virtual</b> Public Method [to be customized at Equipment Model level or higher] <b>to handle a Transmit Data transaction on MIL BUS 1553</b>*/
   virtual typ_return            Rt1553_TransmitData (const unsigned char Subaddress,  const unsigned char DataWordCount,       unsigned short pDataWords[32]) {return Nominal;};
/** <b>Virtual</b> Public Method [to be customized at Equipment Model level or higher] <b>to handle a Mode Code transaction on MIL BUS 1553</b>*/
   virtual typ_return            Rt1553_ReceiveModeCommand (const typ_B1553_TxRx TxRx, const typ_B1553_ModeCode ModeCode,       unsigned short pDataWord[32])  {return Nominal;};

// ----- STATIC INLINE Public Methods to get STATIC information on the Objects List

/** Static Public Method to return the <b>number of the instanciated objects</b> (shared by all Object instances)*/
   static inline unsigned long   Get_Number_Of_Objects () {return Number_Of_Objects;};
/** Static Public Method to return the pointer to the <b>First</b> registered Object in the list (shared by all Object instances)*/
   static inline GenericObject*  Get_First ()             {return pFirst_Object;};
/** Static Public Method to return the pointer to the <b>Last</b> registered Object in the list (shared by all Object instances)*/
   static inline GenericObject*  Get_Last ()              {return pLast_Object;};

// ----- STATIC Public Methods to iterate on all the HW Model instances (registered in the Objects List) the specific Model functions

/** Static Public Method to iterate the <b>Initialize_Model()</b> method on all the GenericObject instances in the list, in order to initialize all of them (shared by all Object instances)*/
   static void                   Initialize_All (const char* Input_File_Name /*! Models Initialization <b>Input File Name</b> (if not provided the default is [GenericObject_Input_Log_File.txt])*/, typ_return &result /* Return code */);
/** Static Public Method to iterate the <b>Get_Model_Input(time,is_the_first_getinput)</b> and <b>Model_Status(time)</b> methods on all the GenericObject <b>DYNAMIC</b> instances in the list (the ones having nx > 0), in order to allow propagating all of them (shared by all Object instances)*/
   static void                   Status_All_Dynamic (double time /*! <b>Time</b> [s] since Simulation Beginning*/, bool is_the_first_getinput /*! Logical flag to identify wether the inputs are acquired for the first time (i.e. also external signals shall be acquired) in a numerical integration step or is an intermediate step */, typ_return &result /* Return code */);
/** Static Public Method to iterate the <b>Get_Model_Input(time,false)</b> and <b>Update_Model(time,is_the_final_updating)</b> method on all the GenericObject <b>DYNAMIC</b> or <b>STATIC between Dynamics</b> instances in the list, in order to update the output for all of them (shared by all Object instances)*/
   static void                   Update_All_Dynamic (double time /*! <b>Time</b> [s] since Simulation Beginning*/, bool is_the_final_updating /*! Logical flag to identify wether the updating is for an intermediate numerical integration step or for the final one (shared by all Object instances)*/, typ_return &result /* Return code */);
/** Static Public Method to iterate the <b>Get_Model_Input(time)</b> and <b>Update_Model(time)</b> methods pair on all the GenericObject <b>STATIC</b> instances in the list (the ones having nx = 0) having the processing mode equal to the provided one (shared by all Object instances)*/
   static void                   Update_All_Static_in_Chain (double time /*! <b>Time</b> [s] since Simulation Beginning*/, typ_processing_mode static_processing_type /*! <b>STATIC</b> Model family identifier based on the updating criteria (before/after dynamic propagation)*/, typ_return &result /* Return code */);
/** Static Public Method to iterate on all the model instances registered in the list, in order to sum up their total power consumption [W] (shared by all Object instances)*/
   static double                 Get_Total_Power_Consumption ();
/** Static Public Method to synchronize all 1553 RT model status*/
   static void                   Synchronizes_All_Rt_Status(typ_return &result /* Return code */);
/** Delete all models*/
   static void                   DeleteAll ();
/** Static Public Method <b>to parse the static command strings not linked to specific model</b> in order to allow performing global actions */
   static void                   Static_Local_Parser (typ_return &result /*! Function <b>Return code</b>*/, char* model_command /*! <b>Model specific</b> command string*/, int n_parameters /*! <b>Number of Parameters</b> parsed after the Model specific Command string*/, char* parameters[] /*! Array of <b>Pointers to the Text-Parameters</b> parsed after the Model specific Command string*/);

// ----- Static Public Methods to emulate 1553 Bus Controller functions

/** Static Public Method to handle a Receive Data transaction on MIL BUS 1553</b>*/
   static typ_return             Bc1553_ReceiveData (int Rt1553_Address, const unsigned char Subaddress, const unsigned char DataWordCount, unsigned short pDataWords[32]);
/** Static Public Method to handle a Transmit Data transaction on MIL BUS 1553</b>*/
   static typ_return             Bc1553_TransmitData (int Rt1553_Address /*! BUS 1553 RT Model Address.*/, const unsigned char Subaddress,  const unsigned char DataWordCount, unsigned short pDataWords[32]);
/** Static Public Method to handle a Mode Code transaction on MIL BUS 1553</b>*/
   static typ_return             Bc1553_ReceiveModeCommand (int Rt1553_Address /*! BUS 1553 RT Model Address.*/, const typ_B1553_TxRx TxRx, const typ_B1553_ModeCode ModeCode, unsigned short pDataWord[32]);

// ----- STATIC Public Methods to implement generic-model utilities

/** Static Public Method to return the pointer to the registered Object in the list matching the provided <b>unique name</b> (shared by all Object instances). Returns NULL if not found */
   static GenericObject*         Get_Object_by_Name (const char* ObjectName /*! <b>Name</b> of the Object to be returned as a pointer. Returns NULL if not found */);
/** Static Public Method to send command strings to a Model, identified by its <b>unique name</b>, in order to perform <b>User defined actions on it</b> (shared by all Object instances); the command format shall be: <b>ModelName.CommandName,param1,parm2,__.</b>; after the Model identification the command is dispatched to its Model_Local_Parser*/
   static typ_return             Execute_Command (char* command_string /*! Text String to execute actions at <b>HW Unit Model</b> level; the command format shall be: ModelName.CommandName,param1,parm2,__.*/);
/** Static Public Method to set one <b>output-to-input (pY2U) connection pointer</b> in order to feed <b>one Model input with a another Model output</b> (shared by all Object instances)*/
   static typ_return             Registry_Output2Input_Connection (double& source_Y /*output-to-input inter-objects link <b>source data</b>, it shall be a registered model output (Y) array element*/, double& target_U /*output-to-input inter-objects link <b>target data</b>, it shall be a different registered model input (U) array element*/, int n = 1 /* double array size */, bool delay_allowed_flag = false /* flag to allow one propagation cycle delay in order to open the topological loops */);
/** Static Public Method to analyze the Models topology (it shall be executed <b>once only</b>) in order to identify the processing order based on the set inter-oject links and the Model properties (Static/Dynamic) (shared by all Object instances)*/
   static typ_return             Topology_Analyzer ();
/** Static Public Method to set one Inter-Object <b>serial digital data link connection</b> (shared by all Object instances)*/
   static typ_return             Registry_Serial_Link_Connection (GenericObject *objA /*! Pointer to the first model connected to the serial link*/, int chA /*! Serial link channel on the first model side*/, GenericObject *objB /*! Pointer to the second model connected to the serial link*/, int chB /*! Serial link channel on the second model side*/);
/** Static Public Method to analyze the Models topology (it shall be executed <b>once only after all connections have been established</b>) in order to check that all the model inputs are connected */
   static typ_return             Input_Connections_Checks ();

// ----- STATIC INLINE Public Methods to handle the common static Time data ([s] since Simulation Beginning, shared by all Object instances)

/** Static Inline Public Method to set the <b>Current Time common data</b> [s] since Simulation Beginning (shared by all Object instances)*/
   static inline typ_return      Set_Current_Epoch (double time /*! <b>Time</b> [s] since Simulation Beginning*/) {Current_Epoch = time; return Nominal;};
/** Static Inline Public Method to get the <b>Current Time common data</b> [s] since Simulation Beginning (shared by all Object instances)*/
   static inline double          Get_Current_Epoch () {return Current_Epoch;};

// ----- STATIC Public Methods to log messages in the Log File and get the WARNING/ERROR counters status

/** Static Public Method to set DSS Log file path*/
   static inline void            LogFileSetPath (const char *path) {strcpy (Path, path);};
/** Static Public Method for low-level raw writing in the commonm Log_File; the message format is the same used by printf (shared by all Object instances)*/
   static typ_return             Log_Write (const char *format /*! Text to be written built as per <b>printf</b> C function*/, ...);
/** Static Public Method to trace in the commonm Log_File a <b>Text String Message</b>; the message format is the same used by printf (shared by all Object instances)*/
   static typ_return             Log_Message (const char *format /*! Message built as per <b>printf</b> C function*/, ...);
/** Static Public Method to trace in the commonm Log_File a <b>WARNING Text String Message</b>; it also increments the WARNINGS counter, the message format is the same used by printf (shared by all Object instances)*/
   static typ_return             Log_Warning (const char *format /*! Message built as per <b>printf</b> C function*/, ...);
/** Static Public Method to trace in the commonm Log_File an <b>ERROR Text String Message</b>; it also increments the ERRORS counter, the message format is the same used by printf (shared by all Object instances)*/
   static typ_return             Log_Error   (const char *format /*! Message built as per <b>printf</b> C function*/, ...);
/** Pointer to Static Public Method to manage all messages at <b>User</b> level. The User shall define his own Static Function and assign it to this pointer*/
   static void                  (*User_Message) (const char *message /*! Message dispatched to the User*/ , const char *source /*! Message type identifier*/);
/** Static Public Method for opening a User Custom Log File*/
   static typ_return             Log_Open_Custom (char* User_File_Name) {Log_File=fopen(User_File_Name,"wt"); return Log_File ? Nominal : Error;};
/** Static Public Method to return the <b>WARNINGS counter status</b> (shared by all Object instances)*/
   static inline unsigned long   Get_N_Warnings () {return N_Warnings;};
/** Static Public Method to return the <b>ERRORS counter status</b> (shared by all Object instances)*/
   static inline unsigned long   Get_N_Errors () {return N_Errors;};

// ----- INLINE Public Methods to provide access to the main internal basic data

/** Inline Public Method to return the current Object <b>unique Numerical Identifier</b> sequencially assigned by the GenericObject Constructor at class instanciation*/
   inline unsigned long          Get_Id ()                  {return Id;};
/** Inline Public Method to return the current Object Text String Identifier, that is the <b>Unique Object Name identifier</b>*/
   inline char*                  Get_Name ()                {return Name;};
/** Inline Public Method to return the pointer to the <b>Next</b> Object in the list; if the current Object is the last one it returns NULL*/
   inline GenericObject*         Get_Next ()                {return pNext_Object;};
/** Inline Public Method to return the pointer to the <b>Previous</b> Object in the list; if the current Object is the first one it returns NULL*/
   inline GenericObject*         Get_Previous ()            {return pPrevious_Object;};
/** Inline Public Method to return the parameter used to identify when the Model has to be processed in the frame of a single Dynamics Propagation step*/
   inline typ_processing_mode    Get_processing_mode ()     {return processing_mode;};
/** Inline Public Method to return the <b>nesting level</b> used to define the output-input linked Models processing order*/
   inline unsigned long          Get_nesting_level ()       {return nesting_level;};
/** Inline Public Method to return the <b>Dynamics STATUS double-array X (Xdot) number of items</b>*/
   inline int                    Get_nx ()                  {return nx;};
/** Inline Public Method to return the <b>INPUT  double-array U number of items</b>*/
   inline int                    Get_nu ()                  {return nu;};
/** Inline Public Method to return the <b>OUTPUT double-array Y number of items</b>*/
   inline int                    Get_ny ()                  {return ny;};
/** Inline Public Method to return the <b>Time at last model updating</b> [s] since Simulation Beginning*/
   inline double                 Get_time ()                {return time_at_last_update;};
/** Inline Public Method to return the <b>time increment at last model updating</b> [s]*/
   inline double                 Get_delta_time ()          {return delta_time_at_last_update;};
/** Inline Public Method to return the HW equipment <b>OFF/ON functional status</b> (typ_off_on, 0=OFF, 1=ON)*/
   inline typ_off_on             Get_off_on ()              {return (Power_Supplied_Flag ? OFF_ON_Switch_Status : Off);};
/** Inline Public Method to return the HW equipment <b>OFF/ON switch, or relay, status</b> (typ_off_on, 0=OFF, 1=ON)*/
   inline typ_off_on             Get_switch_status ()       {return OFF_ON_Switch_Status;};
/** Inline Public Method to return the HW equipment <b>current power consumption [W]</b>*/
   inline double                 Get_power_consumption ()   {if (Get_off_on () == On) return PowerLoad; else return 0.0;};
/** Inline Public Method to return the model scheduling time  <b>(if <=0 execute ALWAYS)</b>*/
   inline double                 Get_model_discretization() {return UpdateStepTime;};
/** Inline Public Method to return the pointer to the output connected to the model i-th input port. Returns NULL if the input port is not connected*/
   inline double*                Get_input_connection(int i){return (nesting_level>1) ? pY2U[i].pValue : NULL;};

// ----- Public Methods to provide acces by pointer to the dynamic status and its derivative

/** Public Method to return the <b>pointer to the ix-th Dynamics STATUS derivative array Xdot</b> component; used to build the overall numerical integrator Dynamics STATUS array*/
   double                        *Get_Pointer_X_dot (int ix /*! index [0..nx-1] of the <b>X Dynamics STATUS array</b> component*/);
/** Public Method to return the <b>pointer to the ix-th Dynamics STATUS array X</b> component; used to build the overall numerical integrator Dynamics STATUS array */
   double                        *Get_Pointer_X (int ix /*! index [0..nx-1] of the <b>X Dynamics STATUS array</b> component*/);
/** Public Method to return the <b>pointer to the ix-th Dynamics OUTPUT array Y</b> component; used to build the overall numerical integrator Dynamics OUTPUT array */
   double                        *Get_Pointer_Y (int iy /*! index [0..ny-1] of the <b>X Dynamics OUTPUT array</b> component*/);
   /** Public Method to return the <b>pointer to the iu-th Dynamics INPUT array U</b> component; used to build the overall numerical integrator Dynamics INPUT array */
   double                        *Get_Pointer_U (int iu /*! index [0..nu-1] of the <b>U Dynamics INPUT array</b> component*/);

// ----- Public Methods to provide acces by value to the model input and output data

/** Public Method to return the <b>value to the iu-th input array U</b> component; available for the overall numerical integrator Dynamics STATUS array*/
   double                        Get_Value_U (int iu /*! index [0..nu-1] of the <b>U input array</b> component*/);
/** Public Method to return the <b>value to the iy-th output array Y</b> component; available for the overall numerical integrator Dynamics STATUS array*/
   double                        Get_Value_Y (int iy /*! index [0..ny-1] of the <b>Y output array</b> component*/);

// ----- Public Methods to implement single-model utilities

/** Public Method to get the value of all the Model inputs connected through an <b>output-to-input (pY2U) connection pointer</b> inter-objects link*/
   typ_return                    Get_Connected_Inputs ();
/** Public Method to send command strings to the Model, in order to perform <b>User defined actions on it</b>; the model_command format shall be: <b>CommandName,param1,parm2,__.</b>; after appending the model_command to the model name it is dispatched using the STATIC method Execute_Command*/
   typ_return                    Execute_Model_Command (const char* model_command_string /*! Text String to execute actions on <b>the current HW Unit Model</b> level; the model_command format shall be: CommandName,param1,parm2,__.*/);
/** Public Method to trace in the proprietary Debug_File a <b>Text String Data Message</b> (instance dependent)*/
   void                          Log_Debug   (const char *format /*! Message built as per <b>printf</b> C function*/, ...);
/** Public Method to get the value of the <b>i-th</b> published data descriptor. If "i" exceedes the published data number returns an empty descriptor*/
   typ_DataPublish               Get_DataPublish (unsigned int i);
/** Public Method to get the pointer of the input port connected to the model i-th output port. Returns NULL if the output port is not connected*/
   double*                       Get_output_connection(int i);

protected:

// ----- Protected Methods for the digital data Serial Link implementation

/** Protected Method <b>for allocating the digital Serial Links</b> array*/
   typ_return                    Serial_CreateLinks (const int N_links /*! Number of the Serial Link channels*/);
/** Protected Method <b>for Sending unsolicited 16-bit data via digital Serial Link</b> as a master */
   typ_return                    Serial_TransmitData (const int channel /*! Serial Link local identifier*/, const int DataWordCount /*! Number of 16-bit data words*/, unsigned short *pDataWords /*! 16-bit data words buffer*/);
/** Protected Method <b>for Sending unsolicited 8-bit data via digital Serial Link</b> as a master */
   typ_return                    Serial_TransmitData (const int channel /*! Serial Link local identifier*/, const int DataByteCount /*! Number of 8-bit data words*/, char *pDataBytes /*! 8-bit data bytes buffer*/);
/** Protected Method <b>for Acquiring solicited 16-bit data via digital Serial Link</b> as a master */
   typ_return                    Serial_ReceiveData (const int channel /*! Serial Link local identifier*/, const int DataWordCount /*! Number of 16-bit data words*/, unsigned short *pDataWords /*! 16-bit data words buffer*/);
/** Protected Method <b>for Acquiring solicited 8-bit data via digital Serial Link</b> as a master */
   typ_return                    Serial_ReceiveData (const int channel /*! Serial Link local identifier*/, const int DataByteCount /*! Number of 8-bit data words*/, char *pDataBytes /*! 8-bit data bytes buffer*/);

// ----- Protected Methods for the BUS 1553 implementation

/** Protected Method to set the BUS 1553 RT Model Address and instanciate the relevant RT Model*/
   typ_return                    Set_Rt1553_Address (int Rt1553_Address /*! BUS 1553 RT Model Address.*/, bool allow_shared_Rt1553_Address_flag = false /*! Flag to allow creating more Objects wit the same TR Address */);
/** Protected Method to force part of a BUS 1553 transaction buffer*/
   typ_return                    Modify_Rt1553_Buffer (typ_Rt1553_word *Rt1553_word, unsigned short pDataWords[32]);

// ----- VIRTUAL Protected Methods, they can be implemented at upper Levels to customize the HW Unit Models (Level 2) or to provide specific environment interfaces (Level 3)

/** <b>Virtual Protected</b> Method to be called by the model Switch_ON command. It allows customizing to perform specific actions at Switch-ON al (Level-3 I/F configuration)*/
   virtual typ_return            Command_Switch_ON ();
/** <b>Virtual Protected</b> Method to be called by the model Switch_OFF command. It allows customizing to perform specific actions at Switch-OFF al (Level-3 I/F configuration)*/
   virtual typ_return            Command_Switch_OFF ();

private:

/** Private Method to set the pointer to the Next <b>GenericObject</b> to be registered in the list*/
   typ_return SetNext (GenericObject *NextGenericObjectPointer /*! Pointer to the Next <b>GenericObject</b> to be registered in the list*/)
      {pNext_Object = NextGenericObjectPointer; return ((pNext_Object) ? (Nominal) : (Error));};

/** Static Private Method to check the Models topology consistency in order to detect TOPOLOGY errors like ALGEBRICAL LOOPS */
   static typ_return             Topology_Checks ();

// ----- C++ data publishing methods

public:

/** Public Method to set the HW equipment <b>OFF/ON switch, or relay, status</b> (typ_off_on, 0=OFF, 1=ON)*/
   void       Set_switch_status (typ_off_on status) {(status == On) ? Command_Switch_ON () : Command_Switch_OFF ();};
/** Public Method to acquire a C++ registered data component*/
   double     GetValue (char *DataName /*! C++ data publishing name*/, int offset /*! Component offset for array accessing*/, char *DataUnitBuffer = 0 /*! Optional C++ data unit text string output*/);
/** Public Method to acquire a C++ registered single-data value*/
   double     GetValue (char *DataName /*! C++ data publishing name*/, char *DataUnitBuffer = 0 /*! Optional C++ data unit text string output*/)
      {return GetValue (DataName, 0, DataUnitBuffer);};
/** Public Method to set a C++ registered data component*/
   typ_return SetValue (char *DataName /*! C++ data publishing name*/, int offset /*! Component offset for array accessing*/, double DataValue /*! C++ data value to be set*/);
/** Public Method to set a C++ registered single-data value*/
   typ_return SetValue (char *DataName /*! C++ data publishing name*/, double DataValue /*! C++ data value to be set*/)
      {return SetValue (DataName, 0, DataValue);};
/** Public static Method to get from the RAM pointer the pointer to the registration data structure, if any */
   static typ_DataPublish* GetPublishedDataPointer (void *in_DataPointer /*! published data RAM pointer*/);
/** Public static Method to get from the RAM pointer the registered data value name (text string), if any */
   static typ_return GetPublishedDataName (void *in_DataPointer /*! published data RAM pointer*/, char *out_NameBuffer /*! published data full name*/);

// ----- Public Method for publication of Generic Object variables

/** Virtual public method to publish the user-defined model's properties*/
   virtual typ_return                    Publish_Generic_Object (void);

private:

/** Protected Method to register a generic C++ data array*/
   typ_return  PublishNewData (void *DataPointer /*! pointer to C++ data to be published*/, typ_DataType DataType /*! published data C++ data type identifier*/, int DataSize /*! published data size in [byte]*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit /*! published data units*/);

protected:

/** Inline Protected Method to register bool C++ single-data for publishing purposes*/
   inline typ_return Publish (bool &Data /*! C++ data to be published*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)&Data, typ_bool, sizeof(bool), 1, DataName, DataUnit);};
/** Inline Protected Method to register char C++ single-data for publishing purposes*/
   inline typ_return Publish (char &Data /*! C++ data to be published*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)&Data, typ_char, sizeof(char), 1, DataName, DataUnit);};
/** Inline Protected Method to register unsigned char C++ single-data for publishing purposes*/
   inline typ_return Publish (unsigned char &Data /*! C++ data to be published*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)&Data, typ_uchar, sizeof(unsigned char), 1, DataName, DataUnit);};
/** Inline Protected Method to register short C++ single-data for publishing purposes*/
   inline typ_return Publish (short &Data /*! C++ data to be published*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)&Data, typ_short, sizeof(short), 1, DataName, DataUnit);};
/** Inline Protected Method to register unsigned short C++ single-data for publishing purposes*/
   inline typ_return Publish (unsigned short &Data /*! C++ data to be published*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)&Data, typ_ushort, sizeof(unsigned short), 1, DataName, DataUnit);};
/** Inline Protected Method to register int C++ single-data for publishing purposes*/
   inline typ_return Publish (int &Data /*! C++ data to be published*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)&Data, typ_int, sizeof(int), 1, DataName, DataUnit);};
/** Inline Protected Method to register unsigned int C++ single-data for publishing purposes*/
   inline typ_return Publish (unsigned int &Data /*! C++ data to be published*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)&Data, typ_uint, sizeof(unsigned int), 1, DataName, DataUnit);};
/** Inline Protected Method to register flost C++ single-data for publishing purposes*/
   inline typ_return Publish (float &Data /*! C++ data to be published*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)&Data, typ_float, sizeof(float), 1, DataName, DataUnit);};
/** Inline Protected Method to register double C++ single-data for publishing purposes*/
   inline typ_return Publish (double &Data /*! C++ data to be published*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)&Data, typ_double, sizeof(double), 1, DataName, DataUnit);};

/** Inline Protected Method to register bool C++ array for publishing purposes*/
   inline typ_return Publish (bool *Data /*! C++ data to be published*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)Data, typ_bool, sizeof(bool), DataNumber, DataName, DataUnit);};
/** Inline Protected Method to register char C++ array for publishing purposes*/
   inline typ_return Publish (char *Data /*! C++ data to be published*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)Data, typ_char, sizeof(char), DataNumber, DataName, DataUnit);};
/** Inline Protected Method to register unsigned char C++ array for publishing purposes*/
   inline typ_return Publish (unsigned char *Data /*! C++ data to be published*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)Data, typ_uchar, sizeof(unsigned char), DataNumber, DataName, DataUnit);};
/** Inline Protected Method to register short C++ array for publishing purposes*/
   inline typ_return Publish (short *Data /*! C++ data to be published*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)Data, typ_short, sizeof(short), DataNumber, DataName, DataUnit);};
/** Inline Protected Method to register unsigned short C++ array for publishing purposes*/
   inline typ_return Publish (unsigned short *Data /*! C++ data to be published*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)Data, typ_ushort, sizeof(unsigned short), DataNumber, DataName, DataUnit);};
/** Inline Protected Method to register int C++ array for publishing purposes*/
   inline typ_return Publish (int *Data /*! C++ data to be published*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)Data, typ_int, sizeof(int), DataNumber, DataName, DataUnit);};
/** Inline Protected Method to register unsigned int C++ array for publishing purposes*/
   inline typ_return Publish (unsigned int *Data /*! C++ data to be published*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)Data, typ_uint, sizeof(unsigned int), DataNumber, DataName, DataUnit);};
/** Inline Protected Method to register flost C++ array for publishing purposes*/
   inline typ_return Publish (float *Data /*! C++ data to be published*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)Data, typ_float, sizeof(float), DataNumber, DataName, DataUnit);};
/** Inline Protected Method to register double C++ array for publishing purposes*/
   inline typ_return Publish (double *Data /*! C++ data to be published*/, int DataNumber /*! number of data (i.e. array size)*/, const char *DataName /*! C++ data publishing name*/, const char *DataUnit = 0 /*! published data units*/)
      {return PublishNewData ((void*)Data, typ_double, sizeof(double), DataNumber, DataName, DataUnit);};

};

#endif // __GenericObject_h end
