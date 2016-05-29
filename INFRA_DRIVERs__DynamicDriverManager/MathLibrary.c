//==============================================================================
// Include files

#include <analysis.h>
#include "toolbox.h"
#include <ansi_c.h>
#include <utility.h>
#include "MathLibrary.h"

//==============================================================================
// Constants

#define			INSTR_TYPE_CONTINUE							0
#define			INSTR_TYPE_YES_NO							1	

//==============================================================================
// Types

typedef struct
{
	double			x,y;
	
} tsSinglePoint;
	
//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
int  ShowMessage ( int type , char *pTitle , char *pText, char *pPicture );

//==============================================================================

int		MTH_PolynomialExtrapolationSingle( double *vlfXaxisArray , double *vlfYaxisArray ,  int iNumberOfPoints , double lfXcoordinate , double *pYcoordinate )
{
	tsSinglePoint		*vtInputArray		=	NULL;
	
	int					index				=	0,
						iNewIndex			=	0,
						iCurrentIndex		=	0,
						iAddIndex			=	0;
	
	int					*pMapping			=	NULL;
	
	double				lfLowerX			=	0.0,
						lfLowerY			=	0.0;
	
	double				result				=	0.0,
						dividend			=	0.0,
						divisor				=	0.0;
	
	if (( vlfXaxisArray == NULL ) || ( vlfYaxisArray == NULL ) || ( pYcoordinate == NULL ) || ( iNumberOfPoints <= 0 ))
		return -1;
	
	vtInputArray = calloc( iNumberOfPoints , sizeof(tsSinglePoint));
	pMapping = calloc( iNumberOfPoints , sizeof(int)); 
	
	if (( vtInputArray == NULL ) || ( pMapping == NULL ))
		return -1;
	
	// -------- Sort and copy to new array -------------------------// 
	for ( iNewIndex = 0 ; iNewIndex < iNumberOfPoints ; iNewIndex++ )
	{
		lfLowerX = vlfXaxisArray[iNewIndex];  
		
		for ( index = 0 ; index < iNumberOfPoints ; index++ )
		if (( pMapping[index] == 0 ) && ( vlfXaxisArray[index] <= lfLowerX ))
		{
			iCurrentIndex = index;	
			lfLowerX = vlfXaxisArray[index];
			lfLowerY = vlfYaxisArray[index];
		}
		
		pMapping[iCurrentIndex] = 1;
		vtInputArray[iNewIndex].x = lfLowerX;
		vtInputArray[iNewIndex].y = lfLowerY;
	}
	
	for ( iAddIndex = 0 ; iAddIndex < iNumberOfPoints ; iAddIndex++ ) 
	{
		dividend = vtInputArray[iAddIndex].y;
		divisor = 1.0;
		
		for ( index = 0 ; index < iNumberOfPoints ; index++ )    
			if ( index != iAddIndex )
			{
				dividend *= ( lfXcoordinate - vtInputArray[index].x );
			
				divisor *= ( vtInputArray[iAddIndex].x - vtInputArray[index].x );
			}
			
		result += dividend / divisor;
	}
	
	*pYcoordinate = result;
	
	return 0;
}

