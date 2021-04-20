/******************************************************************************

   PROJECT:      DSS - S/C Dynamics Simulation Software
   FILE:         GYRO_HONEYWELL.cpp
   AUTHOR:       Marco Anania / Francesco Sabbioni

   OVERVIEW
   ========
   Source file for the GYRO_HONEYWELL 

   CHANGE LOG
   ==========
   ***************************************************************************
   18/07/2011 F.Sabbioni
   1) SHRT_MAX has been replaced with  32767;
   2) SHRT_MIN has been replaced with -32768;
   3) DELTA_INT_16 been replaced with  65535;
   4) Check on the "is_the_final_updating" flag has been added in the Update 
      Level-2 function
   ***************************************************************************
   
   *******************************************************************
   F.Sabbioni 11/04/2012
   Added Wrap around TC/TM (B1553 Rx/Tx transactions)  
   *******************************************************************
   *******************************************************************
   D.Cascone 
   Added bias due to the MGT commutation  
   Now the rate measurement is initialiazed with the value set in dss.set for the SC
   *******************************************************************  
   
   *******************************************************************
   F.Sabbioni/L. Galvagni/C. Celiberti 03/07/2017
   ARW noise is now computed in the Update function and added to the Derivative State (Xdot)
   at each integration step in the Status function
   The following statement shall be added in the DSS ini file: GYRO_HONEYWELL_1.UPDATE_STEP_TIME = 0.125
   in order to correctly avaluate the ARW noise value and all the model outputs.
   The Y.w_meas is in unit reference frame
   *******************************************************************
   
******************************************************************************/
#include "GYRO_HONEYWELL.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include "MathLib.h"

//-------------------------------------------------------------------------
GYRO_HONEYWELL::GYRO_HONEYWELL (const char *ModelName, int RT_Address)
       :GenericObject (ModelName, sizeof(GYRO_HONEYWELL) , sizeof(X), sizeof(U), sizeof(Y), &X, &Xdot, &U, &Y)
