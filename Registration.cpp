/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2021 Frank Werner-Krippendorf <mail@hb9fxq.ch>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "SoapySpectranV6.hpp"
#include <SoapySDR/Registry.hpp>

static std::vector<SoapySDR::Kwargs> findSpectranV6(const SoapySDR::Kwargs &args)
{
    SoapySDR_logf(SOAPY_SDR_DEBUG, "SpectranV6 spectranstream lib");

    std::vector<SoapySDR::Kwargs> results;
    SoapySDR::Kwargs soapyInfo;
    soapyInfo["label"] = "SpectranV6 RTSA HTTP Stream";
    soapyInfo["serial"] = "SPECTRAN-V6-00000000000";
    soapyInfo["manufacturer"] = "HB9FXQ";
    soapyInfo["product"] = "SpectranV6 RTSA HTTP Stream";

    results.push_back(soapyInfo);
    return results;
}

static SoapySDR::Device *makeSpectranV6(const SoapySDR::Kwargs &args)
{
    return new SoapySpectranV6(args);
}

static SoapySDR::Registry registerSpectranV6("SpectranV6", &findSpectranV6, &makeSpectranV6, SOAPY_SDR_ABI_VERSION);
