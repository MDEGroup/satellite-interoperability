// -----------------------------------------------------------------------------
//
// (C) 2021 TAS-I
//
// -----------------------------------------------------------------------------
//
// Sub-System    : tasi_sve_dss
//
// File Name     : SumL3Smp.cpp
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

/// @file /tasi/sve/dss/SumL3Smp.cpp

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
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3::Extra_Includes--

using namespace ::Smp;
using namespace ::Smp::Publication;


namespace tasi
{
    namespace sve
    {
        namespace dss
        {


            // -----------------------------------------------------------------------------
            // ------------------------------- Static fields -------------------------------
            // -----------------------------------------------------------------------------

            ::Smp::Mdk::Management::RequestContainer<SumL3>::Map SumL3::requestHandlers;


            // -----------------------------------------------------------------------------
            // -------------------------- Constructors/Destructor --------------------------
            // -----------------------------------------------------------------------------

// --OPENING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Default_Constructor--
            /// Default constructor.
            SumL3::SumL3(void)
                : sum(0.0)
            // MARKER: CONSTRUCTOR PRE BODY: START
            // INSERT HERE INITIALISATION LIST
            // MARKER: CONSTRUCTOR PRE BODY: END
            {
                // MARKER: OPERATION BODY: START
                // INSERT HERE OPERATION BODY
                // Initialise private fields
                ::tasi::sve::dss::SumL3::_Initialise();
                // MARKER: OPERATION BODY: END
            }
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Default_Constructor--

// --OPENING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Constructor--
            /// Constructor setting name, description and parent.
                SumL3::SumL3(
                    ::Smp::String8 name,
                    ::Smp::String8 description,
                    ::Smp::IComposite* parent)
                throw (::Smp::InvalidObjectName)
                : ::Smp::Mdk::Management::ManagedModel(name, description, parent),
                  sum(0.0)
            // MARKER: CONSTRUCTOR PRE BODY: START
            // INSERT HERE INITIALISATION LIST
            // MARKER: CONSTRUCTOR PRE BODY: END
            {
                // MARKER: OPERATION BODY: START
                // INSERT HERE OPERATION BODY
                // Initialise private fields
                ::tasi::sve::dss::SumL3::_Initialise();
                // MARKER: OPERATION BODY: END
            }
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Constructor--

// --OPENING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Destructor--
            /// Virtual destructor that is called by inherited classes as well.
            SumL3::~SumL3(void)
            {
                // MARKER: OPERATION BODY: START
                // INSERT HERE OPERATION BODY
                try
                {
                    // Cleanup private fields
                    ::tasi::sve::dss::SumL3::_Cleanup();
                }
                catch (...)
                {
                    // Catch all exceptions in destructors.
                }    

                // MARKER: OPERATION BODY: END
            }
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Destructor--


            // -----------------------------------------------------------------------------
            // ------------------- Implementation of IModel interface ----------------------
            // -----------------------------------------------------------------------------

// --OPENING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Publish--
            /// Publish fields, operations and properties of the model.
            /// @param receiver Publication receiver.
            void SumL3::Publish(
                ::Smp::IPublication* receiver)
                throw (::Smp::IModel::InvalidModelState)
            {
                // Call base class implementation first
                ::Smp::Mdk::Management::ManagedModel::Publish(receiver);

                // Publish custom fields
                // MARKER: PUBLISH BODY: START
                // INSERT HERE CUSTOMISED PUBLISH SECTION
                // MARKER: PUBLISH BODY: END

                // Populate the request handlers (only once)
                if (requestHandlers.empty())
                {
                    PopulateRequestHandlers<SumL3>(this, requestHandlers);
                }
                
                //--------------------------- Publish Fields -------------------------

                // Publish field sum
                receiver->PublishField("sum", "", &sum, true, true, false, false);

                // Publish field nInputs
                receiver->PublishField("nInputs", "", &nInputs, true, true, false, false);

                // Publish field sumProgressiveID
                receiver->PublishField("sumProgressiveID", "", &sumProgressiveID, false, false, false, false);
                
                //------------------------ Publish Entry Points --------------------

                // Publish entry point UpdateSum
                receiver->PublishOperation("UpdateSum", "", ::Smp::Publication::Uuid_Void);
            }
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Publish--

