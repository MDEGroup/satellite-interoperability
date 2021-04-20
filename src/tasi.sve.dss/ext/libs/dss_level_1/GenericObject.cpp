/******************************************************************************

    PROJECT:      DSS - S/C Dynamics Simulation Software
    FILE:         GenericObject.cpp
    RELEASE:      0.01
    AUTHOR:       Luigi Galvagni
    DATE:         2008/10/17

    OVERVIEW
    ========
    Source Code for the HW Model Generic Object abstract class

    CHANGE LOG
    ==========
   Luigi Galvagni 2011/01/18 added C++ Data Publishing management
   Luigi Galvagni 2011/02/25 generic model public methods defined at Level 1

 * Francesco Sabbioni 2011/03/04
   Model_Local_Parser: in "FORCE_INPUT" and "FORCE_OUTPUT" the pU and pY pointers
   index vector evaluation has been updated:
   k = atoi(parameters[0]) + i; -> k = atoi(parameters[0]) + i - 1;

 * Francesco Sabbioni 2011/04/04
   Model_Local_Parser: "UNFORCE_INPUT" and "UNFORCE_OUTPUT" updated (index k has been
   removed, range values for index i has been modified)

   Luigi Galvagni 2011/07/29 Added the time-scheduled Commands Queue management ("MODEL_COMMAND_STACK").
                             Time description modified to "since simulation beginning"; it was "since
                             2000/01/01 00:00:00",  but that is not implemented by the schedulers.
   Luigi Galvagni 2012/04/14 Removed from the "GenericObject.cpp" source file the following defines
                             (to be defined in the "main" file or in the environment "make" file):
                             #define _WRITE_OUTPUT_ON_CONSOLE
                             #define _WRITE_WARNINGS_ON_CONSOLE
                             #define _WRITE_ERRORS_ON_CONSOLE
   Luigi Galvagni 2012/05/07 Modified "Set_Rt1553_Address" to allow declaring more objects sharing the
                             same Rt1553_Address (S1 PDHT need).
   Luigi Galvagni 2013/04/02 Removed "Save_Breakpoint_Data", data registration will be used for this purpose.
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
                             Added "Serial_ProcessData" (slave side) and "Serial_ProduceData" (slave side) virtual methods with
                             empty default implementation for answering to serial link transaction requests coming from a master.
                             Added "Registry_Serial_Link_Connection" public static method for setting Inter-Object serial link connections.
   LG/M.Anania    2013/10/23 Added "Get_Object_by_Name" static public method to retrive the pointer of a GenericObject matching the user provided name.
   Luigi Galvagni 2014/11/08 Local variable "model_result" initialized to "Nominal" in all the recursive "while" loops (otherwise it coul be randomically "Error")
   M.Anania       2015/05/25 Added "UpdateStepTime" private variable, modified "Update_Model" method to call update on the base of model step
                             Added "Get_model_discretization" public method to get "UpdateStepTime" private variable
   L.Galvagni     2015/07/16 Added methods "GetPublishedDataPointer" and "GetPublishedDataName" to get the published data properties
                             from the data RAM address (pointer).
   M.Anania       2015/07/31 Added method "Get_output_connection()" to return the pointer to the input connected to the model i-th output
   M.Anania       2015/10/19 Introduced method DeleteAll() to delete all the instantiated object
                             Adaptation for 64 bit compilers that issue a warning when printing pointer as %i
   L.Galvagni     2015/12/04 Added the "delay_flag" to the "typ_objects_link" data structure to allow acquiring on the link "old data"
                             (one propagation cycle old i.e. not updated yet) in order to open a toplogical connection loop in a user-controlled way.
                             "Registry_Output2Input_Connection" function modified accordingly, the default flag value is "false".
                             "Topology_Checks" function modified to take into account the "delay_flag" during the topological analysis
   M.Anania       2016/05/13  Introduced public static method "Static_Local_Parser" to manage global commands
                              Introduced public static methods"Bc1553_ReceiveData", "Bc1553_TransmitData" and "Bc1553_ReceiveModeCommand" to
                              emulate 1553 bus controller functions
   M.Anania       24/08/2016  Added closing of debug file, if opened, in destructor method
   M.Anania       07/03/2017  Bug fixed in "Log_Debug" function
                              Changed function "Update_Model" introduced Update call during initialization phase (time == 0)
                              Introduced overload of "Serial_***Data" functions for 8-bit data communication
                              Introduced static function Input_Connections_Checks()
                              SET_COMPONENTS command indexing now starts from 1 instead of 0 (in line with FORCE_* commands)
   D.Pagnozzi
   D.Andreetti    13/11/2017  Added public method to publish the Generic Object properties relevant for breakpoints
                              ( As public can be invoked from external classes such as SDVE_Level3Template)
   L.Galvagni     2018/01/20  Added static publig "Publish_All_Flag" boolean flag to allow disabling the automatic data publishing (enable by default)

******************************************************************************/

#include <stdarg.h>
#include <time.h>
#include "GenericObject.h"

// Static Data Initialization

bool              GenericObject::Publish_All_Flag     = true;
InputFile*        GenericObject::input_file           = NULL;
FILE*             GenericObject::Log_File             = NULL;
FILE*             GenericObject::Log_1553_File        = NULL;
unsigned long     GenericObject::N_Warnings           = 0;
unsigned long     GenericObject::N_Errors             = 0;
double            GenericObject::Current_Epoch        = 0.0;
unsigned long     GenericObject::Number_Of_Objects    = 0;
GenericObject*    GenericObject::pFirst_Object        = NULL;
GenericObject*    GenericObject::pLast_Object         = NULL;
bool              GenericObject::topology_solved_flag = false;
char              GenericObject::Path[GENERIC_OBJECT_LEN_LOG_FILE_NAME];
char*             GenericObject::pCommandBuffer       = NULL;
GenericObject::typ_CommandData*  GenericObject::pCommandQueueBuffer  = NULL;
GenericObject::typ_CommandData*  GenericObject::pCurrentCommand      = NULL;

void (*GenericObject::User_Message) (const char *message, const char *source) = NULL;

//-------------------------------------------------------------------------
GenericObject::GenericObject (const char*    UniqueName,    // Text string Unique Object Name identifier, to be used for logging, reporting and data/commands assignement (shall not contain blanks or dots, max length 63 characters)
                              const int      ClassSizeof,   // Sizeof (in bytes) of the derived class, used to reset the whole RAM allocated for it
                              const int      StatusSizeof,  // Sizeof (in bytes) of the STATUS Data Structure (X and Xdot) (it shall built by double data only)
                              const int      InputSizeof,   // Sizeof (in bytes) of the INPUT  Data Structure (U) (it shall built by double data only)
                              const int      OutputSizeof,  // Sizeof (in bytes) of the OUTPUT Data Structure (Y) (it shall built by double data only)
                              const void*    pointer_X,     // Pointer to the STATUS Data Structure (X) (it shall built by double data only)
                              const void*    pointer_Xdot,  // Pointer to the STATUS DERIVATIVE Data Structure (Xdot) (it shall built by double data only)
                              const void*    pointer_U,     // Pointer to the INPUT Data Structure (U) (it shall built by double data only)
                              const void*    pointer_Y)     // Pointer to the OUTPUT Data Structure (Y) (it shall built by double data only)
//-------------------------------------------------------------------------
{
   char              log_file_name[GENERIC_OBJECT_LEN_LOG_FILE_NAME];
   time_t            current_time;
   GenericObject     *model;

// Reset the whole class instance allocated RAM
   if (!ClassSizeof)    memset (this, '\0', sizeof(GenericObject));
   else                 memset (this, '\0', ClassSizeof);
   Id = ++Number_Of_Objects; // Increment the objects counter and store the object instance numerical identifier

// Build the object list chain
   if (Number_Of_Objects == 1) // First "GenericObject" instanciated
   {
      pFirst_Object    = this;
      pPrevious_Object = NULL;

// Set the DSS Log Text File name based on current date and time
      if (Log_File)
         fclose (Log_File);
      time (&current_time);
      tm *time_now = localtime (&current_time);
      if (!strlen (Path))
         strcat (Path, "./");
      sprintf (log_file_name, GENERIC_OBJECT_LOG_FILE_NAME, Path,
         time_now->tm_year+1900, time_now->tm_mon+1, time_now->tm_mday,
         time_now->tm_hour, time_now->tm_min, time_now->tm_sec);
      Log_File = fopen (log_file_name, "wt");
   }
   else
   {
// Look for a "GenericObject" already registered having the same name
      model = Get_Object_by_Name (UniqueName);

// If a previous "GenericObject" with the same name has been found then do not registry this one
      if (model)
      {
         Log_Error ("GenericObject CONSTRUCTOR : NOT ALLOWED attempt to registry a Model (Id=%i,N=%i) identified by the already used name \"%s\". The Model name shall be unique!" , Id, Number_Of_Objects, UniqueName);
         return;
      }
      pPrevious_Object = pLast_Object;
      pPrevious_Object->SetNext (this);
   }
   pLast_Object = this;
   pNext_Object = NULL;

// Store the instance name and numerical identifier
   strncpy (Name, UniqueName, STD_SHORT_STRING_SIZE); // Store the instance Name

// Store the Dynamics Model properties, size and pointers passed by the Level 2 derived class
   nx    = StatusSizeof / sizeof(double);    // Dynamics STATUS double-array X (Xdot) number of items
   nu    = InputSizeof  / sizeof(double);    // INPUT  double-array U number of items
   ny    = OutputSizeof / sizeof(double);    // OUTPUT double-array Y number of items
   pX    = (double*)pointer_X;               // Pointer to the model Dynamics STATUS array X
   pXdot = (double*)pointer_Xdot;            // Pointer to the model Dynamics STATUS DERIVATIVE array Xdot
   pU    = (double*)pointer_U;               // Pointer to the model INPUT  array U
   pY    = (double*)pointer_Y;               // Pointer to the model OUTPUT array Y

// Initialize to the nominal value TRUE the Power Supply connection flag
   Power_Supplied_Flag = true;

// Trace on the Log File the registered object informations
   Log_Message ("GenericObject CONSTRUCTOR : \"% 24s\" >> (Id=%i,N=%i), this= #%p, Previous_Object= #%p, Next_Object= #%p, First_Object= #%p, Last_Object= #%p, ClassSizeof= %i",
                Name, Id, Number_Of_Objects, this, pPrevious_Object, pNext_Object,pFirst_Object,pLast_Object,ClassSizeof);
}

