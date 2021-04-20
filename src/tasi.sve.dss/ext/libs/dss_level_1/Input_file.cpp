/******************************************************************************

    PROJECT:      DSS - S/C Dynamics Simulation Software
    FILE:         input_file.h
    RELEASE:      0.00
    AUTHOR:       Luigi Galvagni
    DATE:         2008/10/17

    OVERVIEW
    ========
    Source code for the input file hadler

    CHANGE LOG
    ==========

   2012/03/12 L.Galvagni, F.Sabbioni fixed wrong implementation of "load" for bool.
              When the Object name is provided it shall be managed with the same method
              used without the name.

   Luigi Galvagni 2013/09/17 Removed dependency from the "std::ifstream" C++ class,
                             managed using FILE* C-like, since is more portable.
                             Added the default constructor and destructor;

   LG/D.Pascucci 2013/09/26  Removed "readMultilineSymbol" methods since reundant with respect to
                             the "readSymbol" ones. Inside the method "readSymbol" the special branch
                             (Ntokens == 1) is now constrained also to the "enable_tokenizer" flag.

******************************************************************************/

#include <string.h>

#include "Input_file.h"
#include "GenericObject.h"

//-------------------------------------------------------------------------
InputFile::InputFile ()
//-------------------------------------------------------------------------
{
   memset (this, '\0', sizeof(InputFile));
}

//-------------------------------------------------------------------------
InputFile::~InputFile ()
//-------------------------------------------------------------------------
{
   if (f) close ();
}

//-------------------------------------------------------------------------
typ_return InputFile::open (const char* filename)
//-------------------------------------------------------------------------
{
// Reset buffer memory pointer
   enable_tokenizer = true;
   buffer = 0;
   strcpy (current_input_file_name, (filename ? filename : INPUTFILE_DEFAULT));

// Write log file header
   GenericObject::Log_Write ("\n================================================================================\n");
   GenericObject::Log_Write ("Opening file \"%s\" for model initialization reading\n", current_input_file_name);
   GenericObject::Log_Write ("================================================================================\n\n");

// Open input file as a stream
   f = fopen (current_input_file_name, "rb");
   if (f)
   {
      error = 0;
      GenericObject::Log_Write ("Input file \"%s\" succesfully open\n\n", current_input_file_name);
   }
   else
   {
      error = 1;
      GenericObject::Log_Error ("Unable to open for reading the input file \"%s\"\n\n", current_input_file_name);
      *current_input_file_name = '\0';
   }
   return (error ? Error : Nominal);
}

