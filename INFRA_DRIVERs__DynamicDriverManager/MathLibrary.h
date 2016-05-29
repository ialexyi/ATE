#ifndef __MathLibrary_H__
#define __MathLibrary_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int   MTH_PolynomialExtrapolationSingle(double *vlfXaxisArray,double *vlfYaxisArray, int iNumberOfPoints,double lfXcoordinate, double *pYcoordinate);
int   MTH_PolynomialExtrapolationArray(double *vlfXaxisArray,double *vlfYaxisArray, int iNumberOfPoints, int iStartIndex , int iBlockLength ,int iOutputNumberOfPoints, double lfStartX,double lfStopX, double **vXgeneratedArray,double **vYgeneratedArray);

//==== Use only between low X and high X ============//
int   MTH_LinearExtrapolationSingle(double *vlfXaxisArray,double *vlfYaxisArray, int iNumberOfPoints,double lfXcoordinate, double *pYcoordinate);
int   MTH_LinearExtrapolationArray(double *vlfXaxisArray,double *vlfYaxisArray, int iNumberOfPoints, int iStartIndex , int iBlockLength ,int iOutputNumberOfPoints, double lfStartX,double lfStopX, double **vXgeneratedArray,double **vYgeneratedArray);
int   MTH_InterpolationSingle(int type,double *vlfXaxisArray, double *vlfYaxisArray,int iNumberOfPoints, double lfXcoordinate,double *pYcoordinate);
int   MTH_InterpolationArray(int type,double *vlfXaxisArray, double *vlfYaxisArray,int iNumberOfPoints, int iStartIndex , int iBlockLength , int iOutputNumberOfPoints,double lfStartX, double lfStopX, double **vXgeneratedArray,double **vYgeneratedArray);


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __MathLibrary_H__ */