int		MTH_PolynomialExtrapolationArray( double *vlfXaxisArray , double *vlfYaxisArray ,  int iNumberOfPoints , int iStartIndex , int iBlockLength , int iOutputNumberOfPoints , double lfStartX , double lfStopX , double **vXgeneratedArray  , double **vYgeneratedArray )
{
	tsSinglePoint		*vtInputArray		=	NULL;
	
	int					index				=	0,
						iNewIndex			=	0,
						iCurrentIndex		=	0,
						iNewApplyIndex		=	0,
						iAddIndex			=	0;
	
	int					*pMapping			=	NULL;
	
	double				lfLowerX			=	0.0,
						lfLowerY			=	0.0;
	
	double				result				=	0.0,
						dividend			=	0.0,
						divisor				=	0.0;
	
	double				lfStep				=	0.0,
						lfCurrentX			=	0.0;
	
	double				*vOutXarray			=	NULL,
						*vOutYarray			=	NULL;
	
	int					bDoAgain			=	0;
	
	if (( vlfXaxisArray == NULL ) || ( vlfYaxisArray == NULL ) || ( iNumberOfPoints <= 0 ))
		return -1;
	
	lfStep = ( lfStopX - lfStartX ) / ( iOutputNumberOfPoints - 1 );
	lfCurrentX = lfStartX;
	
	vOutXarray = calloc( iOutputNumberOfPoints , sizeof(double));
	vOutYarray = calloc( iOutputNumberOfPoints , sizeof(double));          
	
	vtInputArray = calloc( iNumberOfPoints , sizeof(tsSinglePoint));
	pMapping = calloc( iNumberOfPoints , sizeof(int)); 
	
	if (( vtInputArray == NULL ) || ( pMapping == NULL ) || ( vOutXarray == NULL ) || ( vOutYarray == NULL ))
		return -1;
	
	// -------- Sort and copy to new array -------------------------// 
	do
	{
		bDoAgain = 0;
		
		for ( index = iStartIndex ; index < iBlockLength ; index++ ) 
			if ( pMapping[index] == 0 )
			{
				bDoAgain = 1;
				break;
			}
		
		if ( bDoAgain == 0 )
			break;
		
		// -------- Sort and copy to new array -------------------------// 
		for ( iNewIndex = iStartIndex ; iNewIndex < iBlockLength ; iNewIndex++ )
		{
			if ( pMapping[iNewIndex] )
				continue;
			
			lfLowerX = vlfXaxisArray[iNewIndex];  
			lfLowerY = vlfYaxisArray[iNewIndex];
			iCurrentIndex = iNewIndex;
			
			for ( index = iStartIndex ; index < iBlockLength ; index++ )
			if (( pMapping[index] == 0 ) && ( vlfXaxisArray[index] <= lfLowerX ))
			{
				iCurrentIndex = index;	
				lfLowerX = vlfXaxisArray[index];
				lfLowerY = vlfYaxisArray[index];
			}
	
			pMapping[iCurrentIndex] = 1;
			
			vtInputArray[iNewApplyIndex].x = lfLowerX;
			vtInputArray[iNewApplyIndex].y = lfLowerY;
			iNewApplyIndex++;
		}

	} while( 1 );
	
	iNumberOfPoints = iNewApplyIndex;
	
	for ( iNewIndex = 0 ; iNewIndex < iOutputNumberOfPoints ; iNewIndex++ )  
	{
		result = 0.0;
		
		for ( iAddIndex = 0 ; iAddIndex < iNumberOfPoints ; iAddIndex++ ) 
		{
			dividend = vtInputArray[iAddIndex].y;
			divisor = 1.0;
		
			for ( index = 0 ; index < iNumberOfPoints ; index++ )    
				if ( index != iAddIndex )
				{
					dividend *= ( lfCurrentX - vtInputArray[index].x );
			
					divisor *= ( vtInputArray[iAddIndex].x - vtInputArray[index].x );
				}
			
			result += dividend / divisor;
		}
		
		vOutXarray[iNewIndex] = lfCurrentX;
		vOutYarray[iNewIndex] = result;
		lfCurrentX += lfStep;
	}
	
	if ( vXgeneratedArray )
		*vXgeneratedArray = vOutXarray;
	else
		free(vOutXarray);
		
	if ( vYgeneratedArray )
		*vYgeneratedArray = vOutYarray;
	else
		free(vOutYarray);
	
	if ( vtInputArray )
		free( vtInputArray );
	
	if ( pMapping )
		free( pMapping );
	
	return 0;
}

