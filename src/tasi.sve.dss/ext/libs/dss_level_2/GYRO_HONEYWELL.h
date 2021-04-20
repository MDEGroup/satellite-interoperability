#if !defined(__GYRO_HONEYWELL_h) // Sentry, use file only if it's not already included.
#define __GYRO_HONEYWELL_h

/******************************************************************************
*
*    PROJECT:      DSS - S/C Dynamics Simulation Software
*    FILE:         GYRO_HONEYWELL.h
*    RELEASE:      0.00
*    AUTHOR:       Marco Anania / Francesco Sabbioni
*    DATE:         
*
*    OVERVIEW
*    ========
*    Header file for the GYRO_HONEYWELL Model
*
*    CHANGE LOG
*
*
   *******************************************************************
   F.Sabbioni 11/04/2012
   Added Wrap around TC/TM (B1553 Rx/Tx transactions)  
   *******************************************************************
*    ==========
*
******************************************************************************/

#include "../dss_level_1/GenericObject.h"
//#include "MGT.h"

//#define MAX_MGT_NUM     6
#define SEC_FIELD_LSB   40.96E-6
#define CGRS_MAX_RATE   0.26179939 // [rad/s] | Rate_Max = 375 deg/s/[200 Hz / 8 Hz]; 8Hz = SW sample_rate 

/**
* Class definition for the HONEYWELL three axis ring laser gyro Unit Model .
*/

class GYRO_HONEYWELL : public GenericObject
{

/*-------------------------------------------------------------------------
   Data Type Definition
-------------------------------------------------------------------------*/

public:

   struct typ_status /** Type STATUS shall be mandatory implemented (eventually empty) at GYRO_HONEYWELL HW Model */ 
   {
      double angle[3];                  //!< Comulated angles [LSB]
   };

   struct typ_input /** Type INPUT shall be mandatory implemented (eventually empty) at GYRO_HONEYWELL HW Model */ 
   {
      double omega[3];            //!< Angular velocity along channel Body Reference Frame [rad/s]
   };

   struct typ_output /** Type OUTPUT shall be mandatory implemented (eventually empty) at GYRO_HONEYWELL HW Model */ 
   {
      double angle_LSB[3];     //!< Comulated angle (16 bit 2'complement) [LSB]
      double frame_timer_200Hz;   //!< Frame timer (200Hz 8bit counter) [LSB]
      double w_meas[3];     //!< POST-PROCESSED measured angular velocity in BRF [rad/s]
      double health_status_bits;  //!< health status bits
   };

   /** Define a struct containing all the ENABLE/DISABLE FLAGS of the model (0-> DISABLED 1->ENABLED) **/
   struct model_flags
   {
      int ARW;                          //!< Enable Angular Random Walk error
      int output_noise;                 //!< Enable Quantization Output Noise error
      int rate_bias;                    //!< Enable Bias error
 //     int mgt_dist;                     //!< Enable MGT disturbance
   };

   struct typ_parameters  /** Type PARAMETER shall be mandatory implemented (eventually empty) at GYRO_HONEYWELL HW Model */ 
   {
      double bias;                  //!< Residual Uncalibrated bias       [°/hr]
   //   double mgn_bias;              //!Bias induce by the magnetic field [°/hr/Gauss]
      double p_sc[3];               //!Position wrt the sc [m]
      double sf;                    //!< Scale Factor            [(rad)/(LSB) ]
      double sample_time;           //!< Sample time                 [s]
      double time_tag_LSB;          //!< Time Tag LSB       [(seconds)/(LSB)]
      double ARW_noise;             //!< Angular Random Walk   [°/sqrt(hr)]
      double OUT_noise;             //!< Output Noise                [microrad]
      double BRF2UNIT[3][3];        //!< Unit mounting matrix (BRF to UNIT)    [3x3]
      double UNIT2BRF[3][3];        //!< Unit mounting matrix (UNIT to BRF)    [3x3]
      model_flags    noise_select;  //!< Model error sources FLAGS
     // double enable_mgt_dist;       //!< Enable Drift due to MGT disturbance
   };
    