//-------------------------------------------------------------------------
{
   if (1 <= RT_Address && RT_Address <= 32)
      Set_Rt1553_Address (RT_Address);
}
//-------------------------------------------------------------------------
GYRO_HONEYWELL::~GYRO_HONEYWELL ()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void   GYRO_HONEYWELL::Initialize     (typ_return &result)
//-------------------------------------------------------------------------
{
   //Initializations
   OFF_ON_Switch_Status = Off;
   
   result = Nominal;

   // INITIALIZE VALIDITY FLAG STRUCTURE
   Health_Bits.hb_int = 0;
   Health_Bits.hb.data_validity= 1;
   
   // Initialize time frame counter
   internal_clock_200Hz = 0;
   internal_clock_1Hz   = 0;
   memset(&UTC_TIME, '\0', sizeof(UTC_TIME));
   
   // Initialize ARW values
   memset(ARW_noise, '\0', sizeof(ARW_noise));

   /* *********************************************************************************
   * READS CHANNEL PARAMETERS FROM SET FILE
   **********************************************************************************/
   /* ********************************************************
   * BODY TO UNIT MOUNTING MATRIX
   ***********************************************************/
   input_file->load (&P.BRF2UNIT[0][0],  Name, "BRF2UNIT" , 9);
   MAT_INV(P.UNIT2BRF,P.BRF2UNIT);
   /* ********************************************************
   *  RESIDUAL UNCALIBRATED BIAS [°/hr]
   ***********************************************************/
   input_file->load (&P.bias,  Name, "BIAS" , 1);
   /* ********************************************************
   *  RESIDUAL UNCALIBRATED MAGNETIC BIAS [°/hr/GAUSS]
   ***********************************************************/
  // input_file->load (&P.mgn_bias, Name,"MAGNETIC_BIAS", 1); 
   /* ********************************************************
   * ANGULAR RANDOM WALK [°/sqrt(hr)]
   ***********************************************************/
   input_file->load (&P.ARW_noise,  Name, "ARW" , 1);
   /* ********************************************************
   * READ OUT NOISE [rad]
   ***********************************************************/
   input_file->load (&P.OUT_noise,  Name, "RON" , 1);
   /* ********************************************************
   * OUTPUT TIME TAG LSB [Sec/(LSB)]
   ***********************************************************/
   input_file->load (&P.time_tag_LSB,  Name, "TIME_LSB" , 1);
   /* ********************************************************
   * OUTPUT SCALE FACTOR  [rad/(LSB)]
   ***********************************************************/
   input_file->load (&P.sf,  Name, "ANGLE_LSB" , 1);
   /* ********************************************************
   *  SYNCRO SAMPLE TIME [s]
   ***********************************************************/
   input_file->load (&P.sample_time,  Name, "SAMPLE_TIME" , 1);
   /* ********************************************************
   *   MODEL ERROR SOURCES ENABLE FLAGS
   ***********************************************************/
   input_file->load (&P.noise_select.ARW,          Name, "ARW_ENABLED",     1);
   input_file->load (&P.noise_select.output_noise, Name, "OWN_ENABLED",     1);
   input_file->load (&P.noise_select.rate_bias,    Name, "BIAS_ENABLED",    1);
   //input_file->load (&P.noise_select.mgt_dist,     Name, "MGT_DISTURBANCE", 1);
   /* ********************************************************
   *   GYRO POSITION WRT SC
   ***********************************************************/   
   input_file->load (P.p_sc,      Name, "POSITION_WRT_SC", 3); 
   /* ********************************************************
   *   FIRST OUTPUT RATE
   ***********************************************************/   
   if( input_file->load (Y.w_meas, "SC_ANGULAR_RATE", 3, false) ){
      for(int i=0; i<3; i++)
         Y.w_meas[i] = Y.w_meas[i]*DEG2RAD;
   }
   
   //Initialize wrap around data words
   for(int i=0; i<32; i++)
      crgs_wrap_around_words[i] = 0x0000;
   
   // Publish the INPUT array data
   Publish (U.omega,        3,    "U.omega_BRF",   "rad/s");

   // Publish the OUTPUT array data
   Publish ( Y.angle_LSB,                    3,   "Y.angle_LSB", "");
   Publish (&Y.frame_timer_200Hz,            1,   "Y.frame_timer_200Hz", "");
   Publish ( Y.w_meas,                       3,   "Y.w_meas", "rad/s");
   Publish (&Y.health_status_bits,           1,   "Y.health_status_bits", "");
   
}

//-------------------------------------------------------------------------
void   GYRO_HONEYWELL::Get_Input      (typ_return &result, double time, bool is_the_first_getinput)
//-------------------------------------------------------------------------
{
   result = Nominal;
}


