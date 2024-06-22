#include "scene_frame.h"
#include "ui_ext.h"
#include "hardware.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h> //!!!
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "timer.h"

using namespace DirectX;

float real_to_float(real value)
{
	return float(value.integer) + float(value.fraction) / float(max_real_fraction);
}

XMVECTOR real_vector_to_xmvector(vector<real, 4> value)
{
	return XMVectorSet(
		real_to_float(value.x),
		real_to_float(value.y),
		real_to_float(value.z),
		real_to_float(value.w));
}

XMMATRIX real_matrix_to_xmmatrix(matrix<real, 4, 4> value)
{
	return XMMatrixSet(
		real_to_float(value.m[0][0]), real_to_float(value.m[0][1]), real_to_float(value.m[0][2]), real_to_float(value.m[0][3]),
		real_to_float(value.m[1][0]), real_to_float(value.m[1][1]), real_to_float(value.m[1][2]), real_to_float(value.m[1][3]),
		real_to_float(value.m[2][0]), real_to_float(value.m[2][1]), real_to_float(value.m[2][2]), real_to_float(value.m[2][3]),
		real_to_float(value.m[3][0]), real_to_float(value.m[3][1]), real_to_float(value.m[3][2]), real_to_float(value.m[3][3]));
}

scene::scene()
{
	wnd_handler = nullptr;
}

scene::~scene()
{
	set_window(nullptr);
}

struct camera_view
{
	XMVECTOR position;
	XMVECTOR target;
	XMVECTOR up;
	XMMATRIX projection;
	XMMATRIX view;
};

struct texture_vertex
{
	vector<float, 3> point;
	vector<float, 2> texture;

	texture_vertex(
		vector<float, 3> point_value,
		vector<float, 2> texture_value)
		: point(point_value), texture(texture_value) {}
};

struct cbPerObject
{
    XMMATRIX WVP;
};

struct d3d11_resource_set
{
	IDXGISwapChain *swap_chain;
	ID3D11Device *d3d11_device;
	ID3D11DeviceContext *d3d11_devcon;
	ID3D11RenderTargetView *render_target_view;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	
	cbPerObject cbPerObj;
	ID3D11Buffer* cbPerObjectBuffer;
	XMMATRIX WVP;
	XMMATRIX World;

	camera_view camera;
	XMVECTOR DefaultForward;
	XMVECTOR DefaultRight;
	XMVECTOR camForward;
	XMVECTOR camRight;
	XMMATRIX camRotationMatrix;
	XMMATRIX groundWorld;
	float moveLeftRight;
	float moveBackForward;
	float camYaw;
	float camPitch;
	bool move_forward;
	bool move_back;
	bool move_right;
	bool move_left;
	nanoseconds last_move_time;

	ID3D11Buffer* squareIndexBuffer;
	ID3D11Buffer* squareVertBuffer;
	ID3DBlob* VS_Buffer;
	ID3DBlob* PS_Buffer;
	ID3D11VertexShader *VS;
	ID3D11PixelShader *PS;
	ID3D11InputLayout *vertLayout;

	XMMATRIX cube1World;
	XMMATRIX cube2World;
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;
	float rot = 0.01f;
	
	ID3D11Texture2D *ui_texture;
	ID3D11ShaderResourceView *ui_srv;
	ID3D11SamplerState *ui_sampler_state;
};

void UpdateCamera(d3d11_resource_set *rd)
{
	rd->camRotationMatrix = XMMatrixRotationRollPitchYaw(rd->camPitch, rd->camYaw, 0);
    rd->camera.target = XMVector3TransformCoord(rd->DefaultForward, rd->camRotationMatrix );
    rd->camera.target = XMVector3Normalize(rd->camera.target);

    XMMATRIX RotateYTempMatrix;
    RotateYTempMatrix = XMMatrixRotationY(rd->camYaw);

    rd->camRight = XMVector3TransformCoord(rd->DefaultRight, RotateYTempMatrix);
    rd->camera.up = XMVector3TransformCoord(rd->camera.up, RotateYTempMatrix);
    rd->camForward = XMVector3TransformCoord(rd->DefaultForward, RotateYTempMatrix);

	rd->moveBackForward = 0.0f;
	if(rd->move_forward) rd->moveBackForward += 10.0f * 0.0000000001f * float(now().value - rd->last_move_time.value);
	if(rd->move_back) rd->moveBackForward -= 10.0f * 0.0000000001f * float(now().value - rd->last_move_time.value);
	rd->moveLeftRight = 0.0f;
	if(rd->move_right) rd->moveLeftRight += 10.0f * 0.0000000001f * float(now().value - rd->last_move_time.value);
	if(rd->move_left) rd->moveLeftRight -= 10.0f * 0.0000000001f * float(now().value - rd->last_move_time.value);
	rd->last_move_time = now();
    rd->camera.position += rd->moveLeftRight*rd->camRight;
    rd->camera.position += rd->moveBackForward*rd->camForward;

    rd->camera.target = rd->camera.position + rd->camera.target;    

    rd->camera.view = XMMatrixLookAtLH( rd->camera.position, rd->camera.target, rd->camera.up );
}