//================================================================================================================================================//
int		MTH_LinearExtrapolationSingle( double *vlfXaxisArray , double *vlfYaxisArray ,  int iNumberOfPoints , double lfXcoordinate , double *pYcoordinate )
{
	tsSinglePoint		*vtInputArray		=	NULL;
	
	int					index				=	0,
						iNewIndex			=	0,
						iCurrentIndex		=	0,
						iAddIndex			=	0;
	
	int					*pMapping			=	NULL;
	
	double				lfLowerX			=	0.0,
						lfLowerY			=	0.0;
	
	double				result				=	0.0,
						dividend			=	0.0,
						divisor				=	0.0;
	
	double				lfSigmaX			=	0.0,
						lfSigmaY			=	0.0,
						lfSigmaXY			=	0.0,
						lfSigmaXX			=	0.0,	  
						lfSigmaYY			=	0.0;  
						
	double				lfIntercept			=	0.0, 
						lfSlope				=	0.0;
						
	
	if (( vlfXaxisArray == NULL ) || ( vlfYaxisArray == NULL ) || ( pYcoordinate == NULL ) || ( iNumberOfPoints <= 0 ))
		return -1;
	
	vtInputArray = calloc( iNumberOfPoints , sizeof(tsSinglePoint));
	pMapping = calloc( iNumberOfPoints , sizeof(int)); 
	
	if (( vtInputArray == NULL ) || ( pMapping == NULL ))
		return -1;
	
	// -------- Sort and copy to new array -------------------------// 
	for ( iNewIndex = 0 ; iNewIndex < iNumberOfPoints ; iNewIndex++ )
	{
		lfLowerX = vlfXaxisArray[iNewIndex];  
		
		for ( index = 0 ; index < iNumberOfPoints ; index++ )
		if (( pMapping[index] == 0 ) && ( vlfXaxisArray[index] <= lfLowerX ))
		{
			iCurrentIndex = index;	
			lfLowerX = vlfXaxisArray[index];
			lfLowerY = vlfYaxisArray[index];
		}
		
		pMapping[iCurrentIndex] = 1;
		vtInputArray[iNewIndex].x = lfLowerX;
		vtInputArray[iNewIndex].y = lfLowerY;
	}
	
	
	lfSigmaX = 0.0;
	lfSigmaY = 0.0;
	lfSigmaXY = 0.0;
	lfSigmaXX = 0.0;  
	lfSigmaYY = 0.0;  
	
	for ( iAddIndex = 0 ; iAddIndex < iNumberOfPoints ; iAddIndex++ ) 
	{
		lfSigmaX += vtInputArray[iAddIndex].x;
		lfSigmaY += vtInputArray[iAddIndex].y;  
		lfSigmaXY += vtInputArray[iAddIndex].x * vtInputArray[iAddIndex].y;  
		lfSigmaXX += vtInputArray[iAddIndex].x * vtInputArray[iAddIndex].x; 
		lfSigmaYY += vtInputArray[iAddIndex].y * vtInputArray[iAddIndex].y;   
	}
	
	divisor = ((double)iNumberOfPoints * lfSigmaXX) - (lfSigmaX * lfSigmaX);
	
	if ( divisor == 0 )
		divisor = 1E-15;
	
	dividend = ((double)iNumberOfPoints * lfSigmaXY) - (lfSigmaX * lfSigmaY);
		
	lfSlope = dividend / divisor;
	
	lfIntercept = ( lfSigmaY / (double)iNumberOfPoints ) - (( dividend * lfSigmaX ) / ( divisor * (double)iNumberOfPoints ));

	result = lfIntercept + lfSlope * lfXcoordinate; 
			
	*pYcoordinate = result;
	
	return 0;
}

