/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  UTF8Utils.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <utility>

namespace utf8
{
//! Returns true if the character is either the only or the first in UTF8 sequence.
template<typename T>
bool IsLeadingCharacter(T character) noexcept
{
   static_assert(sizeof(T) == 1);

   const bool first_bit_set = (character & 0x80) != 0;
   const bool second_bit_set = (character & 0X40) != 0;

   return !first_bit_set || second_bit_set;
}

//! Pops the last UTF32 character from the UTF8 string
template <typename StringType>
StringType PopLastCharacter(StringType str) noexcept
{
   while (!str.empty())
   {
      const auto lastCharacter = str.back();
      str.pop_back();

      if (IsLeadingCharacter(lastCharacter))
         break;
   }

   return std::move(str);
}

//! Return the number of UTF32 characters in the UTF8 string
template <typename StringType>
auto Length(const StringType& str) noexcept
{
   return std::count_if(
      begin(str), end(str), [](auto c) { return IsLeadingCharacter(c); });
}

using SizeType = size_t;
constexpr auto NPOS = std::numeric_limits<SizeType>::max();

template <class T>
constexpr T* ToAddress(T* p) noexcept
{
   static_assert(!std::is_function_v<T>);
   return p;
}

template <class T>
constexpr auto ToAddress(const T& p) noexcept
{
   return ToAddress(p.operator->());
}

//! Returns an UTF8 sub string 
template <typename StringType>
StringType SubString(const StringType& str, SizeType first, SizeType count = NPOS)
{
   if (count == 0)
      return {};

   const auto strBegin = begin(str);
   const auto strEnd = end(str);

   const auto firstIndex = std::find_if(
      strBegin, strEnd,
      [&first](auto c)
      {
         if (IsLeadingCharacter(c))
            return first-- == 0;
         return false;
      });

   if (firstIndex == strEnd)
      return {};

   const auto lastIndex = std::find_if(
      firstIndex, strEnd,
      [&count](auto c)
      {
         if (IsLeadingCharacter(c))
            return count-- == 0;
         return false;
      });

   // C++ 20 is required to construct std::string_view from [first, last)
   return StringType(ToAddress(firstIndex), std::distance(firstIndex, lastIndex));
}

//! Returns an UTF8 sub string starting from the index specified
template <typename StringType>
StringType LeftSubString(const StringType& str, SizeType idx)
{
   return SubString(str, 0, idx);
}

//! Returns an UTF8 sub string ending at the index specified
template <typename StringType>
StringType RightSubString(const StringType& str, SizeType idx)
{
   const auto length = Length(str);

   if (idx > length)
      return {};

   return SubString(str, length - idx);
}
}
