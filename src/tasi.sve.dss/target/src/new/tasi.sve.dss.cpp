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
// File Name     : tasi.sve.dss.cpp
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

/// @file tasi.sve.dss.cpp
///    This package is generated from tasi.sve.dss.cat using the SMP2 Package Generation Tool.
///    This package is generated. DO NOT EDIT.

// -----------------------------------------------------------------------------
// --------------------------------- Includes ----------------------------------
// -----------------------------------------------------------------------------

#include "tasi.sve.dss.h"
#include "Mdk/Factory.h"                   // Factories of the models
#include "tasi/sve/dss/DssRoot.h"
#include "tasi/sve/dss/SumL3.h"
#include "tasi/sve/dss/GyroHoneywell.h"
#include "tasi.sve.common.h"
#include "esa.smp2.smp.h"

// --OPENING ELEMENT--tasi_sve_dss::Extra_Includes--
// MARKER: FILE INCLUSION: START
// INSERT HERE #INCLUDE PREPROCESSOR DIRECTIVES
// MARKER: FILE INCLUSION: END

// MARKER: EXTRA DEFINITIONS: START
// INSERT HERE EXTRA FUNCTION / VARIABLE DEFINITIONS
// MARKER: EXTRA DEFINITIONS: END
// --CLOSING ELEMENT--tasi_sve_dss::Extra_Includes--

// -----------------------------------------------------------------------------
// ----------------------------- Global variables ------------------------------
// -----------------------------------------------------------------------------

namespace
{
    /// Collection for all model factories.
    Smp::FactoryCollection factories_tasi_sve_dss;

    /// Maintain initialisation status, is set to true by Initialise(), false
    /// by Finalise().
    Smp::Bool initialised_tasi_sve_dss = false;
}

// -----------------------------------------------------------------------------
// --------------------------- Initialise Function -----------------------------
// -----------------------------------------------------------------------------

extern "C"
{
// --OPENING ELEMENT--Initialise_tasi_sve_dss--
    /// Initialise Package tasi_sve_dss.
    /// @param simulator Dynamic Simulator for registration of factories.
    /// @param typeRegistry Type Registry for registration of types.
    /// @return True if initialisation was successful, false otherwise.
    bool Initialise_tasi_sve_dss(
        Smp::IDynamicSimulator* simulator, 
        Smp::Publication::ITypeRegistry* typeRegistry)
    {
        // Avoid double initialisation
        if (initialised_tasi_sve_dss == true)
        {
            return true;
        }
        else
        {
            initialised_tasi_sve_dss = true;
        }

        // Initialisation of dependent Packages
        initialised_tasi_sve_dss &= Initialise_tasi_sve_common(simulator, typeRegistry);
        initialised_tasi_sve_dss &= Initialise_esa_smp2_smp(simulator, typeRegistry);
        if (!initialised_tasi_sve_dss)
        {
            return false;
        }


        {
            // Register factories for Models defined in package
            Smp::IFactory* factory = NULL;

            // Factory for the Model DssRoot
            factory = new Smp::Mdk::Factory< ::tasi::sve::dss::DssRoot>(
                "DssRoot",    //name
                "",    //empty description
                ::tasi::sve::dss::Uuid_DssRoot,    //specification uuid
                ::tasi::sve::dss::Uuid_DssRoot);    //implementation uuid
                            
            simulator->RegisterFactory(factory);
            
            factories_tasi_sve_dss.push_back(factory);

            // Factory for the Model SumL3
            factory = new Smp::Mdk::Factory< ::tasi::sve::dss::SumL3>(
                "SumL3",    //name
                "",    //empty description
                ::tasi::sve::dss::Uuid_SumL3,    //specification uuid
                ::tasi::sve::dss::Uuid_SumL3);    //implementation uuid
                            
            simulator->RegisterFactory(factory);
            
            factories_tasi_sve_dss.push_back(factory);

            // Factory for the Model GyroHoneywell
            factory = new Smp::Mdk::Factory< ::tasi::sve::dss::GyroHoneywell>(
                "GyroHoneywell",    //name
                "",    //empty description
                ::tasi::sve::dss::Uuid_GyroHoneywell,    //specification uuid
                ::tasi::sve::dss::Uuid_GyroHoneywell);    //implementation uuid
                            
            simulator->RegisterFactory(factory);
            
            factories_tasi_sve_dss.push_back(factory);
        }

        // MARKER: INITIALISE_STATIC BODY: START
        // INSERT HERE CUSTOMISED INITIALISE_STATIC SECTION
        // MARKER: INITIALISE_STATIC BODY: END

        return true;
    }
// --CLOSING ELEMENT--Initialise_tasi_sve_dss--

// --OPENING ELEMENT--Initialisetasi_sve_dss--
    /// Initialise Package tasi_sve_dss.
    /// This is a wrapper function for SMP2 C++ Mapping compatibility.
    /// @param simulator Dynamic Simulator for registration of factories.
    /// @param typeRegistry Type Registry for registration of types.
    /// @return True if initialisation was successful, false otherwise.
    bool Initialisetasi_sve_dss(
        Smp::IDynamicSimulator* simulator, 
        Smp::Publication::ITypeRegistry* typeRegistry)
    {
        return Initialise_tasi_sve_dss(simulator, typeRegistry);
    }
// --CLOSING ELEMENT--Initialisetasi_sve_dss--
}

// -----------------------------------------------------------------------------
// ---------------------------- Finalise Function ------------------------------
// -----------------------------------------------------------------------------

extern "C"
{
// --OPENING ELEMENT--Finalise_tasi_sve_dss--
    /// Finalise Package tasi_sve_dss.
    /// @return True if finalisation was successful, false otherwise.
    bool Finalise_tasi_sve_dss()
    {
        // Assume that cleanup succeeds
        bool result = true;
        // Remember that cleanup has been done
        initialised_tasi_sve_dss = false;

        // Release all model factories
        for (Smp::FactoryCollection::const_iterator
            it = factories_tasi_sve_dss.begin();
            it != factories_tasi_sve_dss.end();
            ++it)
        {
            delete (*it);
        }
        factories_tasi_sve_dss.clear();

        // Finalisation of dependent Packages
        result &= Finalise_tasi_sve_common();
        result &= Finalise_esa_smp2_smp();

        // MARKER: FINALISE_STATIC BODY: START
        // INSERT HERE CUSTOMISED FINALISE_STATIC SECTION
        // MARKER: FINALISE_STATIC BODY: END

        return result;
    }
// --CLOSING ELEMENT--Finalise_tasi_sve_dss--

// --OPENING ELEMENT--Finalisetasi_sve_dss--
    /// Finalise Package tasi_sve_dss.
    /// This is a wrapper function for SMP2 C++ Mapping compatibility.
    /// @return True if finalisation was successful, false otherwise.
    bool Finalisetasi_sve_dss()
    {
        return Finalise_tasi_sve_dss();
    }
// --CLOSING ELEMENT--Finalisetasi_sve_dss--
}

// --OPENING ELEMENT--SPR_LOG_SECTION--
// The SPR history contains the description and reference of
// the SPRs closed on the file.
//
// MARKER: SPR_LOG: START
// Insert SPR log here (Including: Date, Author, SPR REF, Description). 
// MARKER: SPR_LOG: END
// --CLOSING ELEMENT--SPR_LOG_SECTION--

