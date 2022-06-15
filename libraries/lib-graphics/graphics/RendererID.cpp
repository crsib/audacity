/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  RendererID.h

  Dmitry Vedenko

**********************************************************************/
#include "RendererID.h"

#include "Renderer.h"

#include <algorithm>
#include <string>
#include <deque>
#include <vector>

namespace graphics
{

namespace
{
const auto independentId = RegisterRenderer(
   RendererPriority::Disabled, "Independent", RendererFactory {});

struct RendererProvider final
{
   RendererProvider(
      RendererPriority _priority, size_t _id, std::string_view _name,
      RendererFactory _factory)
       : name(_name)
       , id(name, _id, _priority)
       , factory(std::move(_factory))
   {
   }

   std::string name;
   RendererID id;
   RendererFactory factory;
};

std::deque<RendererProvider>& GetRendererProvidersList()
{
   static std::deque<RendererProvider> Renderers;
   return Renderers;
}

RendererProvider* GetRendererProvider(std::string_view name)
{
   auto& Renderers = GetRendererProvidersList();

   auto it = std::find_if(
      Renderers.begin(), Renderers.end(),
      [&name](const auto& renderer) { return name == renderer.name; });

   if (it != Renderers.end())
      return &(*it);

   return nullptr;
}
}

RendererID GetRendererIndependentID()
{
   return independentId;
}

RendererID RegisterRenderer(
   RendererPriority priority, std::string_view name, RendererFactory factory)
{
   auto provider = GetRendererProvider(name);

   if (provider != nullptr)
      return provider->id;

   static size_t id = 0;

   auto& Renderers = GetRendererProvidersList();
   Renderers.emplace_back(priority, id++, name, std::move(factory));

   return Renderers.back().id;
}

bool operator==(const RendererID& lhs, const RendererID& rhs) noexcept
{
   return lhs.mID == rhs.mID;
}

bool operator!=(const RendererID& lhs, const RendererID& rhs) noexcept
{
   return !(lhs == rhs);
}

bool RendererID::IsValid() const noexcept
{
   return mID != InvalidRenderer;
}

std::string_view RendererID::GetName() const noexcept
{
   return mName;
}

RendererPriority RendererID::GetPriority() const noexcept
{
   return mPriority;
}

RendererID::RendererID(
   std::string_view name, size_t id, RendererPriority priority)
    : mName(name)
    , mID(id)
    , mPriority(priority)
{
}

RendererID FindRendererID(std::string_view name)
{
   auto provider = GetRendererProvider(name);

   return provider != nullptr ? provider->id : RendererID {};
}

std::unique_ptr<Renderer> CreateBestRenderer()
{
   auto& providers = GetRendererProvidersList();
   const auto renderersCount = providers.size();

   std::vector<size_t> ids;
   ids.reserve(providers.size());

   for (size_t i = 0; i < renderersCount; ++i)
      ids.push_back(i);

   std::sort(ids.begin(), ids.end(), [&providers](size_t lhs, size_t rhs) {
      return providers[lhs].id.GetPriority() < providers[rhs].id.GetPriority();
   });

   for (auto id : ids)
   {
      auto& provider = providers[id];

      if (
         provider.id.GetPriority() == RendererPriority::Disabled ||
         provider.factory == nullptr)
         continue;

      auto renderer = provider.factory();
      if (renderer != nullptr && renderer->IsAvailable())
         return renderer;
   }

   return {};
}

std::unique_ptr<Renderer> CreateRenderer(const RendererID& id)
{
   auto provider = GetRendererProvider(id.GetName());

   if (
      provider == nullptr ||
      provider->id.GetPriority() == RendererPriority::Disabled ||
      provider->factory == nullptr)
      return {};

   auto renderer = provider->factory();

   if (renderer != nullptr && renderer->IsAvailable())
      return renderer;

   return {};
}

} // namespace graphics