void scene::set_window(void *wnd_handler_value)
{
	d3d11_resource_set *rd;
	if(wnd_handler != nullptr)
	{
		rd = (d3d11_resource_set *)(render_device);
		rd->swap_chain->Release();
		rd->d3d11_device->Release();
		rd->d3d11_devcon->Release();
		delete rd;
		//!!!
	}
	wnd_handler = wnd_handler_value;
	if(wnd_handler == nullptr) return;
	rd = new d3d11_resource_set();
	render_device = (void *)(rd);
	RECT rect;
	GetClientRect(HWND(wnd_handler), &rect);

	DXGI_MODE_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));
    bufferDesc.Width = UINT(rect.right - rect.left);
    bufferDesc.Height = UINT(rect.bottom - rect.top);
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc; 
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = HWND(wnd_handler); 
    swapChainDesc.Windowed = TRUE; 
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
        D3D11_SDK_VERSION, &swapChainDesc, &rd->swap_chain, &rd->d3d11_device, NULL, &rd->d3d11_devcon);

    ID3D11Texture2D* BackBuffer;
    rd->swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&BackBuffer));
    rd->d3d11_device->CreateRenderTargetView(BackBuffer, NULL, &rd->render_target_view);
    BackBuffer->Release();

	 D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width     =  bufferDesc.Width;
    depthStencilDesc.Height    =  bufferDesc.Height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0; 
    depthStencilDesc.MiscFlags      = 0;

    //Create the Depth/Stencil View
    rd->d3d11_device->CreateTexture2D(&depthStencilDesc, NULL, &rd->depthStencilBuffer);
    rd->d3d11_device->CreateDepthStencilView(rd->depthStencilBuffer, NULL, &rd->depthStencilView);
	
    rd->d3d11_devcon->OMSetRenderTargets(1, &rd->render_target_view, rd->depthStencilView );

	D3DCompileFromFile(L"vs.hlsl", 0, 0, "VS", "vs_5_0", 0, 0, &rd->VS_Buffer, 0);
    D3DCompileFromFile(L"ps.hlsl", 0, 0, "PS", "ps_5_0", 0, 0, &rd->PS_Buffer, 0);
    rd->d3d11_device->CreateVertexShader(rd->VS_Buffer->GetBufferPointer(), rd->VS_Buffer->GetBufferSize(), NULL, &rd->VS);
    rd->d3d11_device->CreatePixelShader(rd->PS_Buffer->GetBufferPointer(), rd->PS_Buffer->GetBufferSize(), NULL, &rd->PS);
    rd->d3d11_devcon->VSSetShader(rd->VS, 0, 0);
    rd->d3d11_devcon->PSSetShader(rd->PS, 0, 0);

	texture_vertex v[] =
    {
        texture_vertex(vector<float, 3>( -1.0f, -1.0f, 0.0f), vector<float, 2>(0.0f, 0.0f)),
        texture_vertex(vector<float, 3>( 1.0f, -1.0f, 0.0f),  vector<float, 2>(1.0f, 0.0f)),
        texture_vertex(vector<float, 3>( +1.0f, +1.0f, 0.0f), vector<float, 2>(1.0f, 1.0f)),
        texture_vertex(vector<float, 3>( -1.0f, 1.0f, 0.0f), vector<float, 2>(0.0f, 1.0f))
    };

    DWORD indices[] = {
        // front face
        0, 1, 2,
        0, 2, 3,
    };

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    rd->d3d11_device->CreateBuffer(&indexBufferDesc, &iinitData, &rd->squareIndexBuffer);
    rd->d3d11_devcon->IASetIndexBuffer( rd->squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( texture_vertex) * 4;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 
    ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
    vertexBufferData.pSysMem = v;
    rd->d3d11_device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &rd->squareVertBuffer);

    UINT stride = sizeof(texture_vertex);
    UINT offset = 0;
    rd->d3d11_devcon->IASetVertexBuffers( 0, 1, &rd->squareVertBuffer, &stride, &offset );
	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);
    rd->d3d11_device->CreateInputLayout(layout, numElements, rd->VS_Buffer->GetBufferPointer(), 
		rd->VS_Buffer->GetBufferSize(), &rd->vertLayout);
    rd->d3d11_devcon->IASetInputLayout(rd->vertLayout);
    rd->d3d11_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = float(rect.right - rect.left);
    viewport.Height = float(rect.bottom - rect.top);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
    rd->d3d11_devcon->RSSetViewports(1, &viewport);

	ID3D11RasterizerState *rs;
	D3D11_RASTERIZER_DESC rs_desc;
	rs_desc.AntialiasedLineEnable = false;
	rs_desc.CullMode = D3D11_CULL_BACK;
	rs_desc.DepthBias = 0;
	rs_desc.DepthBiasClamp = 0.0f;
	rs_desc.DepthClipEnable = true;
	rs_desc.FillMode = D3D11_FILL_SOLID;
	rs_desc.FrontCounterClockwise = true;
	rs_desc.MultisampleEnable = false;
	rs_desc.ScissorEnable = false;
	rs_desc.SlopeScaledDepthBias = 0.0f;
	rd->d3d11_device->CreateRasterizerState(&rs_desc, &rs);
	rd->d3d11_devcon->RSSetState(rs);

	D3D11_BUFFER_DESC cbbd;    
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerObject);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;
    cbbd.MiscFlags = 0;
    rd->d3d11_device->CreateBuffer(&cbbd, NULL, &rd->cbPerObjectBuffer);

	rd->camera.position = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	rd->camera.target = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	rd->camera.up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    rd->camera.view = XMMatrixLookAtLH( rd->camera.position, rd->camera.target, rd->camera.up );
    rd->camera.projection = XMMatrixPerspectiveFovLH(
		0.4f*3.14f,
		float(rect.right - rect.left)/float(rect.bottom - rect.top),
		1.0f,
		1000.0f);
	rd->DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
	rd->DefaultRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
	rd->camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
	rd->camRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
	rd->moveLeftRight = 0.0f;
	rd->moveBackForward = 0.0f;
	rd->camYaw = 0.0f;
	rd->camPitch = 0.0f;
	rd->move_forward = false;
	rd->move_back = false;
	rd->move_right = false;
	rd->move_left = false;
	rd->last_move_time = now();
	
	D3D11_TEXTURE2D_DESC screenshot_desc = CD3D11_TEXTURE2D_DESC(
		DXGI_FORMAT_R8G8B8A8_UNORM,     // format
		rect.right - rect.left,               // width
		rect.bottom - rect.top,              // height
		1,                              // arraySize
		1,                              // mipLevels
		D3D11_BIND_SHADER_RESOURCE,     // bindFlags
		D3D11_USAGE_DYNAMIC,            // usage
		D3D11_CPU_ACCESS_WRITE,         // cpuaccessFlags
		1,                              // sampleCount
		0,                              // sampleQuality
		0);                               // miscFlags

	rd->d3d11_device->CreateTexture2D(
		&screenshot_desc, //texture format
		nullptr,          // pixel buffer use to fill the texture
		&rd->ui_texture  // created texture
		);

    rd->d3d11_device->CreateShaderResourceView(rd->ui_texture, NULL, &rd->ui_srv);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	rd->d3d11_device->CreateSamplerState( &sampDesc, &rd->ui_sampler_state );

	/*timer *t = new timer();
	t->period = 0;
	t->period << milliseconds(16);
	auto t_clb = [] (void *data) -> timer_trigger_postaction
	{
		scene *sc = (scene *)(data);
		d3d11_resource_set *rd = (d3d11_resource_set *)(sc->render_device);
		rd->rot += 0.001f;
		if(rd->rot > 6.26f)
			rd->rot = 0.0f;
		rd->cube1World = XMMatrixIdentity();

		//Define cube1's world space matrix
		XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		rd->Rotation = XMMatrixRotationAxis( rotaxis, rd->rot);
		rd->Translation = XMMatrixTranslation( 0.0f, 0.0f, 4.0f );

		//Set cube1's world space using the transformations
		rd->cube1World = rd->Translation * rd->Rotation;

		//Reset cube2World
		rd->cube2World = XMMatrixIdentity();

		//Define cube2's world space matrix
		rd->Rotation = XMMatrixRotationAxis( rotaxis, -rd->rot);
		rd->Scale = XMMatrixScaling( 1.3f, 1.3f, 1.3f );

		//Set cube2's world space matrix
		rd->cube2World = rd->Rotation * rd->Scale;
		return timer_trigger_postaction::repeat;
	};
	t->callback = t_clb;
	t->data = (void *)(this);
	t->run();*/
}