   struct typ_cds_time  /** Type CDS format time*/
   {
      unsigned short    DAY,
                        SMSEC_H,
                        SMSEC_L;
   }; 
    
#ifdef LITTLE_ENDIAN

   struct typ_health_bits
    {
      unsigned GO_NOGO           :1; //MSB
      unsigned status_control    :6;
      unsigned data_validity     :1;
      unsigned operating_modes   :8; //LSB    
    };

#else
    struct typ_health_bits
    {
      unsigned operating_modes    :8; //LSB
      unsigned data_validity      :1;
      unsigned status_control     :6;
      unsigned GO_NOGO            :1; //MSB
    };

#endif

/*-------------------------------------------------------------------------
   Data declarations
-------------------------------------------------------------------------*/

public:

   typ_status  X;       //!< STATUS X shall be mandatory instanciated at Level 2 HW Model
   typ_status  Xdot;    //!< STATUS DERIVATIVE Xdot shall be mandatory instanciated at Level 2 HW Model
   typ_input   U;       //!< INPUT  U shall be mandatory instanciated at Level 2 HW Model
   typ_output  Y;       //!< OUTPUT Y shall be mandatory instanciated at Level 2 HW Model
   typ_parameters P;    //!< PARAMETER P shall be mandatory instanciated at Level 2 HW Model
   
//private:
   
   double S0,K0;                              //!< S0: Scale factor K0: Bias
   double lastAng_LSB[3];                     //!< Last measured angles
 //  MGT    *MGT_vec[MAX_MGT_NUM];              //!< MGT pointers to get the induced magnetic field vector
   unsigned short internal_clock_200Hz;       //!< Frame timer (200 Hz  8bit counter)
   unsigned short internal_clock_1Hz;         //!< Frame timer (1   Hz 16bit counter)
   typ_cds_time   UTC_TIME;                   //!< Internal UTC timer
                
   /** Define UNION between structure rapresentation and integer rappresentation of Health status bits  **/
   union hbits {
      unsigned short hb_int;
      typ_health_bits hb;
   } Health_Bits;
     
   //ANGULAR VELOCITY ON GYRO ASSEMBLY AXES
   double   w_crgs_axes[3];
   
   // CRGS Wrap around data words
   unsigned short   crgs_wrap_around_words[32];
   
   // ARW value (calculated in Status and used in Update)
   double   ARW_noise[3];
   
/*-------------------------------------------------------------------------
   Function declarations
-------------------------------------------------------------------------*/

public:

   GYRO_HONEYWELL (const char* ModelName, int RT_Address);
   ~GYRO_HONEYWELL ();

   void   Initialize     (typ_return &result);                 
   void   Get_Input      (typ_return &result, double time, bool is_the_first_getinput = true);                                                
   void   Status         (typ_return &result, double time);                                                
   void   Update         (typ_return &result, double time, bool is_the_final_updating = true);             
   void   Local_Parser   (typ_return &result, char* model_command, int n_parameters, char* parameters[]);

   typ_return Command_Switch_ON();  

   virtual typ_return Rt1553_ReceiveData  (const unsigned char Subaddress,  const unsigned char DataWordCount,       unsigned short pDataWords[32]);
   virtual typ_return Rt1553_TransmitData (const unsigned char Subaddress,  const unsigned char DataWordCount,       unsigned short pDataWords[32]);
   virtual typ_return Rt1553_ReceiveModeCommand (const typ_B1553_TxRx TxRx, const typ_B1553_ModeCode ModeCode,       unsigned short pDataWord[32]);        
        
protected:

   /** Compute Angular Random Walk noise*/
   double      ARW            (double arw, double sampleFreq); //!< Return Angular Random Walk noise
   /** Compute Output White Noise*/
   double      QWN            ();                           //!< Return Quantization Withe Noise
   
};

#endif // __GYRO_HONEYWELL_h sentry.