//-------------------------------------------------------------------------
GenericObject::~GenericObject ()
//-------------------------------------------------------------------------
{
// Delete the Inter-Object digital data Serial Links array
   _Deallocate (serial_link);
   N_serial_links = 0;

// Delete the output-to-input inter-object links connection pointer array
   _Deallocate (pY2U);

// Delete and free the published data information
   _Deallocate (P_PublishData)
   N_PublishData = 0;

// Delete the User-forced INPUT and OUTPUT information arrays
   _Deallocate (pU_Forced);

// Trace in the Log File the registered object informations before destroying it
   Log_Message ("GenericObject DESTRUCTOR \"% 24s\" >> (Id=%i,N=%i), this= #%p, Previous_Object= #%p, Next_Object= #%p, First_Object= #%p, Last_Object= #%p",
      Name, Id, Number_Of_Objects, this, pPrevious_Object, pNext_Object,pFirst_Object,pLast_Object);

// Close debug file
   if (Debug_File)
      fclose (Debug_File); // Close the trace file, if open
   Debug_File = NULL;

// Remove this object from the registration chain
   if (pPrevious_Object)
      pPrevious_Object->pNext_Object = pNext_Object;
   if (pNext_Object)
      pNext_Object->pPrevious_Object = pPrevious_Object;
   if (pFirst_Object == this)
      pFirst_Object = pNext_Object;
   if (pLast_Object == this)
      pLast_Object = pPrevious_Object;

// If the last Model is going to be destroied then report the program termination and close the Log File
   Number_Of_Objects--;
   if (Number_Of_Objects <= 0)
   {
// Deallocate the allocable resources
      _Deallocate (input_file);
      _Deallocate (pCommandQueueBuffer);
      _Deallocate (pCommandBuffer);
// Close the Log File
      if (Log_File)
      {
         Log_Write("\n\n   Number of Warnings : %i\n\n   Number of ERRORS   : %i\n\n   ---      E N D     O F     P R O G R A M      ---\n\n", N_Warnings, N_Errors);
         fclose (Log_File);
      }
// Close the 1553 Log File
      if (Log_1553_File)
         fclose (Log_1553_File);
// Reset to default all the Static Variables
      Log_File             = NULL;
      Log_1553_File        = NULL;
      N_Warnings           = 0;
      N_Errors             = 0;
      Current_Epoch        = 0.0;
      Number_Of_Objects    = 0;
      pFirst_Object        = NULL;
      pLast_Object         = NULL;
      topology_solved_flag = false;
      pCurrentCommand      = NULL;
   }
}

//-------------------------------------------------------------------------
void GenericObject::DeleteAll ()
//-------------------------------------------------------------------------
{
   /************************************************************
   * Delete all instantiated objects
   ************************************************************/
   GenericObject *model  = GenericObject::Get_First ();
   while (model) // Iterate on all the registered Objects
   {
      GenericObject *nextModel = model->Get_Next();
      delete model;
      model = nextModel;
   }
}

//-------------------------------------------------------------------
void GenericObject::Initialize_Model (typ_return &result, char* Input_File_Name)
//-------------------------------------------------------------------
{
   char              *c;
   bool              found;
   int               i, j, n_Commands, old_error;
   typ_CommandData   dummy;

// If the Input_File_Name has been provided here then that means the
// configuration input file has to be managed at this level
   if (Input_File_Name)
   {
// If the "input_file" has not been already open then open it
      if (!input_file)
      {
         input_file = new InputFile;
         if (input_file)
         {
            input_file->open (Input_File_Name);
            if (input_file->error)
            {
               result = Error;
               Log_Error("%s Initialize_Model : Error opening input file", Get_Name());
            }
         }
         else
         {
            result = Error;
            Log_Error("%s Initialize_Model : Memory allocation error for input file ptr", Get_Name());
         }
      }
// If the currently open "input_file" does not match the required one then swap on it
      else if (strcmp (Input_File_Name, input_file->current_input_file_name))
      {
         input_file->close ();
         input_file->open (Input_File_Name);
         if (input_file->error)
            result = Error;
      }
      if (result == Error)
         return;
   }

// If the Commad Descriptors queue has not been loaded yet, then load it from the input file
   if (!pCommandQueueBuffer)
   {
      input_file->DisableLineTokenizer(); // Disable the instruction lines tokenization, i.e. acquire each input line as a single token
      n_Commands = input_file->GetRowsNumber ("MODEL_COMMAND_STACK", 1);
      pCommandQueueBuffer = new typ_CommandData[n_Commands+1];
      if (!pCommandQueueBuffer)
      {
         Log_Error ("Initialize_Model : Unable to allocate in RAM the 'CommandQueueBuffer'. Cannot continue");
         return;
      }
      memset (pCommandQueueBuffer, '\0', (n_Commands+1)*sizeof(typ_CommandData));
      pCommandQueueBuffer[n_Commands].elapsed_time = 1.E+32;

      if (n_Commands > 0)
      {
         old_error = input_file->error;
         found     = input_file->readSymbol ("MODEL_COMMAND_STACK", n_Commands);

         if (found && old_error == input_file->error)
         {
            pCommandBuffer = new char[n_Commands*TOKEN_SIZE+1];
            if (!pCommandBuffer)
            {
               Log_Error ("Initialize_Model : Unable to allocate in RAM the 'CommandBuffer'. Cannot continue");
               return;
            }
            else
            {
               memset (pCommandBuffer, '\0',               n_Commands*TOKEN_SIZE+1);
               memcpy (pCommandBuffer, input_file->buffer, n_Commands*TOKEN_SIZE);
               for (i = 0; i < n_Commands; i++)
               {
                  c = strtok (pCommandBuffer+i*TOKEN_SIZE, " ,\t");
                  if (c) pCommandQueueBuffer[i].elapsed_time = atof (c);
                  c = strtok (NULL, "\n");
                  if (c) pCommandQueueBuffer[i].pCommand_String = c + strspn (c, " ,\t");
               }
// Order command data by ascending dispatching time
               for (i = 0; i < n_Commands; i++)
                  for (j = i+1; j < n_Commands; j++)
                     if (pCommandQueueBuffer[j].elapsed_time < pCommandQueueBuffer[i].elapsed_time)
                     {
                        dummy                  = pCommandQueueBuffer[j];
                        pCommandQueueBuffer[j] = pCommandQueueBuffer[i];
                        pCommandQueueBuffer[i] = dummy;
                     }
               pCurrentCommand = pCommandQueueBuffer;
            }
         }
      }
      input_file->EnableLineTokenizer(); // Enable (default) the instruction lines tokenization, i.e. acquire several tokens from the same input line
   }

// Initialize data for Update_Model scheduling
   LastUpdateExecTime =  0.0;
   UpdateStepTime     = -1.0;    // Default behaviour is UPDATE ALWAYS
   input_file->load (&UpdateStepTime, Name, "UPDATE_STEP_TIME" , 1, false);

// Initialize the specific model
   Initialize (result);

// Allocate the User-forced INPUT and OUTPUT information arrays
   pU_Forced = pY_Forced = NULL;
   if (nu > 0 || ny > 0)
   {
      pU_Forced = new typ_forced_value [ nu+ny ];
      if (pU_Forced)
      {
         memset (pU_Forced, '\0', (nu+ny)*sizeof(typ_forced_value));
         pY_Forced = pU_Forced + nu;
      }
   }

// Publish model's attributes
   if (Publish_All_Flag)
      if (Publish_Generic_Object() == Error)
        result = Error;
}

//-------------------------------------------------------------------
void GenericObject::Get_Model_Input (typ_return &result, double time, bool is_the_first_getinput)
//-------------------------------------------------------------------
{
   int i;
   Current_Epoch = time;

// Get the registered inputs to other model
   Get_Connected_Inputs ();

// Get the model specific INPUT
   Get_Input (result, time, is_the_first_getinput);

// Force the INPUT values set by the user
   if (pU_Forced)
      for (i = 0; i < nu; i++)
      {
         pU_Forced[i].actual_value = pU[i]; // Store the actual INPUT value
         if (pU_Forced[i].forced_flag)
            pU[i] = pU_Forced[i].forced_value; // Override the INPUT when required
      }
}

//-------------------------------------------------------------------
void GenericObject::Model_Status (typ_return &result, double time)
//-------------------------------------------------------------------
{
   Current_Epoch = time;
   Status (result, time);
}

//-------------------------------------------------------------------
void GenericObject::Update_Model (typ_return &result, double time, bool is_the_final_updating)
//-------------------------------------------------------------------
{
   int i;
   Current_Epoch = time;

// Build the model specific OUTPUT
   if( UpdateStepTime<=0 ||                              // The model has not update step time
         time - LastUpdateExecTime >= UpdateStepTime ||  // The step time is elapsed
            time == 0){                                  // At inizialization phase
      Update (result, time, is_the_final_updating);
      // Store the execution time of Update only on last call
      if( is_the_final_updating )
         LastUpdateExecTime = time;
   }

// Force the OUTPUT values set by the user
   if (pY_Forced)
      for (i = 0; i < ny; i++)
      {
         pY_Forced[i].actual_value = pY[i]; // Store the actual OUTPUT value
         if (pY_Forced[i].forced_flag)
            pY[i] = pY_Forced[i].forced_value; // Override the OUTPUT when required
      }

// Dispatch expired commands
   if (is_the_final_updating)
      while (pCurrentCommand
          && pCurrentCommand->pCommand_String
          && pCurrentCommand->elapsed_time <= time)
      {
         Execute_Command (pCurrentCommand->pCommand_String);
         pCurrentCommand++;
      }
}

//-------------------------------------------------------------------
void GenericObject::Static_Local_Parser (typ_return &result, char* model_command, int n_parameters, char*  parameters [])
//-------------------------------------------------------------------
{
   char     complete_file_name[GENERIC_OBJECT_LEN_LOG_FILE_NAME];
   bool     command_identified = false;
   time_t   current_time;

   switch (*model_command)
   {
      case 'E':
         if(!strcmp (model_command, "ENABLE_1553_LOG")){
            command_identified = true;
            if (n_parameters == 0)
            {
               if( Log_1553_File )
                  Log_Warning ("Static command string \"%s\" 1553 bus logging is already enabled", model_command);
               else{
                  time (&current_time);
                  tm *time_now = localtime (&current_time);
                  sprintf (complete_file_name, "%s1553_dump_%04i%02i%02i_%02i%02i%02i.txt", Path,
                           time_now->tm_year+1900, time_now->tm_mon+1, time_now->tm_mday,
                           time_now->tm_hour, time_now->tm_min, time_now->tm_sec);
                  Log_1553_File = fopen (complete_file_name, "wt"); // Open the 1553 debug file
                  if ( Log_1553_File )
                     result = Nominal;
                  else
                     Log_Warning ("Static command string \"%s\" unable to open for writing the \"%s\" 1553 log file", model_command, complete_file_name);
               }
            }
            else
            {
               Log_Warning ("Static command string \"%s\" does not require any parameter, \"%i\" have been found instead", model_command, n_parameters);
               return;
            }
         }
         break;

      case 'D':
         if(!strcmp (model_command, "DISABLE_1553_LOG")){
            command_identified = true;
            if (n_parameters == 0)
            {
               if( Log_1553_File == NULL )
                  Log_Warning ("Static command string \"%s\" 1553 bus logging is not enabled", model_command);
               else{
                  fclose( Log_1553_File );
                  Log_1553_File = NULL;
                  result = Nominal;
               }
            }
            else
            {
               Log_Warning ("Static command string \"%s\" does not require any parameter, \"%i\" have been found instead", model_command, n_parameters);
               return;
            }
         }
         break;
   }

   // The global Command has not been identified
   if (!command_identified)
      Log_Warning ("Static_Local_Parser : FAILED processing static command \"%s\" with \"%i\" parameter-tokens", model_command, n_parameters);
}