int		MTH_LinearExtrapolationArray( double *vlfXaxisArray , double *vlfYaxisArray ,  int iNumberOfPoints , int iStartIndex , int iBlockLength ,  int iOutputNumberOfPoints , double lfStartX , double lfStopX , double **vXgeneratedArray  , double **vYgeneratedArray )
{
	tsSinglePoint		*vtInputArray		=	NULL;
	
	int					index				=	0,
						iNewIndex			=	0,
						iCurrentIndex		=	0,
						iNewApplyIndex		=	0, 
						iAddIndex			=	0;
	
	int					*pMapping			=	NULL;
	
	double				lfLowerX			=	0.0,
						lfLowerY			=	0.0;
	
	double				result				=	0.0,
						dividend			=	0.0,
						divisor				=	0.0;
	
	double				lfSigmaX			=	0.0,
						lfSigmaY			=	0.0,
						lfSigmaXY			=	0.0,
						lfSigmaXX			=	0.0,	  
						lfSigmaYY			=	0.0;  
						
	double				lfIntercept			=	0.0, 
						lfSlope				=	0.0;
						
	double				lfStep				=	0.0,
						lfCurrentX			=	0.0;
	
	double				*vOutXarray			=	NULL,
						*vOutYarray			=	NULL;
	
	int					bDoAgain			=	0;
	
	if (( vlfXaxisArray == NULL ) || ( vlfYaxisArray == NULL ) || ( iNumberOfPoints <= 0 ))
		return -1;
	
	lfStep = ( lfStopX - lfStartX ) / ( iOutputNumberOfPoints - 1 );
	lfCurrentX = lfStartX;
	
	vOutXarray = calloc( iOutputNumberOfPoints , sizeof(double));
	vOutYarray = calloc( iOutputNumberOfPoints , sizeof(double));          
	
	vtInputArray = calloc( iNumberOfPoints , sizeof(tsSinglePoint));
	pMapping = calloc( iNumberOfPoints , sizeof(int)); 
	
	if (( vtInputArray == NULL ) || ( pMapping == NULL ) || ( vOutXarray == NULL ) || ( vOutYarray == NULL ))
		return -1;
	
	// -------- Sort and copy to new array -------------------------// 
	do
	{
		bDoAgain = 0;
		
		for ( index = iStartIndex ; index < iBlockLength ; index++ ) 
			if ( pMapping[index] == 0 )
			{
				bDoAgain = 1;
				break;
			}
		
		if ( bDoAgain == 0 )
			break;
		
		// -------- Sort and copy to new array -------------------------// 
		for ( iNewIndex = iStartIndex ; iNewIndex < iBlockLength ; iNewIndex++ )
		{
			if ( pMapping[iNewIndex] )
				continue;
			
			lfLowerX = vlfXaxisArray[iNewIndex];  
			lfLowerY = vlfYaxisArray[iNewIndex];
			iCurrentIndex = iNewIndex;
			
			for ( index = iStartIndex ; index < iBlockLength ; index++ )
			if (( pMapping[index] == 0 ) && ( vlfXaxisArray[index] <= lfLowerX ))
			{
				iCurrentIndex = index;	
				lfLowerX = vlfXaxisArray[index];
				lfLowerY = vlfYaxisArray[index];
			}
	
			pMapping[iCurrentIndex] = 1;
			
			vtInputArray[iNewApplyIndex].x = lfLowerX;
			vtInputArray[iNewApplyIndex].y = lfLowerY;
			iNewApplyIndex++;
		}

	} while( 1 );
	
	iNumberOfPoints = iNewApplyIndex;
	
	lfSigmaX = 0.0;
	lfSigmaY = 0.0;
	lfSigmaXY = 0.0;
	lfSigmaXX = 0.0;  
	lfSigmaYY = 0.0;  
	
	for ( iAddIndex = 0 ; iAddIndex < iNumberOfPoints ; iAddIndex++ ) 
	{
		lfSigmaX += vtInputArray[iAddIndex].x;
		lfSigmaY += vtInputArray[iAddIndex].y;  
		lfSigmaXY += vtInputArray[iAddIndex].x * vtInputArray[iAddIndex].y;  
		lfSigmaXX += vtInputArray[iAddIndex].x * vtInputArray[iAddIndex].x; 
		lfSigmaYY += vtInputArray[iAddIndex].y * vtInputArray[iAddIndex].y;   
	}
	
	divisor = ((double)iNumberOfPoints * lfSigmaXX) - (lfSigmaX * lfSigmaX);
	
	if ( divisor == 0 )
		divisor = 1E-15;
	
	dividend = ((double)iNumberOfPoints * lfSigmaXY) - (lfSigmaX * lfSigmaY);
		
	lfSlope = dividend / divisor;
	
	lfIntercept = ( lfSigmaY / (double)iNumberOfPoints ) - (( dividend * lfSigmaX ) / ( divisor * (double)iNumberOfPoints ));

	for ( iNewIndex = 0 ; iNewIndex < iOutputNumberOfPoints ; iNewIndex++ )  
	{
		result = lfIntercept + lfSlope * lfCurrentX; 
			
		vOutXarray[iNewIndex] = lfCurrentX;
		vOutYarray[iNewIndex] = result;
		lfCurrentX += lfStep;
	}
	
	if ( vXgeneratedArray )
		*vXgeneratedArray = vOutXarray;
	else
		free(vOutXarray);
		
	if ( vYgeneratedArray )
		*vYgeneratedArray = vOutYarray;
	else
		free(vOutYarray);
	
	if ( pMapping )
		free(pMapping);
	
	if ( vtInputArray )
		free(vtInputArray);
	
	return 0;
}

