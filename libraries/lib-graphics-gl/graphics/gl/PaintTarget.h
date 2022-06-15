/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  PaintTarget.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "graphics/Point.h"
#include "graphics/Transform.h"
#include "graphics/Color.h"
#include "graphics/Brush.h"

#include "GLFunctions.h"

namespace graphics::gl
{

class GLRenderer;
class Context;
class Framebuffer;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

class Program;
using ProgramPtr = std::shared_ptr<Program>;
class ProgramConstants;
using ProgramConstantsPtr = std::shared_ptr<ProgramConstants>;

class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;

class VertexBuffer;

//! Type of the index
using IndexType = uint16_t;

static constexpr IndexType PrimitiveRestartIndex =
   std::numeric_limits<uint16_t>::max();

//! Type of the vertex
/*!
 * "Shared" type for a single vertex. It is expected that shaders will
 * calculate the color as texture(uv) * mulColor + addColor.
 *
 * This allows to use a single program to handle most of the cases.
 */
struct Vertex final
{
   Point pos;
   PointType<int16_t> uv;

   Color mulColor;
   Color addColor;
};

//! Sink to write the geometry to.
/*!
 * This class is used to improve the rendering performance
 * by generating large batches of data for each draw call.
 *
 * It works in the following way:
 * * If state is different from the previous Append - generates a new batch
 * * Writes transformed to NDC vertices into the vertex buffer
 * * Writes offseted indices into the index buffer
 *
 * If any of the buffers is full or when EndRendering is called -
 * the buffer contents is submitted to the GPU and a set of draw calls
 * is generated, based on batch info, recorded previously
 */
class PaintTarget final
{
public:
   ~PaintTarget();

   //! Appends a set of vertices to the sink
   bool Append(
      GLenum primitiveMode, const Vertex* vertices, size_t vertexCount,
      const IndexType* indices, size_t indexCount);

   //! Sets the current transformation of the sink
   void SetTransform(const Transform& transform);
   //! Sets the current transformation of the sink
   void SetTransform(const FullTransform& transform);

   //! Sets the default shader to be used
   void SetDefaultShader();
   //! Setups the shader for the given brush
   void SetupShadersForBrush(const Brush& brush);

   //! Returns the view port size of the PaintTarget
   Size GetSize() const noexcept;

   //! Sets a custom program to the state
   void SetProgram(const ProgramPtr& program, const ProgramConstantsPtr& constants);

   //! Sets a VertexArrayObject to the state
   void SetVertexArray(const VertexArrayPtr& vertexArray);
   //! Sets a texture to the state
   void SetTexture(const TexturePtr& texture);
   //! Enables clipping
   void EnableClipping(const Rect& rect);
   //! Disables clipping
   void DisableClipping();

private:
   explicit PaintTarget(GLRenderer& renderer, Context& context);

   void BeginRendering(const std::shared_ptr<Framebuffer>& framebuffer);
   void EndRendering();
   void RestartRendering();

   GLRenderer& mRenderer;
   Context& mContext;

   std::shared_ptr<Program> mDefaultProgram;

   class StreamTarget;
   std::vector<std::unique_ptr<StreamTarget>> mStreamTargets;
   size_t mCurrentStreamTargetIndex { 0 };

   // Chain of transformations: model space -> world space -> NDC
   struct VertexTransform final
   {
      Transform mFastTransform;
      FullTransform mFullTransform;

      float mViewportWidth { 0.0f };
      float mViewportHeight { 0.0f };

      bool mIsFullTransform { false };

      void SetTransform(const Transform& transform);
      void SetTransform(const FullTransform& transform);

      Vertex TransformedVertex(Vertex input, float yMult) const noexcept;
   };

   std::shared_ptr<Framebuffer> mFramebuffer;

   VertexTransform mCurrentTransform;

   class GradientBrushesCache;
   std::unique_ptr<GradientBrushesCache> mGradientBrushesCache;

   friend class PaintTargetsStack;
}; // class PaintTarget
} // namespace graphics::gl