//-------------------------------------------------------------------
void GenericObject::Model_Local_Parser (typ_return &result, char* model_command, int n_parameters, char* parameters[])
//-------------------------------------------------------------------
{
   char     comlete_file_name[GENERIC_OBJECT_LEN_LOG_FILE_NAME];
   bool     command_identified = false;
    int     i, k, SubAddress, ModeCode, enabling_flag;
   time_t   current_time;

   switch (*model_command)
   {
      case 'D':
         if (!strcmp (model_command, "DISABLE_DEBUG"))
         {
            command_identified = true;
            if (n_parameters == 0)
            {
               if (Debug_File)
                  fclose (Debug_File); // Close the trace file, if open
               Debug_File = NULL;
               result = Nominal;
            }
            else
            {
               Log_Warning ("Command string \"%s\". Model \"%s.DISABLE_DEBUG\" does not require any parameter, \"%i\" have been found instead", model_command, Name, n_parameters);
               return;
            }
         }
      break;

      case 'E':
         if (!strcmp (model_command, "ENABLE_DEBUG"))
         {
            command_identified = true;
            if (n_parameters == 0)
            {
               if (Debug_File)
                  fclose (Debug_File); // Close the previous trace file, if already open
               time (&current_time);
               tm *time_now = localtime (&current_time);
               sprintf (comlete_file_name, "%s_debug_%s_%04i%02i%02i_%02i%02i%02i.txt", Path, Name,
                  time_now->tm_year+1900, time_now->tm_mon+1, time_now->tm_mday,
                  time_now->tm_hour, time_now->tm_min, time_now->tm_sec);
               Debug_File = fopen (comlete_file_name, "wt"); // Open the new trace file
               if (Debug_File)
                  result = Nominal;
               else
                  Log_Warning ("Command string \"%s\". Model \"%s.ENABLE_DEBUG\" unable to open for writing the \"%s\" utput text file", model_command, Name, comlete_file_name);
            }
            else
            {
               Log_Warning ("Command string \"%s\". Model \"%s.ENABLE_DEBUG\" does not require any parameter, \"%i\" have been found instead", model_command, Name, n_parameters);
               return;
            }
         }
         break;

      case 'F':
         if(!strcmp(model_command, "FORCE_INPUT"))
         {
            command_identified = true;
            if (n_parameters >= 2 // There shall be at least the First Index and the First Value
             && atoi(parameters[0]) >= 1 // First Index check
             && atoi(parameters[0]) + n_parameters - 2 <= nu // Last index check
             && pU_Forced)
            {
               for (i = 1; i < n_parameters; i++)
               {
                  k = atoi(parameters[0]) + i - 1;
                  if     (*parameters[i] == '*') // Reset the forcing flag
                     pU_Forced[k-1].forced_flag = 0;
                  else if(*parameters[i] == '=') // Do nothing, leave as it is
                     ;
                  else
                  {
                     pU_Forced[k-1].forced_flag = 1;
                     pU_Forced[k-1].forced_value = atof (parameters[i]);
                  }
               }
               result = Nominal;
            }
         }
         else if(!strcmp(model_command, "FORCE_OUTPUT"))
         {
            command_identified = true;
            if (n_parameters >= 2 // There shall be at least the First Index and the First Value
             && atoi(parameters[0]) >= 1 // First Index check
             && atoi(parameters[0]) + n_parameters - 2 <= ny // Last index check
             && pY_Forced)
            {
               for (i = 1; i < n_parameters; i++)
               {
                  k = atoi(parameters[0]) + i - 1;
                  if     (*parameters[i] == '*') // Reset the forcing flag
                     pY_Forced[k-1].forced_flag = 0;
                  else if(*parameters[i] == '=') // Do nothing, leave as it is
                     ;
                  else
                  {
                     pY_Forced[k-1].forced_flag = 1;
                     pY_Forced[k-1].forced_value = atof (parameters[i]);
                  }
               }
               result = Nominal;
            }
         }
         else if(!strcmp(model_command, "FREEZE_INPUT"))
         {
            command_identified = true;
            for (i = 0; i < nu; i++)
            {
               {
                  pU_Forced[i].forced_flag  = 1;
                  pU_Forced[i].forced_value = pU[i];
               }
            }
            result = Nominal;
         }
         else if(!strcmp(model_command, "FREEZE_OUTPUT"))
         {
            command_identified = true;
            for (i = 0; i < ny; i++)
            {
               {
                  pY_Forced[i].forced_flag  = 1;
                  pY_Forced[i].forced_value = pY[i];
               }
            }
            result = Nominal;
         }
      break;

      case 'R':
         if (Rt1553_RemoteTerminalAddress && !memcmp (model_command, "RT_", 3))
         {
            if(!strcmp(model_command, "RT_SET_STATUS") && n_parameters == 1)
            {
               command_identified = true;
               enabling_flag = atoi (parameters[0]);
               if (0 <= enabling_flag && enabling_flag <= 1)
               {
                  result = Nominal;
                  Rt1553_Set_RT_Status (enabling_flag==1?Enable:Disable);
               }
            }
            else if(!strcmp(model_command, "RT_SET_SA_STATUS") && n_parameters == 3)
            {
               command_identified = true;
               SubAddress    = atoi (parameters[0]);
               enabling_flag = atoi (parameters[2]);
               if (1 <= SubAddress    && SubAddress    <= 30
               &&  0 <= enabling_flag && enabling_flag <=  1
               && strchr ("TR", *parameters[1]))
               {
                  result = Nominal;
                  Rt1553_Set_SA_Status (SubAddress, *parameters[1]=='T'?DSS_TX:DSS_RX, enabling_flag==1?Enable:Disable);
               }
            }
            else if(!strcmp(model_command, "RT_SET_MC_STATUS") && n_parameters == 2)
            {
               command_identified = true;
               ModeCode      = atoi (parameters[0]);
               enabling_flag = atoi (parameters[1]);
               if (0 <= ModeCode     && ModeCode       <= 31
               &&  0 <= enabling_flag && enabling_flag <=  1)
               {
                  result = Nominal;
                  Rt1553_Set_MC_Status (ModeCode, enabling_flag==1?Enable:Disable);
               }
            }
            else if(!strcmp(model_command, "RT_FORCE_SA_TX_BUFFER"))
            {
               command_identified = true;
               if (1 <= n_parameters && n_parameters <= 33)
               {
                  SubAddress = atoi (parameters[0]);
                  if (1 <= SubAddress && SubAddress <= 30)
                  {
                     result = Nominal;
                     if (n_parameters == 1) // No further parameters, reset all the forcing flags
                        for (i = 0; i < 32; i++)
                           TxModifier[SubAddress].forced_flag[i] = 0;
                     else
                        for(i = 1; i < n_parameters; i++)
                        {
                           if     (*parameters[i] == '*') // Reset the forcing flag
                              TxModifier[SubAddress].forced_flag[i-1] = 0;
                           else if(*parameters[i] == '=') // Do nothing, leave as it is
                              ;
                           else
                           {
                              TxModifier[SubAddress].forced_flag[i-1] = 1;
                              TxModifier[SubAddress].word_value[i-1]  = (unsigned short)_AtoI(parameters[i]);
                           }
                        }
                  }
               }
             }
            else if(!strcmp(model_command, "RT_FORCE_SA_RX_BUFFER"))
            {
               command_identified = true;
               if (1 <= n_parameters && n_parameters <= 33)
               {
                  SubAddress = atoi (parameters[0]);
                  if (1 <= SubAddress && SubAddress <= 30)
                  {
                     result = Nominal;
                     if (n_parameters == 1) // No further parameters, reset all the forcing flags
                        for (i = 0; i < 32; i++)
                           RxModifier[SubAddress].forced_flag[i] = 0;
                     else
                        for(i = 1; i < n_parameters; i++)
                        {
                           if     (*parameters[i] == '*') // Reset the forcing flag
                              RxModifier[SubAddress].forced_flag[i-1] = 0;
                           else if(*parameters[i] == '=') // Do nothing, leave as it is
                              ;
                           else
                           {
                              RxModifier[SubAddress].forced_flag[i-1] = 1;
                              RxModifier[SubAddress].word_value[i-1]  = (unsigned short)_AtoI(parameters[i]);
                           }
                        }
                  }
               }
            }
            else if(!strcmp(model_command, "RT_FORCE_MC_BUFFER"))
            {
               command_identified = true;
               if (1 <= n_parameters && n_parameters <= 2)
               {
                  ModeCode = atoi (parameters[0]);
                  if (0 <= ModeCode && ModeCode <= 31)
                  {
                     result = Nominal;
                     if (n_parameters == 1) // No further parameters, reset all the forcing flags
                        McModifier[ModeCode].forced_flag[0] = 0;
                     else // i.e. if n_parameters == 2
                     {
                        if     (*parameters[1] == '=') // Reset the forcing flag
                           McModifier[ModeCode].forced_flag[0] = 0;
                        else if(*parameters[1] == '*') // Do nothing, leave as it is
                           ;
                        else
                        {
                           McModifier[ModeCode].forced_flag[0] = 1;
                           McModifier[ModeCode].word_value[0]  = (unsigned short)_AtoI(parameters[1]);
                        }
                     }
                  }
               }
            }
         }
      break;

      case 'S':
         if (!strcmp (model_command, "SWITCH_ON"))
         {
            command_identified = true;
            result = Command_Switch_ON();
         }
         else if(!strcmp(model_command, "SWITCH_OFF"))
         {
            command_identified = true;
            result = Command_Switch_OFF();
         }
         else if(!strcmp(model_command, "SET_POWER_LOAD") && n_parameters == 1)
         {
            command_identified = true;
            result = Nominal;
            PowerLoad = atof (parameters[0]);
         }
         else if(!strcmp(model_command, "SET_VALUE") && n_parameters == 2)
         {
            command_identified = true;
            result = SetValue (parameters[0], atof (parameters[1]));
         }
         else if(!strcmp(model_command, "SET_COMPONENTS") && n_parameters > 2)
         {
            command_identified = true;
            result = Nominal;
            for (i = 0; i < n_parameters-2 && result == Nominal; i++)
               if (SetValue (parameters[0], atoi(parameters[1])+i-1, atof(parameters[2+i])) == Error)
                  result = Error;
         }
      break;

      case 'U':
         if(!strcmp(model_command, "UNFORCE_INPUT"))
         {
            command_identified = true;
            if (pU_Forced)
               for (i = 0; i < nu; i++)
                  pU_Forced[i].forced_flag = 0;
            result = Nominal;
         }
         else if(!strcmp(model_command, "UNFORCE_OUTPUT"))
         {
            command_identified = true;
            if (pY_Forced)
               for (i = 0; i < ny; i++)
                  pY_Forced[i].forced_flag = 0;
            result = Nominal;
         }
      break;
   }

// If the command has not been identified then parse it with the specific model parser
   if (!command_identified)
      Local_Parser (result, model_command, n_parameters, parameters);
}

//-------------------------------------------------------------------------
void GenericObject::Initialize_All (const char* Input_File_Name, typ_return &result)
//-------------------------------------------------------------------------
{
   typ_return        model_result;
   GenericObject     *model;

   _Deallocate (input_file) // Close and free the input file, if already open

   input_file = new InputFile; // Open the model setup input file
   if (!input_file)
   {
      result = Error;
      return;
   }

   input_file->open (Input_File_Name);
   result = input_file->error ? Error : Nominal;
   if (result == Error)
      Log_Error ("Initialize_All : Unable to open for reading the input file \"%s\"", Input_File_Name);

   model = Get_First ();
   while (result == Nominal && model) // Iterate on all the registered Objects
   {
      model_result = Nominal;
      Log_Write ("\n>>>> Reading Input File to initialize Model \"%s\" <<<<\n\n", model->Get_Name());
      model->Initialize_Model (model_result);
      if (model_result == Error)
         result = Error;

      model = model->Get_Next();
   }
   input_file->close ();
   _Deallocate (input_file);

// Now call the Update function for all the Objects to complete their initialization
   model = Get_First ();

   while (result == Nominal && model) // Iterate on all the registered Objects
   {
      model_result = Nominal;
      Log_Write ("\n>>>> \"%s\": Initialize_All() --> Get_Connected_Inputs()  <<<<\n\n", model->Get_Name());
      model->Get_Model_Input (model_result, Current_Epoch, true);
      if (model_result == Error)
      {
         result = Error;
         Log_Error ("\n ERROR: \"%s\" Initialize_All() --> Get_Connected_Inputs <<<<<\n\n", model->Get_Name());
      }
      else
      {
         model->Update_Model (model_result, Current_Epoch, true);
         if (model_result == Error)
         {
            result = Error;
            Log_Error ("\n ERROR: \"%s\" Initialize_All() --> Update_Model <<<<<\n\n", model->Get_Name());
         }
      }
      model = model->Get_Next();
   }
}