//-------------------------------------------------------------------------
void   GYRO_HONEYWELL::Status         (typ_return &result, double time)
//-------------------------------------------------------------------------
{  
   result = Nominal;
   //static bool firstTimeWarning[3]={true,true,true};

   if(OFF_ON_Switch_Status==On){
      
      //PROJECTS ANGULAR VELOCITY ON GYRO ASSEMBLY AXES
      result = MAT_PROD(&w_crgs_axes[0],&P.BRF2UNIT[0][0], U.omega, 3, 3, 1);

      /* Check high rate condition*/
      if(w_crgs_axes[0] > CGRS_MAX_RATE || w_crgs_axes[1] > CGRS_MAX_RATE || w_crgs_axes[2] > CGRS_MAX_RATE)
      {
         Log_Warning ("%s high rate condition is occurred", Name);   
      }    
      
      /*********************************************************
      * COMPUTE SCALE FACTOR MODEL
      ***********************************************************/
      S0 = P.sf; 
      /*********************************************************
      * COMPUTE BIAS MODELS 
      * constant bias + magnetic momentum induced bias
      ***********************************************************/
      K0 = 0.0;
      if( P.noise_select.rate_bias > 0 ){
         K0 += P.bias * DEG2RAD /3600; 
      }                         
      // If enabled computes MGT disturbance
    //  if( P.noise_select.mgt_dist > 0 ) { 
    //     if( MGT_vec[0] == NULL || 
    //           MGT_vec[1] == NULL || 
    //              MGT_vec[2] == NULL){
    //        MGT_vec[0] = (MGT*) Get_Object_by_Name ("MGT1");
    //        MGT_vec[1] = (MGT*) Get_Object_by_Name ("MGT2");
    //        MGT_vec[2] = (MGT*) Get_Object_by_Name ("MGT3");   
    //     }
    //     double b_sc_total_module_G = 0.0;
    //     double b_sc_mgt[3]         = {0.0,0.0,0.0}; 
    //     double b_sc_total[3]       = {0.0,0.0,0.0};
    //     // Get the Magnetic Field induced by the Magnetotorquers and add it to the Earth Field
    //     for (int j = 0; j < 3; j++){
    //        if (MGT_vec[j] != NULL){
    //           MGT_vec[j]->B_Field (b_sc_mgt, P.p_sc);
    //           Log_Debug ("%s MGT-%d B_Field =[%g %g %g]", Name, j+1, b_sc_mgt[0], b_sc_mgt[1], b_sc_mgt[2]);
    //           for (int i = 0; i < 3 ; i++)  
    //              b_sc_total[i] += b_sc_mgt[i] * 1e4; // Total magneti field in GAUSS
    //        }
    //        else if(firstTimeWarning[j]){
    //           firstTimeWarning[j] = false;
    //           Log_Warning ("%s MGT%d model not found in MGT disturbance computation", Name, j+1);
    //        }
    //     }
    //     // Calculate magnetic bias
    //     b_sc_total_module_G = Sqrt(b_sc_total[0]*b_sc_total[0]+b_sc_total[1]*b_sc_total[1]+b_sc_total[2]*b_sc_total[2]);
    //     // Sum magnetic bias to total rate bias in rad/s
    //     K0 += P.mgn_bias * b_sc_total_module_G * DEG2RAD /3600;
    //     Log_Debug ("%s MGT disturbance b_sc_total=[%g %g %g] b_sc_total_module_G=%g K0=%g", Name, b_sc_total[0], b_sc_total[1], b_sc_total[2], b_sc_total_module_G, K0);
    //  }

      /*********************************************************
      * COMPUTE DERIVATIVE STATES
      ***********************************************************/
      for (int i=0; i<3; i++)
      {
         Xdot.angle[i] = (w_crgs_axes[i] + K0 + ARW_noise[i]) / S0;
      }
      Log_Debug ("%s.GYRO_HONEYWELL::Status --- ARW_noise = %f", Name, ARW_noise);   
      
      /*********************************************************
      * MANAGE real rapresentation of the data values
      * ANGLE COUNTER is a 16bit 2's complement
      ***********************************************************/
      for (int i=0; i<3; i++)
      {
         if(X.angle[i] > 32767)
            X.angle[i] = -32768 + fmod(X.angle[i], 32767);
         else if(X.angle[i] < -32768)
            X.angle[i] = 32767 + fmod(X.angle[i], -32768);
      }
      
   }
}

