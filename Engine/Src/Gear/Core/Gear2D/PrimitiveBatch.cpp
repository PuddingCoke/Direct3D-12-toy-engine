#include<Gear/Core/Gear2D/PrimitiveBatch.h>

PrimitiveBatch::PrimitiveBatch(const DXGI_FORMAT format, GraphicsContext* const context) :
	context(context), lineWidth(1.f)
{
	lineVS = new Shader(g_PrimitiveBatchLineVSBytes, sizeof(g_PrimitiveBatchLineVSBytes));

	circleVS = new Shader(g_PrimitiveBatchCircleVSBytes, sizeof(g_PrimitiveBatchCircleVSBytes));

	rcLineVS = new Shader(g_PrimitiveBatchRCLineVSBytes, sizeof(g_PrimitiveBatchRCLineVSBytes));

	lineGS = new Shader(g_PrimitiveBatchLineGSBytes, sizeof(g_PrimitiveBatchLineGSBytes));

	rcLineGS = new Shader(g_PrimitiveBatchRCLineGSBytes, sizeof(g_PrimitiveBatchRCLineGSBytes));

	primitivePS = new Shader(g_PrimitiveBatchPSBytes, sizeof(g_PrimitiveBatchPSBytes));

	{
		D3D12_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { layout,_countof(layout) };
		desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
		desc.VS = lineVS->getByteCode();
		desc.GS = lineGS->getByteCode();
		desc.PS = primitivePS->getByteCode();
		desc.RasterizerState = States::rasterCullBack;
		desc.BlendState = States::defBlendDesc;
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = format;
		desc.SampleDesc.Count = 1;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&lineState));
	}

	{
		D3D12_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
			{"POSITION",1,DXGI_FORMAT_R32_FLOAT,0,8,D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,1}
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { layout,_countof(layout) };
		desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
		desc.VS = circleVS->getByteCode();
		desc.GS = lineGS->getByteCode();
		desc.PS = primitivePS->getByteCode();
		desc.RasterizerState = States::rasterCullBack;
		desc.BlendState = States::defBlendDesc;
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = format;
		desc.SampleDesc.Count = 1;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&circleState));
	}

	{
		D3D12_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"POSITION", 1, DXGI_FORMAT_R32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { layout,_countof(layout) };
		desc.pRootSignature = GlobalRootSignature::getGraphicsRootSignature()->get();
		desc.VS = rcLineVS->getByteCode();
		desc.GS = rcLineGS->getByteCode();
		desc.PS = primitivePS->getByteCode();
		desc.RasterizerState = States::rasterCullBack;
		desc.BlendState = States::defBlendDesc;
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = format;
		desc.SampleDesc.Count = 1;

		GraphicsDevice::get()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&roundCapLineState));
	}
}

PrimitiveBatch::~PrimitiveBatch()
{
	delete lineVS;
	delete circleVS;
	delete rcLineVS;
	delete primitivePS;
	delete lineGS;
	delete rcLineGS;
}

void PrimitiveBatch::begin()
{
	lineRenderer.begin();
	circleRenderer.begin();
	roundCapLineRenderer.begin();
}

void PrimitiveBatch::end()
{
	context->setGSConstants(1, &lineWidth, 0);

	lineRenderer.end(context, lineState.Get());
	circleRenderer.end(context, circleState.Get());
	roundCapLineRenderer.end(context, roundCapLineState.Get());
}

void PrimitiveBatch::drawLine(const float x1, const float y1, const float x2, const float y2, const float r, const float g, const float b, const float a)
{
	lineRenderer.addLine(x1, y1, x2, y2, r, g, b, a);
}

void PrimitiveBatch::drawCircle(const float x, const float y, const float length, const float r, const float g, const float b, const float a)
{
	circleRenderer.addCircle(x, y, length, r, g, b, a);
}

void PrimitiveBatch::drawRoundCapLine(const float x1, const float y1, const float x2, const float y2, const float width, const float r, const float g, const float b, const float a)
{
	roundCapLineRenderer.addRoundCapLine(x1, y1, x2, y2, width, r, g, b, a);
}

void PrimitiveBatch::setLineWidth(const float width)
{
	lineWidth = width;
}

PrimitiveBatch::LineRenderer::LineRenderer() :
	vertices(new float[2 * 6 * maxLineNum]), idx(0),
	vertexBuffer(new VertexBuffer(sizeof(float) * 6, sizeof(float) * 6 * 2 * maxLineNum, true, nullptr, nullptr, nullptr))
{
}

