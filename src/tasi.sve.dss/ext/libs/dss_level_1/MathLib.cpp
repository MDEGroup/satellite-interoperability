/******************************************************************************

    PROJECT:      DSS - S/C Dynamics Simulation Software
    FILE:         MathLib.h
    RELEASE:      0.01
    AUTHOR:       Luigi Galvagni, Marco Anania, Francesco Sabbioni
    DATE:         2009/06/15

    OVERVIEW
    ========
    Source Code for the Mathematical Lirary

    CHANGE LOG
    ==========

******************************************************************************/

#include "MathLib.h"

//-------------------------------------------------------------------------
typ_return VEC_CROSS (double v1Xv2_out[3], double v1_in[3], double v2_in[3])
// 3x1 VECTORS CROSS PRODUCT
//-------------------------------------------------------------------------
{
   v1Xv2_out[0] = v1_in[1] * v2_in[2] - v1_in[2] * v2_in[1];
   v1Xv2_out[1] = v1_in[2] * v2_in[0] - v1_in[0] * v2_in[2];
   v1Xv2_out[2] = v1_in[0] * v2_in[1] - v1_in[1] * v2_in[0];
   return Nominal;
}

//-------------------------------------------------------------------------
double VEC_DOT (double* v1_in, double* v2_in, const int n_in)
// nx1 VECTORS DOT PRODUCT
//-------------------------------------------------------------------------
{
   int      i;
   double   v1v2_out = 0.0;

   for (i = 0; i < n_in; i++)
      v1v2_out += v1_in[i] * v2_in[i];
   return v1v2_out;
}