//-------------------------------------------------------------------------
void   GYRO_HONEYWELL::Update (typ_return &result, double time, bool is_the_final_updating)
//-------------------------------------------------------------------------
{
   unsigned short delta_time_frame = 0.0;
   double actualAng_LSB[3], deltaAng_LSB[3];
   static double residual_delta_time = 0.0;
   result = Nominal;

   delta_time_at_last_update = time - time_at_last_update;

   if(OFF_ON_Switch_Status==On && is_the_final_updating == true && delta_time_at_last_update>0)
   {      

         /*********************************************************
         * ARW COMPUTATION
         * ARW is computed as a GAUSSIAN WHITE NOISE and will be added 
         * in the STATUS function on the Derivative State (Xdot) at each integration step
         *********************************************************/
         for (int i=0; i<3; i++)
         {  
             ARW_noise[i] = ARW(P.ARW_noise,1.0/delta_time_at_last_update);
         }
         Log_Debug ("%s.GYRO_HONEYWELL::Update --- delta_time_at_last_update = %f, ARW_noise = %f", Name, delta_time_at_last_update, ARW_noise);   
         
         /* ********************************************************
         * Time counters update
         ***********************************************************/
         time_at_last_update = time;
         delta_time_frame = (unsigned short)((delta_time_at_last_update + residual_delta_time) / P.time_tag_LSB); 
         residual_delta_time = (delta_time_at_last_update + residual_delta_time) - delta_time_frame *P.time_tag_LSB;

         /* Update 200Hz time frame counter*/
         internal_clock_200Hz = internal_clock_200Hz + delta_time_frame;

         if (internal_clock_200Hz >= 200) 
         {
            /* Reset 200Hz time frame counter */
            internal_clock_200Hz = internal_clock_200Hz - 200; // DC. the counter wrap up mechanism takes into account the dt due to lower sampling

            /* Update 1Hz time frame counter */
            internal_clock_1Hz = internal_clock_1Hz + 1;
         }
         
         /* Update 1553 time counter*/
         // SEC_FIELD_LSB = 40.96 us
         unsigned long seconds_lsb = ((unsigned int)UTC_TIME.SMSEC_H << 16) + (unsigned int)UTC_TIME.SMSEC_L;
         seconds_lsb += delta_time_at_last_update / SEC_FIELD_LSB;
         // Check day wrap
         if( seconds_lsb >= 86400.0/SEC_FIELD_LSB ){
            UTC_TIME.DAY += 1;
            seconds_lsb = 0;
         }
         UTC_TIME.SMSEC_H = (seconds_lsb & 0xFFFF0000) >> 16;
         UTC_TIME.SMSEC_L = seconds_lsb & 0x0000FFFF;
   
         /* ********************************************************
         * MANAGE real rapresentation of the data values
         * ANGLE COUNTER is a 16bit 2's complement
         * TIME TAG COUNTER is a 8bit unsigned
         ***********************************************************/
         for (int i=0; i<3; i++)
         {
            if(X.angle[i] > 32767)
               X.angle[i] = -32768 + fmod(X.angle[i], 32767);
            else if(X.angle[i] < -32768)
               X.angle[i] = 32767 + fmod(X.angle[i], -32768);
         }
         
         /* ************************************************************
         * Compute OUTPUT QUANTIZATION NOISE (if enabled)
         ***************************************************************/
         double qwn;
         if(P.noise_select.output_noise > 0)
            qwn = QWN() / S0;  // LSB
         else
            qwn = 0;   
         
         /* ************************************************************
         *  UPDATE OUTPUTS
         ***************************************************************/
         for(int i=0; i<3; i++)
         {
            Y.angle_LSB[i] = X.angle[i] + qwn;     //LSB
            /* ********************************************************
            * MANAGE real rapresentation of the data values
            * ANGLE COUNTER is a 16bit 2's complement
            ***********************************************************/
            if(Y.angle_LSB[i] > 32767)
               Y.angle_LSB[i] = -32768 + fmod(Y.angle_LSB[i], 32767);
            else if(Y.angle_LSB[i] < -32768)
               Y.angle_LSB[i] = 32767 + fmod(Y.angle_LSB[i], -32768);
         }
      
         Y.frame_timer_200Hz  = internal_clock_200Hz;         // FMTR_200HZ_1HZ
         Y.health_status_bits = Health_Bits.hb_int;
         
         /* *******************************************************
         * Compute measured PRE-PROCESSED angular velocity (CHANNEL)
         * Y.w_meas [rad/s]
         * If the delta angle is more of the maximum delta angle avaible at the
         * maximum angular velocity it means that a counter overflow was
         * occurred
         * MAX VEL = 375[deg/s]/(200[Hz]/8[Hz]) = 15 [deg/s] = 0.26179938 [rad/s]
         ********************************************************/
         for(int i=0; i<3; i++)
         {
            actualAng_LSB[i] = Y.angle_LSB[i];
            
            /* The following code is the same used in SW-PREPROCESSING */
            //65535 = INT16_MAX - INT16_MIN = 32767 - -32768;
            deltaAng_LSB[i] = actualAng_LSB[i] - lastAng_LSB[i];
            
            if(deltaAng_LSB[i] > 32767)
            {
               deltaAng_LSB[i] = deltaAng_LSB[i] - 65535;
            }               
            else if(deltaAng_LSB[i] < -32768)
            {
               deltaAng_LSB[i] = deltaAng_LSB[i] + 65535;
            }            
            Y.w_meas[i] = (deltaAng_LSB[i]*S0) / (delta_time_at_last_update);

            lastAng_LSB[i]  = actualAng_LSB[i];
         }
   }
   else if(OFF_ON_Switch_Status==Off)
   {
      memset(&Y, '\0', sizeof(Y));  
   }
   
}


