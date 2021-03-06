/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#pragma once

#include <algorithm>
#include <optional>
#include <set>
#include <vector>

#include "common/common_types.h"
#include "frontend/decoder/decoder_detail.h"
#include "frontend/decoder/matcher.h"

namespace Dynarmic::A32 {

template <typename Visitor>
using Thumb32Matcher = Decoder::Matcher<Visitor, u32>;

template <typename V>
std::vector<Thumb32Matcher<V>> GetThumb32DecodeTable() {
    std::vector<Thumb32Matcher<V>> table = {

#define INST(fn, name, bitstring) Decoder::detail::detail<Thumb32Matcher<V>>::GetMatcher(&V::fn, name, bitstring),
#include "thumb32.inc"
#undef INST

    };

    // If a matcher has more bits in its mask it is more specific, so it should come first.
    std::stable_sort(table.begin(), table.end(), [](const auto& matcher1, const auto& matcher2) {
        return Common::BitCount(matcher1.GetMask()) > Common::BitCount(matcher2.GetMask());
    });

    // Exceptions to the above rule of thumb.
    const std::set<std::string> comes_first {
            "LDR (lit)",
    };

    std::stable_partition(table.begin(), table.end(), [&](const auto& matcher) {
        return comes_first.count(matcher.GetName()) > 0;
    });

    return table;
}

template<typename V>
std::optional<std::reference_wrapper<const Thumb32Matcher<V>>> DecodeThumb32(u32 instruction) {
    static const auto table = GetThumb32DecodeTable<V>();

    const auto matches_instruction = [instruction](const auto& matcher) { return matcher.Matches(instruction); };

    auto iter = std::find_if(table.begin(), table.end(), matches_instruction);
    return iter != table.end() ? std::optional<std::reference_wrapper<const Thumb32Matcher<V>>>(*iter) : std::nullopt;
}

} // namespace Dynarmic::A32