PrimitiveBatch::LineRenderer::~LineRenderer()
{
	delete[] vertices;
	delete vertexBuffer;
}

void PrimitiveBatch::LineRenderer::begin()
{
	idx = 0;
}

void PrimitiveBatch::LineRenderer::end(GraphicsContext* const context, ID3D12PipelineState* const pipelineState)
{
	if (idx > 0)
	{
		context->updateBuffer(vertexBuffer, vertices, sizeof(float) * idx);

		context->setPipelineState(pipelineState);

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		context->setVertexBuffers(0, { vertexBuffer });

		context->transitionResources();

		context->draw(idx / 6, 1, 0, 0);
	}
}

void PrimitiveBatch::LineRenderer::addLine(const float x1, const float y1, const float x2, const float y2, const float r, const float g, const float b, const float a)
{
	vertices[idx] = x1;
	vertices[idx + 1] = y1;
	vertices[idx + 2] = r;
	vertices[idx + 3] = g;
	vertices[idx + 4] = b;
	vertices[idx + 5] = a;
	vertices[idx + 6] = x2;
	vertices[idx + 7] = y2;
	vertices[idx + 8] = r;
	vertices[idx + 9] = g;
	vertices[idx + 10] = b;
	vertices[idx + 11] = a;
	idx += 12;
}

PrimitiveBatch::CircleRenderer::CircleRenderer() :
	vertices(new float[7 * maxCircleNum]), idx(0),
	vertexBuffer(new VertexBuffer(sizeof(float) * 7, sizeof(float) * 7 * maxCircleNum, true, nullptr, nullptr, nullptr))
{
}

PrimitiveBatch::CircleRenderer::~CircleRenderer()
{
	delete[] vertices;
	delete vertexBuffer;
}

void PrimitiveBatch::CircleRenderer::begin()
{
	idx = 0;
}

void PrimitiveBatch::CircleRenderer::end(GraphicsContext* const context, ID3D12PipelineState* const pipelineState)
{
	if (idx > 0)
	{
		context->updateBuffer(vertexBuffer, vertices, sizeof(float) * idx);

		context->setPipelineState(pipelineState);

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		context->setVertexBuffers(0, { vertexBuffer });

		context->transitionResources();

		context->draw(128, idx / 7, 0, 0);
	}
}

void PrimitiveBatch::CircleRenderer::addCircle(const float x, const float y, const float length, const float r, const float g, const float b, const float a)
{
	if (length < 1.f)
	{
		return;
	}

	vertices[idx] = x;
	vertices[idx + 1] = y;
	vertices[idx + 2] = length;
	vertices[idx + 3] = r;
	vertices[idx + 4] = g;
	vertices[idx + 5] = b;
	vertices[idx + 6] = a;
	idx += 7;
}

PrimitiveBatch::RCLineRenderer::RCLineRenderer() :
	vertices(new float[7 * 2 * maxLineNum]), idx(0),
	vertexBuffer(new VertexBuffer(sizeof(float) * 7, sizeof(float) * 7 * 2 * maxLineNum, true, nullptr, nullptr, nullptr))
{
}

PrimitiveBatch::RCLineRenderer::~RCLineRenderer()
{
	delete[] vertices;
	delete vertexBuffer;
}

void PrimitiveBatch::RCLineRenderer::begin()
{
	idx = 0;
}

void PrimitiveBatch::RCLineRenderer::end(GraphicsContext* const context, ID3D12PipelineState* const pipelineState)
{
	if (idx > 0)
	{
		context->updateBuffer(vertexBuffer, vertices, sizeof(float) * idx);

		context->setPipelineState(pipelineState);

		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		context->setVertexBuffers(0, { vertexBuffer });

		context->transitionResources();

		context->draw(idx / 7, 1, 0, 0);
	}
}

void PrimitiveBatch::RCLineRenderer::addRoundCapLine(const float x1, const float y1, const float x2, const float y2, const float width, const float r, const float g, const float b, const float a)
{
	vertices[idx] = x1;
	vertices[idx + 1] = y1;
	vertices[idx + 2] = width / 2.f;
	vertices[idx + 3] = r;
	vertices[idx + 4] = g;
	vertices[idx + 5] = b;
	vertices[idx + 6] = a;
	vertices[idx + 7] = x2;
	vertices[idx + 8] = y2;
	vertices[idx + 9] = width / 2.f;
	vertices[idx + 10] = r;
	vertices[idx + 11] = g;
	vertices[idx + 12] = b;
	vertices[idx + 13] = a;

	idx += 14;
}
