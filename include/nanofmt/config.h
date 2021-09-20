// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#if !defined(NANOFMT_NS)
#    define NANOFMT_NS nanofmt
#endif

#if !defined(__has_cpp_attribute)
#    define __has_cpp_attribute(x) 0
#endif
#if !defined(__has_builtin)
#    define __has_builtin(x) 0
#endif

#if defined(__clang__) || defined(__GNUC__)
#    undef NANOFMT_CLANG_OR_GCC 1
#    define NANOFMT_CLANG_OR_GCC 1
#endif

// wow this is annoying; see https://github.com/isocpp/CppCoreGuidelines/issues/1173
//
#if !defined(NANOFMT_GSL_SUPPRESS)
#    if __has_cpp_attribute(gsl::suppress)
#        if defined(__clang__)
// clang-format off
#        define NANOFMT_GSL_SUPPRESS(rule) [[gsl::suppress(#rule)]]
// clang-format on
#        elif defined(_MSC_VER)
#            define NANOFMT_GSL_SUPPRESS(rule) [[gsl::suppress(rule)]]
#        else
#            define NANOFMT_GSL_SUPPRESS(rule)
#        endif
#    else
#        define NANOFMT_GSL_SUPPRESS(rule)
#    endif
#endif

#if __has_builtin(__builtin_clz)
#    undef NANOFMT_HAS_BUILTIN_CLZ
#    define NANOFMT_HAS_BUILTIN_CLZ 1
#endif

#if defined(_WIN32)
#    undef NANOFMT_HAS_BSR
#    define NANOFMT_HAS_BSR 1
#endif

#if !defined(NANOFMT_IF_CONSTEXPR)
#    if __cpp_if_constexpr
#        define NANOFMT_IF_CONSTEXPR 1
#    endif
#endif

#if !defined(NANOFMT_HAS_FLOAT) && NANOFMT_IF_CONSTEXPR
#    define NAONFMT_HAS_FLOAT 1
#endif
