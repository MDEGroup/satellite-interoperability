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
// File Name     : tasi.sve.dss.h
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

/// @file tasi.sve.dss.h
///    This package is generated from tasi.sve.dss.cat using the SMP2 Package Generation Tool.
///    This package is generated. DO NOT EDIT.

#ifndef _tasi_sve_dss_H
#define _tasi_sve_dss_H

// -----------------------------------------------------------------------------
// --------------------------------- Includes ----------------------------------
// -----------------------------------------------------------------------------

#include "Smp/IDynamicSimulator.h"
#include "Smp/Publication/ITypeRegistry.h"

// -----------------------------------------------------------------------------
// -------------------------- Namespace Documentation --------------------------
// -----------------------------------------------------------------------------

/// The tasi namespace.
namespace tasi
{
    /// This is the top-level namespace of tasi.sve.
    namespace sve
    {
        /// This namespace provides the dss elements of tasi.sve.
        namespace dss
        {
        }
    }
}

// -----------------------------------------------------------------------------
// --------------------------------- Entry Point -------------------------------
// -----------------------------------------------------------------------------

// Entry points for static library
extern "C"
{
    /// Initialise Package tasi_sve_dss.
    /// @param simulator Dynamic Simulator for registration of factories.
    /// @param typeRegistry Type Registry for registration of types.
    /// @return True if initialisation was successful, false otherwise.
    bool Initialise_tasi_sve_dss(
        Smp::IDynamicSimulator* simulator,
        Smp::Publication::ITypeRegistry* typeRegistry);

    /// Finalise Package tasi_sve_dss.
    /// @return True if finalisation was successful, false otherwise.
    bool Finalise_tasi_sve_dss();

    /// Initialise Package tasi_sve_dss.
    /// This is a wrapper function for SMP2 C++ Mapping compatibility.
    /// @param simulator Dynamic Simulator for registration of factories.
    /// @param typeRegistry Type Registry for registration of types.
    /// @return True if initialisation was successful, false otherwise.
    bool Initialisetasi_sve_dss(
        Smp::IDynamicSimulator* simulator,
        Smp::Publication::ITypeRegistry* typeRegistry);

    /// Finalise Package tasi_sve_dss.
    /// This is a wrapper function for SMP2 C++ Mapping compatibility.
    /// @return True if finalisation was successful, false otherwise.
    bool Finalisetasi_sve_dss();
}

#endif  // _tasi_sve_dss_H

// --OPENING ELEMENT--SPR_LOG_SECTION--
// The SPR history contains the description and reference of
// the SPRs closed on the file.
//
// MARKER: SPR_LOG: START
// Insert SPR log here (Including: Date, Author, SPR REF, Description). 
// MARKER: SPR_LOG: END
// --CLOSING ELEMENT--SPR_LOG_SECTION--