// type : 0 - Linear , 1 - Cosine , 2 - Cubic , 3 - Hermite
//================================================================================================================================================//
int		MTH_InterpolationSingle( int type , double *vlfXaxisArray , double *vlfYaxisArray ,  int iNumberOfPoints , double lfXcoordinate , double *pYcoordinate )
{
	int					error				=	0;
	
	tsSinglePoint		*vtInputArray		=	NULL;
	
	int					index				=	0,
						iNewIndex			=	0,
						iCurrentIndex		=	0;
	
	int					*pMapping			=	NULL;
	
	double				lfLowerX			=	0.0,
						lfLowerY			=	0.0;
	
	double				result				=	0.0,
						mu					=	0.0,
						mu2					=	0.0,
						a0					=	0.0,
						a1					=	0.0,
						a2					=	0.0,
						a3					=	0.0;
							
	double				lfMaxX				=	0.0, 
						lfMinX				=	0.0,
						lfDelta				=	0.0;
						
	int					iFirstIndex			=	0,
						iStoreIndex			=	0,
						iSecondIndex		=	0,
						iNewApplyIndex		=	0;
	
	int					bExit				=	0,
						bDoAgain			=	0;
	
	do
	{
		if (( vlfXaxisArray == NULL ) || ( vlfYaxisArray == NULL ) || ( pYcoordinate == NULL ) || ( iNumberOfPoints <= 0 ))
		{
			error = -1;
			break;
		}
	
		vtInputArray = calloc( iNumberOfPoints , sizeof(tsSinglePoint));
		pMapping = calloc( iNumberOfPoints , sizeof(int)); 
	
		if (( vtInputArray == NULL ) || ( pMapping == NULL ))
		{
			error = -1;
			break;
		}
	
		lfMaxX = vlfXaxisArray[0]; 
		lfMinX = vlfXaxisArray[0]; 
	
		for ( index = 0 ; index < iNumberOfPoints ; index++ )
		{
			if ( vlfXaxisArray[index] > lfMaxX )
				lfMaxX = vlfXaxisArray[index];
	
			if ( vlfXaxisArray[index] < lfMinX )
				lfMinX = vlfXaxisArray[index];
		}
		
		if ( lfXcoordinate < lfMinX ) 
			lfXcoordinate = lfMinX;
							
		if ( lfXcoordinate > lfMaxX )
			lfXcoordinate = lfMaxX;

		for ( index = 0 ; index < iNumberOfPoints ; index++ )
			if ( lfXcoordinate == vlfXaxisArray[index] )
			{
				*pYcoordinate = vlfYaxisArray[index];
			
				bExit = 1;
			
				break;
			}
		
		if ( bExit )
			break;
		
		do
		{
			bDoAgain = 0;
			
			for ( index = 0 ; index < iNumberOfPoints ; index++ ) 
				if ( pMapping[index] == 0 )
				{
					bDoAgain = 1;
					break;
				}
			
			if ( bDoAgain == 0 )
				break;
			
			// -------- Sort and copy to new array -------------------------// 
			for ( iNewIndex = 0 ; iNewIndex < iNumberOfPoints ; iNewIndex++ )
			{
				if ( pMapping[iNewIndex] )
					continue;
				
				lfLowerX = vlfXaxisArray[iNewIndex];  
				lfLowerY = vlfYaxisArray[iNewIndex];
				iCurrentIndex = iNewIndex;
				
				for ( index = 0 ; index < iNumberOfPoints ; index++ )
				if (( pMapping[index] == 0 ) && ( vlfXaxisArray[index] <= lfLowerX ))
				{
					iCurrentIndex = index;	
					lfLowerX = vlfXaxisArray[index];
					lfLowerY = vlfYaxisArray[index];
				}
		
				pMapping[iCurrentIndex] = 1;
				
				vtInputArray[iNewApplyIndex].x = lfLowerX;
				vtInputArray[iNewApplyIndex].y = lfLowerY;
				iNewApplyIndex++;
			}

		} while( bExit == 0 );
		
		lfDelta = fabs(lfXcoordinate - vtInputArray[0].x);  
	
		for ( iNewIndex = 0 ; iNewIndex < iNumberOfPoints ; iNewIndex++ ) 
			if ( fabs(lfXcoordinate - vtInputArray[iNewIndex].x) < lfDelta )
			{
				lfDelta = fabs(lfXcoordinate - vtInputArray[iNewIndex].x);
				iStoreIndex = iNewIndex;
			}

		do
		{
			if (( iStoreIndex ) && (( lfXcoordinate - vtInputArray[iStoreIndex].x ) < 0 ))
			{
				iFirstIndex = iStoreIndex - 1;
				iSecondIndex = iStoreIndex;
				
				while ( vtInputArray[iSecondIndex].x == vtInputArray[iFirstIndex].x )
				{
					iFirstIndex--;
					iSecondIndex--;
					
					if ( iFirstIndex <= 0 )
						break;
				};
				
				break;
			}
	
			iFirstIndex = iStoreIndex;
			iSecondIndex = iStoreIndex + 1;
			
			while ( vtInputArray[iSecondIndex].x == vtInputArray[iFirstIndex].x )
			{
				iFirstIndex++;
				iSecondIndex++;
				
				if ( iSecondIndex >= iNumberOfPoints )
					break;
			};
		
		} while(0);
		
		if ( iSecondIndex == iNumberOfPoints ) 
		{
			iSecondIndex = iNumberOfPoints - 1;
		}
	
		if ((( iFirstIndex == 0 ) || ( iSecondIndex == ( iNumberOfPoints - 1 ))) && ( type > 0 ))
			type = 1;
	
		mu = ( lfXcoordinate - vtInputArray[iFirstIndex].x ) / ( vtInputArray[iSecondIndex].x - vtInputArray[iFirstIndex].x );  
	
		if ( IsNotANumber(mu) )
			mu = 0;
		
		switch(type)
		{
			default:
			case 0 :		// Linear
			
				result = (( vtInputArray[iFirstIndex].y * ( 1.0 - mu )) + ( vtInputArray[iSecondIndex].y * mu ));	
				
				break;
			
			case 1 :		// Cosine
			
				mu2 = ( 1.0 - cos( mu * Pi())) / 2.0;
				result = (( vtInputArray[iFirstIndex].y * ( 1.0 - mu )) + ( vtInputArray[iSecondIndex].y * mu ));	
				
				break;
		
			case 2 :		// Cubic
			
				mu2 = mu * mu;
				a0 = vtInputArray[iSecondIndex+1].y - vtInputArray[iSecondIndex].y - vtInputArray[iFirstIndex-1].y + vtInputArray[iFirstIndex].y;
				a1 = vtInputArray[iFirstIndex-1].y - vtInputArray[iFirstIndex].y - a0;
				a2 = vtInputArray[iSecondIndex].y - vtInputArray[iFirstIndex-1].y;
				a3 = vtInputArray[iFirstIndex].y;
			
				result = (a0*mu*mu2+a1*mu2+a2*mu+a3);	
				
				break;
			
			case 3 :		// Hermite
			
				mu2 = mu * mu;
				a0 = (-0.5 * vtInputArray[iFirstIndex-1].y) + ( 1.5 * vtInputArray[iFirstIndex].y ) - ( 1.5 * vtInputArray[iSecondIndex].y ) + ( 0.5 * vtInputArray[iSecondIndex+1].y );
				a1 = vtInputArray[iFirstIndex-1].y - ( 2.5 * vtInputArray[iFirstIndex].y ) + ( 2.0 * vtInputArray[iSecondIndex].y) - ( 0.5 * vtInputArray[iSecondIndex+1].y);
				a2 = (-0.5 * vtInputArray[iFirstIndex-1].y) + (0.5 * vtInputArray[iSecondIndex].y);
				a3 = vtInputArray[iFirstIndex].y;
			
				result = (a0*mu*mu2+a1*mu2+a2*mu+a3);	
				
				break;
		}
	
		*pYcoordinate = result;
		
	} while(0);
	
	if ( vtInputArray )
		free( vtInputArray );
	
	if ( pMapping )
		free( pMapping );
			
	return error;
}

