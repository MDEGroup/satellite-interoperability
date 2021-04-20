#if !defined(__input_file_h) // Sentry, use file only if it's not already included.
#define __input_file_h

/******************************************************************************

    PROJECT:      DSS - S/C Dynamics Simulation Software
    FILE:         input_file.h
    RELEASE:      0.00
    AUTHOR:       Luigi Galvagni
    DATE:         2008/10/17

    OVERVIEW
    ========
    Header file for the input file hadler

    CHANGE LOG
    ==========
   Luigi Galvagni 2013/09/17 Removed dependency from the "std::ifstream" C++ class,
                             managed using FILE* C-like, since is more portable.
                             Added the default constructor and destructor.
   LG/D.Pascucci 2013/09/26  Removed "readMultilineSymbol" methods since reundant with respect to
                             the "readSymbol" ones. Inside the method "readSymbol" the special branch
                             (Ntokens == 1) is now constrained also to the "enable_tokenizer" flag.

******************************************************************************/

#include "GenericTypes.h"

#define  TOKEN_SIZE              256                                 //!< Maximum allowed size for a single input token
#define  LINE_SIZE               1024                                //!< Maximum allowed size for a single input instruction
#define  INPUTFILE_DEFAULT       "dss.set"                           //!< Default Input File Name (if not provided)
#define  REAL_NUMBERS            "0123456789.Ee+-"                   //!< Allowed characters set for Real Numbers
#define  SEPARATORS              " ,;\t\r\n"                         //!< Allowed token separators

/**
* Class definition to handle the HW Models Input File.
*/
class InputFile
{

/*-------------------------------------------------------------------------
   Data declarations
-------------------------------------------------------------------------*/

public:

   char        *buffer;                                              //!< RAM allocated buffer to store the parsed tokens
   char        current_input_file_name[LINE_SIZE];                   //!< Name of the currently used input file
   char        line[LINE_SIZE];                                      //!< Working RAM to allocated an input instruction
   int         error;                                                //!< Errors counter
   int         iline;                                                //!< Input Text File line identifier

private:

