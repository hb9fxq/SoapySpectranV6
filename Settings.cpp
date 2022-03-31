/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Frank Werner-Krippendorf <mail@hb9fxq.ch>

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

SoapySpectranV6::SoapySpectranV6(const SoapySDR::Kwargs &args)
{

    if (args.count("endpoint") == 1)
    {
        m_endpoint = args.at("endpoint").c_str();
    }

    if (args.count("fcoffset") == 1)
    {
        m_fc_offset = std::stod(args.at("fcoffset"));
    }
}

SoapySpectranV6::~SoapySpectranV6(void)
{
    //TODO: stop stream
}

/*******************************************************************
 * Identification API
 ******************************************************************/

std::string SoapySpectranV6::getDriverKey(void) const
{
    return "SpectranV6";
}

std::string SoapySpectranV6::getHardwareKey(void) const
{
    return "SpectranV6";
}

SoapySDR::Kwargs SoapySpectranV6::getHardwareInfo(void) const
{
    SoapySDR::Kwargs args;
    args["serial"] = "SPECTRAN-V6-00000000000";
    return args;
}

/*******************************************************************
 * Channels API
 ******************************************************************/

size_t SoapySpectranV6::getNumChannels(const int dir) const
{
    return (dir == SOAPY_SDR_RX) ? 1 : 0;
}

/*******************************************************************
 * Antenna API
 ******************************************************************/

std::vector<std::string> SoapySpectranV6::listAntennas(const int direction, const size_t channel) const
{
    std::vector<std::string> antennas;
    antennas.push_back("RX");
    return antennas;
}

void SoapySpectranV6::setAntenna(const int direction, const size_t channel, const std::string &name)
{
    // TODO: Select IQ Demodulator block via setAntenna
}

std::string SoapySpectranV6::getAntenna(const int direction, const size_t channel) const
{
    return "RX";
}

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/

bool SoapySpectranV6::hasDCOffsetMode(const int direction, const size_t channel) const
{
    return false; //TODO: center offset via DCOffset
}

/*******************************************************************
 * Gain API
 ******************************************************************/

std::vector<std::string> SoapySpectranV6::listGains(const int direction, const size_t channel) const
{
    std::vector<std::string> results;
    results.push_back("ReferenceLevel"); //TODO
    return results;
}

bool SoapySpectranV6::hasGainMode(const int direction, const size_t channel) const
{
    return true;
}

void SoapySpectranV6::setGainMode(const int direction, const size_t channel, const bool automatic)
{

    SoapySDR_logf(SOAPY_SDR_DEBUG, "Setting AGC: %s", automatic ? "Automatic" : "Manual");
}

bool SoapySpectranV6::getGainMode(const int direction, const size_t channel) const
{
    return true;
}

void SoapySpectranV6::setGain(const int direction, const size_t channel, const double value)
{
    SoapySDR::Device::setGain(direction, channel, value);
}

void SoapySpectranV6::setGain(const int direction, const size_t channel, const std::string &name, const double value)
{
    if (name == "ReferenceLevel")
    {
        m_lnagain = value;
    }
}

double SoapySpectranV6::getGain(const int direction, const size_t channel, const std::string &name) const
{
    if (name == "ReferenceLevel")
    {
        return m_lnagain;
    }

    return 0;
}

SoapySDR::Range SoapySpectranV6::getGainRange(const int direction, const size_t channel, const std::string &name) const
{
    if (name == "ReferenceLevel")
    {
        return SoapySDR::Range(-60, 0);
    }

    return SoapySDR::Range(0, 15);
}

/*******************************************************************
 * Frequency API
 ******************************************************************/

void SoapySpectranV6::setFrequency(
    const int direction,
    const size_t channel,
    const std::string &name,
    const double frequency,
    const SoapySDR::Kwargs &args)
{

    if (name == "RF")
    {
        SoapySDR_log(SOAPY_SDR_INFO, "setFrequency RF");
        m_fc = frequency;
        updateDemod();
    }
}

double SoapySpectranV6::getFrequency(const int direction, const size_t channel, const std::string &name) const
{
    if (name == "RF")
    {
        return m_fc;
    }

    return 0;
}

std::vector<std::string> SoapySpectranV6::listFrequencies(const int direction, const size_t channel) const
{
    std::vector<std::string> names;
    names.push_back("RF");
    return names;
}

SoapySDR::RangeList SoapySpectranV6::getFrequencyRange(
    const int direction,
    const size_t channel,
    const std::string &name) const
{
    SoapySDR::RangeList results;
    if (name == "RF")
    {
        results.push_back(SoapySDR::Range(1000000, 8000000000));
    }
    return results;
}

SoapySDR::ArgInfoList SoapySpectranV6::getFrequencyArgsInfo(const int direction, const size_t channel) const
{
    SoapySDR::ArgInfoList freqArgs;
    return freqArgs;
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/

void SoapySpectranV6::setSampleRate(const int direction, const size_t channel, const double rate)
{
    SoapySDR_logf(SOAPY_SDR_DEBUG, "setSampleRate: %d", rate);

    m_sample_rate = rate;
    updateDemod();
}

double SoapySpectranV6::getSampleRate(const int direction, const size_t channel) const
{
    return m_sample_rate;
}

std::vector<double> SoapySpectranV6::listSampleRates(const int direction, const size_t channel) const
{
    std::vector<double> results;

    results.push_back(1e6);
    results.push_back(2e6);
    results.push_back(2.048e6);
    results.push_back(3e6);
    results.push_back(4e6);
    results.push_back(5e6);
    results.push_back(6e6);
    results.push_back(7e6);
    results.push_back(8e6);
    results.push_back(9e6);
    results.push_back(10e6);
    results.push_back(11e6);
    results.push_back(12e6);
    results.push_back(13e6);
    results.push_back(14e6);
    results.push_back(92.16e6);
    results.push_back(122.88e6);
    results.push_back(184.32e6);
    results.push_back(245.76e6);


    return results;
}

void SoapySpectranV6::setBandwidth(const int direction, const size_t channel, const double bw)
{
    SoapySDR::Device::setBandwidth(direction, channel, bw);
}

double SoapySpectranV6::getBandwidth(const int direction, const size_t channel) const
{
    return SoapySDR::Device::getBandwidth(direction, channel);
}

std::vector<double> SoapySpectranV6::listBandwidths(const int direction, const size_t channel) const
{
    std::vector<double> results;
    return results;
}

/*******************************************************************
 * Settings API
 ******************************************************************/

SoapySDR::ArgInfoList SoapySpectranV6::getSettingInfo(void) const
{
    SoapySDR::ArgInfoList setArgs;
    return setArgs;
}

void SoapySpectranV6::writeSetting(const std::string &key, const std::string &value)
{
}

std::string SoapySpectranV6::readSetting(const std::string &key) const
{
    return "";
}
