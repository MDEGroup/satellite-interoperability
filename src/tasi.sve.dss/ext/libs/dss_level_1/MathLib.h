#if !defined(__MathLib_h) // Use file only if it's not already included.
#define __MathLib_h

/******************************************************************************

    PROJECT:      DSS - S/C Dynamics Simulation Software
    FILE:         MathLib.h
    RELEASE:      0.01
    AUTHOR:       Luigi Galvagni, Marco Anania, Francesco Sabbioni
    DATE:         2009/06/15

    OVERVIEW
    ========
    Header file for the Mathematical Lirary

    CHANGE LOG
    ==========

   Luigi Galvagni 2013/09/16 Added a second instance of the "Saturate" function to manage
                             different "tlow" and "thigh" saturation thresholds
   Marco Anania   2015/05/25 Included optional parameter "assign_q_old" to function "CONTINUITY_QUAT"
   Marco Anania   2015/06/30 QUAT_PROP extended to support negative time propagation
                             QUAT_COMPOSE function introduced
                             QUAT_INV function introduced

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "GenericTypes.h"

/*----------------------------------------------------------------------*
*       MATHEMATICAL AND MISCELLANEOUS FUNCTIONS
*----------------------------------------------------------------------*/

/** Square Root, the output is set to zero if the input is less than zero */
inline double Sqrt(double x)                 {return ((x>0.)?sqrt(x):0.);}
/** Arc-Tangent of y/x, the output is set to zero if both the inputs are zero */
inline double Atan2(double y, double x)      {return ((fabs(y)>1.E-14||fabs(x)>1.E-14)?atan2(y,x):0.);}
/** Arc-Sine, the input is clipped to [-1,+1] */
inline double Asin(double x)                 {return ((fabs(x)<=1.)?asin(x):asin(x/fabs(x)));}
/** Arc-Cosine, the input is clipped to [-1,+1] */
inline double Acos(double x)                 {return ((fabs(x)<=1.)?acos(x):acos(x/fabs(x)));}
/** Clip the input value to the thresholds [-t,+t] */
inline double Saturate(double x, double t)   {return ((x<-t)?-t:(x>t?t:x));}
/** Clip the input value to the thresholds [tlow,thigh] */
inline double Saturate(double x, double tlow, double thigh) {return ((x<tlow)?tlow:(x>thigh?thigh:x));}
/** Dead-Band below the threshold [-t,+t] */
inline double Threshold(double x, double t)  {return ((fabs(x)<t)?0.:(x<0.?(x+t):(x-t)));}
/** Quantize the input value on an integer number of LSB = q */
inline double Quantize(double x, double q)   {return (q*floor(x/q+0.5));}
/** Norm of a 3-dimension double array */
inline double Norm(double x[3])              {return sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);}

/** Macro to provide the sign (+1,-1) of the input value */
#define Sign(x)                              (((x)<0.)?(-1):(1))
/** Macro to provide the maximum of the input values */
#ifndef max
   #define max(x,y)                          (((x)>(y))?(x):(y) )
#endif
/** Macro to provide the minimum of the input values */
#ifndef min
   #define min(x,y)                          (((x)<(y))?(x):(y) )
#endif
/** Macro to reduce an angle in the range -PI, +PI */
#define Angle(x)                             (InRange((x),-PI,PI))
/** Macro to provide the number of elements of the input array */
#define SizeArray(array)                     (sizeof(array)/sizeof(array[0]))

/*----------------------------------------------------------------------*
*       DEFINE COMMON CONSTANT
*----------------------------------------------------------------------*/