// type : 0 - Linear , 1 - Cosine , 2 - Cubic , 3 - Hermite
int		MTH_InterpolationArray( int type , double *vlfXaxisArray , double *vlfYaxisArray ,  int iNumberOfPoints , int iStartIndex , int iBlockLength , int iOutputNumberOfPoints , double lfStartX , double lfStopX , double **vXgeneratedArray  , double **vYgeneratedArray )
{
	tsSinglePoint		*vtInputArray		=	NULL;
	
	int					index				=	0,
						iNewIndex			=	0,
						iCurrentIndex		=	0;

	int					*pMapping			=	NULL;
	
	double				lfLowerX			=	0.0,
						lfLowerY			=	0.0;
	
	double				result				=	0.0,
						mu					=	0.0,
						mu2					=	0.0,
						a0					=	0.0,
						a1					=	0.0,
						a2					=	0.0,
						a3					=	0.0;
							
	double				lfMaxX				=	0.0, 
						lfMinX				=	0.0,
						lfDelta				=	0.0;
						
	int					iFirstIndex			=	0,
						iStoreIndex			=	0,
						iSecondIndex		=	0,
						iNewApplyIndex		=	0;
						
	int					iCurrentType		=	0;
	
	double				*vOutXarray			=	NULL,
						*vOutYarray			=	NULL;
	
	double				lfStep				=	0.0,
						lfCurrentX			=	0.0;
		
	int					bDoAgain			=	0;
	
	if (( vlfXaxisArray == NULL ) || ( vlfYaxisArray == NULL ) || ( iNumberOfPoints <= 0 ))
		return -1;
	
	lfStep = ( lfStopX - lfStartX ) / ( iOutputNumberOfPoints - 1 );
	lfCurrentX = lfStartX;
	
	vOutXarray = calloc( iOutputNumberOfPoints , sizeof(double));
	vOutYarray = calloc( iOutputNumberOfPoints , sizeof(double));          
	
	vtInputArray = calloc( iNumberOfPoints , sizeof(tsSinglePoint));
	pMapping = calloc( iNumberOfPoints , sizeof(int)); 
	
	if (( vtInputArray == NULL ) || ( pMapping == NULL ) || ( vOutXarray == NULL ) || ( vOutYarray == NULL ))
	{
		if ( vOutXarray ) 
			free(vOutXarray);
		
		if ( vOutYarray ) 
			free(vOutYarray);
		
		if ( vtInputArray ) 
			free(vtInputArray);
		
		if ( pMapping )
			free(pMapping);
		
		return -1;
	}
	
	lfMaxX = vlfXaxisArray[0]; 
	lfMinX = vlfXaxisArray[0]; 

	for ( index = 0 ; index < iNumberOfPoints ; index++ )
	{
		if ( vlfXaxisArray[index] > lfMaxX )
			lfMaxX = vlfXaxisArray[index];

		if ( vlfXaxisArray[index] < lfMinX )
			lfMinX = vlfXaxisArray[index];
	}
	
	do
	{
		bDoAgain = 0;
		
		for ( index = iStartIndex ; index < iBlockLength ; index++ ) 
			if ( pMapping[index] == 0 )
			{
				bDoAgain = 1;
				break;
			}
		
		if ( bDoAgain == 0 )
			break;
		
		// -------- Sort and copy to new array -------------------------// 
		for ( iNewIndex = iStartIndex ; iNewIndex < iBlockLength ; iNewIndex++ )
		{
			if ( pMapping[iNewIndex] )
				continue;
			
			lfLowerX = vlfXaxisArray[iNewIndex];  
			lfLowerY = vlfYaxisArray[iNewIndex];
			iCurrentIndex = iNewIndex;
			
			for ( index = iStartIndex ; index < iBlockLength ; index++ )
			if (( pMapping[index] == 0 ) && ( vlfXaxisArray[index] <= lfLowerX ))
			{
				iCurrentIndex = index;	
				lfLowerX = vlfXaxisArray[index];
				lfLowerY = vlfYaxisArray[index];
			}
	
			pMapping[iCurrentIndex] = 1;
			
			vtInputArray[iNewApplyIndex].x = lfLowerX;
			vtInputArray[iNewApplyIndex].y = lfLowerY;
			iNewApplyIndex++;
		}

	} while( 1 );
		
	iNumberOfPoints = iNewApplyIndex;

	if (( lfStartX < lfMinX ) || ( lfStopX > lfMaxX ))
	{
		ShowMessage ( INSTR_TYPE_CONTINUE , "Interpolation Error.","Check Start,Stop Frequencies!", NULL );
		
		if ( vOutXarray ) 
			free(vOutXarray);
		
		if ( vOutYarray )
			free(vOutYarray);
		
		if ( vtInputArray )
			free(vtInputArray);
		
		if ( pMapping ) 
			free(pMapping);
		
		return MTH_PolynomialExtrapolationArray( vlfXaxisArray , vlfYaxisArray ,  iNumberOfPoints , iStartIndex , iBlockLength, iOutputNumberOfPoints , lfStartX , lfStopX , vXgeneratedArray  , vYgeneratedArray );
	}
		
	lfDelta = fabs(lfCurrentX - vtInputArray[0].x);  
	
	for ( iNewIndex = 0 ; iNewIndex < iNumberOfPoints ; iNewIndex++ ) 
		if ( fabs(lfCurrentX - vtInputArray[iNewIndex].x) < lfDelta )
		{
			lfDelta = fabs(lfCurrentX - vtInputArray[iNewIndex].x);
			iStoreIndex = iNewIndex;
		}
	
	if(( lfCurrentX - vtInputArray[iStoreIndex].x ) > 0 )
	{
		iFirstIndex = iStoreIndex - 1;
		iSecondIndex = iStoreIndex;
		
		while ( vtInputArray[iSecondIndex].x == vtInputArray[iFirstIndex].x )
		{
			iFirstIndex--;
			iSecondIndex--;
			
			if ( iFirstIndex <= 0 )
				break;
		};
	}
	else
	{
		iFirstIndex = iStoreIndex;
		iSecondIndex = iStoreIndex + 1;
		
		while ( vtInputArray[iSecondIndex].x == vtInputArray[iFirstIndex].x )
		{
			iFirstIndex++;
			iSecondIndex++;
			
			if ( iSecondIndex >= iNumberOfPoints )
				break;
		};
	}
		
	if ( iSecondIndex == iNumberOfPoints ) 
	{
		ShowMessage ( INSTR_TYPE_CONTINUE , "Interpolation Error.","Check Start,Stop Frequencies!" , NULL );
		
		return MTH_PolynomialExtrapolationArray( vlfXaxisArray , vlfYaxisArray ,  iNumberOfPoints , iStartIndex , iBlockLength, iOutputNumberOfPoints , lfStartX , lfStopX , vXgeneratedArray  , vYgeneratedArray );
	}
	
	for ( iNewIndex = 0 ; iNewIndex < iOutputNumberOfPoints ; iNewIndex++ )  
	{	
		if ( lfCurrentX > vtInputArray[iSecondIndex].x )
		{
			iFirstIndex++;
			iSecondIndex++;
		}
		
		if ( iSecondIndex >= iNumberOfPoints )
			break;
			
		iCurrentType = type;
		
		if ((( iFirstIndex == 0 ) || ( iSecondIndex == ( iNumberOfPoints - 1 ))) && ( iCurrentType > 0 ))
			iCurrentType = 1;
	
		do
		{
			if ( lfCurrentX == vtInputArray[iFirstIndex].x )
			{	
				result = vtInputArray[iFirstIndex].y;
				break;
			}
			
			if ( lfCurrentX == vtInputArray[iSecondIndex].x )
			{	
				result = vtInputArray[iSecondIndex].y;
				break;
			}
			
			mu = ( lfCurrentX - vtInputArray[iFirstIndex].x ) / ( vtInputArray[iSecondIndex].x - vtInputArray[iFirstIndex].x );  
	
			switch(iCurrentType)
			{
				default:
				case 0 :		// Linear
			
					result = (( vtInputArray[iFirstIndex].y * ( 1.0 - mu )) + ( vtInputArray[iSecondIndex].y * mu ));	
				
					break;
			
				case 1 :		// Cosine
			
					mu2 = ( 1.0 - cos( mu * Pi())) / 2.0;
					result = (( vtInputArray[iFirstIndex].y * ( 1.0 - mu2 )) + ( vtInputArray[iSecondIndex].y * mu2 ));	
				
					break;
		
				case 2 :		// Cubic
			
					mu2 = mu * mu;
					a0 = vtInputArray[iSecondIndex+1].y - vtInputArray[iSecondIndex].y - vtInputArray[iFirstIndex-1].y + vtInputArray[iFirstIndex].y;
					a1 = vtInputArray[iFirstIndex-1].y - vtInputArray[iFirstIndex].y - a0;
					a2 = vtInputArray[iSecondIndex].y - vtInputArray[iFirstIndex-1].y;
					a3 = vtInputArray[iFirstIndex].y;
			
					result = (a0*mu*mu2+a1*mu2+a2*mu+a3);	
				
					break;
			
				case 3 :		// Hermite
			
					mu2 = mu * mu;
					a0 = (-0.5 * vtInputArray[iFirstIndex-1].y) + ( 1.5 * vtInputArray[iFirstIndex].y ) - ( 1.5 * vtInputArray[iSecondIndex].y ) + ( 0.5 * vtInputArray[iSecondIndex+1].y );
					a1 = vtInputArray[iFirstIndex-1].y - ( 2.5 * vtInputArray[iFirstIndex].y ) + ( 2.0 * vtInputArray[iSecondIndex].y) - ( 0.5 * vtInputArray[iSecondIndex+1].y);
					a2 = (-0.5 * vtInputArray[iFirstIndex-1].y) + (0.5 * vtInputArray[iSecondIndex].y);
					a3 = vtInputArray[iFirstIndex].y;
			
					result = (a0*mu*mu2+a1*mu2+a2*mu+a3);	
				
					break;
			}
			
		} while(0);
		
		vOutXarray[iNewIndex] = lfCurrentX;
		vOutYarray[iNewIndex] = result;
		lfCurrentX += lfStep;
	}
	
	if ( vXgeneratedArray )
		*vXgeneratedArray = vOutXarray;
	else
		free(vOutXarray);
		
	if ( vYgeneratedArray )
		*vYgeneratedArray = vOutYarray;
	else
		free(vOutYarray);
	
	if ( pMapping ) 
		free(pMapping);
		
	if ( vtInputArray )
		free(vtInputArray);
	
	return 0;
}