//-------------------------------------------------------------------------
void GenericObject::Status_All_Dynamic (double time, bool is_the_first_getinput, typ_return &result)
//-------------------------------------------------------------------------
{
   typ_return        model_result;
   GenericObject     *model = Get_First();

   while (result == Nominal && model) // Iterate on all the registered Objects
   {
      model_result = Nominal;
      if (model->Get_nx() > 0) // Execute on DYNAMIC models only
      {
         model->Get_Model_Input (model_result, time, is_the_first_getinput);
         if (model_result == Error)
            result = Error;
         else
         {
            model->Model_Status (model_result, time);
            if (model_result == Error)
               result = Error;
         }
      }
      else if (is_the_first_getinput // Execute once only per propagation step ...
            && model->Get_processing_mode() == Dynamic_to_be_propagated) // ... on STATIC models between DYNAMICs
      {
         model->Get_Model_Input (model_result, time, true);
         if (model_result == Error)
            result = Error;
      }
      model = model->Get_Next();
   }
}

//-------------------------------------------------------------------------
void GenericObject::Update_All_Dynamic (double time, bool is_the_final_updating, typ_return &result)
//-------------------------------------------------------------------------
{
   typ_return        model_result;
   GenericObject     *model = Get_First();

   while (result == Nominal && model) // Iterate on all the registered Objects
   {
      model_result = Nominal;
      if (model->Get_nx() > 0 // Execute on DYNAMIC models ...
       || model->Get_processing_mode() == Dynamic_to_be_propagated      // ... OR on STATIC models between DYNAMICs
       || model->Get_processing_mode() == Unconstrained_always_updated) // ... OR on UNCONSTRAINED models
      {
         model->Get_Model_Input (model_result, time, false); // The external inputs acquisition (is_the_first_getinput) has been already done by "Status_All_Dynamic"
         if (model_result == Error)
            result = Error;
         else
         {
            model->Update_Model (model_result, time, is_the_final_updating);
            if (model_result == Error)
               result = Error;
         }
      }
      model = model->Get_Next();
   }
}

//-------------------------------------------------------------------------
void GenericObject::Update_All_Static_in_Chain (double time, typ_processing_mode static_processing_type, typ_return &result)
//-------------------------------------------------------------------------
{
   typ_return        model_result;
   GenericObject     *model;

   if (static_processing_type == Dynamic_to_be_propagated)
   {
      Log_Error ("Process_All_Static_in_Chain : the \"static_processing_type\" shall be different than \"Dynamic_to_be_propagated\"");
      result = Error;
      return;
   }

   result = Nominal;
   model  = Get_First ();
   while (result == Nominal && model) // Iterate on all the registered Objects
   {
      model_result = Nominal;
      if (model->Get_nx() == 0  // Execute on STATIC models only ...
       && model->Get_processing_mode() == static_processing_type) // ... having the same provided processing type
      {
         model->Get_Model_Input (model_result, time, true);
         if (model_result == Error)
            result = Error;
         else
         {
            model->Update_Model (model_result, time, true);
            if (model_result == Error)
               result = Error;
         }
      }
      model = model->Get_Next();
   }
}

//-------------------------------------------------------------------------
double GenericObject::Get_Total_Power_Consumption ()
//-------------------------------------------------------------------------
{
   double            total_power = 0.0;
   GenericObject     *model      = Get_First();

   while (model) // Iterate on all the registered Objects
   {
      total_power += model->Get_power_consumption();
      model        = model->Get_Next();
   }
   return total_power;
}

