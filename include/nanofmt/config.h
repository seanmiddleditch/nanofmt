// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#ifndef NANOFMT_CONFIG_H_
#define NANOFMT_CONFIG_H_ 1
#pragma once

#if !defined(NANOFMT_NS)
#    define NANOFMT_NS nanofmt
#endif

// wow this is annoying; see https://github.com/isocpp/CppCoreGuidelines/issues/1173
//
#if defined(__has_cpp_attribute)
#    if __has_cpp_attribute(gsl::suppress)
#        if defined(__clang__)
// clang-format off
#        define NANOFMT_GSL_SUPPRESS(rule) [[gsl::suppress(#rule)]]
// clang-format on
#        endif
#    elif defined(_MSC_VER)
#        define NANOFMT_GSL_SUPPRESS(rule) [[gsl::suppress(rule)]]
#    endif
#endif
#if !defined(NANOFMT_GSL_SUPPRESS)
#    define NANOFMT_GSL_SUPPRESS(rule)
#endif

#endif
