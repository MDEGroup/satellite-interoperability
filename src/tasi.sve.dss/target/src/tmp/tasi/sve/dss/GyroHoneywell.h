// -----------------------------------------------------------------------------
//
// (C) 2021 TAS-I
//
// -----------------------------------------------------------------------------
//
// Sub-System    : tasi_sve_dss
//
// File Name     : GyroHoneywell.h
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

/// @file /tasi/sve/dss/GyroHoneywell.h

#ifndef _TASI_SVE_DSS_GYROHONEYWELL_H_
#define _TASI_SVE_DSS_GYROHONEYWELL_H_

// -----------------------------------------------------------------------------
// ------------------------- Includes for Simple Types -------------------------
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// --------------------------------- Includes ----------------------------------
// -----------------------------------------------------------------------------

#include "Mdk/SimpleTypes.h"
#include "Mdk/Model.h"
#include "Smp/IDynamicInvocation.h"
#include "Mdk/Management/ManagedModel.h"

// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Includes--
// MARKER: FILE INCLUSION: START
#include "libs/dss_level_2/GYRO_HONEYWELL.h"

// MARKER: FILE INCLUSION: END

// MARKER: FORWARD DECLARATIONS: START
// INSERT HERE FORWARD DECLARATIONS
// MARKER: FORWARD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Includes--

namespace tasi
{
    namespace sve
    {
        namespace dss
        {

            // ----------------------------------- UUID ------------------------------------

            /// Universally unique identifier of model GyroHoneywell.
            /// @return Universally Unique Identifier of GyroHoneywell.
            static const ::Smp::Mdk::Uuid Uuid_GyroHoneywell("a9755682-b63b-352c-b87d-f4d407e0856a");

            // ------------------------ Declaration of Model class ---------------------------
            class GyroHoneywell:
                  public ::Smp::Mdk::Management::ManagedModel,
                  virtual public ::Smp::IDynamicInvocation
            {

                // ----------------------------- PUBLIC MEMBERS ----------------------------
                public:

                // -----------------------------------------------------------------------------
                // -------------------------- Constructors/Destructor --------------------------
                // -----------------------------------------------------------------------------

                    /// Default constructor.
                    GyroHoneywell(void);

                    /// Constructor setting name, description and parent.
                    /// @param name Name of new model instance.
                    /// @param description Description of new model instance.
                    /// @param parent Parent of new model instance.
                    GyroHoneywell(
                        ::Smp::String8 name,
                        ::Smp::String8 description,
                        ::Smp::IComposite* parent)
                        throw (::Smp::InvalidObjectName);

                    /// Virtual destructor that is called by inherited classes as well.
                    virtual ~GyroHoneywell(void);

                // -----------------------------------------------------------------------------
                // --------------------------------- Operations --------------------------------
                // -----------------------------------------------------------------------------

                    virtual void Update(void);


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
// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Public_Fields--
                // MARKER: EXTRA FIELD DECLARATIONS: START
                    GYRO_HONEYWELL* gyroL2;
                // MARKER: EXTRA FIELD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Public_Fields--

// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Public_Methods--
                // MARKER: EXTRA METHOD DECLARATIONS: START
                // INSERT HERE EXTRA METHOD DECLARATIONS
                // MARKER: EXTRA METHOD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Public_Methods--

                // ----------------------------- PROTECTED MEMBERS ----------------------------
                protected:

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
// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Protected_Fields--
                // MARKER: EXTRA FIELD DECLARATIONS: START
                // INSERT HERE EXTRA FIELD DECLARATIONS
                // MARKER: EXTRA FIELD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Protected_Fields--

// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Protected_Methods--
                // MARKER: EXTRA METHOD DECLARATIONS: START
                // INSERT HERE EXTRA METHOD DECLARATIONS
                // MARKER: EXTRA METHOD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Protected_Methods--

                // ----------------------------- PRIVATE MEMBERS ----------------------------
                private:

// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Copy/Assignment--
                // -----------------------------------------------------------------------------
                // ------------------------ (Hidden) Copy / Assignment -------------------------
                // -----------------------------------------------------------------------------

                // MARKER: COPY / ASSIGNMENT DECLARATION: START
                // CUSTOMISE COPY / ASSIGNMENT DECLARATION
                    /// Hide copy constructor.
                    GyroHoneywell(const GyroHoneywell&);
                    /// Hide assignment operator.
                        /// @return Instance a value was assigned to.
                    GyroHoneywell& operator =(const GyroHoneywell&);
                // MARKER: COPY / ASSIGNMENT DECLARATION: END
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Copy/Assignment--

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
                    static ::Smp::Mdk::Management::RequestContainer<GyroHoneywell>::Map requestHandlers;

                // -----------------------------------------------------------------------------
                // ------------------------- Custom private declarations -----------------------
                // -----------------------------------------------------------------------------
// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Private_Fields--
                // MARKER: EXTRA FIELD DECLARATIONS: START
                // INSERT HERE EXTRA FIELD DECLARATIONS
                // MARKER: EXTRA FIELD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Private_Fields--

// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Private_Methods--
                // MARKER: EXTRA METHOD DECLARATIONS: START
                // INSERT HERE EXTRA METHOD DECLARATIONS
                // MARKER: EXTRA METHOD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::Extra_Private_Methods--
            };

            // -----------------------------------------------------------------------------
            // ---------------------- Implentation of templated methods --------------------
            // -----------------------------------------------------------------------------

// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::PopulateRequestHandlers--
            /// It populates the static map of request handler to implement the invoke function.
            /// The map will be populated recursively first with the class handlers and then
            /// with all parent handlers. This way specialised handlers take precedence over
            /// parent class ones.
            /// @param bluePrint an object to use as blue print if needed
            /// @param handlers the map to be populated.
            template <typename _Type>
            void GyroHoneywell::PopulateRequestHandlers(
            _Type* bluePrint,
            typename ::Smp::Mdk::Management::RequestContainer<_Type>::Map& handlers)
            {
                typedef ::Smp::Mdk::Management::RequestContainer<_Type> Help;

                // MARKER: CUSTOM CODE: START
                // Add custom code here
                // MARKER: CUSTOM CODE: END

                Help::template AddIfMissing< void>(handlers, "Update", ::Smp::ST_None, &GyroHoneywell::Update);

            }
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::PopulateRequestHandlers--

// --OPENING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::After_Declaration--
            // MARKER: EXTRA METHOD DECLARATIONS: START
            // INSERT HERE EXTRA METHOD DECLARATIONS
            // MARKER: EXTRA METHOD DECLARATIONS: END
// --CLOSING ELEMENT--::tasi::sve::dss::GyroHoneywell/GyroHoneywell::After_Declaration--

        }
    }
}


#endif // _TASI_SVE_DSS_GYROHONEYWELL_H_

// --OPENING ELEMENT--SPR_LOG_SECTION--
// The SPR history contains the description and reference of
// the SPRs closed on the file.
//
// MARKER: SPR_LOG: START
// Insert SPR log here (Including: Date, Author, SPR REF, Description). 
// MARKER: SPR_LOG: END
// --CLOSING ELEMENT--SPR_LOG_SECTION--