//-------------------------------------------------------------------------
GenericObject* GenericObject::Get_Object_by_Name (const char* ObjectName)
//-------------------------------------------------------------------------
{
   GenericObject     *model;

   model = Get_First();
   while (model && strcmp (ObjectName, model->Get_Name()))
      model = model->Get_Next();
   return model;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Execute_Command (char* command_string)
//-------------------------------------------------------------------------
{
   typ_return        result = Error;
   char              *working_copy, *model_name, *model_command, *parameters[MAX_COMMAND_PARAMETERS_NUMBER];
   int               n_parameters, length;
   GenericObject     *model;

   if (!command_string)
   {
      Log_Error ("Execute_Command : Received command string having NULL pointer");
      return result;
   }
   else if (!strlen(command_string))
   {
      Log_Error ("Execute_Command : Received empty command string");
      return result;
   }
   Log_Message ("Execute_Command : Received the following command string : \"%s\"", command_string);

// Duplicate the input command on a working copy
   length = strlen (command_string) + 32;
   working_copy = (char*) malloc (length);
   if (!working_copy)
   {
      Log_Error ("Execute_Command : Unable to allocate memory for the command string \"%s\" working copy", command_string);
      return result;
   }
   memset (working_copy, '\0', length);
   strcpy (working_copy, command_string);

   // Check if the Command String has a ModelName (has '.' character) or it is static
   if( strchr(working_copy, '.') ){
      // Parse the Model Command String. The command format shall be: ModelName.CommandName,param1,parm2,....
      model_name    = strtok (working_copy, "."); // Get the first token i.e. the Model Name
      model_command = strtok (NULL,         ","); // Get the Model specific command string
   }
   else{
      // Parse the Static Command String. The command format shall be: CommandName,param1,parm2,....
      model_name    = NULL;
      model_command = strtok (working_copy, ","); // Get the specific command string
   }
   n_parameters  = 0;
   parameters[0] = strtok (NULL,         ","); // Get the first parameter text-string
   while (parameters[n_parameters] && n_parameters < MAX_COMMAND_PARAMETERS_NUMBER)
   {
      n_parameters++;
      parameters[n_parameters] = strtok (NULL, ","); // Get the next parameter text-string
   }

   if (!model_name){
      Static_Local_Parser(result, model_command, n_parameters, parameters);   // Static command execution
      if (result == Nominal)
         Log_Message ("Static_Local_Parser : successfully processed command \"%s\" with \"%i\" parameter-tokens", model_command, n_parameters);
      else
         Log_Warning ("Static_Local_Parser : FAILED processing command \"%s\" with \"%i\" parameter-tokens", model_command, n_parameters);
   }
   else if (!strlen(model_name))
      Log_Warning ("Execute_Command : the Model Name is empty");
   else if (!model_command)
      Log_Warning ("Execute_Command : the Model Command pointer is NULL (i.e. Model Command not provided)");
   else if (!strlen(model_command))
      Log_Warning ("Execute_Command : the Model Command is empty (i.e. Model Command not provided)");
   else
   {
// Model object identification (by name, case sensitive)
      model = Get_Object_by_Name (model_name);

// Specific Model command execution (checks and messages to be implemented at Specific Model level)
      if (model)
      {
         model->Model_Local_Parser (result, model_command, n_parameters, parameters);
         if (result == Nominal)
            Log_Message ("%s.Model_Local_Parser : successfully processed command \"%s\" with \"%i\" parameter-tokens", model->Get_Name(), model_command, n_parameters);
         else
            Log_Warning ("%s.Model_Local_Parser : FAILED processing command \"%s\" with \"%i\" parameter-tokens", model->Get_Name(), model_command, n_parameters);
      }
      else
         Log_Warning ("Execute_Command : There is no registered Object-Model matching the Name \"%s\"", model_name);
   }
   free (working_copy);
   return result;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Registry_Output2Input_Connection (double& source_Y, double& target_U, int n, bool delay_allowed_flag)
//-------------------------------------------------------------------------
{
   char           suorce_Y_Name[256], target_U_Name[256];
   int            i, i_source_Y, i_target_U;
   GenericObject  *source_Object, *target_Object;

   if (topology_solved_flag)
   {
      Log_Error ("Registry_Output2Input_Connection : no more inter-Objects connections can be established after the TOPOLOGY ANALYSIS has been executed!");
      return Error;
   }

// "source_Object" and related output double-array "source_Object->Y[i_source_Y]" component identification
   source_Object = Get_First();
   i_source_Y    = -1;
   while (source_Object && i_source_Y == -1)
   {
      for (i = 0; i < source_Object->Get_ny() && i_source_Y == -1; i++)
         if (source_Object->pY + i == &source_Y)
            i_source_Y = i;
      if (i_source_Y == -1)
         source_Object = source_Object->Get_Next();
   }

   if (i_source_Y == -1)
   {
      Log_Error ("Registry_Output2Input_Connection : Unable to identify the double-array \"source_Object->Y[i_source_Y]\" component from pointer #%p", &source_Y);
      return Error;
   }

// Store the source_Y data name
   sprintf (suorce_Y_Name, "%s.Y[%i]", source_Object->Get_Name(), i_source_Y);
   if (source_Object->GetPublishedDataPointer (&source_Y))
   {
      strcat (suorce_Y_Name, "=");
      source_Object->GetPublishedDataName (&source_Y, suorce_Y_Name+strlen(suorce_Y_Name));
   }

// "target_Object" and related input double-array "target_Object->U[i_target_U]" component identification
   target_Object = Get_First();
   i_target_U    = -1;
   while (target_Object && i_target_U == -1)
   {
      for (i = 0; i < target_Object->Get_nu() && i_target_U == -1; i++)
         if (target_Object->pU + i == &target_U)
            i_target_U = i;
      if (i_target_U == -1)
         target_Object = target_Object->Get_Next();
   }

   if (i_target_U == -1)
   {
      Log_Error ("Registry_Output2Input_Connection : Link \"%s ---> ????\" Unable to identify the double-array \"target_Object->U[i_target_U]\" component from pointer #%p", suorce_Y_Name, &target_U);
      return Error;
   }

// Store the target_U data name
   sprintf (target_U_Name, "%s.U[%i]", target_Object->Get_Name(), i_target_U);
   if (target_Object->GetPublishedDataPointer (&target_U))
   {
      strcat (target_U_Name, "=");
      target_Object->GetPublishedDataName (&target_U, target_U_Name+strlen(target_U_Name));
   }

// Verify the "source_Object" and the "target_Object" do not match (they shall be different)
   if (target_Object == source_Object)
   {
      Log_Error ("Registry_Output2Input_Connection : Invalid link \"%s ---> %s\" : a Model cannot be linked to itself!", suorce_Y_Name, target_U_Name);
      return Error;
   }

// Verify the double-array "target_Object->U[i_target_U]" component has not been linked yet (each input U[i] shall be linked to a single connection)
   if (target_Object->pY2U && target_Object->pY2U[i_target_U].pValue)
   {
      Log_Error ("Registry_Output2Input_Connection : Busy link \"%s ---> %s\" : the INPUT \"%s\" has beel already connected (each input U[i] shall be linked to a single connection)", suorce_Y_Name, target_U_Name, target_U_Name);
      return Error;
   }

// If the "target_Object" output-to-input inter-object link pointers array has not been allocated yet then allocate it now
   if (!target_Object->pY2U)
   {
      target_Object->pY2U = new typ_objects_link[target_Object->Get_nu()];                       // Allocate the output-to-input inter-object link connection pointers array
      if (target_Object->pY2U)
         memset (target_Object->pY2U, '\0', target_Object->Get_nu() * sizeof(typ_objects_link)); // Reset the output-to-input inter-object link connection pointers array
      else
      {
         Log_Error ("Registry_Output2Input_Connection : Unable to allocate the output-to-input inter-object link connection pointers array \"pY2U\" for the \"%s\" Model", target_Object->Get_Name());
         return Error;
      }
   }

// If all checks have been successfull then establish the output-to-input inter-object link(s)
   for (i = 0; i < n; i++)
   {
      target_Object->pY2U[i_target_U + i].pObject    = source_Object;
      target_Object->pY2U[i_target_U + i].pValue     = &source_Y + i;
      target_Object->pY2U[i_target_U + i].delay_flag = delay_allowed_flag;
   }
   if (Topology_Checks () == Error)
   {
      Log_Error ("Registry_Output2Input_Connection : Link \"%s ---> %s\" cannot be established since causes TOPOLOGICAL errors!", suorce_Y_Name, target_U_Name);
      for (i = 0; i < n; i++)
      {
         target_Object->pY2U[i_target_U + i].pObject    = NULL;
         target_Object->pY2U[i_target_U + i].pValue     = NULL;
         target_Object->pY2U[i_target_U + i].delay_flag = false;
      }
      return Error;
   }
// Notify the nominal link creation
   if (n)
      Log_Message ("Registry_Output2Input_Connection : Link \"%s ---> %s\" successfully established {%i Links}", suorce_Y_Name, target_U_Name, n);
   else
      Log_Message ("Registry_Output2Input_Connection : Link \"%s ---> %s\" successfully established", suorce_Y_Name, target_U_Name);

   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Topology_Checks ()
//-------------------------------------------------------------------------
{
   bool           topology_updated;
   int            i, max_order;
   GenericObject  *model;

   if (!Number_Of_Objects)
      return Nominal;

// Initialize the TOPOLOGY ANALYSIS : reset the propagation mode and the nesting order
   model = Get_First();
   while (model) // Iterate on all the registered Objects: fist screening between DYNAMIC and STATIC
   {
      model->processing_mode = model->Get_nx() > 0 ? Dynamic_to_be_propagated : Static_before_propagation_processing;
      model->nesting_level   = 0;
      model = model->Get_Next();
   }

// Repeat the Models TOPOLOGY UPDATING until an algebrical loop is detected or no more updating is required
   topology_updated = true;
   max_order        = 0;
   while (topology_updated && max_order <= int(Number_Of_Objects))
   {
      topology_updated = false;
      max_order        = 0;
      model            = Get_First();
      while (model) // Iterate on all the registered Objects
      {
         if (!model->pY2U) // No connection to the input U has been established
         {
            if (model->nesting_level < 1)
            {
               model->nesting_level = 1;
               topology_updated     = true;
            }
         }
         else for (i = 0; i < model->Get_nu(); i++) // Check all the "model" inputs
            if (model->pY2U[i].pObject && !model->pY2U[i].delay_flag) // Real-Time connection to the input U[i] has been established
            {

// Check whether the nesting level is in the proper order
               if (model->nesting_level <= model->pY2U[i].pObject->nesting_level)
               {
                  model->nesting_level = 1 + model->pY2U[i].pObject->nesting_level;
                  topology_updated     = true;
               }

// Check for STATIC models getting inputs from DYNAMIC ones
               if (model->Get_nx() == 0 // Static Model
                && model->processing_mode != Static_after_propagation_processing // Not marked yet as "Static_after_propagation_processing"
                && (model->pY2U[i].pObject->processing_mode == Dynamic_to_be_propagated
                 || model->pY2U[i].pObject->processing_mode == Static_after_propagation_processing))
               {
                  model->processing_mode = Static_after_propagation_processing;
                  topology_updated       = true;
               }
            }
         if (max_order < int(model->nesting_level))
            max_order = model->nesting_level;
         model = model->Get_Next();
      }
   }

// If a CONNECTIONS LOOP has been found then notify the error condition
   if (max_order > int(Number_Of_Objects))
   {
      Log_Error ("Topology_Checks : CONNECTIONS LOOP detected. The Models connection topolgy cannot be managed!");
      return Error;
   }

// To identify STATIC models BETWEEN DYNAMICs then loop again until no more updating is required
   topology_updated = true;
   while (topology_updated)
   {
      topology_updated = false;
      model            = Get_First();
      while (model) // Iterate on all the registered Objects
      {
         if (model->processing_mode == Dynamic_to_be_propagated // Model is DINAMIC or STATIC BETWEEN DINAMICs
          && model->pY2U) // Connections to the input U have been established
         {
            for (i = 0; i < model->Get_nu(); i++) // Check all the "model" inputs
               if (model->pY2U[i].pObject // Connection to the input U[i] has been established
                && model->pY2U[i].pObject->processing_mode == Static_after_propagation_processing)
               {
                  model->pY2U[i].pObject->processing_mode = Dynamic_to_be_propagated;
                  topology_updated = true;
               }
         }
         model = model->Get_Next();
      }
   }
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Input_Connections_Checks ()
//-------------------------------------------------------------------------
{
   GenericObject  *model;

   if (Number_Of_Objects)
   {
      Log_Message ("Input_Connections_Checks : Starting the model input connection check.");

// Iterate on all the registered Objects
      model = Get_First();
      while (model)
      {
         // Check all the model inputs are connected
         for (int i = 0; i < model->Get_nu(); i++)
            if ( !model->pY2U || !model->pY2U[i].pObject )
               Log_Warning("Input_Connections_Checks : model %s input U[%d] is not connected to any output", model->Get_Name(), i);
         model = model->Get_Next();
      }
   }
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Topology_Analyzer ()
//-------------------------------------------------------------------------
{
   int            i, j, N;
   unsigned long  *ordered_value, dummy;
   GenericObject  *model, **ordered_list;

   if (topology_solved_flag)
   {
      Log_Error ("Topology_Analyzer : the TOPOLOGY ANALYSIS has been already executed, it cannot be run twice!");
      return Error;
   }
   topology_solved_flag = true;

   if (!Number_Of_Objects)
      return Nominal;

   if (Topology_Checks () == Error)
      return Error;

// If a CONSISTENT TOPOLOGY has been detected then order the Models based on their nesting level
   ordered_list = new  GenericObject*[Number_Of_Objects];
   if (!ordered_list)
   {
      Log_Error ("Topology_Analyzer : Unable to allocate in RAM the Objects List array \"ordered_list[%i]\" for ORDERING purpouses", Number_Of_Objects);
      return Error;
   }
   memset (ordered_list, '\0', Number_Of_Objects * sizeof(GenericObject*));

   ordered_value = new unsigned long [Number_Of_Objects];
   if (!ordered_value)
   {
      delete [] ordered_list;
      Log_Error ("Topology_Analyzer : Unable to allocate in RAM the Ordered Values array \"ordered_value[%i]\" for ORDERING purpouses", Number_Of_Objects);
      return Error;
   }
   memset (ordered_value, '\0', Number_Of_Objects * sizeof(unsigned long));

// Set the ordering parameter: First key "nesting level", Second key "registration order"
   model = Get_First();
   N     = 0;
   while (model) // Iterate on all the registered Objects
   {
      ordered_value[N]  = model->nesting_level * Number_Of_Objects + model->Id; // First key "nesting level", Second key "registration order"
      ordered_list[N++] = model;
      model = model->Get_Next();
   }

// Order the Objects based on the ordering parameter
   for (i = 0; i < N; i++)
      for (j = i+1; j < N; j++)
         if (ordered_value[i] > ordered_value[j])
         {
            model              = ordered_list[i];
            ordered_list[i]    = ordered_list[j];
            ordered_list[j]    = model;
            dummy              = ordered_value[i];
            ordered_value[i]   = ordered_value[j];
            ordered_value[j]   = dummy;
         }

// Re-build the iterator links chain based on the new defined order
   pFirst_Object                    = ordered_list[0];
   pFirst_Object->pPrevious_Object  = NULL;
   pLast_Object                     = ordered_list[N-1];
   pLast_Object->pNext_Object       = NULL;
   for (i = 0; i < N-1; i++)
      ordered_list[i]->pNext_Object = ordered_list[i+1];
   for (i = 1; i < N;   i++)
      ordered_list[i]->pPrevious_Object = ordered_list[i-1];

   Log_Message ("Topology_Analyzer : the TOPOLOGY ANALYSIS has been completed:");
   for (i = 0; i < N; i++)
   Log_Message ("\"% 24s\" >> (Id=%i,N=%i), nesting=%i, weight=%i, this= #%p, Previous_Object= #%p, Next_Object= #%p, First_Object= #%p, Last_Object= #%p",
                ordered_list[i]->Name, ordered_list[i]->Id, i+1, ordered_list[i]->nesting_level, ordered_value[i], ordered_list[i], ordered_list[i]->pPrevious_Object, ordered_list[i]->pNext_Object, ordered_list[i]->pFirst_Object, ordered_list[i]->pLast_Object);

   delete [] ordered_list;
   delete [] ordered_value;
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Registry_Serial_Link_Connection (GenericObject *objA, int chA, GenericObject *objB, int chB)
//-------------------------------------------------------------------------
{
   GenericObject  *pObject;

// Check the first object "objA"
   pObject = Get_First();
   while (pObject && pObject != objA)
      pObject = pObject->Get_Next();
   if (pObject != objA)
   {
      Log_Error ("Registry_Serial_Link_Connection : pointer to the GenericObject \"A\" [ #%p ] does not point to any valid registered GenericObject", objA);
      return Error;
   }

// Check the first serial link channel "chA" is in the range of the allocated ones
   if (chA < 0 || objA->N_serial_links <= chA)
   {
      Log_Error ("Registry_Serial_Link_Connection : link channel out-of-range; \"%s.serial_link[%i]\", shall be in the range [ 0 .. %i ]", objA->Get_Name(), chA, objA->N_serial_links-1);
      return Error;
   }

// Check the first serial link channel "chA" is not already connected
   if (objA->serial_link[chA].p_Other)
   {
      Log_Error ("Registry_Serial_Link_Connection : link channel multiple connection; \"%s.serial_link[%i]\" is already connected to \"%s.serial_link[%i]\"", objA->Get_Name(), chA, objA->serial_link[chA].p_Other->Get_Name(), objA->serial_link[chA].ch_Other);
      return Error;
   }

// Check the second object "objB"
   pObject = Get_First();
   while (pObject && pObject != objB)
      pObject = pObject->Get_Next();
   if (pObject != objB)
   {
      Log_Error ("Registry_Serial_Link_Connection : pointer to the GenericObject \"B\" [ #%p ] does not point to any valid registered GenericObject", objB);
      return Error;
   }

// Check the second serial link channel "chB" is in the range of the allocated ones
   if (chB < 0 || objB->N_serial_links <= chB)
   {
      Log_Error ("Registry_Serial_Link_Connection : link channel out-of-range; \"%s.serial_link[%i]\", shall be in the range [ 0 .. %i ]", objB->Get_Name(), chB, objB->N_serial_links-1);
      return Error;
   }

// Check the second serial link channel "chB" is not already connected
   if (objB->serial_link[chB].p_Other)
   {
      Log_Error ("Registry_Serial_Link_Connection : link channel multiple connection; \"%s.serial_link[%i]\" is already connected to \"%s.serial_link[%i]\"", objB->Get_Name(), chB, objB->serial_link[chB].p_Other->Get_Name(), objB->serial_link[chB].ch_Other);
      return Error;
   }

// Register the connection : objA[chA] <--> objB[chB]
   objA->serial_link[chA].p_Other  = objB;
   objA->serial_link[chA].ch_Other = chB;
   objB->serial_link[chB].p_Other  = objA;
   objB->serial_link[chB].ch_Other = chA;
   Log_Message ("Registry_Serial_Link_Connection : Digital Serial Link connection \"%s.serial_link[%i] <---> %s.serial_link[%i]\" successfully established", objA->Get_Name(), chA, objB->Get_Name(), chB);
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Log_Write (const char *format, ...)
//-------------------------------------------------------------------------
{
   va_list        arguments;
   typ_return     result = Nominal;
   char           msg[1024];

   va_start (arguments, format);             // Initialize the parameters list handler
   vsprintf (msg, format, arguments);        // Write the provided text on the "msg" text string
   va_end (arguments);                       // Terminate the parameters list handler
   if (User_Message)
      User_Message (msg, "Write");           // Dispatch the "msg" message to the User
   if (Log_File)
   {
      fprintf (Log_File, "%s", msg);         // Write the "msg" message on the Log_File
      fflush (Log_File);                     // Flush the Log File to keep it in case of SW crash
   }
   else // In case the Log_File is not accessible trace the error
   {
      N_Errors++;
      result = Error;
   }
   return result;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Log_Message (const char *format, ...)
//-------------------------------------------------------------------------
{
   va_list        arguments;
   typ_return     result = Nominal;
   char           msg[1024];

   va_start (arguments, format);             // Initialize the parameters list handler
   vsprintf (msg, format, arguments);        // Write the provided text on the "msg" text string
   va_end (arguments);                       // Terminate the parameters list handler
   if (User_Message)
      User_Message (msg, "Message");         // Dispatch the "msg" message to the User
   if (Log_File)
   {
      fprintf (Log_File, "% 12.3f            : %s\n", Current_Epoch, msg); // Trace the EPOCH and the "msg" message in the Log File
      fflush (Log_File);                     // Flush the Log File to keep it in case of SW crash
   }
   else // In case the Log_File is not accessible trace the error
   {
      N_Errors++;
      result = Error;
   }
   return result;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Log_Warning (const char *format, ...)
//-------------------------------------------------------------------------
{
   va_list        arguments;
   typ_return     result = Nominal;
   char           msg[1024];

   va_start (arguments, format);             // Initialize the parameters list handler
   vsprintf (msg, format, arguments);        // Write the provided text on the "msg" text string
   va_end (arguments);                       // Terminate the parameters list handler
   if (User_Message)
      User_Message (msg, "Warning");         // Dispatch the "msg" message to the User
   if (Log_File)
   {
      fprintf  (Log_File, "% 12.3f >> WARNING : %s\n", Current_Epoch, msg); // Trace EPOCH & WARNING "msg" message in the Log File
      fflush (Log_File);                     // Flush the Log File to keep it in case of SW crash
      N_Warnings++;
   }
   else // In case the Log_File is not accessible trace the error
   {
      N_Errors++;
      result = Error;
   }
   return result;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Log_Error (const char *format, ...)
//-------------------------------------------------------------------------
{
   va_list        arguments;
   typ_return     result = Nominal;
   char           msg[1024];

   va_start (arguments, format);             // Initialize the parameters list handler
   vsprintf (msg, format, arguments);        // Write the provided text on the "msg" text string
   va_end (arguments);                       // Terminate the parameters list handler
   if (User_Message)
      User_Message (msg, "Error");           // Dispatch the "msg" message to the User
   if (Log_File)
   {
      fprintf  (Log_File, "% 12.3f >> ERROR :   %s\n", Current_Epoch, msg); // Trace EPOCH & ERROR "msg" message in the Log File
      fflush (Log_File);                     // Flush the Log File to keep it in case of SW crash
      N_Errors++;
   }
   else // In case the Log_File is not accessible trace the error
   {
      N_Errors++;
      result = Error;
   }
   return result;
}

//-------------------------------------------------------------------------
void GenericObject::Log_Debug (const char *format, ...)
//-------------------------------------------------------------------------
{
   va_list        arguments;
   char           msg[1024];

   va_start (arguments, format);             // Initialize the parameters list handler
   vsprintf (msg, format, arguments);        // Write the provided text on the "msg" text string
   va_end (arguments);                       // Terminate the parameters list handler
   if (Debug_File)
   {
      if (User_Message)
         User_Message (msg, "Debug");        // Dispatch the "msg" message to the User
      fprintf  (Debug_File, "% 12.3f            : %s\n", Current_Epoch, msg); // Trace the EPOCH and the "msg" message in the Debug File
      fflush (Debug_File);                   // Flush the Debug File to keep it in case of SW crash
   }
}

//-------------------------------------------------------------------------
GenericObject::typ_DataPublish GenericObject::Get_DataPublish (unsigned int i)
//-------------------------------------------------------------------------
{
   typ_DataPublish   result;

   if (int(i) < N_PublishData && P_PublishData)
      result = P_PublishData[i];
   else
      memset (&result, '\0', sizeof(typ_DataPublish));
   return result;
}

//-------------------------------------------------------------------------
double* GenericObject::Get_Pointer_X_dot (int ix)
//-------------------------------------------------------------------------
{
   if (ix >= 0 && ix < nx)
      return (pXdot + ix);
   Log_Error ("%s.Get_Pointer_X_dot : \"ix\" (%i) shall be in the Dynamic Status size range [ 0 .. %i-1 ]", Name, ix, nx);
   return NULL;
}

//-------------------------------------------------------------------------
double* GenericObject::Get_Pointer_X (int ix)
//-------------------------------------------------------------------------
{
   if (ix >= 0 && ix < nx)
      return (pX + ix);
   Log_Error ("%s.Get_Pointer_X : \"ix\" (%i) shall be in the Dynamic Status size range [ 0 .. %i-1 ]", Name, ix, nx);
   return NULL;
}

//-------------------------------------------------------------------------
double* GenericObject::Get_Pointer_Y (int iy)
//-------------------------------------------------------------------------
{
   if (iy >= 0 && iy < ny)
      return (pY + iy);
   Log_Error ("%s.Get_Pointer_Y : \"iy\" (%i) shall be in the output array Y size range [ 0 .. %i-1 ]", Name, iy, ny);
   return NULL;
}

//-------------------------------------------------------------------------
double* GenericObject::Get_Pointer_U (int iu)
//-------------------------------------------------------------------------
{
   if (iu >= 0 && iu < nu)
      return (pU + iu);
   Log_Error ("%s.Get_Pointer_U : \"iu\" (%i) shall be in the input array U size range [ 0 .. %i-1 ]", Name, iu, nu);
   return NULL;
}

//-------------------------------------------------------------------------
double GenericObject::Get_Value_U (int iu)
//-------------------------------------------------------------------------
{
   if (iu >= 0 && iu < nu)
      return pU[iu];
   Log_Error ("%s.Get_Value_U : \"iu\" (%i) shall be in the input array U size range [ 0 .. %i-1 ]", Name, iu, nu);
   return 0.0;
}

//-------------------------------------------------------------------------
double GenericObject::Get_Value_Y (int iy)
//-------------------------------------------------------------------------
{
   if (iy >= 0 && iy < ny)
      return pY[iy];
   Log_Error ("%s.Get_Value_Y : \"iy\" (%i) shall be in the output array Y size range [ 0 .. %i-1 ]", Name, iy, ny);
   return 0.0;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Get_Connected_Inputs ()
//-------------------------------------------------------------------------
{
   int      i;

   if (pY2U) // If at least one output-to-input connection (inter-objects link) has been defined then
   {
      for (i = 0; i < nu; i++)         // loop on all the input components
         if (pY2U[i].pValue)           // if the input U[i] is linked to another object output Y[k] then
            pU[i] = *pY2U[i].pValue;   // assign to the input U[i] the value of the joined double data (Y[k])
   }
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Execute_Model_Command (const char* model_command_string)
//-------------------------------------------------------------------------
{
   char        *command_string;
   typ_return  result;

   command_string = new char [strlen(Name) + strlen(model_command_string) + 16];
   if (!command_string)
   {
      Log_Error ("Execute_Model_Command : unable to allocate the RAM to store the command \"%s.%s\"", Get_Name(), model_command_string);
      return Error;
   }
   sprintf (command_string, "%s.%s", Get_Name(), model_command_string);
   result = Execute_Command (command_string);
   delete [] command_string;
   return result;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Serial_CreateLinks (const int N_links)
//-------------------------------------------------------------------------
{
// Check whether the Digital Serial Link array already exists
   if (serial_link)
   {
      Log_Error ("Serial_CreateLinks : serial links array multiple declaration; \"%s.serial_link[%i]\" already exists, it cannot be created twice", Get_Name(), N_serial_links);
      return Error;
   }

// Check the number of Digital Serial Link to be allocated
   if (N_links < 1)
   {
      Log_Error ("Serial_CreateLinks : the serial links array size shall be >= 1 \"%s.serial_link[%i]\"", Get_Name(), N_links);
      return Error;
   }

// Allocate the required Digital Serial Links
   serial_link = new typ_serial_link_channel[N_links];
   if (!serial_link)
   {
      Log_Error ("Serial_CreateLinks : unable to allocate the RAM to store the serial links array \"%s.serial_link[%i]\"", Get_Name(), N_links);
      return Error;
   }
   memset (serial_link, '\0', N_links*sizeof(typ_serial_link_channel));
   N_serial_links = N_links;
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Serial_TransmitData (const int channel, const int DataWordCount, unsigned short *pDataWords)
//-------------------------------------------------------------------------
{
// Check the Digital Serial Link channel is in the range of the allocate ones
   if (channel < 0 || N_serial_links <= channel)
   {
      Log_Error ("Serial_TransmitData : link channel out-of-range; \"%s.serial_link[%i]\", shall be in the range [ 0 .. %i ]", Get_Name(), channel, N_serial_links-1);
      return Error;
   }

// Check the Digital Serial Link channel has been connected to a Partner-Object
   if (!serial_link[channel].p_Other)
   {
      Log_Error ("Serial_TransmitData : unconnected link channel; \"%s.serial_link[%i]\" has never been connected to any Partner-Object", Get_Name(), channel);
      return Error;
   }

// Execute the slave Partner-Object virtual method "Serial_ProcessData" for Processing the unsolicited data receive by "this" master via digital Serial Link
   return serial_link[channel].p_Other->Serial_ProcessData (serial_link[channel].ch_Other, DataWordCount, pDataWords);
}

//-------------------------------------------------------------------------
typ_return GenericObject::Serial_TransmitData (const int channel, const int DataByteCount, char *pDataBytes)
//-------------------------------------------------------------------------
{
// Check the Digital Serial Link channel is in the range of the allocate ones
   if (channel < 0 || N_serial_links <= channel)
   {
      Log_Error ("Serial_TransmitData : link channel out-of-range; \"%s.serial_link[%i]\", shall be in the range [ 0 .. %i ]", Get_Name(), channel, N_serial_links-1);
      return Error;
   }

// Check the Digital Serial Link channel has been connected to a Partner-Object
   if (!serial_link[channel].p_Other)
   {
      Log_Error ("Serial_TransmitData : unconnected link channel; \"%s.serial_link[%i]\" has never been connected to any Partner-Object", Get_Name(), channel);
      return Error;
   }

// Execute the slave Partner-Object virtual method "Serial_ProcessData" for Processing the unsolicited data receive by "this" master via digital Serial Link
   return serial_link[channel].p_Other->Serial_ProcessData (serial_link[channel].ch_Other, DataByteCount, pDataBytes);
}

//-------------------------------------------------------------------------
typ_return GenericObject::Serial_ReceiveData (const int channel, const int DataWordCount, unsigned short *pDataWords)
//-------------------------------------------------------------------------
{
// Check the Digital Serial Link channel is in the range of the allocate ones
   if (channel < 0 || N_serial_links <= channel)
   {
      Log_Error ("Serial_ReceiveData : link channel out-of-range; \"%s.serial_link[%i]\", shall be in the range [ 0 .. %i ]", Get_Name(), channel, N_serial_links-1);
      return Error;
   }

// Check the Digital Serial Link channel has been connected to a Partner-Object
   if (!serial_link[channel].p_Other)
   {
      Log_Error ("Serial_ReceiveData : unconnected link channel; \"%s.serial_link[%i]\" has never been connected to any Partner-Object", Get_Name(), channel);
      return Error;
   }

// Execute the slave Partner-Object virtual method "Serial_ProduceData" for Generating the solicited data requested by "this" master via digital Serial Link
   return serial_link[channel].p_Other->Serial_ProduceData (serial_link[channel].ch_Other, DataWordCount, pDataWords);
}

//-------------------------------------------------------------------------
typ_return GenericObject::Serial_ReceiveData (const int channel, const int DataByteCount, char *pDataBytes)
//-------------------------------------------------------------------------
{
// Check the Digital Serial Link channel is in the range of the allocate ones
   if (channel < 0 || N_serial_links <= channel)
   {
      Log_Error ("Serial_ReceiveData : link channel out-of-range; \"%s.serial_link[%i]\", shall be in the range [ 0 .. %i ]", Get_Name(), channel, N_serial_links-1);
      return Error;
   }

// Check the Digital Serial Link channel has been connected to a Partner-Object
   if (!serial_link[channel].p_Other)
   {
      Log_Error ("Serial_ReceiveData : unconnected link channel; \"%s.serial_link[%i]\" has never been connected to any Partner-Object", Get_Name(), channel);
      return Error;
   }

// Execute the slave Partner-Object virtual method "Serial_ProduceData" for Generating the solicited data requested by "this" master via digital Serial Link
   return serial_link[channel].p_Other->Serial_ProduceData (serial_link[channel].ch_Other, DataByteCount, pDataBytes);
}

//-------------------------------------------------------------------------
typ_return GenericObject::Set_Rt1553_Address (int Rt1553_Address, bool allow_shared_Rt1553_Address_flag)
//-------------------------------------------------------------------------
{
   GenericObject  *model;

   if (Rt1553_Address < 1 || 31 < Rt1553_Address)
   {
       Log_Error ("%s.Set_Rt1553_Address : Invalid Bus 1553 Remote Terminal Address \"%d\"", Name, Rt1553_Address);
       return Error;
   }

   model = Get_First();
   while (model) // Iterate on all the registered Objects
   {
      if (Rt1553_Address == model->Rt1553_RemoteTerminalAddress)
      {
         if (allow_shared_Rt1553_Address_flag)
            Log_Warning ("%s.Set_Rt1553_Address : The Bus 1553 Remote Terminal Address \"%d\" has been already assigned to \"%s\"", Name, Rt1553_Address, model->Name);
         else
         {
            Log_Error ("%s.Set_Rt1553_Address : The Bus 1553 Remote Terminal Address \"%d\" has been already reserved by \"%s\"", Name, Rt1553_Address, model->Name);
            return Error;
         }
      }
      model = model->Get_Next();
   }

   Rt1553_RemoteTerminalAddress = (unsigned char)Rt1553_Address;
   memset (TxModifier, '\0', 32*sizeof(typ_Rt1553_word));
   memset (RxModifier, '\0', 32*sizeof(typ_Rt1553_word));
   memset (McModifier, '\0', 32*sizeof(typ_Rt1553_word));

   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Modify_Rt1553_Buffer (typ_Rt1553_word *Rt1553_word, unsigned short pDataWords[32])
//-------------------------------------------------------------------------
{
    int i;

    if (Rt1553_RemoteTerminalAddress)
        for (i = 0; i < 32; i++)
            if (Rt1553_word->forced_flag[i])
                pDataWords[i] = Rt1553_word->word_value[i];

    return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Bc1553_ReceiveData (int Rt1553_Address, const unsigned char Subaddress, const unsigned char DataWordCount, unsigned short pDataWords[32])
//-------------------------------------------------------------------------
{
   /******************************************************************************
   * Loop on all objects dispatching the 1553 RX request
   * ***************************************************************************/
   GenericObject  *model = GenericObject::Get_First ();
   while ( model ) // Iterate on all the registered Objects
   {
      if( model->Rt1553_RemoteTerminalAddress == Rt1553_Address){
         // Perform Rt1553_ReceiveData request
         model->Rt1553_ReceiveData (Subaddress, DataWordCount, pDataWords);
        // LOG request on dump file
        if( Log_1553_File != NULL ){
           fprintf(Log_1553_File,"%s.ReceiveData %d,%d\t\t", model->Get_Name(), Rt1553_Address, Subaddress);
           for(int i=0; i<DataWordCount; i++)
              fprintf(Log_1553_File,"%04X\t", pDataWords[i]);
           fprintf(Log_1553_File,"\n");
        }
        // Request served
        break;
      }
      model = model->Get_Next();
   }
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Bc1553_TransmitData (int Rt1553_Address, const unsigned char Subaddress, const unsigned char DataWordCount, unsigned short pDataWords[32])
//-------------------------------------------------------------------------
{
   /******************************************************************************
   * Loop on all objects dispatching the 1553 TX request
   * ***************************************************************************/
   GenericObject  *model = GenericObject::Get_First ();
   while ( model ) // Iterate on all the registered Objects
   {
      if( model->Rt1553_RemoteTerminalAddress == Rt1553_Address){
         // Perform Rt1553_ReceiveData request
         model->Rt1553_TransmitData (Subaddress, DataWordCount, pDataWords);
         // LOG request on dump file
         if( Log_1553_File != NULL ){
           fprintf(Log_1553_File,"%s.TransmitData %d,%d\t\t", model->Get_Name(), Rt1553_Address, Subaddress);
           for(int i=0; i<DataWordCount; i++)
              fprintf(Log_1553_File,"%04X\t", pDataWords[i]);
           fprintf(Log_1553_File,"\n");
         }
         // Request served
         break;
      }
      model = model->Get_Next();
   }
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Bc1553_ReceiveModeCommand (int Rt1553_Address, const typ_B1553_TxRx TxRx, const typ_B1553_ModeCode ModeCode, unsigned short pDataWord[32])
//-------------------------------------------------------------------------
{
   /******************************************************************************
   * Loop on all objects dispatching the 1553 ModeCommand request
   * ***************************************************************************/
   GenericObject  *model = GenericObject::Get_First ();
   while ( model ) // Iterate on all the registered Objects
   {
      if( model->Rt1553_RemoteTerminalAddress == Rt1553_Address){
         // Perform Rt1553_ReceiveModeCommand request
         model->Rt1553_ReceiveModeCommand (TxRx, ModeCode, pDataWord);
      }
      model = model->Get_Next();
   }
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Command_Switch_ON ()
//-------------------------------------------------------------------------
{
   OFF_ON_Switch_Status = On;
   PowerLoad            = PowerLoad_at_Switch_ON;
   if (Rt1553_RemoteTerminalAddress)
      Rt1553_Set_RT_Status (Enable);
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return GenericObject::Command_Switch_OFF ()
//-------------------------------------------------------------------------
{
   OFF_ON_Switch_Status = Off;
   PowerLoad            = 0.0;
   if (Rt1553_RemoteTerminalAddress)
      Rt1553_Set_RT_Status (Disable);
   return Nominal;
}

//-------------------------------------------------------------------------
void GenericObject::Synchronizes_All_Rt_Status(typ_return &result)
//-------------------------------------------------------------------------
{
   result = Nominal;

   GenericObject  *model;

   model = GenericObject::Get_First ();
   while (result == Nominal && model ) // Iterate on all the registered Objects
   {
      if(model->Rt1553_RemoteTerminalAddress)
      {
         if (model->Get_switch_status() == Off)
            model->Rt1553_Set_RT_Status(Disable);
         else
            model->Rt1553_Set_RT_Status(Enable);
      }
      model = model->Get_Next();
   }
}

//-------------------------------------------------------------------------
typ_return GenericObject::PublishNewData (void *DataPointer, typ_DataType DataType, int DataSize, int DataNumber, const char *DataName, const char *DataUnit)
//-------------------------------------------------------------------------
{
   int                  i;
   typ_DataPublish      *Old_PublishData;

// Check the DataName input string
   if (!DataName)
   {
      Log_Error ("\"%s\".PublishNewData : DataName has not been provided (NULL pointer)", Name);
      return Error;
   }
   if (!strlen(DataName))
   {
      Log_Error ("\"%s\".PublishNewData : the DataName string is empty", Name);
      return Error;
   }
   if (strlen(DataName) > 32)
   {
      Log_Error ("\"%s\".PublishNewData : the DataName string exceedes 32 characters (\"%s\")", Name, DataName);
      return Error;
   }
   for (i = 0; i < N_PublishData && strcmp(DataName,P_PublishData[i].name); i++)
   {};
   if (i < N_PublishData)
   {
      Log_Error ("\"%s\".PublishNewData : attempt to register the already registered DataName (\"%s\"), it shall be unique", Name, DataName);
      return Error;
   }
   if (DataNumber == 0)
   {
      Log_Warning ("\"%s\".PublishNewData : \"%s\" data array is empty (i.e. DataNumber = 0), not published", Name, DataName);
      return Nominal;
   }
   if (DataNumber < 0)
   {
      Log_Error ("\"%s\".PublishNewData : \"%s\" wrong number of array data (i.e. DataNumber = %i)", Name, DataName, DataNumber);
      return Error;
   }

// Check the DataUnit input string
   if (strlen(DataUnit) > 14)
   {
      Log_Error ("\"%s\".PublishNewData : \"%s\" the DataUnit string exceedes 14 characters (\"%s\")", Name, DataName, DataUnit);
      return Error;
   }

// Allocate the memory for storing published data, based on 32-items batch
   if (!(N_PublishData % 32))
   {
      Old_PublishData = P_PublishData;
      P_PublishData   = new typ_DataPublish[N_PublishData+32];
      if (!P_PublishData)
      {
         Log_Error ("Unable to allocate the RAM for storing published data \"%s\"", Name);
         return Error;
      }
      memset (P_PublishData, '\0', (N_PublishData+32)*sizeof(typ_DataPublish));
      if (Old_PublishData)
      {
         memcpy (P_PublishData, Old_PublishData, N_PublishData*sizeof(typ_DataPublish));
         delete [] Old_PublishData;
      }
   }

// Store the input new data
   memset (P_PublishData+N_PublishData, '\0', sizeof(typ_DataPublish));
   P_PublishData[N_PublishData].pParent   = this;
   P_PublishData[N_PublishData].p         = DataPointer;
   P_PublishData[N_PublishData].type      = DataType;
   P_PublishData[N_PublishData].size      = DataSize;
   P_PublishData[N_PublishData].number    = DataNumber;
   strcpy (P_PublishData[N_PublishData].name, DataName);
   if (DataUnit)
      strcpy (P_PublishData[N_PublishData].unit, DataUnit);
   N_PublishData++;

   Log_Message ("\"%s\".PublishNewData : data \"%s[%i]\" has been registered for publishing purposes [ptr: %p ; size: %d ; type: %d] ", Name, DataName, DataNumber, DataPointer, DataSize, DataType);
   return Nominal;
}

//-------------------------------------------------------------------------
double GenericObject::GetValue (char *DataName, int offset, char *DataUnitBuffer)
//-------------------------------------------------------------------------
{
   int      i;
   double   result;

   if (DataUnitBuffer)
      *DataUnitBuffer = '\0';
   result = 0.0;

   for (i = 0; i < N_PublishData && strcmp(DataName,P_PublishData[i].name); i++)
   {};
   if (i == N_PublishData)
      Log_Warning ("\"%s\".GetValue : C++ DataName (\"%s\") has not been registered for publishing", Name, DataName);
   else
   {
      if (offset < 0 || P_PublishData[i].number <= offset)
         Log_Warning ("\"%s\".GetValue : (\"%s\") offset \"%i\" exceedes the array limits [0..%i]", Name, DataName, offset, P_PublishData[i].number);
      else
      {
         switch(P_PublishData[i].type)
         {
            case typ_bool  :  result = (double)(*(((bool*)            P_PublishData[i].p) + offset)); break;
            case typ_char  :  result = (double)(*(((char*)            P_PublishData[i].p) + offset)); break;
            case typ_uchar :  result = (double)(*(((unsigned char*)   P_PublishData[i].p) + offset)); break;
            case typ_short :  result = (double)(*(((short*)           P_PublishData[i].p) + offset)); break;
            case typ_ushort:  result = (double)(*(((unsigned short*)  P_PublishData[i].p) + offset)); break;
            case typ_int   :  result = (double)(*(((int*)             P_PublishData[i].p) + offset)); break;
            case typ_uint  :  result = (double)(*(((unsigned int*)    P_PublishData[i].p) + offset)); break;
            case typ_float :  result = (double)(*(((float*)           P_PublishData[i].p) + offset)); break;
            case typ_double:  result =         (*(((double*)          P_PublishData[i].p) + offset)); break;
         }
         if (DataUnitBuffer)
            strcpy (DataUnitBuffer, P_PublishData[i].unit);
      }
   }
   return result;
}

//-------------------------------------------------------------------------
typ_return GenericObject::SetValue (char *DataName, int offset, double DataValue)
//-------------------------------------------------------------------------
{
   int      i;

   for (i = 0; i < N_PublishData && strcmp(DataName,P_PublishData[i].name); i++)
   {};
   if (i == N_PublishData)
   {
      Log_Warning ("\"%s\".SetValue : C++ DataName (\"%s\") has not been registered for publishing", Name, DataName);
      return Error;
   }

   if (offset < 0 || P_PublishData[i].number <= offset)
   {
      Log_Warning ("\"%s\".SetValue : (\"%s\") offset \"%i\" exceedes the array limits [0..%i]", Name, DataName, offset, P_PublishData[i].number);
      return Error;
   }

   switch(P_PublishData[i].type)
   {
      case typ_bool  :  *(((bool*)            P_PublishData[i].p) + offset) = (bool)           DataValue; break;
      case typ_char  :  *(((char*)            P_PublishData[i].p) + offset) = (char)           DataValue; break;
      case typ_uchar :  *(((unsigned char*)   P_PublishData[i].p) + offset) = (unsigned char)  DataValue; break;
      case typ_short :  *(((short*)           P_PublishData[i].p) + offset) = (short)          DataValue; break;
      case typ_ushort:  *(((unsigned short*)  P_PublishData[i].p) + offset) = (unsigned short) DataValue; break;
      case typ_int   :  *(((int*)             P_PublishData[i].p) + offset) = (int)            DataValue; break;
      case typ_uint  :  *(((unsigned int*)    P_PublishData[i].p) + offset) = (unsigned int)   DataValue; break;
      case typ_float :  *(((float*)           P_PublishData[i].p) + offset) = (float)          DataValue; break;
      case typ_double:  *(((double*)          P_PublishData[i].p) + offset) =                  DataValue; break;
   }

   return Nominal;
}

//-------------------------------------------------------------------------
GenericObject::typ_DataPublish* GenericObject::GetPublishedDataPointer (void *in_DataPointer)
//-------------------------------------------------------------------------
{
   int               i;
   typ_DataPublish   *pData = NULL;
   GenericObject     *model = Get_First();

// Iterate on all the registered Objects
   while (model && !pData)
   {

// Check wether "in_DataPointer" belongs to a published data
   for (i = 0; i < model->N_PublishData && !pData; i++)
      if (model->P_PublishData[i].p <= in_DataPointer
       && (char*)in_DataPointer-(char*)model->P_PublishData[i].p < model->P_PublishData[i].size*model->P_PublishData[i].number
       && (((char*)in_DataPointer-(char*)model->P_PublishData[i].p) % model->P_PublishData[i].size) == 0)
         pData = model->P_PublishData + i;
      model = model->Get_Next();
   }

   return pData;
}

//-------------------------------------------------------------------------
typ_return GenericObject::GetPublishedDataName (void *in_DataPointer, char *out_NameBuffer)
//-------------------------------------------------------------------------
{
   typ_DataPublish   *pData;

   *out_NameBuffer = '\0';
   pData = GetPublishedDataPointer (in_DataPointer);
   if (!pData)
   {
      Log_Warning ("GetPublishedDataName : in_DataPointer (\"#%X\") RAM address does not match any published data", in_DataPointer);
      return Error;
   }

// Write into "out_NameBuffer" the data name
   sprintf (out_NameBuffer, "%s.%s", pData->pParent->Name, pData->name);

// Append to "out_NameBuffer" the data array index (1..N) MATLAB-like
   if (pData->number > 1)
      sprintf (out_NameBuffer+strlen(out_NameBuffer), "(%i)", (int)(1 + ((char*)in_DataPointer-(char*)pData->p)/pData->size));

// Append to "out_NameBuffer" the data measurement unit
   if (strlen(pData->unit))
      sprintf (out_NameBuffer+strlen(out_NameBuffer), " [%s]", pData->unit);

   return Nominal;
}

//-------------------------------------------------------------------------
double* GenericObject::Get_output_connection (int iOut)
//-------------------------------------------------------------------------
{
   GenericObject     *model;
   double            *pU;

// Check output is in model range
   if( iOut >= Get_ny() ){
      Log_Error("Get_output_connection: requested output index (%d) is out of range (ny=%d)",iOut,Get_ny());
      return NULL;
   }
// Loop on all model searching input connected to this model Y
   model = GenericObject::Get_First ();
   while (model) // Iterate on all the registered Objects
   {
      for (int i = 0; i < model->Get_nu(); i++){
         pU = model->Get_input_connection(i);
         if( pU && pU == Get_Pointer_Y(iOut) )
            return pU;
      }
      model = model->Get_Next();
   }
   return NULL;
}

// ----- Protected Method for publication of Generic Object variables

/** Protected Method to publish the Generic Object properties relevant for breakpoints*/
//-------------------------------------------------------------------------
typ_return GenericObject::Publish_Generic_Object (void)
//-------------------------------------------------------------------------
{
   typ_return  result = Nominal;
   int         i;
   char        pub_name[32];

   memset (pub_name, '\0', 32);

// ----- Static Model parameters, published only on Id == 1 occurrence

   if (Id == 1)
   {
      result &= Publish (Current_Epoch, "GO.current_epoch", "s");
   }

// ----- Standard Dynamics Model parameters

   result &= Publish (time_at_last_update,       "GO.time_at_last_update",       "s");
   result &= Publish (delta_time_at_last_update, "GO.delta_time_at_last_update", "s");
   if (pU_Forced)
   {
      for (i = 0; i < nu; i++)
      {
         sprintf (pub_name, "GO.pU_Forced_%d_actual_value", i);
         result &= Publish (pU_Forced[i].actual_value, pub_name, "");
         sprintf (pub_name, "GO.pU_Forced_%d_forced_value", i);
         result &= Publish (pU_Forced[i].forced_value, pub_name, "");
         sprintf (pub_name, "GO.pU_Forced_%d_forced_flag", i);
         result &= Publish (pU_Forced[i].forced_flag,  pub_name, "");
       }
   }
   if (pY_Forced)
   {
     for (i = 0; i < ny; i++)
      {
         sprintf (pub_name, "GO.pY_Forced_%d_actual_value", i);
         result &= Publish (pY_Forced[i].actual_value, pub_name, "");
         sprintf (pub_name, "GO.pY_Forced_%d_forced_value", i);
         result &= Publish (pY_Forced[i].forced_value, pub_name, "");
         sprintf (pub_name, "GO.pY_Forced_%d_forced_flag", i);
         result &= Publish (pY_Forced[i].forced_flag,  pub_name, "");
       }
   }

// ----- Power Standard Model parameters

   result &= Publish ((int&)OFF_ON_Switch_Status,  "GO.OFF_ON_Switch_Status",   ""); // Note:  sizeof(OFF_ON_Switch_Status) = 4
   result &= Publish (Power_Supplied_Flag,         "GO.Power_Supplied_Flag",    "");
   result &= Publish (PowerLoad,                   "GO.PowerLoad",              "W");
   result &= Publish (PowerLoad_at_Switch_ON,      "GO.PowerLoad_at_Switch_ON", "W");

// ----- MIL BUS 1553 Remote Terminal parameters

   result &= Publish (Rt1553_RemoteTerminalAddress,         "GO.Rt1553_RemoteTerminalAddress", "");  // HW Model parameter: MIL BUS 1553 Model <b>Remote Terminal Address</b>
   result &= Publish (&Enable_SubAddress_flag[0][0],  32*2, "GO.Enable_SubAddress_flag",       "");  // HW Model parameter: MIL BUS 1553 Model <b>SubAddress Transaction Enabling</b> array ([RX/TX][SAid])
   result &= Publish (Enable_ModeCode_flag,           32,   "GO.Enable_ModeCode_flag",         "");  // HW Model parameter: MIL BUS 1553 Model <b>ModeCode Transaction Enabling</b> array ([MCid])
   for (i = 0; i < 32; i++)
   {
      sprintf (pub_name, "GO.TxModifier_%d_word_value", i);
      result &= Publish (TxModifier[i].word_value,  32, pub_name,  "");     // HW Model parameter: MIL BUS 1553 Model <b>Forced SubAddres Tx-Transactions</b> data ([SAid])
      sprintf (pub_name, "GO.TxModifier_%d_forced_flag", i);
      result &= Publish (TxModifier[i].forced_flag, 32, pub_name,  "");
      //
      sprintf (pub_name, "GO.RxModifier_%d_word_value", i);
      result &= Publish (RxModifier[i].word_value,  32, pub_name,  "");     // HW Model parameter: MIL BUS 1553 Model <b>Forced SubAddres Rx-Transactions</b> data ([SAid])
      sprintf (pub_name, "GO.RxModifier_%d_forced_flag", i);
      result &= Publish (RxModifier[i].forced_flag, 32, pub_name,  "");
      //
      sprintf (pub_name, "GO.McModifier_%d_word_value", i);
      result &= Publish (McModifier[i].word_value,  32, pub_name,  "");     // HW Model parameter: MIL BUS 1553 Model <b>Forced SubAddres ModeCode Transactions</b> data ([MCid])
      sprintf (pub_name, "GO.McModifier_%d_forced_flag", i);
      result &= Publish (McModifier[i].forced_flag, 32, pub_name,  "");
   }

// ----- Object variable for Update_Model scheduling

   result &= Publish (LastUpdateExecTime, "GO.LastUpdateExecTime",   "s");  // Time at last Update execution
   result &= Publish (UpdateStepTime,     "GO.UpdateStepTime",       "s");  // Time step for Update function execution <b>if <=0 execute ALWAYS</b>

   return result;
}