void scene::render(bitmap &upper_layer)
{
	d3d11_resource_set *rd = (d3d11_resource_set *)(render_device);

	D3D11_VIDEO_COLOR_RGBA bgColor { 1.0f, 1.0f, 0.0f, 1.0f };
    rd->d3d11_devcon->ClearRenderTargetView(rd->render_target_view, (float *)(&bgColor));
	rd->d3d11_devcon->ClearDepthStencilView(rd->depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	UpdateCamera(rd);

	D3D11_MAPPED_SUBRESOURCE mapped;
	rd->d3d11_devcon->Map(rd->ui_texture, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	void *src = upper_layer.data, *dest = mapped.pData;
	move_addr(&src, upper_layer.width * (upper_layer.height - 1) * sizeof(alpha_color));
	for(uint32 i = 0; i < upper_layer.height; i++)
	{
		memcpy(dest, src, upper_layer.width * sizeof(alpha_color));
		move_addr(&src, -int32(upper_layer.width * sizeof(alpha_color)));
		move_addr(&dest, mapped.RowPitch);
	}
	rd->d3d11_devcon->Unmap(rd->ui_texture, 0);

	rd->cube2World = XMMatrixIdentity();
    rd->WVP = rd->cube2World * rd->camera.view * rd->camera.projection;
    rd->cbPerObj.WVP = XMMatrixTranspose(rd->WVP);
    rd->d3d11_devcon->UpdateSubresource( rd->cbPerObjectBuffer, 0, NULL, &rd->cbPerObj, 0, 0 );
    rd->d3d11_devcon->VSSetConstantBuffers( 0, 1, &rd->cbPerObjectBuffer );
	rd->d3d11_devcon->PSSetShaderResources( 0, 1, &rd->ui_srv );
    rd->d3d11_devcon->PSSetSamplers( 0, 1, &rd->ui_sampler_state );

    //Draw the second cube
    rd->d3d11_devcon->DrawIndexed( 6, 0, 0 );

	

    rd->swap_chain->Present(0, 0);
}

bool scene_frame_hit_test(indefinite<frame> fm, vector<int32, 2> point)
{
	scene_frame *sf = (scene_frame *)(fm.addr);
	return utility<frame>().rectangular_hit_test(&sf->fm, point);
}

void scene_frame_subframes(indefinite<frame> fm, array<handleable<frame>> *frames)
{
	scene_frame *sf = (scene_frame *)(fm.addr);
	frames->push(sf->layout);
}

void scene_frame_mouse_move(indefinite<frame> fm)
{
	scene_frame *sf = (scene_frame *)(fm.addr);
	d3d11_resource_set *rd = (d3d11_resource_set *)(sf->sc->render_device);
	rd->camPitch += 0.01f * float(mouse()->position.y - mouse()->prev_position.y);
	rd->camYaw += 0.01f * float(mouse()->position.x - mouse()->prev_position.x);
}

void scene_frame_key_press(indefinite<frame> fm)
{
	scene_frame *sf = (scene_frame *)(fm.addr);
	d3d11_resource_set *rd = (d3d11_resource_set *)(sf->sc->render_device);
	if(keyboard()->key_pressed[uint32(key_code::w)])
		rd->move_forward = true;
	if(keyboard()->key_pressed[uint32(key_code::s)])
		rd->move_back = true;
	if(keyboard()->key_pressed[uint32(key_code::d)])
		rd->move_right = true;
	if(keyboard()->key_pressed[uint32(key_code::a)])
		rd->move_left = true;
}

void scene_frame_key_release(indefinite<frame> fm)
{
	scene_frame *sf = (scene_frame *)(fm.addr);
	d3d11_resource_set *rd = (d3d11_resource_set *)(sf->sc->render_device);
	if(!keyboard()->key_pressed[uint32(key_code::w)])
		rd->move_forward = false;
	if(!keyboard()->key_pressed[uint32(key_code::s)])
		rd->move_back = false;
	if(!keyboard()->key_pressed[uint32(key_code::d)])
		rd->move_right = false;
	if(!keyboard()->key_pressed[uint32(key_code::a)])
		rd->move_left = false;
}

void scene_frame_render(indefinite<frame> fm, vector<int32, 2> point, bitmap_processor *bp, bitmap *bmp)
{
	scene_frame *sf = (scene_frame *)(fm.addr);
	sf->layout.core->x = sf->fm.x;
	sf->layout.core->y = sf->fm.y;
	sf->layout.core->width = sf->fm.width;
	sf->layout.core->height = sf->fm.height;
	sf->layout.core->render(sf->layout.object, point, bp, bmp);
	sf->sc->render(*bmp);
}

scene_frame::scene_frame()
{
	fm.hit_test = scene_frame_hit_test;
	fm.subframes = scene_frame_subframes;
	fm.render = scene_frame_render;
	fm.mouse_move = scene_frame_mouse_move;
	fm.key_press = scene_frame_key_press;
	fm.key_release = scene_frame_key_release;
}
