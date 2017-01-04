#include "DX10Renderer.h"

//external controls
void DX10Renderer::SetTechnique( unsigned int i )
{
	pRenderTechnique = pEffect->GetTechniqueByIndex(i);
}

void DX10Renderer::SetShadowMapTechnique( unsigned int i)
{
	pRenderShadowMapTechnique = pEffect->GetTechniqueByIndex(i);
}

void DX10Renderer::ToggleShadowMapSize()
{
	//set size according to current size
	if ( shadowMapViewport.Width == viewport.Width )
	{
		shadowMapViewport.Width = viewport.Width * 2;
		shadowMapViewport.Height = viewport.Height * 2;
	}
	else
	{
		shadowMapViewport.Width = viewport.Width;
		shadowMapViewport.Height = viewport.Height;
	}

	//release current shadow map resources
	pShadowMapDepthView->Release();
	pShadowMapSRView->Release();
	pShadowMap->Release();

	//recreate resources - create shadow map texture desc
	D3D10_TEXTURE2D_DESC texDesc;
	texDesc.Width = shadowMapViewport.Width;
	texDesc.Height = shadowMapViewport.Height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D10_USAGE_DEFAULT;
	texDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;   
    
	// Create the depth stencil view desc
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;		

	//create shader resource view desc
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	//create texture and depth/resource views
	pD3DDevice->CreateTexture2D( &texDesc, NULL, &pShadowMap );
	pD3DDevice->CreateDepthStencilView( pShadowMap, &descDSV, &pShadowMapDepthView );
	pD3DDevice->CreateShaderResourceView( pShadowMap, &srvDesc, &pShadowMapSRView );	
}

void DX10Renderer::SetShadowMapBias( float b )
{
	shadowMapBias = b;
}

void DX10Renderer::SetlightPos(bool direction)
{
	//adjust the value for rotation
	double lightDirection;

	if ( direction )
	{
		lightDirection = 0.01f;
	}
	else 
	{
		lightDirection = -0.01f;
	}

	//rotate light position
	XMFLOAT3 yaxis(0, 1, 0), o(0, 0, 0), u(0, 1, 0);
	XMMATRIX rotMatrix = XMMatrixRotationAxis(XMLoadFloat3(&yaxis), lightDirection);

	//new rotated light pos
	XMVECTOR olpos = XMLoadFloat3(&lightPos);
	XMVECTOR lpos = XMVector3TransformCoord(olpos, rotMatrix);
	XMStoreFloat3(&lightPos, lpos);

	//get new view matrix
	XMVECTOR fp = XMLoadFloat3(&o), up = XMLoadFloat3(&u);
	XMMATRIX vmat = XMMatrixLookAtLH(lpos, fp, up);

	//create new light view proj matrix
	XMMATRIX lpmat = XMLoadFloat4x4(&lightProjMatrix);
	XMMATRIX lvpmat = XMMatrixMultiply(vmat, lpmat);
	XMStoreFloat4x4(&lightViewProjMatrix, lvpmat);
}