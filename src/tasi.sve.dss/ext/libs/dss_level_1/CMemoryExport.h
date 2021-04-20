/******************************************************************************

    PROJECT:      CSG Constellation Simulator
    FILE:         CMemoryExport
    RELEASE:      1.00
    AUTHOR:       Daniele Pagnozzi,  Danilo Andreetti, Luigi Galvagni
    DATE:         11/10/2017

    OVERVIEW
    ========
        This class allows coping a quantity of data within a publishable
        static variable.
        Such machinery allows saving and restore of models with dynamic
        variables

    HOW TO USE
    ========
        In Level2:
                * inherit CMemoryExport
                * implement virtual methods dedicated to saving and restoring data
                * in these methods, use functions ExportData to make a copy of the
                  data to be saved or ImportData to load saved data
                  Third argument of these functions is an optional flag (default
                  value is false). When such flag is set as true data will be
                  output in a binary file

       In Level3:
                * Publish variable DataStorage before PublishAllData method is
                  called, e.g. :
                      Publish(DataStorage,DataSize, PublicName, "bytes" );
                   -Publish is a protected method and cannot be used within
                    CMemoryExport class -

    CHANGE LOG

    ==========

******************************************************************************/

#ifndef _CMemoryExport_H
#define _CMemoryExport_H

#include <stdio.h>
#include "stdint.h"             // includes uint8_t
#include "GenericObject.h"      // used for Log messages

#define MAX_SIZE_SAVE_BUFFER     0x800000

/** Global utility to write & flush a User message on a User provided file */
void fprintfile (FILE* f /*! User file */, const char *format, ...);

class CMemoryExport
{

/*-------------------------------------------------------------------------
   Data declarations
-------------------------------------------------------------------------*/

public:

   static FILE*       save_debug;
   static FILE*       restore_debug;

   static uint8_t*    DataStorage;         //!< Public static RAM buffer for binary sequential data saving (it shall be public for publishing purposes)
   static int         NestingLevel;        //!< Public static counter to trace the nesting level in the serial data management

private:

   static uint8_t*    pCurrent;            //!< Private static pointer to the current position for sequential buffer access

/*-------------------------------------------------------------------------
   Function declarations
-------------------------------------------------------------------------*/

public:

/** Class <b>Constructor</b> (unique), allocates the <b>static RAM buffer</b> if it has not been allocated yet */
   CMemoryExport (int MaxDataSize = 0 /*! Optional parameter is unused, it has been kept only for compatibility with the previous version */);

/** Public Static Method to <b>rewind</b> the RAM buffer, i.e. to move the <b>current pointer</b> on the top of the RAM buffer */
   static void Reset (bool clear_flag = false /*! Optional flag allows <b>clearing</b> the content of the wwhole RAM buffer too */);

/** Public Static Method for <b>sequentially writing</b> the user data into the RAM buffer. It returns <b>true</b> if the operation is succesfully completed */
   static bool ExportData (void* ptrData /*! Pointer to the user data to be written out */, int sizeofData /*! Size [byte] of the user data to be written */, const char* description = NULL /*! Field description text */);

/** Public Static Method for <b>sequentially reading</b> the user data from the RAM buffer. It returns <b>true</b> if the operation is succesfully completed */
   static bool ImportData (void* ptrData /*! Pointer to the user buffer where the read data will be stored */, int sizeofData /*! Size [byte] of the user data to be read */, const char* description = NULL /*! Field description text */);

protected:

/** <b>Virtual</b> Protected Method to be called by Level3 on the occurence of the "SAVE" event */
   virtual bool Save() { return true; };

/** <b>Virtual</b> Protected Method to be called by Level3 on the occurence of the "RESTORE" event */
   virtual bool Restore() { return true; };

};

#endif