//-------------------------------------------------------------------------
void  GYRO_HONEYWELL::Local_Parser   (typ_return &result, char* model_command, int n_parameters, char* parameters[])
//-------------------------------------------------------------------------
{  
   result = Error;
   switch (toupper(model_command[0]))
    {
        case 'S':
            if (!strcmp (model_command, "SET_DATA_VALIDITY_BIT") && n_parameters == 1){
               Health_Bits.hb.data_validity  = atoi(parameters[0]);
               result                        = Nominal;
            }
            else if (!strcmp (model_command, "SET_GO_NOG_BIT") && n_parameters == 1){
               Health_Bits.hb.GO_NOGO  = atoi(parameters[0]);
               result                  = Nominal;
            }
            break;
    }
    if (result == Error)
        Log_Warning ("Command string \"%s\" is not allowed for the Model \"%s\", or some of the \"%i\" parameters is wrong", model_command, Name, n_parameters);
}

//-------------------------------------------------------------------------
typ_return GYRO_HONEYWELL::Command_Switch_ON ()
//-------------------------------------------------------------------------
{
   GenericObject::Command_Switch_ON ();
   time_at_last_update = Get_Current_Epoch();
   // Reset internal status variables
   internal_clock_200Hz = 0;
   internal_clock_1Hz   = 0;
   memset(&UTC_TIME, '\0', sizeof(UTC_TIME));
   memset(&X, '\0', sizeof(X));
   return Nominal;
}


//-------------------------------------------------------------------------
double GYRO_HONEYWELL::ARW(double arw, double sample_time)
//-------------------------------------------------------------------------
{
/******************************************************************************
* Create an Gaussian white noise to be add to the ANGLE input in order to add Angular Random
* Walk effect on GYRO measure. INPUTS:
* arw             --> ARW noise value [°/sqrt(hr)] 1-sigma
* sampling_time   --> integration sampling time [s]
*******************************************************************************/
   double sigma;
   // Convert ARW to sigma of White Noise
   sigma = (arw * Sqrt(sample_time)) * DEG2RAD /60; //rad/s
   return RandomNormal(0,sigma);
}


//-------------------------------------------------------------------------
double GYRO_HONEYWELL::QWN()
//-------------------------------------------------------------------------
{
/******************************************************************************
* Create an Gaussian white noise to be add to the ANGULAR output in order to add 
* the QUANTIZATION WHITE NOISE
*******************************************************************************/
   return RandomNormal(0,P.OUT_noise); //[('')/LSB]
}

//-------------------------------------------------------------------------
typ_return GYRO_HONEYWELL::Rt1553_ReceiveData (const unsigned char Subaddress, const unsigned char DataWordCount, unsigned short pDataWords[32])
//-------------------------------------------------------------------------
{
    typ_return result = Nominal;
    
    int i;
    
    //unsigned short datavalue = 0;
    if(OFF_ON_Switch_Status == On)
    {    
        // Apply the User-Required transaction modifications
        Modify_Rt1553_Buffer (&RxModifier[Subaddress], pDataWords); 

        switch (Subaddress)
        {
            case 30: /* Wrap around */
                if ( DataWordCount <= 32 )
                {
                  result = Nominal;
                  for(i=0; i<DataWordCount; i++)
                     crgs_wrap_around_words[i] = pDataWords[i];
                }
                else
                {
                  result = Nominal;
                  Log_Warning ("%s.RT Receiving Subaddress %d, wrong dataword numbers (DataWordCount = %d)", Name, Subaddress, DataWordCount);
                }
            break;
            

            default:
               Log_Warning ("%s.RT Receiving Subaddress %d is not implemented for the model.", Name, Subaddress);   
            break;
        }
    }
    
    return result;
}

