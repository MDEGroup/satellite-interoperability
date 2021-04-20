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
#include "tasi/sve/common/PulseGenerator.h"

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

                Smp::Char8* buffer1 = new Smp::Char8[strlen("Model Updated iteration:")+4];
                sprintf(buffer1,"Model Updated iteration:%d",n_update);
            	if (n_update % 2 ==0){
            		_UpdateAllModels();

            		m_simulator->GetLogger()->Log(this, buffer1, ::Smp::Services::LMK_Warning);
            	}

            	n_update++;
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
                return this->GetName();
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
            	//dummy variables
            	sum->UpdateSum->Execute();
            	gyroL3->Update();


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

                if (common::PulseGenerator::RtcPulse != NULL &&
                	this->hasToBeRegistered == true)
                {
                	common::PulseGenerator::RtcPulse->RegisterReceiver(dynamic_cast<common::IPulseReceiver*>(this));

                }
                // MARKER: CONNECT BODY: END
            }
// --CLOSING ELEMENT--DssRoot::Connect--

            // -----------------------------------------------------------------------------
            // ------------------------ Custom method definitions --------------------------
            // -----------------------------------------------------------------------------
// --OPENING ELEMENT--::tasi::sve::dss::DssRoot/DssRoot::Extra_Methods--
            // MARKER: EXTRA METHOD DEFINITIONS: START
            // INSERT HERE EXTRA METHOD DEFINITIONS
            Smp::Management::IManagedModel* DssRoot::CreateContainedModel(
                                Smp::IDynamicSimulator* dynsim,
                                Smp::String8 obj_name,
                                Smp::IComposite* parent,
                                Smp::Management::IManagedContainer* container,
                                Smp::Uuid uuid) {


                            Smp::IComponent*                component = NULL;
                            Smp::Management::IManagedModel* model     = NULL;
                            Smp::Char8*                     name = new Smp::Char8[strlen(obj_name)+1];
                            sprintf(name,"%s",obj_name);



                            component          = dynsim->CreateInstance(uuid);
                            model              = dynamic_cast<Smp::Management::IManagedModel*>(component);

                            model->SetName(name);

                            Smp::Char8* buffer = new Smp::Char8[strlen(name)+strlen(" instance")+1];
                            sprintf(buffer,"%s%s", name," instance");

                            model->SetDescription(buffer);
                            model->SetParent(parent);
                            container->AddComponent(component);

                            delete[] buffer;
                            delete[] name;

                            return model;
                        }

                        void DssRoot::CreateAllContainedModels(Smp::IDynamicSimulator* dynsim)
                        {

                        	Smp::Management::IManagedModel*     model     = NULL;
                        	Smp::Management::IManagedContainer* container = NULL;
                        	Smp::IComposite*                    parent    = dynamic_cast<Smp::IComposite*>(this);

                        	container = this->l3Models;
                        	//Create the SUM model
                        	model = DssRoot::CreateContainedModel(
                        			dynsim,
            						"SUM_L3",
            						parent,
            						container,
            						::tasi::sve::dss::Uuid_SumL3);
                        	assert(model);
                        	this->sum = dynamic_cast<dss::SumL3*>(model);
                        	//Create the model for the gyroscope, brand Honeywell
                        	model = DssRoot::CreateContainedModel(
                        			dynsim,
            						"GYR01",
            						parent,
            						container,
            						::tasi::sve::dss::Uuid_GyroHoneywell);
                        	assert(model);
                        	this->gyroL3 = dynamic_cast<dss::GyroHoneywell*>(model);
                        	this->gyroL3->gyroL2 = new GYRO_HONEYWELL("GYRO_HONEYWELL_1",1); //nomi presi dal file csg_unit_names.h

                        	//Create the model for the gyroscope, brand Honeywell
                        	model = DssRoot::CreateContainedModel(
                        			dynsim,
            						"GYR02",
            						parent,
            						container,
            						::tasi::sve::dss::Uuid_GyroHoneywell);
                        	assert(model);
                        	this->gyro02L3 = dynamic_cast<dss::GyroHoneywell*>(model);
                        	this->gyro02L3 -> gyroL2 = new GYRO_HONEYWELL("GYRO_HONEYWELL_2",2);

                        }
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