//-------------------------------------------------------------------------
typ_return VEC_NORM  (double* normalized_out, double* vector_in, const int n_in)
// nx1 VECTOR  NORMALIZATION
//-------------------------------------------------------------------------
{
   int      i;
   double   module = 0.0;

   for (i = 0; i < n_in; i++)
      module += vector_in[i] * vector_in[i];
   module = Sqrt (module);
   if (module < 1.E-14)
   {
      for (i = 0; i < n_in; i++)
         normalized_out[i] = 0.0;
      return Error;
   }
   for (i = 0; i < n_in; i++)
      normalized_out[i] = vector_in[i] / module;
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return MAT_PROD (double* m1m2_out, double* m1_in, double* m2_in, const int n1_in, const int n2_in, const int n3_in)
// n1xn2 * n2xn3 MATRIX PRODUCT
//-------------------------------------------------------------------------
{
   int   i, j, k;

   for (i = 0; i < n1_in; i++)
      for (j = 0; j < n3_in; j++)
      {
         m1m2_out [n3_in * i + j] = 0.0;
         for (k = 0; k < n2_in; k++)
            m1m2_out [n3_in * i + j] += m1_in [n2_in * i + k] * m2_in [n3_in * k + j];
      }
   return Nominal;
}

//-------------------------------------------------------------------------
double MAT_DET (double matrix_in[3][3])
// 3x3 MATRIX DETERMINANT
//-------------------------------------------------------------------------
{
   return   matrix_in[0][0] * matrix_in[1][1] * matrix_in[2][2] - matrix_in[0][0] * matrix_in[1][2] * matrix_in[2][1]
          - matrix_in[1][0] * matrix_in[0][1] * matrix_in[2][2] + matrix_in[0][1] * matrix_in[1][2] * matrix_in[2][0]
          + matrix_in[1][0] * matrix_in[2][1] * matrix_in[0][2] - matrix_in[0][2] * matrix_in[2][0] * matrix_in[1][1];
}

//-------------------------------------------------------------------------
typ_return MAT_INV (double inverse_out[3][3], double matrix_in[3][3])
// 3x3 MATRIX INVERSION
//-------------------------------------------------------------------------
{
   int      i, j;
   double   det;

   det = MAT_DET (matrix_in);
   if (det < 1.E-14)
   {
      for (i = 0; i < 3; i++)
         for (j = 0; j < 3; j++)
            inverse_out[i][j] = 0.0;
      return Error;
   }
   inverse_out[0][0]= (matrix_in[2][2]*matrix_in[1][1]-matrix_in[2][1]*matrix_in[1][2]) / det;
   inverse_out[0][1]=-(matrix_in[2][2]*matrix_in[0][1]-matrix_in[2][1]*matrix_in[0][2]) / det;
   inverse_out[0][2]= (matrix_in[1][2]*matrix_in[0][1]-matrix_in[1][1]*matrix_in[0][2]) / det;
   inverse_out[1][0]=-(matrix_in[2][2]*matrix_in[1][0]-matrix_in[2][0]*matrix_in[1][2]) / det;
   inverse_out[1][1]= (matrix_in[2][2]*matrix_in[0][0]-matrix_in[2][0]*matrix_in[0][2]) / det;
   inverse_out[1][2]=-(matrix_in[1][2]*matrix_in[0][0]-matrix_in[1][0]*matrix_in[0][2]) / det;
   inverse_out[2][0]= (matrix_in[2][1]*matrix_in[1][0]-matrix_in[2][0]*matrix_in[1][1]) / det;
   inverse_out[2][1]=-(matrix_in[2][1]*matrix_in[0][0]-matrix_in[2][0]*matrix_in[0][1]) / det;
   inverse_out[2][2]= (matrix_in[1][1]*matrix_in[0][0]-matrix_in[1][0]*matrix_in[0][1]) / det;
   return Nominal;
}

//-------------------------------------------------------------------------
typ_return MAT_TRANSPOSE (double trans_out[3][3], double matrix_in[3][3])
// 3x3 MATRIX TRANSPOSITION
//-------------------------------------------------------------------------
{
   for (int i = 0; i < 3; i++)
         for (int j = 0; j < 3; j++)
            trans_out[i][j] = matrix_in[j][i];

   return Nominal;
}

//-------------------------------------------------------------------------
double RandomNormal (double mean, double sigma)
// NORMAL RANDOM SAMPLE GENERATOR
//-------------------------------------------------------------------------
{
   double   xx, yy, normal;

   xx = (double)(rand()) / (double)RAND_MAX;
   do{
      yy = (double)(rand()) / (double)RAND_MAX;
   } while (yy < 0.000000001 || yy > 0.999999999);
   normal = cos (2 * PI * xx) * Sqrt (-2.0 * log (yy));

   return normal * sigma + mean;
}

//-------------------------------------------------------------------------
double InRange (double   value       /*! in - Value to be reduced within the User-provided range */,
                double   lower_bound /*! in - Range lower limit */,
                double   upper_bound /*! in - Range upper limit */)
//-------------------------------------------------------------------------
{
   double range;

   if (value < lower_bound) // If "value" is behind the provide range
   {
      range = upper_bound - lower_bound;
      value = upper_bound - fmod (upper_bound - value, range);
   }
   else if (upper_bound < value) // If "value" is over the provide range
   {
      range = upper_bound - lower_bound;
      value = lower_bound + fmod (value - lower_bound, range);
   }
   return value;
}

//-------------------------------------------------------------------
/** Function to get the Rotation Matrix from the Quaternion. It normalizes the input quaternion */
void QUAT_TO_ROT (double R_fixed2body[3][3] /*! out - Rotation Matrix Fixed->Body */,
                  double q[4]               /*! in&out - Quaternion vector, q[0] is the scalar component */)
//-------------------------------------------------------------------
{
   int      i;
   double   dum;

// Quaternion normalization
   dum = Sqrt (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
   for (i = 0; i < 4; i++)
      q[i] /= dum;

// Rotation matrix from the FIXED to the BODY-local reference frame
   R_fixed2body[0][0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
   R_fixed2body[0][1] = 2.*( q[1]*q[2] + q[3]*q[0]);
   R_fixed2body[0][2] = 2.*( q[1]*q[3] - q[2]*q[0]);
   R_fixed2body[1][0] = 2.*( q[1]*q[2] - q[3]*q[0]);
   R_fixed2body[1][1] = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
   R_fixed2body[1][2] = 2.*( q[1]*q[0] + q[2]*q[3]);
   R_fixed2body[2][0] = 2.*( q[1]*q[3] + q[2]*q[0]);
   R_fixed2body[2][1] = 2.*(-q[1]*q[0] + q[2]*q[3]);
   R_fixed2body[2][2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
}

//-------------------------------------------------------------------
/** Function to get the Quaternion from the Rotation Matrix */
void ROT_TO_QUAT (double q[4]                     /*! out - Quaternion vector, q[0] is the scalar component */,
                  const double R_fixed2body[3][3] /*! in - Rotation Matrix Fixed->Body */)
//-------------------------------------------------------------------
{
   int      i;
   double   dum;

// Quaternion based on rotation matrix from the FIXED to the BODY-local reference frame; q[0] is the scalar component
   q[0] = Sqrt (1. + R_fixed2body[0][0] + R_fixed2body[1][1] + R_fixed2body[2][2]) / 2.;
   if (q[0] > 1.E-3)
   {
      q[1] = (R_fixed2body[1][2] - R_fixed2body[2][1]) / (4.*q[0]);
      q[2] = (R_fixed2body[2][0] - R_fixed2body[0][2]) / (4.*q[0]);
      q[3] = (R_fixed2body[0][1] - R_fixed2body[1][0]) / (4.*q[0]);
   }
   else // Close to 180 deg. rotation
   {
      q[1] = Sqrt (1. + R_fixed2body[0][0] - R_fixed2body[1][1] - R_fixed2body[2][2]) / 2.;
      q[2] = Sqrt (1. - R_fixed2body[0][0] + R_fixed2body[1][1] - R_fixed2body[2][2]) / 2.;
      q[3] = Sqrt (1. - R_fixed2body[0][0] - R_fixed2body[1][1] + R_fixed2body[2][2]) / 2.;
      if (q[1] >= q[2] && q[1] >= q[3])
      {
         q[0] = (R_fixed2body[1][2] - R_fixed2body[2][1]) / (4.*q[1]);
         q[2] = (R_fixed2body[1][0] + R_fixed2body[0][1]) / (4.*q[1]);
         q[3] = (R_fixed2body[2][0] + R_fixed2body[0][2]) / (4.*q[1]);
      }
      else if (q[2] >= q[1] && q[2] >= q[3])
      {
         q[0] = (R_fixed2body[2][0] - R_fixed2body[0][2]) / (4.*q[2]);
         q[1] = (R_fixed2body[1][0] + R_fixed2body[0][1]) / (4.*q[2]);
         q[3] = (R_fixed2body[1][2] + R_fixed2body[2][1]) / (4.*q[2]);
      }
      else
      {
         q[0] = (R_fixed2body[0][1] - R_fixed2body[1][0]) / (4.*q[3]);
         q[1] = (R_fixed2body[2][0] + R_fixed2body[0][2]) / (4.*q[3]);
         q[2] = (R_fixed2body[1][2] + R_fixed2body[2][1]) / (4.*q[3]);
      }
   }

// Quaternion normalization
   dum = Sqrt (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
   for (i = 0; i < 4; i++)
      q[i] /= dum;
}

//---------------------------------------------------------------------------------------------
void QUAT_PROP(typ_return &result, double q_in[4], double w_brf[3], double dT, double q_out[4])
//---------------------------------------------------------------------------------------------
{
   double   norm_w, dum;
   double   x, c, s;
   int      i;

   result = Nominal;

   norm_w = Sqrt(w_brf[0]*w_brf[0] + w_brf[1]*w_brf[1] + w_brf[2]*w_brf[2]);
   x      = norm_w * dT /2.0;
   if(fabs(x) > 1.E-9)
   {
      c = cos(x);
      s = sin(x);

      double Om[4][4] = {{c,                 -w_brf[0]*s/norm_w,  -w_brf[1]*s/norm_w,  -w_brf[2]*s/norm_w},
                         {w_brf[0]*s/norm_w,  c,                   w_brf[2]*s/norm_w,  -w_brf[1]*s/norm_w},
                         {w_brf[1]*s/norm_w, -w_brf[2]*s/norm_w,   c,                   w_brf[0]*s/norm_w},
                         {w_brf[2]*s/norm_w,  w_brf[1]*s/norm_w,  -w_brf[0]*s/norm_w,   c}};

      result = MAT_PROD(&q_out[0], &Om[0][0], &q_in[0], 4, 4, 1);
   }
   else
   {
      for(i=0; i<4; i++)
         q_out[i] = q_in[i];
   }

   dum = Sqrt (q_out[0]*q_out[0] + q_out[1]*q_out[1] + q_out[2]*q_out[2] + q_out[3]*q_out[3]);
   for (i = 0; i < 4; i++)
      q_out[i] /= dum;
}

//---------------------------------------------------------------------------------------------
void CONTINUITY_QUAT (typ_return &result, double Q_dot[4], double Q_old[4], bool assign_q_old)
//---------------------------------------------------------------------------------------------
{
   int i;

   result = Nominal;

   if(Sqrt( (Q_dot[0]-Q_old[0])*(Q_dot[0]-Q_old[0])+
            (Q_dot[1]-Q_old[1])*(Q_dot[1]-Q_old[1])+
            (Q_dot[2]-Q_old[2])*(Q_dot[2]-Q_old[2])+
            (Q_dot[3]-Q_old[3])*(Q_dot[3]-Q_old[3])) > 1.0)
      for(i = 0; i < 4; i++)
         Q_dot[i] = -Q_dot[i];

   if( assign_q_old ){
      for(i = 0; i < 4; i++)
         Q_old[i] = Q_dot[i];
   }
}

//---------------------------------------------------------------------------------------------
void QUAT_COMPOSE(double q_out[4], double q_1[4], double q_2[4])
// QUAT3_OUT = QUAT1_IN(FIRST ROTATION) @ QUAT2_IN(SECOND ROTATION)
//---------------------------------------------------------------------------------------------
{
   double CSI[4][4];

   CSI[0][0] = (+q_2[0]);
   CSI[0][1] = (-q_2[1]);
   CSI[0][2] = (-q_2[2]);
   CSI[0][3] = (-q_2[3]);

   CSI[1][0] = (+q_2[1]);
   CSI[1][1] = (+q_2[0]);
   CSI[1][2] = (+q_2[3]);
   CSI[1][3] = (-q_2[2]);

   CSI[2][0] = (+q_2[2]);
   CSI[2][1] = (-q_2[3]);
   CSI[2][2] = (+q_2[0]);
   CSI[2][3] = (+q_2[1]);

   CSI[3][0] = (+q_2[3]);
   CSI[3][1] = (+q_2[2]);
   CSI[3][2] = (-q_2[1]);
   CSI[3][3] = (+q_2[0]);

   MAT_PROD (q_out, CSI[0], q_1, 4, 4, 1);

}

//---------------------------------------------------------------------------------------------
void QUAT_INV(double q_out[4], double q_in[4])
//---------------------------------------------------------------------------------------------
{
   q_out[0] =  q_in[0];
   q_out[1] = -q_in[1];
   q_out[2] = -q_in[2];
   q_out[3] = -q_in[3];
}


