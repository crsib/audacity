/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  RendererID.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <string_view>

namespace graphics
{
class Renderer;

//! Priority of the renderer. Used to determine which renderer to use if no preference is specified
enum class RendererPriority
{
   Preferred,
   PreferredFallback,
   Fallback,
   Disabled
};

//! Identifier of the renderer
class GRAPHICS_API RendererID final
{
public:
   RendererID() = default;
   RendererID(const RendererID&) = default;
   RendererID(RendererID&&) = default;
   RendererID& operator=(const RendererID&) = default;
   RendererID& operator=(RendererID&&) = default;
   RendererID(std::string_view name, size_t id, RendererPriority priority);
   
   bool IsValid() const noexcept;
   
   std::string_view GetName() const noexcept;

   RendererPriority GetPriority() const noexcept;

   GRAPHICS_API friend bool
   operator==(const RendererID& lhs, const RendererID& rhs) noexcept;

   GRAPHICS_API friend bool
   operator!=(const RendererID& lhs, const RendererID& rhs) noexcept;

private:
   static constexpr size_t InvalidRenderer = std::numeric_limits<size_t>::max();

   std::string_view mName;
   size_t mID { InvalidRenderer };

   RendererPriority mPriority { RendererPriority::Fallback };
};

//! A factory function for creating renderer
using RendererFactory = std::function<std::unique_ptr<Renderer>()>;

//! Gets and ID used for renderer-independent resources
GRAPHICS_API RendererID GetRendererIndependentID();

//! Registers a new renderer
GRAPHICS_API RendererID RegisterRenderer(
   RendererPriority priority, std::string_view name, RendererFactory factory);

//! Finds RendererID by name
GRAPHICS_API RendererID FindRendererID(std::string_view name);

//! Creates the best possible renderer
GRAPHICS_API std::unique_ptr<Renderer> CreateBestRenderer();

//! Tries to create renderer with given ID. Returns nullptr if renderer can't be used.
GRAPHICS_API std::unique_ptr<Renderer> CreateRenderer(const RendererID& id);

} // namespace graphics
