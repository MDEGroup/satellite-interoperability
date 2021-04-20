// -----------------------------------------------------------------------------
//
// (C) 2021 TAS-I
//
// -----------------------------------------------------------------------------
//
// Sub-System    : tasi_sve_dss
//
// File Name     : DssRoot.h
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

/// @file /tasi/sve/dss/DssRoot.h

#ifndef _TASI_SVE_DSS_DSSROOT_H_
#define _TASI_SVE_DSS_DSSROOT_H_

// -----------------------------------------------------------------------------
// ------------------------- Includes for Simple Types -------------------------
// -----------------------------------------------------------------------------

#include "Smp/SimpleTypes.h"

// -----------------------------------------------------------------------------
// --------------------------------- Includes ----------------------------------
// -----------------------------------------------------------------------------

#include "Mdk/SimpleTypes.h"
#include "Mdk/Model.h"
#include "Smp/IDynamicInvocation.h"
#include "Mdk/Management/ManagedModel.h"
#include "Mdk/Composite.h"
#include "Mdk/Management/ManagedContainer.h"
#include "Mdk/EntryPoint.h"
#include "Mdk/Management/EntryPointPublisher.h"
#include "tasi/sve/common/IPulseReceiver.h"
#include "Smp/IModel.h"

// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Includes--
// MARKER: FILE INCLUSION: START
#include "tasi/sve/common/CommonRoot.h"
#include "SumL3.h"
#include "GyroHoneywell.h"


// MARKER: FILE INCLUSION: END

// MARKER: FORWARD DECLARATIONS: START
// INSERT HERE FORWARD DECLARATIONS
// MARKER: FORWARD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Includes--

namespace tasi
{
    namespace sve
    {
        namespace dss
        {

            // ----------------------------------- UUID ------------------------------------

            /// Universally unique identifier of model DssRoot.
            /// @return Universally Unique Identifier of DssRoot.
            static const ::Smp::Mdk::Uuid Uuid_DssRoot("fb0fb2df-6bd0-3441-8667-b5aa40ab7b98");