//-------------------------------------------------------------------------
typ_return InputFile::close ()
//-------------------------------------------------------------------------
{
   if (f)
      fclose (f);
   f = NULL;
   GenericObject::Log_Write ("\n================================================================================\n");
   if (current_input_file_name)
      GenericObject::Log_Write ("Input file \"%s\" succesfully closed\n", current_input_file_name);
   else
      GenericObject::Log_Write ("The input file has been closed\n");
   GenericObject::Log_Write ("================================================================================\n\n");
   *current_input_file_name = '\0';
   if (buffer)
   {
      delete [] buffer;
      buffer = 0;
   }
   enable_tokenizer = true;
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return InputFile::load (double v[], const char* VarName, int VarSize, bool mustexist)
//-------------------------------------------------------------------------
{
   char        *c;
   typ_return  found;
   int         i, old_error = error;

// Read data vector from file
   found = readSymbol (VarName, VarSize, mustexist);
   if (found == Nominal && old_error == error) for (i = 0; i < VarSize; i++)
   {
      c = buffer + TOKEN_SIZE * i;
      if (strspn (c, REAL_NUMBERS) < strlen (c))
      {
         GenericObject::Log_Error ("Symbol \"%s\" : illegal character in NUMERIC field \"%s\"", VarName, c);
         error++;
      }
      else v[i] = atof (c);
   }
   if (!found && !mustexist)
   {
      GenericObject::Log_Write ("[ ");
      for (i = 0; i < VarSize; i++)
      {
         if (i) GenericObject::Log_Write (" ");
         GenericObject::Log_Write ("%g", v[i]);
      }
      GenericObject::Log_Write (" ] \n");
   }

   if (buffer) {delete buffer; buffer = 0;}
   return found;
}

//-------------------------------------------------------------------------
typ_return InputFile::load (double v[], const char* ModelName, const char* VarName, int VarSize, bool mustexist)
//-------------------------------------------------------------------------
{
   char  name_variable[LINE_SIZE];

   sprintf (name_variable, "%s.%s", ModelName, VarName);
   return load (v, name_variable, VarSize, mustexist);
}

//-------------------------------------------------------------------------
typ_return InputFile::load (int v[], const char* VarName, int VarSize, bool mustexist)
//-------------------------------------------------------------------------
{
   char        *c;
   typ_return  found;
   int         i, old_error = error;

// Read data vector from file
   found = readSymbol (VarName, VarSize, mustexist);
   if (found == Nominal && old_error == error) for (i = 0; i < VarSize; i++)
   {
      c = buffer + TOKEN_SIZE * i;
      if (strspn (c, REAL_NUMBERS) < strlen (c))
      {
         GenericObject::Log_Error ("Symbol \"%s\" : illegal character in NUMERIC field \"%s\"", VarName, c);
         error++;
      }
      else v[i] = atoi (c);
   }
   if (!found && !mustexist)
   {
      GenericObject::Log_Write ("[ ");
      for (i = 0; i < VarSize; i++)
      {
         if (i) GenericObject::Log_Write (" ");
         GenericObject::Log_Write ("%i", v[i]);
      }
      GenericObject::Log_Write (" ] \n");
   }

   if (buffer) {delete buffer; buffer = 0;}
   return found;
}

//-------------------------------------------------------------------------
typ_return InputFile::load (int v[], const char* ModelName, const char* VarName, int VarSize, bool mustexist)
//-------------------------------------------------------------------------
{
   char  name_variable[LINE_SIZE];

   sprintf (name_variable, "%s.%s", ModelName, VarName);
   return load (v, name_variable, VarSize, mustexist);
}

//-------------------------------------------------------------------------
typ_return InputFile::load (bool v[], const char* VarName, int VarSize, bool mustexist)
//-------------------------------------------------------------------------
{
   int         i, *ival;
   typ_return  result;

   ival = new int[VarSize];
   for (i = 0; i < VarSize; i++)
      ival[i] = (int)v[i];
   result = load (ival, VarName, VarSize, mustexist);
   if (result == Nominal)
      for (i = 0; i < VarSize; i++)
         v[i] = (ival[i] != 0);
   delete [] ival;

   return result;
}

//-------------------------------------------------------------------------
typ_return InputFile::load (bool v[], const char* ModelName, const char* VarName, int VarSize, bool mustexist)
//-------------------------------------------------------------------------
{
   char  name_variable[LINE_SIZE];

   sprintf (name_variable, "%s.%s", ModelName, VarName);
   return load (v, name_variable, VarSize, mustexist);
}

//-------------------------------------------------------------------------
typ_return InputFile::load (char* v[], const char* VarName, int VarSize, bool mustexist)
//-------------------------------------------------------------------------
{
   char        *c;
   typ_return  found;
   int         i, old_error = error;

// Read data vector from file
   found = readSymbol (VarName, VarSize, mustexist);
   if (found == Nominal && old_error == error) for (i = 0; i < VarSize; i++)
   {
      c = buffer + TOKEN_SIZE * i;
      strcpy (v[i], c);
   }
   if (!found && !mustexist)
   {
      GenericObject::Log_Write ("[ ");
      for (i = 0; i < VarSize; i++)
      {
         if (i) GenericObject::Log_Write (" ");
         GenericObject::Log_Write ("%s", v[i]);
      }
      GenericObject::Log_Write (" ] \n");
   }

   if (buffer) {delete buffer; buffer = 0;}
   return found;
}

//-------------------------------------------------------------------------
typ_return InputFile::load (char* v[],const char* ModelName,const char* VarName, int VarSize, bool mustexist)
//-------------------------------------------------------------------------
{
   char  name_variable[LINE_SIZE];

   sprintf (name_variable, "%s.%s", ModelName, VarName);
   return load (v, name_variable, VarSize, mustexist);
}

//-------------------------------------------------------------------------
typ_return InputFile::readSymbol (const char* Symbol, int Ntokens, bool mustexist)
//-------------------------------------------------------------------------
{
   char  *c, line_copy[LINE_SIZE];
   bool  found;
   int   itoken;

   if (!f)
      return Error;

// Reset the input file status
   clearerr (f);
   fseek (f, 0, SEEK_SET);
   iline = 0;

// Look for "Symbol" in the input file (case sensitive)
   found = false;
   ReadLine ();
   while (!found && !feof(f))
   {
      memset (line_copy, '\0', LINE_SIZE);
      strcpy (line_copy, line);
      c = strtok (line, SEPARATORS);
      if (!c || strcmp (c, Symbol)) ReadLine ();
      else
      {
         found = true;
         outside_square_brackets = true;

// Memory buffer allocation for tokens storaging
         if (buffer) {delete buffer; buffer = 0;}
         buffer = new char[TOKEN_SIZE*Ntokens];
         memset (buffer, '\0', TOKEN_SIZE*Ntokens);

// Check the second token in current line (it must be "=")
         c = strtok (NULL, SEPARATORS);
         if (!c || *c != '=')
         {
            GenericObject::Log_Error ("at line % 5i : \"=\" was expected after symbol \"%s\"", iline, Symbol);
            error++;
         }

// Load a single token data
         else if (Ntokens == 1 && enable_tokenizer)
         {
            c = strtok (NULL, SEPARATORS);
            if (!c)
            {
               GenericObject::Log_Error ("at line % 5i : at least one value was expected for symbol \"%s\"", iline, Symbol);
               error++;
            }
            else if (c[0] == '[' && c[1] == '\0')
            {
               c = strtok (NULL, SEPARATORS);
               if (!c)
               {
                  GenericObject::Log_Error ("at line % 5i : one value was expected for symbol \"%s\"", iline, Symbol);
                  error++;
               }
               else strncpy (buffer, c, TOKEN_SIZE-1);
            }
            else strncpy (buffer, c, TOKEN_SIZE-1);
         }

// Load an array of tokens data
         else
         {

// Check first token in array data (it must be "[")
            GetArrayToken (c);
            outside_square_brackets = false;
            if (c && strcmp (c, "["))
            {
               GenericObject::Log_Error ("at line % 5i : \"[\" was expected to open \"%s\" array", iline, Symbol);
               error++;
            }
            else if (!enable_tokenizer && strlen(c+2))
            {
               GenericObject::Log_Error ("at line % 5i : Symbol \"%s\" is a Multi-Line array, each entry shall be on a NEW LINE (\"%s\" not allowed after \"[\" on the same line)", iline, Symbol, c+2);
               error++;
            }
            else
            {
// Read the array tokens
               for (itoken = 0; itoken < Ntokens; itoken++)
               {
                  GetArrayToken (c);
                  if (c && !strcmp (c, "]"))
                  {
                     GenericObject::Log_Error ("at line % 5i : reached unexpected array terminator \"]\" on symbol \"%s\"", iline, Symbol);
                     error++;
                  }
                  else strncpy (buffer+TOKEN_SIZE*itoken, c, TOKEN_SIZE-1);
               }

// Check last token in array data (it must be "]")
               GetArrayToken (c);
               if (c && strcmp (c, "]"))
               {
                  GenericObject::Log_Error ("at line % 5i : \"]\" was expected to close \"%s\" array", iline, Symbol);
                  error = true;
               }
            }
         }
      }
   }

// Message/Error when symbol has not been found

   if (!found)
   {
      if (mustexist)
      {
         GenericObject::Log_Error ("Symbol \"%s\" not found", Symbol);
         error++;
      }
      else
         GenericObject::Log_Write ("%s (Optional) not found and set to default := ", Symbol);
   }
   else
   {
      if (strchr (line_copy, '[') && !strchr (line_copy, ']'))
         GenericObject::Log_Write ("%s ...\n", line_copy);
      else
         GenericObject::Log_Write ("%s\n", line_copy);
   }

   return (found ? Nominal : Error);
}

//-------------------------------------------------------------------------
typ_return InputFile::readSymbol (const char* ModelName, const char* Symbol, int Ntokens, bool mustexist)
//-------------------------------------------------------------------------
{
   char  name_symbol[LINE_SIZE];

   sprintf (name_symbol, "%s.%s", ModelName, Symbol);
   return readSymbol (name_symbol, Ntokens, mustexist);
}

//-------------------------------------------------------------------------
int InputFile::GetRowsNumber (const char* Symbol, int n_columns)
//-------------------------------------------------------------------------
{
   char  *c, line_copy[LINE_SIZE];
   bool  found;
   int   ntoken = -1, result = 0;

   if (!f)
      return Error;

// Reset the input file status
   clearerr (f);
   fseek (f, 0, SEEK_SET);
   iline = 0;

// Look for "Symbol" in the input file (case sensitive)
   found = false;
   ReadLine ();
   while (!found && !feof(f))
   {
      memset (line_copy, '\0', LINE_SIZE);
      strcpy (line_copy, line);
      c = strtok (line, SEPARATORS);
      if (!c || strcmp (c, Symbol)) ReadLine ();
      else
      {
         found = true;
         outside_square_brackets = true;

// Check the second token in current line (it must be "=")
         c = strtok (NULL, SEPARATORS);
         if (!c || *c != '=')
         {
            GenericObject::Log_Error ("at line % 5i : \"=\" was expected after symbol \"%s\"", iline, Symbol);
            error++;
         }

// Check first token in array data (it must be "[")
         else
         {
            GetArrayToken (c);
            outside_square_brackets = false;
            if (c && *c != '[')
            {
               GenericObject::Log_Error ("at line % 5i : \"[\" was expected to open \"%s\" array", iline, Symbol);
               error++;
            }
            else if (!enable_tokenizer && strlen(c+2))
            {
               GenericObject::Log_Error ("at line % 5i : Symbol \"%s\" is a Multi-Line array, each entry shall be a SINGLE NEW LINE (\"%s\" not allowed after \"[\" on the same line)", iline, Symbol, c+2);
               error++;
            }

// Count the number of tokens
            else
            {
               while (c && !strchr(c,']'))
               {
                  if (enable_tokenizer)
                     c = strtok (NULL, SEPARATORS);
                  else
                     c = NULL;
                  if (!c) GetArrayToken (c);
                  ntoken++;
               }
               if (!enable_tokenizer && c && strcmp(c,"]"))
               {
                  GenericObject::Log_Error ("at line % 5i : Symbol \"%s\" is a Multi-Line array, each entry shall be a SINGLE NEW LINE (\"%s\" not allowed before \"]\" on the same line)", iline, Symbol, c);
                  error++;
               }
            }
         }
      }
   }

// Check if the tokens number is an integer multiple of the columns number
   if (ntoken > 0)
   {
      if (ntoken < n_columns || ntoken % n_columns > 0)
      {
         GenericObject::Log_Error ("at line % 5i : Symbol \"%s\" shall contain an integer multiple of \"%i\" data", iline, Symbol, n_columns);
         error++;
      }
      else result = ntoken / n_columns;
   }

// Message when symbol has not been found

   if (!found)
      GenericObject::Log_Write ("%s (Optional) not found and set to default\n", Symbol);
   else
   {
      if (strchr (line_copy, '[') && !strchr (line_copy, ']'))
         GenericObject::Log_Write ("%s ...\n", line_copy);
      else
         GenericObject::Log_Write ("%s\n", line_copy);
   }

   return result;
}

//-------------------------------------------------------------------------
int InputFile::GetRowsNumber (const char* ModelName, const char* Symbol, int n_columns)
//-------------------------------------------------------------------------
{
   char  name_symbol[LINE_SIZE];

   sprintf (name_symbol, "%s.%s", ModelName, Symbol);
   return GetRowsNumber (name_symbol, n_columns);
}

//-------------------------------------------------------------------------
typ_return InputFile::ReadLine ()
//-------------------------------------------------------------------------
{
   char  c, *p, opc[] = "=[]";
   int   i, len;

   if (!f)
      return Error;

// Read a new text line from the input file
   memset (line, '\0', LINE_SIZE);
   i = 0;
   c = (char) fgetc (f);
// Skip initial empty characters/lines, if present
   while (!feof(f) && strchr (SEPARATORS, c))
      c = (char) fgetc (f);
// Acquire a new text line till new line or carriage return
   while (!feof(f) && !strchr ("\r\n", c) && i < LINE_SIZE)
   {
      line[i++] = c;
      c = (char) fgetc (f);
   }
// Remove terminal empty characters, if present
   p = line + i - 1;
   while (p >= line && strchr(SEPARATORS, *p))
      *p-- = '\0';
   iline++;

// Truncate line to the COMMENT symbol ("//" like C++)
   p = strstr (line, "//");
   if (p) *p = '\0';

// Put blank spaces before and after operating characters
   for (i = 0; i < int(strlen(opc)); i++)
   {
      p = strchr (line, opc[i]);
      if (p)
      {
         len = strlen (p+1) + 1;
         memmove (p+3, p+1, len);
         *p = *(p+2) = ' '; *(p+1) = opc[i];
      }
   }
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return InputFile::GetArrayToken (char*& c)
//-------------------------------------------------------------------------
{
   char  *p;

   typ_return  result = Nominal;

   if (!f)
      return Error;

   if (enable_tokenizer || outside_square_brackets)
      c = strtok (NULL, SEPARATORS);
   else
      c = NULL;

   while (!c && !feof(f)) // Continue on a new line
   {
      ReadLine ();
      if (enable_tokenizer || outside_square_brackets)
         c = strtok (line, SEPARATORS);
      else
      {
         c = line + strspn(line, SEPARATORS);      // Remove initial empty characters
         if (strlen (c))
         {
            p = c + strlen (c) - 1;
            while (p && strchr (SEPARATORS, *p))   // Remove final empty characters
               *p-- = '\0';
         }
         else
            c = NULL;
      }
   }
   if (feof(f))
   {
      GenericObject::Log_Error ("at line % 5i : reached unexpected END of FILE\n", iline);
      error++;
      result = Error;
   }
   return result;
}
