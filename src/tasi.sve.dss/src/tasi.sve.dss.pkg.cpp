// -----------------------------------------------------------------------------
//
// (C) 2021 European Space Agency
// European Space Operations Centre
// Darmstadt, Germany
//
// -----------------------------------------------------------------------------
//
// Sub-System    : tasi_sve_dss
//
// File Name     : tasi.sve.dss.pkg.cpp
//
// Author        : UMF
//
// Last Modified : $Date$
//
// Revision      : $Revision$
//
// Generated from: UMF Code Generator
//                 Package: tasi.sve.dss.pkg - V1.0.0
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/// @file tasi.sve.dss.pkg.cpp
///    This package is generated from tasi.sve.dss.cat using the SMP2 Package Generation Tool.
///    This package is generated. DO NOT EDIT.

// -----------------------------------------------------------------------------
// --------------------------------- Includes ----------------------------------
// -----------------------------------------------------------------------------

#include "tasi.sve.dss.h"

// --OPENING ELEMENT--tasi_sve_dss::Extra_Includes--
// MARKER: FILE INCLUSION: START
// INSERT HERE #INCLUDE PREPROCESSOR DIRECTIVES
// MARKER: FILE INCLUSION: END

// MARKER: EXTRA DEFINITIONS: START
// INSERT HERE EXTRA FUNCTION / VARIABLE DEFINITIONS
// MARKER: EXTRA DEFINITIONS: END
// --CLOSING ELEMENT--tasi_sve_dss::Extra_Includes--

#ifdef  WIN32
#define DLL_EXPORT __declspec(dllexport)    // %RELAX<mconst> Visual Studio requires a define
#else
#define DLL_EXPORT
#endif

// -----------------------------------------------------------------------------
// -------------------------- Initialise Function ------------------------------
// -----------------------------------------------------------------------------

extern "C"
{
// --OPENING ELEMENT--tasi_sve_dss::Initialise--
    /// Global Initialise function of Package tasi_sve_dss.
    /// @param simulator Dynamic Simulator for registration of factories.
    /// @param typeRegistry Type Registry for registration of types.
    /// @return True if initialisation was successful, false otherwise.
    DLL_EXPORT bool Initialise(
        Smp::IDynamicSimulator* simulator, 
        Smp::Publication::ITypeRegistry* typeRegistry)
    {
        bool initialisationSucceeded = true;

        // MARKER: INITIALISE BODY: START
        // INSERT HERE CUSTOMISED INITIALISE SECTION
        // MARKER: INITIALISE BODY: END

        initialisationSucceeded &= Initialise_tasi_sve_dss(simulator, typeRegistry);

        return initialisationSucceeded;
    }
// --CLOSING ELEMENT--tasi_sve_dss::Initialise--
}

// -----------------------------------------------------------------------------
// ---------------------------- Finalise Function ------------------------------
// -----------------------------------------------------------------------------

extern "C"
{
// --OPENING ELEMENT--tasi_sve_dss::Finalise--
    /// Global Finalise function of Package tasi_sve_dss.
    /// @return True if finalisation was successful, false otherwise.
    DLL_EXPORT bool Finalise()
    {
        bool finalisationSucceeded = true;

        // MARKER: FINALISE BODY: START
        // INSERT HERE CUSTOMISED FINALISE SECTION
        // MARKER: FINALISE BODY: END

        finalisationSucceeded &= Finalise_tasi_sve_dss();

        return finalisationSucceeded;
    }
// --CLOSING ELEMENT--tasi_sve_dss::Finalise--
}

// --OPENING ELEMENT--SPR_LOG_SECTION--
// The SPR history contains the description and reference of
// the SPRs closed on the file.
//
// MARKER: SPR_LOG: START
// Insert SPR log here (Including: Date, Author, SPR REF, Description). 
// MARKER: SPR_LOG: END
// --CLOSING ELEMENT--SPR_LOG_SECTION--