            // ------------------------ Declaration of Model class ---------------------------
            class DssRoot:
                  public ::Smp::Mdk::Management::ManagedModel,
                  virtual public ::Smp::IDynamicInvocation,
                  virtual public ::Smp::Mdk::Composite,
                  virtual public ::Smp::Mdk::Management::EntryPointPublisher,
                  virtual public ::tasi::sve::common::IPulseReceiver
            {

                // ----------------------------- PUBLIC MEMBERS ----------------------------
                public:

                // -----------------------------------------------------------------------------
                // -------------------------- Constructors/Destructor --------------------------
                // -----------------------------------------------------------------------------

                    /// Default constructor.
                    DssRoot(void);

                    /// Constructor setting name, description and parent.
                    /// @param name Name of new model instance.
                    /// @param description Description of new model instance.
                    /// @param parent Parent of new model instance.
                    DssRoot(
                        ::Smp::String8 name,
                        ::Smp::String8 description,
                        ::Smp::IComposite* parent)
                        throw (::Smp::InvalidObjectName);

                    /// Virtual destructor that is called by inherited classes as well.
                    virtual ~DssRoot(void);

                // -----------------------------------------------------------------------------
                // -------------------------------- Entry Points -------------------------------
                // -----------------------------------------------------------------------------

                    ::Smp::IEntryPoint* UpdateAllModels; // %RELAX<dmaccess> SMP2 EntryPoint is public


                // -----------------------------------------------------------------------------
                // --------------------------------- Containers --------------------------------
                // -----------------------------------------------------------------------------

                    Smp::Management::IManagedContainer* l3Models; // %RELAX<dmaccess> SMP2 Container is public



                // -----------------------------------------------------------------------------
                // --------------------------------- Properties --------------------------------
                // -----------------------------------------------------------------------------

                    /// Get ReceiverId.
                    /// @return Current value of property ReceiverId.
                    virtual ::Smp::String8 get_ReceiverId(void);


                // -----------------------------------------------------------------------------
                // --------------------------------- Operations --------------------------------
                // -----------------------------------------------------------------------------

                    /// @param pulseId
                    virtual void ReceivePulse(
                        const ::Smp::Int32 pulseId);


                // -----------------------------------------------------------------------------
                // ---------------------------------- IModel -----------------------------------
                // -----------------------------------------------------------------------------

                    /// Publish fields, operations and properties of the model.
                    /// @param receiver Publication receiver.
                    void Publish(
                        ::Smp::IPublication* receiver)
                        throw (::Smp::IModel::InvalidModelState);

                    /// Request for configuration.
                    /// Request the model to perform any custom configuration. The
                    /// model can  create  and  configure  other models  using the
                    /// field values of its published fields.
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
                    void Configure(
                        ::Smp::Services::ILogger* logger)
                        throw (::Smp::IModel::InvalidModelState);

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
                    void Connect(
                        ::Smp::ISimulator* simulator)
                        throw (::Smp::IModel::InvalidModelState);

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
                    virtual const Smp::Uuid& GetUuid(void) const;


                // -----------------------------------------------------------------------------
                // ---------------------------- IDynamicInvocation -----------------------------
                // -----------------------------------------------------------------------------

                    /// Invoke the operation for the given request.
                    /// @param request Request object to invoke.
                    void Invoke(
                        ::Smp::IRequest* request)
                        throw (
                            ::Smp::IDynamicInvocation::InvalidOperationName,
                            ::Smp::IDynamicInvocation::InvalidParameterCount,
                            ::Smp::IDynamicInvocation::InvalidParameterType);


                // -----------------------------------------------------------------------------
                // ------------------------- Custom public declarations ------------------------
                // -----------------------------------------------------------------------------
// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Public_Fields--
                // MARKER: EXTRA FIELD DECLARATIONS: START
                // INSERT HERE EXTRA FIELD DECLARATIONS
                    SumL3* sum;
                    GyroHoneywell* gyroL3;
                    Smp::Bool hasToBeRegistered=true;
                // MARKER: EXTRA FIELD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Public_Fields--

// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Public_Methods--
                // MARKER: EXTRA METHOD DECLARATIONS: START
                    static Smp::Management::IManagedModel* CreateContainedModel(
                            Smp::IDynamicSimulator* dynsim,
                            Smp::String8 obj_name,
                            Smp::IComposite* parent,
                            Smp::Management::IManagedContainer* container,
                            Smp::Uuid uuid);

                    void CreateAllContainedModels(Smp::IDynamicSimulator* dynsim);
                // MARKER: EXTRA METHOD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Public_Methods--

                // ----------------------------- PROTECTED MEMBERS ----------------------------
                protected:


                // -----------------------------------------------------------------------------
                // --------------------------------- Containers --------------------------------
                // -----------------------------------------------------------------------------

                    /// Internal container of managed type for l3Models.
                    Smp::Mdk::Management::ManagedContainer< ::Smp::IModel>* _l3Models;
                    /// Internal iterator for children of container l3Models.
                    typedef ::Smp::Mdk::Container< ::Smp::IModel>::ChildIterator _l3ModelsIterator;


                // -----------------------------------------------------------------------------
                // ---------------------------- Entry Point Handlers ---------------------------
                // -----------------------------------------------------------------------------

                    virtual void _UpdateAllModels();

                // -----------------------------------------------------------------------------
                // ---------------------------- Request Handlers -------------------------------
                // -----------------------------------------------------------------------------

                    /// It populates the static map of request handler to implement the invoke function.
                    /// The map will be populated recursively first with the class handlers and then
                    /// with all parent handlers. This way specialised handlers take precedence over
                    /// parent class ones.
                    /// @param bluePrint an object to use as blue print if needed
                    /// @param handlers the map to be populated.
                    template <typename _Type>
                    static void PopulateRequestHandlers(
                        _Type* bluePrint,
                        typename ::Smp::Mdk::Management::RequestContainer<_Type>::Map& handlers);

                // -----------------------------------------------------------------------------
                // ----------------------- Custom protected declarations -----------------------
                // -----------------------------------------------------------------------------
// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Protected_Fields--
                // MARKER: EXTRA FIELD DECLARATIONS: START
                // INSERT HERE EXTRA FIELD DECLARATIONS

                // MARKER: EXTRA FIELD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Protected_Fields--

// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Protected_Methods--
                // MARKER: EXTRA METHOD DECLARATIONS: START
                // INSERT HERE EXTRA METHOD DECLARATIONS
                // MARKER: EXTRA METHOD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Protected_Methods--

                // ----------------------------- PRIVATE MEMBERS ----------------------------
                private:

// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Copy/Assignment--
                // -----------------------------------------------------------------------------
                // ------------------------ (Hidden) Copy / Assignment -------------------------
                // -----------------------------------------------------------------------------

                // MARKER: COPY / ASSIGNMENT DECLARATION: START
                // CUSTOMISE COPY / ASSIGNMENT DECLARATION
                    /// Hide copy constructor.
                    DssRoot(const DssRoot&);
                    /// Hide assignment operator.
                        /// @return Instance a value was assigned to.
                    DssRoot& operator =(const DssRoot&);
                // MARKER: COPY / ASSIGNMENT DECLARATION: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Copy/Assignment--

                // -----------------------------------------------------------------------------
                // -------------------------- Initialise and Cleanup ---------------------------
                // -----------------------------------------------------------------------------

                    /// Initialise private fields.
                    void _Initialise(void);

                    /// Release private fields.
                    void _Cleanup(void);

                // -----------------------------------------------------------------------------
                // ---------------------------- Request Handlers -------------------------------
                // -----------------------------------------------------------------------------

                    /// Map containing all the model invoke handlers.
                    /// The actual map type is specified in the MDK.
                    static ::Smp::Mdk::Management::RequestContainer<DssRoot>::Map requestHandlers;

                // -----------------------------------------------------------------------------
                // ------------------------- Custom private declarations -----------------------
                // -----------------------------------------------------------------------------
// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Private_Fields--
                // MARKER: EXTRA FIELD DECLARATIONS: START
                    Smp::Int32 n_update =0;
                // MARKER: EXTRA FIELD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Private_Fields--

// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Private_Methods--
                // MARKER: EXTRA METHOD DECLARATIONS: START
                // INSERT HERE EXTRA METHOD DECLARATIONS
                // MARKER: EXTRA METHOD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Private_Methods--
            };