#define PIX4               12.566370614359172953850573533118         //!< 4*pi-greco value
#define PIX2               6.283185307179586476925286766559          //!< 2*pi-greco value
#define PI                 3.1415926535897932384626433832795         //!< pi-greco value
#define PI_2               1.5707963267948966192313216916398         //!< 1/2*pi-greco value
#define PI_4               0.78539816339744830961566084581988        //!< 1/4*pi-greco value
#define DEG2RAD            0.017453292519943295769236907684886       //!< ratio radians/degrees
#define RAD2DEG            57.295779513082320876798154814105         //!< ratio degrees/radians
#define EARTH_RADIUS       6.37813649E+6                             //!< [m] Equatorial radius          - Wertz, Space Mission Analysis & Design, 3rd Edition, Table B-2 & IERS Conventions (2003), Technical Note No.32
#define EARTH_MU           3.986004418E+14                           //!< [m^3/s^2] Gravity constant     - Wertz, Space Mission Analysis & Design, 3rd Edition, Table B-2 & IERS Conventions (2003), Technical Note No.32
#define NANO2UNIT          1.0E-9                                    //!< NANO-prefix scale factor

/*----------------------------------------------------------------------*
*       DEFINE COMMON MATHEMATICAL FUNCTIONS
*----------------------------------------------------------------------*/

/** 3x1 VECTORS CROSS PRODUCT */
typ_return  VEC_CROSS (double  v1Xv2_out[3] /*! cross product result*/, double v1_in[3] /*! first cross product input*/, double v2_in[3] /*! second cross product input*/);
/** nx1 VECTORS DOT PRODUCT */
double      VEC_DOT   (double* v1_in /*! first dot product input*/, double* v2_in /*! second dot product input*/, const int n_in = 3 /*! size of both the input vectors*/);
/** nx1 VECTOR  NORMALIZATION */
typ_return  VEC_NORM  (double* normalized_out /*! normalized output vector*/, double* vector_in /*! input vector*/, const int n_in = 3 /*! size of the input & output vectors*/);
/** n1xn2 * n2xn3 MATRIX PRODUCT */
typ_return  MAT_PROD  (double* m1m2_out /*! matrix product output (n1xn3)*/, double* m1_in /*! first input matrix (n1xn2)*/, double* m2_in /*! second input matrix (n2xn3)*/, const int n1_in /*! number of rows of the first input matrix*/, const int n2_in /*! number of columns of the first input matrix = number of rows of the second input matrix*/, const int n3_in /*! number of columns of the second input matrix*/);
/** 3x3 MATRIX DETERMINANT */
double      MAT_DET   (double  matrix_in[3][3] /*! input matrix*/);
/** 3x3 MATRIX INVERSION */
typ_return  MAT_INV   (double  inverse_out[3][3] /*! inverted output matrix*/, double matrix_in[3][3] /*! input matrix*/);
/** 3x3 MATRIX TRANSPOSITION */
typ_return MAT_TRANSPOSE (double trans_out[3][3], double matrix_in[3][3]);
/** NORMAL RANDOM SAMPLE GENERATOR */
double      RandomNormal (double mean /*! mean value*/, double sigma /*! standard deviation*/);
/** Reduce a periodical value within the User-provided range */
double      InRange (double   value       /*! in - Value to be reduced within the User-provided range */,
                     double   lower_bound /*! in - Range lower limit */,
                     double   upper_bound /*! in - Range upper limit */);

/** Function to get the Rotation Matrix from the Quaternion. It normalizes the input quaternion */
void    QUAT_TO_ROT (double R_fixed2body[3][3] /*! out - Rotation Matrix Fixed->Body */,
                     double q[4]               /*! in&out - Quaternion vector, q[0] is the scalar component */);
/** Function to get the Quaternion from the Rotation Matrix */
void    ROT_TO_QUAT (double q[4]                     /*! out - Quaternion vector, q[0] is the scalar component */,
                     const double R_fixed2body[3][3] /*! in - Rotation Matrix Fixed->Body */);

/** Function to propagate quaternion  */
void QUAT_PROP(typ_return &result, double q_in[4], double w_brf[3], double dT, double q_out[4]);

/** Function to assure quaternion continuity */
void CONTINUITY_QUAT(typ_return &result, double Q_dot[4], double Q_old[4], bool assign_q_old = true);

/** Function for quaternion composition */
void QUAT_COMPOSE(double q_out[4], double q_1[4], double q_2[4]);

/** Function for quaternion inversion */
void QUAT_INV(double q_out[4], double q_in[4]);

#endif // __MathLib_h end

