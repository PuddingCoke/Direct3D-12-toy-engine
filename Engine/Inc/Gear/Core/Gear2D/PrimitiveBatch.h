#pragma once

#ifndef _PRIMITIVEBATCH_H_
#define _PRIMITIVEBATCH_H_

#include<Gear/Core/ResourceManager.h>

#include<Gear/Core/Shader.h>

//due to limitations
//create multiple instances for multiple render targets
class PrimitiveBatch
{
public:

	PrimitiveBatch() = delete;

	PrimitiveBatch(const PrimitiveBatch&) = delete;

	void operator=(const PrimitiveBatch&) = delete;

	//render target format
	PrimitiveBatch(const DXGI_FORMAT format, GraphicsContext* const context);

	~PrimitiveBatch();

	//call this once frame
	void begin();

	//call this once frame
	void end();

	void drawLine(const float x1, const float y1, const float x2, const float y2, const float r, const float g, const float b, const float a = 1.f);

	void drawCircle(const float x, const float y, const float length, const float r, const float g, const float b, const float a = 1.f);

	void drawRoundCapLine(const float x1, const float y1, const float x2, const float y2, const float width, const float r, const float g, const float b, const float a = 1.f);

	void setLineWidth(const float width);

private:

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

		BufferView* vertexBuffer;

		float* vertices;

		int32_t idx;

		static constexpr int32_t maxLineNum = 150000;

	} lineRenderer;

	class CircleRenderer
	{
	public:

		CircleRenderer();

		~CircleRenderer();

		void begin();

		void end(GraphicsContext* const context, ID3D12PipelineState* const pipelineState);

		void addCircle(const float x, const float y, const float length, const float r, const float g, const float b, const float a);

		BufferView* vertexBuffer;

		float* vertices;

		int32_t idx;

		static constexpr int32_t maxCircleNum = 50000;

	} circleRenderer;

	class RCLineRenderer
	{
	public:

		RCLineRenderer();

		~RCLineRenderer();

		void begin();

		void end(GraphicsContext* const context, ID3D12PipelineState* const pipelineState);

		void addRoundCapLine(const float x1, const float y1, const float x2, const float y2, const float width, const float r, const float g, const float b, const float a);

		BufferView* vertexBuffer;

		float* vertices;

		int32_t idx;

		static constexpr int32_t maxLineNum = 150000;

	} roundCapLineRenderer;

};

#endif // !_PRIMITIVEBATCH_H_