//-------------------------------------------------------------------------
typ_return GYRO_HONEYWELL::Rt1553_TransmitData (const unsigned char Subaddress, const unsigned char DataWordCount, unsigned short pDataWords[32])
//-------------------------------------------------------------------------
{
    typ_return result = Nominal;
    int i = 0;
    
    //unsigned int temp_uint;
      //       int temp_int;
    
    if(OFF_ON_Switch_Status == On)
    {   
        for (i = 0; i < DataWordCount; i++) pDataWords[i] = 0x0000;
        switch (Subaddress)
        {
           case 3: /*  */
                /* Supported: 13 datawords */
                if (DataWordCount != 13)
                    result = Error;
                else
                {
                    /*DATAWORD format*/
                    //MSB ----------------- LSB 
                    // 0  -----------------  15  
                   
                    // 1553B_time_tag 
                    pDataWords[0] = UTC_TIME.SMSEC_H;
                    pDataWords[1] = UTC_TIME.SMSEC_L;
                   
                    /*******************/
                    /* fmtr_PLC_status */
                    /*******************/
                    //PLC_reset_active (3 bits -> bit 0 to bit 2)
                    pDataWords[2] |= 0x0007 & ((unsigned short) 0);
                    
                    //attitude_rate_overflow (3 bits -> bit 3 to bit 5)
                    pDataWords[2] |= 0x0038 & ((unsigned short) 0);
                    
                    //stimulate_status (1 bit -> bit 6)
                    pDataWords[2] |= 0x0040 & ((unsigned short) 0);
                    
                    //refresh_in_progress_flag (1 bit -> bit 7)
                    pDataWords[2] |= 0x0080 & ((unsigned short) 0);
                    
                    //FMTR_200HZ_1HZ (8 bits -> bit 8 to bit 15)
                    pDataWords[2] |= 0x00FF & ((unsigned char)internal_clock_200Hz);                    
                    
                    //FMTR_1HZ (16 bits -> bit 0 to bit 15)
                    pDataWords[3] |= 0xFFFF & (internal_clock_1Hz); 
                    
                    /*******************/
                    /* imu_status */
                    // Bits 0-7 = Unit operating modes.
                    // Bit 8 = Data Valid Flag
                    // Bit 9-14 = Status Control Monitors
                    // Bit 15 = GO/NOGO (0 = GO, 1 = NOGO)
                    //
                    // On ICD bits are numbered in accordance with MIL-STD-1750A wherein
                    // left-most bit 0 is the MSB and is transmitted first.
                    /*******************/
                    pDataWords[4] = Health_Bits.hb_int;
                    
                    //M_FRAME_THETA word 6,7,8 (GyroAngle X,Y,Z)
                    pDataWords[5] = (short)(Y.angle_LSB[0]); 
                    pDataWords[6] = (short)(Y.angle_LSB[1]); 
                    pDataWords[7] = (short)(Y.angle_LSB[2]); 
                }

            break;
            
            case 30: /* Wrap around command */
                if ( DataWordCount <= 32 )
                {
                  result = Nominal;
                  for(i=0; i<DataWordCount; i++)
                     pDataWords[i] = crgs_wrap_around_words[i];
                }
                else
                {
                  result = Nominal;
                  Log_Warning ("%s.RT Transmit Subaddress %d, wrong dataword numbers (DataWordCount = %d)", Name, Subaddress, DataWordCount);
                }
            break;
            
            // Blocks or Subaddresses 9 - 20 are updated at 1 Hz, data not implemented 
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
               for (i = 0; i < DataWordCount; i++) 
                  pDataWords[i] = 0xCACA;
               result = Nominal;
            break;
            
            default:
                result = Nominal;         
                Log_Warning ("%s.RT Trasmitting Subaddress %d is not implemented for the model.", Name, Subaddress);   
            break;
        }

        // Apply the User-Required transaction modifications
        Modify_Rt1553_Buffer (&TxModifier[Subaddress], pDataWords);    
    }
    else
        result = Nominal;

    return result;
}

//-------------------------------------------------------------------------
typ_return GYRO_HONEYWELL::Rt1553_ReceiveModeCommand (const typ_B1553_TxRx TxRx, const typ_B1553_ModeCode ModeCode, unsigned short pDataWord[32])
//-------------------------------------------------------------------------
{ 
   return Nominal;
}
