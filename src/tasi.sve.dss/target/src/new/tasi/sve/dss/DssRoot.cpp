// -----------------------------------------------------------------------------
//
// (C) 2021 TAS-I
//
// -----------------------------------------------------------------------------
//
// Sub-System    : tasi_sve_dss
//
// File Name     : DssRoot.cpp
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

/// @file /tasi/sve/dss/DssRoot.cpp

// -----------------------------------------------------------------------------
// --------------------------------- Includes ----------------------------------
// -----------------------------------------------------------------------------

#include "DssRoot.h"
#include "Mdk/Composite.h"
#include "Mdk/Management/ManagedContainer.h"
#include "Mdk/EntryPoint.h"
#include "Mdk/Management/EntryPointPublisher.h"
#include "tasi/sve/common/IPulseReceiver.h"
#include "Smp/IModel.h"
#include "Smp/SimpleTypes.h"
#include "Smp/Publication/IType.h"
#include "Smp/Publication/IEnumerationType.h"
#include "Smp/Publication/IStructureType.h"
#include "Smp/Publication/IClassType.h"
#include "Smp/Publication/IPublishOperation.h"
#include "Mdk/SimpleTypes.h"

// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Includes--
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
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Includes--

namespace tasi
{
    namespace sve
    {
        namespace dss
        {

            // -----------------------------------------------------------------------------
            // ------------------------------- Operations ----------------------------------
            // -----------------------------------------------------------------------------

// --OPENING ELEMENT--DssRoot::ReceivePulse--
            /// @param pulseId
            void DssRoot::ReceivePulse(
                const ::Smp::Int32 pulseId)
            {
                // MARKER: OPERATION BODY: START
                // INSERT HERE OPERATION BODY
                // MARKER: OPERATION BODY: END
            }
// --CLOSING ELEMENT--DssRoot::ReceivePulse--


            // -----------------------------------------------------------------------------
            // ------------------------------- Properties ----------------------------------
            // -----------------------------------------------------------------------------

// --OPENING ELEMENT--DssRoot::get_ReceiverId--
            /// Get ReceiverId.
            /// @return Current value of property ReceiverId.
            ::Smp::String8 DssRoot::get_ReceiverId(void)
            {
                // MARKER: OPERATION BODY: START
                // INSERT HERE OPERATION BODY
                // TODO: return the field value (the attached field was not defined in the catalogue model)
                return "Undefined";
                // MARKER: OPERATION BODY: END
            }
// --CLOSING ELEMENT--DssRoot::get_ReceiverId--




            // -----------------------------------------------------------------------------
            // ------------------------------ Entry Points ---------------------------------
            // -----------------------------------------------------------------------------

// --OPENING ELEMENT--DssRoot::_UpdateAllModels--
            // Handler for Entry Point: UpdateAllModels
            void DssRoot::_UpdateAllModels()
            {
                // MARKER: OPERATION BODY: START
                // INSERT HERE OPERATION BODY
                // MARKER: OPERATION BODY: END
            }
// --CLOSING ELEMENT--DssRoot::_UpdateAllModels--


            // -----------------------------------------------------------------------------
            // ------------------- Implementation of IModel interface ----------------------
            // -----------------------------------------------------------------------------

            // - - - - - - - - - - - - - - Publish - - - - - - - - - - - - - - - -

            // The Publish method is implemented in the DssRootSmp.cpp file.


// --OPENING ELEMENT--DssRoot::Configure--
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

            void DssRoot::Configure(
                ::Smp::Services::ILogger* logger)
                throw (::Smp::IModel::InvalidModelState)
            {
                // MARKER: CONFIGURE BODY: START
                // INSERT HERE CONFIGURE BODY
                // MARKER: CONFIGURE BODY: END

                // Call base implementation last, to perform state transition
                ::Smp::Mdk::Management::ManagedModel::Configure(logger);
            }
// --CLOSING ELEMENT--DssRoot::Configure--


// --OPENING ELEMENT--DssRoot::Connect--
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

            void DssRoot::Connect(
                ::Smp::ISimulator* simulator)
                throw (::Smp::IModel::InvalidModelState)
            {
                // Call Mdk implementation first
                ::Smp::Mdk::Management::ManagedModel::Connect(simulator);

                // MARKER: CONNECT BODY: START
                // INSERT HERE CONNECT BODY
                // MARKER: CONNECT BODY: END
            }
// --CLOSING ELEMENT--DssRoot::Connect--

            // -----------------------------------------------------------------------------
            // ------------------------ Custom method definitions --------------------------
            // -----------------------------------------------------------------------------
// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Methods--
            // MARKER: EXTRA METHOD DEFINITIONS: START
            // INSERT HERE EXTRA METHOD DEFINITIONS
            // MARKER: EXTRA METHOD DEFINITIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Methods--

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

