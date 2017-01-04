#include "DX10Renderer.h"
#include "Camera.h"

extern Camera camera;

//render frame
void DX10Renderer::RenderFrame()
{	
	//set per frame variables
	//***************************************************************************
	
	//effect vars
	ID3D10EffectMatrixVariable* pWorldMatrix = pEffect->GetVariableByName("world")->AsMatrix();
	ID3D10EffectMatrixVariable* pViewProjMatrix = pEffect->GetVariableByName("viewProj")->AsMatrix();
	ID3D10EffectMatrixVariable* pLightViewProjMatrix = pEffect->GetVariableByName("lightViewProj")->AsMatrix();
	ID3D10EffectVectorVariable* pLightPos = pEffect->GetVariableByName("lightPos")->AsVector();
	
	pLightPos->SetFloatVectorArray( (float*) &lightPos, 0, 3);
	pViewProjMatrix->SetMatrix( (float*) &camera.GetViewProjectionMatrix() );	
	pLightViewProjMatrix->SetMatrix( (float*) &lightViewProjMatrix );
	
	//set topology to triangle list
	pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//create shadow map
	//***************************************************************************
		
	//set render targets and viewport
	pD3DDevice->OMSetRenderTargets(0, 0, pShadowMapDepthView);
	pD3DDevice->RSSetViewports(1, &shadowMapViewport);
	pD3DDevice->ClearDepthStencilView( pShadowMapDepthView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

	//render scene
	int currMeshID = -1;	
	for (unsigned int i=0; i < scene.size(); i++)
	{
		//set appropriate vertex buffers
		if ( currMeshID != scene[i].modelID )
		{
			currMeshID = scene[i].modelID;
			pD3DDevice->IASetVertexBuffers( 0,1, &meshes[currMeshID].pVertexBuffer, &meshes[currMeshID].stride, &meshes[currMeshID].offset );
			pD3DDevice->IASetIndexBuffer( meshes[currMeshID].pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		}
		
		//set instance data and draw
		pWorldMatrix->SetMatrix((float*) &scene[i].world);
		pRenderShadowMapTechnique->GetPassByIndex(0)->Apply( 0 );	
		pD3DDevice->DrawIndexed(meshes[scene[i].modelID].numIndices, 0, 0);
	}	

	//render final scene	
	//***************************************************************************
		
	//set render targets and viewports
	pD3DDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);	
	pD3DDevice->RSSetViewports(1, &viewport);
	pD3DDevice->ClearRenderTargetView( pRenderTargetView, D3DXCOLOR(0.6f,0.6f,0.6f,0) );
	pD3DDevice->ClearDepthStencilView( pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );
		
	//bind shadow map texture and set shadow map bias
	pEffect->GetVariableByName("shadowMap")->AsShaderResource()->SetResource( pShadowMapSRView );
	pEffect->GetVariableByName("shadowMapBias")->AsScalar()->SetFloat(shadowMapBias);
	int smSize[2] = { shadowMapViewport.Width, shadowMapViewport.Height };
	pEffect->GetVariableByName("shadowMapSize")->AsVector()->SetIntVector(smSize);

	//render scene
	currMeshID = -1;	
	for (unsigned int i=0; i < scene.size(); i++)
	{
		//set appropriate vertex buffers
		if ( currMeshID != scene[i].modelID )
		{
			currMeshID = scene[i].modelID;
			pD3DDevice->IASetVertexBuffers( 0,1, &meshes[currMeshID].pVertexBuffer, &meshes[currMeshID].stride, &meshes[currMeshID].offset );
			pD3DDevice->IASetIndexBuffer( meshes[currMeshID].pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		}
		
		//set instance data and draw
		pWorldMatrix->SetMatrix((float*) &scene[i].world);
		pRenderTechnique->GetPassByIndex(0)->Apply( 0 );	
		pD3DDevice->DrawIndexed(meshes[scene[i].modelID].numIndices, 0, 0);
	}

	pEffect->GetVariableByName("shadowMap")->AsShaderResource()->SetResource( 0 );

	//swap buffers
	pSwapChain->Present(0,0);
}