   FILE        *f;                                                   //!< Input File Handler
   bool        enable_tokenizer;                                     //!< Flag to Enable/Inhibit the input line tokenization
   bool        outside_square_brackets;                              //!< Flag to notify searching outside the data body

/*-------------------------------------------------------------------------
   Function declarations
-------------------------------------------------------------------------*/

public:

/** Default "InputFile" class constructor */
   InputFile ();
/** "InputFile" class destructor */
   ~InputFile ();
/** Public Method to open the <b>Models Input Text File</b>; if the input file name is not provided the default [dss.set] is used */
   typ_return  open  (const char* filename = 0 /*! <b>Models Input Text File Name</b>; if it is not provided the default [dss.set] is used*/);
/** Public Method to close the <b>Models Input Text File</b> */
   typ_return  close ();
/** Public Method to load from the Models Input Text File an <b>Array of (double) Values</b> */
   typ_return  load (double v[] /*! double <b>Array to be Loaded</b> */, const char* VarName /*! <b>Data Name</b> to be found in the input file*/, int VarSize /*! Expected <b>Array Size</b> */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
/** Public Method to load from the Models Input Text File an <b>Array of (double) Values related to a specific Model</b> */
   typ_return  load (double v[] /*! double <b>Array to be Loaded</b> */, const char* ModelName /*! <b>Model Name</b> to be found in the input file (before dot)*/, const char* VarName /*! <b>Data Name</b> to be found in the input file (after dot)*/, int VarSize /*! Expected <b>Array Size</b> */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
/** Public Method to load from the Models Input Text File an <b>Array of (integer) Values</b> */
   typ_return  load (int v[] /*! integer <b>Array to be Loaded</b> */, const char* VarName /*! <b>Data Name</b> to be found in the input file*/, int VarSize /*! Expected <b>Array Size</b> */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
 /** Public Method to load from the Models Input Text File an <b>Array of (integer) Values related to a specific Model</b> */
   typ_return  load (int v[] /*! integer <b>Array to be Loaded</b> */, const char* ModelName /*! <b>Model Name</b> to be found in the input file (before dot)*/, const char* VarName /*! <b>Data Name</b> to be found in the input file (after dot)*/, int VarSize /*! Expected <b>Array Size</b> */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
/** Public Method to load from the Models Input Text File an <b>Array of (bool) Values</b> */
   typ_return  load (bool v[] /*! double <b>Array to be Loaded</b> */, const char* VarName /*! <b>Data Name</b> to be found in the input file*/, int VarSize /*! Expected <b>Array Size</b> */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
/** Public Method to load from the Models Input Text File an <b>Array of (bool) Values related to a specific Model</b> */
   typ_return  load (bool v[] /*! double <b>Array to be Loaded</b> */, const char* ModelName /*! <b>Model Name</b> to be found in the input file (before dot)*/, const char* VarName /*! <b>Data Name</b> to be found in the input file (after dot)*/, int VarSize /*! Expected <b>Array Size</b> */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
/** Public Method to load from the Models Input Text File an <b>Array of (character) Values</b> */
   typ_return  load (char* v[] /*! character <b>Array to be Loaded</b> */, const char* VarName /*! <b>Data Name</b> to be found in the input file*/, int VarSize /*! Expected <b>Array Size</b> */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
/** Public Method to load from the Models Input Text File an <b>Array of (character) Values related to a specific Model</b> */
   typ_return  load (char* v[] /*! character <b>Array to be Loaded</b> */, const char* ModelName /*! <b>Model Name</b> to be found in the input file (before dot)*/,const char* VarName /*! <b>Data Name</b> to be found in the input file (after dot)*/, int VarSize /*! Expected <b>Array Size</b> */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
/** Public Method to find in the Models Input Text File the provided <b>Symbol</b>, to allocate and copy in the buffer the relevant content and to count the total number of the identified tokens in it*/
   typ_return  readSymbol (const char* Symbol /*! <b>Data Name</b> to be found in the input file*/, int Ntokens /*! <b>Number of tokens</b> expected to be related to the provided Symbol */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
/** Public Method to find in the Models Input Text File the provided <b>Symbol related to a specific Model</b>, to allocate and copy in the buffer the relevant content and to count the total number of the identified tokens in it*/
   typ_return  readSymbol (const char* ModelName /*! <b>Model Name</b> to be found in the input file (before dot)*/,const char* Symbol /*! <b>Data Name</b> to be found in the input file (after dot)*/, int Ntokens /*! <b>Number of tokens</b> expected to be related to the provided Symbol */, bool mustexist=true /*! Logical flag to specify wether the data <b>is mandatory or is optional</b> */);
/** Public Function to return the <b>Number of Rows</b> assuming the provided <b>Symbol</b> identifies a Matrix having the provided number of columns (it shall be: GetRowsNumber * n_columns = Ntokens) */
   int         GetRowsNumber (const char* Symbol /*! <b>Matrix Data Name</b> to be found in the input file*/, int n_columns /*! <b>Number of Columns</b> in the Symbol Matrix*/);
/** Public Function to return the <b>Number of Rows</b> assuming the provided <b>Symbol related to a specific Model</b> identifies a Matrix having the provided number of columns (it shall be: GetRowsNumber * n_columns = Ntokens) */
   int         GetRowsNumber (const char* ModelName /*! <b>Model Name</b> to be found in the input file (before dot)*/,const char* Symbol /*! <b>Matrix Data Name</b> to be found in the input file (after dot)*/, int n_columns /*! <b>Number of Columns</b> in the Symbol Matrix*/);
/** Public Method to Enable (default) the instruction lines tokenization, i.e. to acquire several tokens from the same input line */
   inline void EnableLineTokenizer () {enable_tokenizer = true;};
/** Public Method to Disable the instruction lines tokenization, i.e. to acquire each input line as a single token */
   inline void DisableLineTokenizer () {enable_tokenizer = false;};

private:

/** Private Method to read a new instruction from the Models Input Text File */
   typ_return  ReadLine ();
/** Private Method to update the <b>token pointer [c]</b> to the next token found in the Models Input Text File */
   typ_return  GetArrayToken (char*& c /*! pointer to the <b>current token</b> updated to the next available one by this method */);

};

#endif // __input_file_h sentry.