            // The Configure and Connect methods are defined in the SumL3.cpp file.

            // -----------------------------------------------------------------------------
            // ---------------- Universally Unique Identifier of Model ---------------------
            // -----------------------------------------------------------------------------

                /// Return the Universally Unique Identifier of the Model.
                /// This is defined on type level, not on instance level.
                /// The purpose of this operation is to allow navigating back from
                /// a Model Instance (at run-time) to Metadata in a Catalogue.
                /// @remarks This method has to be overridden for each Model defined
                ///          in a Catalogue. The default implementation returns an
                ///          empty Uuid.
                /// @return  Universally Unique Identifier of the Model.
                const Smp::Uuid& SumL3::GetUuid(void) const
                {
                    return Uuid_SumL3;
                }


            // -----------------------------------------------------------------------------
            // ------------- Implementation of IDynamicInvocation interface ----------------
            // -----------------------------------------------------------------------------

            // ---------------------------------- Invoke -----------------------------------

// --OPENING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Invoke--
            /// Invoke the operation for the given request
            /// @param request Request object to invoke.
            void SumL3::Invoke(
                ::Smp::IRequest* request)
                throw (
                    ::Smp::IDynamicInvocation::InvalidOperationName,
                    ::Smp::IDynamicInvocation::InvalidParameterCount,
                    ::Smp::IDynamicInvocation::InvalidParameterType)
            {
                if (request == NULL)
                {
                    return;
                }
                auto handler = requestHandlers.find(request->GetOperationName());
                if (handler != requestHandlers.end())
                {
                    handler->second->Execute(*this, request);
                }
                else
                {
                    ::Smp::Bool customInvocationSuccessful = false;
                    // MARKER: INVOKE BODY: START
                    // INSERT HERE CUSTOMISED INVOKE SECTION
                    // If the invocation is handled in the custom section set the
                    // customInvocationSuccessful variable to true.
                    // MARKER: INVOKE BODY: END

                    // If custom invocation did not happen
                    if (customInvocationSuccessful == false)
                    {
                        // request does not match any operation provided by this model
                        throw ::Smp::IDynamicInvocation::InvalidOperationName(request->GetOperationName());
                    }
                }
            }
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::Invoke--


            // -----------------------------------------------------------------------------
            // -------------------- Initialise and Cleanup Implementation ------------------
            // -----------------------------------------------------------------------------
            //

// --OPENING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::_Initialise--
            /// Initialise
            void SumL3::_Initialise(void)
            {
                // - - - - - - - Initialise private fields - - - - - - -

                // - - - - - - - - - Setup EntryPoints - - - - - - - - -
                //
                // EntryPoint: UpdateSum
                UpdateSum
                = new ::Smp::Mdk::EntryPoint("UpdateSum",
                                            "",
                                            this,
                                            &SumL3::_UpdateSum);
                // Use existing implementation to manage Entry Points
                this->AddEntryPoint(UpdateSum);


                // MARKER: INIT BODY: START
                // INSERT HERE CUSTOMISED INITIALISE SECTION
                // MARKER: INIT BODY: END
            }
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::_Initialise--

// --OPENING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::_Cleanup--
            /// Cleanup
            void SumL3::_Cleanup(void)
            {
                // MARKER: CLEANUP BODY: START
                // INSERT HERE CUSTOMISED CLEANUP SECTION
                // MARKER: CLEANUP BODY: END

                // EntryPoint: UpdateSum
                if (UpdateSum != NULL)
                {
                    delete UpdateSum;
                }
            }
// --CLOSING ELEMENT--::tasi::sve::dss::SumL3/SumL3Smp.cpp::_Cleanup--

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

