#include "CMemoryExport.h"

FILE*       CMemoryExport::save_debug     = fopen ("/home/sdve/Desktop/save_debug.txt",    "wt");
FILE*       CMemoryExport::restore_debug  = fopen ("/home/sdve/Desktop/restore_debug.txt", "wt");

uint8_t*    CMemoryExport::DataStorage    = NULL;
int         CMemoryExport::NestingLevel   = 0;
uint8_t*    CMemoryExport::pCurrent       = NULL;

//-------------------------------------------------------------------------------
CMemoryExport::CMemoryExport (int /* MaxDataSize */)
//-------------------------------------------------------------------------------
{
// If the RAM buffer has not been allocated yet then allocate it and reset the content
   if (!DataStorage)
   {
      DataStorage = pCurrent = new uint8_t[MAX_SIZE_SAVE_BUFFER];
      if (!DataStorage)
         GenericObject::Log_Error("CMemoryExport constructor: Unable to allocate the RAM buffer (%d [bytes])\n", MAX_SIZE_SAVE_BUFFER);
      else
         memset (DataStorage, '\0', MAX_SIZE_SAVE_BUFFER);
   }
}

//-------------------------------------------------------------------------------
void CMemoryExport::Reset (bool clear_flag)
//-------------------------------------------------------------------------------
{
   if (DataStorage)
   {
      NestingLevel = 0;             // Reset to zero the nesting level
      pCurrent     = DataStorage;   // Restore the current pointer to the beginning of the RAM buffer
      if (clear_flag)               // If required clear the RAM buffer content
         memset (DataStorage, '\0', MAX_SIZE_SAVE_BUFFER);
   }
}

//-------------------------------------------------------------------------------
bool CMemoryExport::ExportData(void* ptrData, int sizeofData, const char* description)
//-------------------------------------------------------------------------------
{
   if (DataStorage)
   {
      if (pCurrent + sizeofData > DataStorage + MAX_SIZE_SAVE_BUFFER) // Check ther is enogh space in the RAM buffer
      {
         GenericObject::Log_Error("CMemoryExport::ExportData: Data @0x%08X size %d exceedes the buffer allocated memory (%d)\n", ptrData, sizeofData, MAX_SIZE_SAVE_BUFFER);
         return false;
      }
      memcpy (pCurrent, ptrData, sizeofData);   // Copy the user data into the RAM buffer
      pCurrent += sizeofData;                   // Shift forward the current pointer to the next not-written-yet RAM buffer location
// Log the written data into the save_debug log file, if required
      if (save_debug)
      {
         fprintf (save_debug, "% 2i\t", CMemoryExport::NestingLevel);
         for (int i = 0; i < CMemoryExport::NestingLevel; i++)
            fprintf (save_debug, "\t");
         fprintf (save_debug, "size = %d\tdata = ", sizeofData);
         if (sizeofData < 64)
            for (int i = 0; i < sizeofData; i++)
               fprintf (save_debug, "%02X", ((unsigned char*)ptrData)[i]);
         if (description)
         {
            fprintf (save_debug, "\t\t\t%s", description);
            if (sizeofData == 1 && strstr (description, "string"))
               fprintf (save_debug, "\t\t|%c|", ((unsigned char*)ptrData)[0]);
         }
         fprintf (save_debug, "\n");
         fflush(save_debug);
      }
      return true;
   }
   return false;
}

//-------------------------------------------------------------------------------
bool CMemoryExport::ImportData(void* ptrData, int sizeofData, const char* description)
//-------------------------------------------------------------------------------
{
   if (DataStorage)
   {
      if (pCurrent + sizeofData > DataStorage + MAX_SIZE_SAVE_BUFFER) // Check ther is enogh data in the RAM buffer
      {
         GenericObject::Log_Error("CMemoryExport::ImportData: Data @0x%08X size %d exceedes the buffer allocated memory (%d)\n", ptrData, sizeofData, MAX_SIZE_SAVE_BUFFER);
         return false;
      }
      memcpy (ptrData, pCurrent, sizeofData);   // Copy the RAM buffer data into the user buffer
      pCurrent += sizeofData;                   // Shift forward the current pointer to the next not-read-yet RAM buffer location
// Log the read data into the restore_debug log file, if required
      if (restore_debug)
      {
         fprintf (restore_debug, "% 2i\t", CMemoryExport::NestingLevel);
         for (int i = 0; i < CMemoryExport::NestingLevel; i++)
            fprintf (restore_debug, "\t");
         fprintf (restore_debug, "size = %d\tdata = ", sizeofData);
         if (sizeofData < 64)
            for (int i = 0; i < sizeofData; i++)
               fprintf (restore_debug, "%02X", ((unsigned char*)ptrData)[i]);
         if (description)
        {
            fprintf (restore_debug, "\t\t\t%s", description);
            if (sizeofData == 1 && strstr (description, "string"))
               fprintf (restore_debug, "\t\t|%c|", ((unsigned char*)ptrData)[0]);
         }
         fprintf (restore_debug, "\n");
         fflush(restore_debug);
      }
      return true;
   }
   return false;
}

#include <stdarg.h>

//-------------------------------------------------------------------------
void fprintfile (FILE* f, const char *format, ...)
/*
   This global procedure writes the User message on the User provide file.
   It uses the same interface as the "C" function "fprintf" and similar.
*/
//-------------------------------------------------------------------------
{
   va_list arguments;

// If the FILE pointer == NULL then do nothing
   if (f)
   {
// If the FILE pointer is "save_debug" or "restore_debug" then trace the NestingLevel at the beginning of the new line
      if (f == CMemoryExport::save_debug || f == CMemoryExport::restore_debug)
      {
         fprintf (f, "% 2i\t", CMemoryExport::NestingLevel);
         for (int i = 0; i < CMemoryExport::NestingLevel; i++)
            fprintf (f, "\t");
      }
      va_start (arguments, format);     // Initialize the parameters list handler
      vfprintf (f, format, arguments);  // Print on the file the User input
      va_end (arguments);               // Terminate the parameters list handler
      fflush (f);                       // Flush the file to keep data in case of SW crash
   }
}
