// -----------------------------------------------------------------------------
//
// (C) 2021 TAS-I
//
// -----------------------------------------------------------------------------
//
// Sub-System    : tasi_sve_dss
//
// File Name     : SumL3.cpp
//
// Author        : UMF
//
// Last Modified : 
//
// Revision      : $Revision$
//
// Generated from: UMF Code Generator
//                 Catalogue: tasi.sve.dss.cat - V1.0.0
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/// @file /tasi/sve/dss/SumL3.cpp

// -----------------------------------------------------------------------------
// --------------------------------- Includes ----------------------------------
// -----------------------------------------------------------------------------

#include "SumL3.h"
#include "Mdk/EntryPoint.h"
#include "Mdk/Management/EntryPointPublisher.h"
#include "Smp/SimpleTypes.h"
#include "Smp/Publication/IType.h"
#include "Smp/Publication/IEnumerationType.h"
#include "Smp/Publication/IStructureType.h"
#include "Smp/Publication/IClassType.h"
#include "Smp/Publication/IPublishOperation.h"
#include "Mdk/SimpleTypes.h"

// --OPENING ELEMENT--::tasi::sve::dss::SumL3/SumL3::Extra_Includes--
// MARKER: FILE INCLUSION: START
// INSERT HERE #INCLUDE PREPROCESSOR DIRECTIVES
// MARKER: FILE INCLUSION: END

// MARKER: FORWARD DECLARATIONS: START
// INSERT HERE FORWARD DECLARATIONS
// MARKER: FORWARD DECLARATIONS: END

// -----------------------------------------------------------------------------
// ------------------------ Custom field definitions ---------------------------
// -----------------------------------------------------------------------------
// MARKER: EXTRA FIELD DEFINITIONS: START
// INSERT HERE EXTRA FIELD DEFINITIONS
// MARKER: EXTRA FIELD DEFINITIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3::Extra_Includes--

namespace tasi
{
    namespace sve
    {
        namespace dss
        {



            // -----------------------------------------------------------------------------
            // ------------------------------ Entry Points ---------------------------------
            // -----------------------------------------------------------------------------

// --OPENING ELEMENT--SumL3::_UpdateSum--
            // Handler for Entry Point: UpdateSum
            void SumL3::_UpdateSum()
            {
                // MARKER: OPERATION BODY: START
                // INSERT HERE OPERATION BODY
                // MARKER: OPERATION BODY: END
            }
// --CLOSING ELEMENT--SumL3::_UpdateSum--


            // -----------------------------------------------------------------------------
            // ------------------- Implementation of IModel interface ----------------------
            // -----------------------------------------------------------------------------

            // - - - - - - - - - - - - - - Publish - - - - - - - - - - - - - - - -

            // The Publish method is implemented in the SumL3Smp.cpp file.


// --OPENING ELEMENT--SumL3::Configure--
            // - - - - - - - - - - - - - - Configure - - - - - - - - - - - - - - -
            //
            /// Request for configuration.  Request the model to perform any custom
            /// configuration.  The  model can  create and configure  other  models
            /// using the field values of its published fields.
            ///
            /// This  method  can only be called  once for each model, and
            /// when the model is in <em>Publishing</em> state.  The model
            /// can still publish further  features in this call,  and can
            /// even create other models, but at the end of this call,  it
            /// needs to enter the<em>Configured</em> state.
            ///
            /// @param logger Logger to use for log messages during Configure().
            ///
            /// @remarks The simulation  environment typically calls this method in
            ///          the <em>Building</em> state.
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            void SumL3::Configure(
                ::Smp::Services::ILogger* logger)
                throw (::Smp::IModel::InvalidModelState)
            {
                // MARKER: CONFIGURE BODY: START
                // INSERT HERE CONFIGURE BODY
                // MARKER: CONFIGURE BODY: END

                // Call base implementation last, to perform state transition
                ::Smp::Mdk::Management::ManagedModel::Configure(logger);
            }
// --CLOSING ELEMENT--SumL3::Configure--


// --OPENING ELEMENT--SumL3::Connect--
            // - - - - - - - - - - - - - - - Connect - - - - - - - - - - - - - - -
            //
            /// Connect model to simulator.
            /// Allow the model to connect to the simulator (ISimulator).
            ///
            /// This method  can only be called once  for each model,  and
            /// only  when the model  is in the <em>Configured</em> state.
            /// When  this  operation is  called,  the  model  immediately
            /// enters  the <em>Connected</em> state,  before it  uses any
            /// of the simulator methods and services.
            ///
            /// In this method, the model may query for and use any of the
            /// available simulation services,  as they are all guaranteed
            /// to be  fully  functional  at that  time.  It  may  as well
            /// connect  to  other  models' functionality  (e.g.  to event
            /// sources),  as it is  guaranteed that  all models have been
            /// created and configured  before the Connect() method of any
            /// model is called.
            ///
            /// @param   simulator  Simulation  Environment  that  hosts the model.
            ///
            /// @remarks The simulation  environment typically calls this method in
            ///          the <em>Connecting</em> state.
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            void SumL3::Connect(
                ::Smp::ISimulator* simulator)
                throw (::Smp::IModel::InvalidModelState)
            {
                // Call Mdk implementation first
                ::Smp::Mdk::Management::ManagedModel::Connect(simulator);

                // MARKER: CONNECT BODY: START
                // INSERT HERE CONNECT BODY
                // MARKER: CONNECT BODY: END
            }
// --CLOSING ELEMENT--SumL3::Connect--

            // -----------------------------------------------------------------------------
            // ------------------------ Custom method definitions --------------------------
            // -----------------------------------------------------------------------------
// --OPENING ELEMENT--::tasi::sve::dss::SumL3/SumL3::Extra_Methods--
            // MARKER: EXTRA METHOD DEFINITIONS: START
            // INSERT HERE EXTRA METHOD DEFINITIONS
            // MARKER: EXTRA METHOD DEFINITIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3::Extra_Methods--

        }
    }
}

// --OPENING ELEMENT--SPR_LOG_SECTION--
// The SPR history contains the description and reference of
// the SPRs closed on the file.
//
// MARKER: SPR_LOG: START
// Insert SPR log here (Including: Date, Author, SPR REF, Description). 
// MARKER: SPR_LOG: END
// --CLOSING ELEMENT--SPR_LOG_SECTION--











