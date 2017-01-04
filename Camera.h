#ifndef _CAMERA
#define _CAMERA

#include <Windows.h>
#include <Xnamath.h>
#include "HRTimer.h"

inline void SetBit(unsigned int& bitField, unsigned int bit)
{
	bitField |= 0x80000000 >> bit;
}

inline void ClearBit(unsigned int& bitField, unsigned int bit)
{
	bitField &= ~(0x80000000 >> bit);
}

inline void FlipBit(unsigned int& bitfield, unsigned int bit)
{
	if ( (bitfield & (0x80000000 >> bit)) == (0x80000000 >> bit) ) 
	{
		bitfield &= ~( 0x80000000 >> bit);
	}
	else bitfield |= 0x80000000 >> bit;;
}

inline bool IsBitSet(unsigned int& bitField, unsigned int bit)
{
	if ( ((bitField << bit) & 0x80000000) == 0x80000000 ) return true;
	else return false;
}

class Camera
{
protected:


	unsigned int flags;
		
	XMFLOAT3 position, viewVector, upVector;
	XMFLOAT4X4 viewMatrix, projectionMatrix, viewProjectionMatrix;
	
	float heading, pitch, movementSpeed;
	XMFLOAT3 dV, dU;

	HRTimer timer;

public:
		
	Camera();

	void PositionCamera( const float camPosition[3], const float focusPoint[3], const float upDirection[3] );
	void HandleInputMessage( unsigned int msg );
	void AdjustOrientation(const float hRad, const float vRad);
	void SetMovementSpeed( const float s );

	void Update();

	void SetPerspectiveProjectionLH( float FOVY, float aspectRatio, float zNear, float zFar );

	const XMFLOAT3& GetCameraPosition();
	const XMFLOAT3& GetCameraUpVector();
	const XMFLOAT3& GetCameraViewVector();
	const XMFLOAT4X4& GetViewMatrix();
	const XMFLOAT4X4& GetProjectionMatrix();
	const XMFLOAT4X4& GetViewProjectionMatrix();
};


#endif};