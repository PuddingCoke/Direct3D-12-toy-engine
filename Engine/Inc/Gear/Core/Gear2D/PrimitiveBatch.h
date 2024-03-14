#pragma once

#ifndef _PRIMITIVEBATCH_H_
#define _PRIMITIVEBATCH_H_

#include<Gear/Core/GraphicsContext.h>
#include<Gear/Core/Shader.h>
#include<Gear/Core/States.h>

#include<Gear/CompiledShaders/PrimitiveBatchLineVS.h>
#include<Gear/CompiledShaders/PrimitiveBatchCircleVS.h>
#include<Gear/CompiledShaders/PrimitiveBatchRCLineVS.h>

#include<Gear/CompiledShaders/PrimitiveBatchRCLineGS.h>
#include<Gear/CompiledShaders/PrimitiveBatchLineGS.h>

#include<Gear/CompiledShaders/PrimitiveBatchPS.h>

class PrimitiveBatch
{
public:

	static PrimitiveBatch* create(const DXGI_FORMAT format, GraphicsContext* const context);

	static PrimitiveBatch* get();

	PrimitiveBatch(const PrimitiveBatch&) = delete;

	void operator=(const PrimitiveBatch&) = delete;

	~PrimitiveBatch();

	void begin();

	void end();

	void drawLine(const float x1, const float y1, const float x2, const float y2, const float r, const float g, const float b, const float a = 1.f);

	void drawCircle(const float x, const float y, const float length, const float r, const float g, const float b, const float a = 1.f);

	void drawRoundCapLine(const float x1, const float y1, const float x2, const float y2, const float width, const float r, const float g, const float b, const float a = 1.f);

	void setLineWidth(const float width);

private:

	static PrimitiveBatch* instance;

	PrimitiveBatch(const DXGI_FORMAT format, GraphicsContext* const context);

	float lineWidth;

	Shader* lineVS;

	Shader* circleVS;

	Shader* rcLineVS;

	Shader* lineGS;

	Shader* rcLineGS;

	Shader* primitivePS;

	ComPtr<ID3D12PipelineState> lineState;

	ComPtr<ID3D12PipelineState> circleState;

	ComPtr<ID3D12PipelineState> roundCapLineState;

	GraphicsContext* context;

	class LineRenderer
	{
	public:

		LineRenderer();

		~LineRenderer();

		void begin();

		void end(GraphicsContext* const context, ID3D12PipelineState* const pipelineState);

		void addLine(const float x1, const float y1, const float x2, const float y2, const float r, const float g, const float b, const float a);

		VertexBuffer* vertexBuffer;

		float* vertices;

		int idx;

		static constexpr int maxLineNum = 150000;

	} lineRenderer;

	class CircleRenderer
	{
	public:

		CircleRenderer();

		~CircleRenderer();

		void begin();

		void end(GraphicsContext* const context, ID3D12PipelineState* const pipelineState);

		void addCircle(const float x, const float y, const float length, const float r, const float g, const float b, const float a);

		VertexBuffer* vertexBuffer;

		float* vertices;

		static constexpr int maxCircleNum = 50000;

		int idx;

	}circleRenderer;

	class RCLineRenderer
	{
	public:

		RCLineRenderer();

		~RCLineRenderer();

		void begin();

		void end(GraphicsContext* const context, ID3D12PipelineState* const pipelineState);

		void addRoundCapLine(const float x1, const float y1, const float x2, const float y2, const float width, const float r, const float g, const float b, const float a);

		VertexBuffer* vertexBuffer;

		float* vertices;

		int idx;

		static constexpr int maxLineNum = 150000;

	} roundCapLineRenderer;

};

#endif // !_PRIMITIVEBATCH_H_