            // -----------------------------------------------------------------------------
            // ---------------------- Implentation of templated methods --------------------
            // -----------------------------------------------------------------------------

// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::PopulateRequestHandlers--
            /// It populates the static map of request handler to implement the invoke function.
            /// The map will be populated recursively first with the class handlers and then
            /// with all parent handlers. This way specialised handlers take precedence over
            /// parent class ones.
            /// @param bluePrint an object to use as blue print if needed
            /// @param handlers the map to be populated.
            template <typename _Type>
            void DssRoot::PopulateRequestHandlers(
            _Type* bluePrint,
            typename ::Smp::Mdk::Management::RequestContainer<_Type>::Map& handlers)
            {
                typedef ::Smp::Mdk::Management::RequestContainer<_Type> Help;

                // MARKER: CUSTOM CODE: START
                // Add custom code here
                // MARKER: CUSTOM CODE: END

                Help::template AddIfMissing< void>(handlers, "UpdateAllModels", ::Smp::ST_None, &DssRoot::_UpdateAllModels);
                // Operation ReceivePulse is not marked for View
                Help::template AddIfMissing< ::Smp::String8>(handlers, "get_ReceiverId", ::Smp::ST_String8, &DssRoot::get_ReceiverId);

            }
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::PopulateRequestHandlers--

// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::After_Declaration--
            // MARKER: EXTRA METHOD DECLARATIONS: START
            // INSERT HERE EXTRA METHOD DECLARATIONS
            // MARKER: EXTRA METHOD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::After_Declaration--

        }
    }
}


#endif // _TASI_SVE_DSS_DSSROOT_H_

// --OPENING ELEMENT--SPR_LOG_SECTION--
// The SPR history contains the description and reference of
// the SPRs closed on the file.
//
// MARKER: SPR_LOG: START
// Insert SPR log here (Including: Date, Author, SPR REF, Description). 
// MARKER: SPR_LOG: END
// --CLOSING ELEMENT--SPR_LOG_SECTION--

