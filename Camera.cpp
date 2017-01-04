#include "Camera.h"

Camera::Camera():	flags(0),
					position( XMFLOAT3(0,0,0) ),
					viewVector( XMFLOAT3(0,0,1) ),
					upVector( XMFLOAT3(0,1,0) ),
					heading(0), 
					pitch(0), 
					movementSpeed(0), 
					dV(XMFLOAT3(0,0,1)),
					dU(XMFLOAT3(0,1,0))
{
	XMMATRIX m = XMMatrixIdentity();
	XMStoreFloat4x4(&viewMatrix, m);
	XMStoreFloat4x4(&projectionMatrix, m);
	XMStoreFloat4x4(&viewProjectionMatrix, m);	

	timer.Start();
}

void Camera::PositionCamera(const float camPosition[3], const float focusPoint[3], const float upDirection[3])
{
	XMVECTOR ep = XMLoadFloat3( (XMFLOAT3*) &camPosition);
	XMVECTOR fp = XMLoadFloat3( (XMFLOAT3*) &focusPoint);
	XMVECTOR ud = XMLoadFloat3( (XMFLOAT3*) &upDirection);
	XMMATRIX vm = XMMatrixLookAtLH( ep, fp, ud);
	XMStoreFloat4x4( &viewMatrix, vm);

	fp = XMVector3Normalize(fp - ep);
	ud = XMVector3Normalize(ud);
	position = camPosition;
	XMStoreFloat3( &viewVector, fp);
	XMStoreFloat3( &upVector, ud);
}

void Camera::SetPerspectiveProjectionLH(float FOVY, float aspectRatio, float zNear, float zFar)
{
	XMMATRIX p2 = XMMatrixPerspectiveFovLH(FOVY, aspectRatio, zNear, zFar);
	XMStoreFloat4x4( &projectionMatrix, p2);	
}

const XMFLOAT3& Camera::GetCameraPosition() 
{ 
	return position; 
}
const XMFLOAT3& Camera::GetCameraUpVector()
{	
	return upVector;
}
const XMFLOAT3& Camera::GetCameraViewVector()
{
	return viewVector;
}
const XMFLOAT4X4& Camera::GetViewMatrix() 
{ 
	return viewMatrix; 
}
const XMFLOAT4X4& Camera::GetProjectionMatrix() 
{ 
	return projectionMatrix; 
}
const XMFLOAT4X4& Camera::GetViewProjectionMatrix()
{
	return viewProjectionMatrix;
}

void Camera::HandleInputMessage( unsigned int msg )
{
	if ( msg == 0 ) SetBit(flags, 0);
	else if ( msg == 4 ) ClearBit(flags, 0);
	else if ( msg == 1 ) SetBit(flags, 1);
	else if ( msg == 5 ) ClearBit(flags, 1);
	else if ( msg == 2 ) SetBit(flags, 2);
	else if ( msg == 6 ) ClearBit(flags, 2);
	else if ( msg == 3 ) SetBit(flags, 3);
	else if ( msg == 7 ) ClearBit(flags, 3);
}

void Camera::Update()
{
	float t = (float) timer.GetElapsedTimeSeconds();
	
	XMMATRIX m = XMMatrixRotationRollPitchYaw( pitch, heading, 0 );

	XMVECTOR view = XMLoadFloat3(&dV);
	XMVECTOR up = XMLoadFloat3(&dU);
	view = XMVector3Normalize(XMVector3Transform(view, m));
	up = XMVector3Normalize(XMVector3Transform(up, m));	
	XMVECTOR r = XMVector3Normalize(XMVector3Cross(up, view));
		
	XMVECTOR eye = XMLoadFloat3(&position);
	if( (flags & 0x80000000) == 0x80000000 ) eye += movementSpeed * t * view;	//forward
	if( (flags & 0x40000000) == 0x40000000 ) eye -= movementSpeed * t * view;	//back
	if( (flags & 0x20000000) == 0x20000000 ) eye -= movementSpeed * t * r;		//left
	if( (flags & 0x10000000) == 0x10000000 ) eye += movementSpeed * t * r;		//right
	
	m = XMMatrixLookAtLH( eye, eye+view, up );
	XMStoreFloat4x4(&viewMatrix, m);
	
	XMStoreFloat3(&position, eye);
	XMStoreFloat3(&viewVector, view);
	XMStoreFloat3(&upVector, up);

	XMMATRIX p = XMLoadFloat4x4(&projectionMatrix);
	m = XMMatrixMultiply(m, p);
	XMStoreFloat4x4(&viewProjectionMatrix, m);
	
	timer.Reset();
}

void Camera::AdjustOrientation( const float hRad, const float pRad )
{
	heading += hRad;
	pitch += pRad;

	if ( heading > XM_2PI ) heading -= (float) XM_2PI;
	else if ( heading < 0 ) heading = (float) XM_2PI + heading;
	
	if ( pitch > XM_2PI ) pitch -= (float) XM_2PI;
	else if ( pitch < 0 ) pitch = (float) XM_2PI + pitch;
}

void Camera::SetMovementSpeed( const float s)
{
	movementSpeed = s;
}