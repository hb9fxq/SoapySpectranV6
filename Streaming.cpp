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

#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Formats.hpp>


std::vector<std::string> SoapySpectranV6::getStreamFormats(const int direction, const size_t channel) const {
    std::vector<std::string> formats;

    formats.push_back(SOAPY_SDR_CS16);
    formats.push_back(SOAPY_SDR_CF32);

    return formats;
}

std::string SoapySpectranV6::getNativeStreamFormat(const int direction, const size_t channel, double &fullScale) const {
     fullScale = 32767.0;
     return SOAPY_SDR_CS16;
}

SoapySDR::ArgInfoList SoapySpectranV6::getStreamArgsInfo(const int direction, const size_t channel) const {
    SoapySDR::ArgInfoList streamArgs;

    SoapySDR::ArgInfo endpointArg;
    endpointArg.key = "endpoint";
    endpointArg.value = "127.0.0.1:54664";
    endpointArg.name = "Spectran HTTP Endpoint";
    endpointArg.description = "Spectran HTTP Endpoint.";
    endpointArg.units = "uri";
    endpointArg.type = SoapySDR::ArgInfo::STRING;
    streamArgs.push_back(endpointArg);

    SoapySDR::ArgInfo fcOffsetArg;
    fcOffsetArg.key = "fcoffset";
    fcOffsetArg.value = "-5e6";
    fcOffsetArg.name = "fC Offset Hz";
    fcOffsetArg.description = "fC Offset Hz";
    fcOffsetArg.units = "Hz";
    fcOffsetArg.type = SoapySDR::ArgInfo::FLOAT;
    streamArgs.push_back(fcOffsetArg);

    SoapySDR::ArgInfo demodBlockArg;
    demodBlockArg.key = "demodblock";
    demodBlockArg.value = "Block_IQDemodulator_1";
    demodBlockArg.name = "DEMODBLOCK";
    demodBlockArg.description = "DEMODBLOCK API NAME";
    demodBlockArg.units = "rtsastring";
    demodBlockArg.type = SoapySDR::ArgInfo::STRING;
    streamArgs.push_back(demodBlockArg);

    SoapySDR::ArgInfo spectranBlockArg;
    spectranBlockArg.key = "spectranblock";
    spectranBlockArg.value = "Block_Spectran_V6B_1";
    spectranBlockArg.name = "SPECTRANBLOCK";
    spectranBlockArg.description = "SPECTRANBLOCK API NAME";
    spectranBlockArg.units = "rtsastring";
    spectranBlockArg.type = SoapySDR::ArgInfo::STRING;
    streamArgs.push_back(spectranBlockArg);




    return streamArgs;
}


/*******************************************************************
 * Stream API
 ******************************************************************/

SoapySDR::Stream *SoapySpectranV6::setupStream(
        const int direction,
        const std::string &format,
        const std::vector<size_t> &channels,
        const SoapySDR::Kwargs &args)
{
    if (format == SOAPY_SDR_CF32) {
        SoapySDR_log(SOAPY_SDR_INFO, "Using format CF32.");
        m_streamer_type = spectran_stream::QUEUED_CF32;
    } else if (format == SOAPY_SDR_CS16) {
        SoapySDR_log(SOAPY_SDR_INFO, "Using format CS16.");
        m_streamer_type = spectran_stream::QUEUED_INT16;
    } else {
        throw std::runtime_error(
                "setupStream invalid format '" + format
                        + "' -- Only CS16 and CF32 are supported by SoapySpectranV6 module.");
    }

    

    SoapySDR_log(SOAPY_SDR_INFO, "setupStream, endpoint: ");
    SoapySDR_log(SOAPY_SDR_INFO, m_endpoint.c_str());

    dev = new spectran_stream(m_streamer_type, m_endpoint);

    return (SoapySDR::Stream *) this;
}

void SoapySpectranV6::closeStream(SoapySDR::Stream *stream)
{
   
}

/*size_t SoapySpectranV6::getStreamMTU(SoapySDR::Stream *stream) const
{
    return bufferLength;
}*/

int SoapySpectranV6::activateStream(
        SoapySDR::Stream *stream,
        const int flags,
        const long long timeNs,
        const size_t numElems)
{
    if (flags != 0) {
        return SOAPY_SDR_NOT_SUPPORTED;
    }
    SoapySDR_log(SOAPY_SDR_INFO, "activateStream");


 
    if(!m_streaming_started){
        dev->StartStreamingThread();
        m_streaming_started = true;
    }

    updateDemod();        

    return 0;
}

int SoapySpectranV6::deactivateStream(SoapySDR::Stream *stream, const int flags, const long long timeNs)
{
    SoapySDR_log(SOAPY_SDR_INFO, "deactivateStream");
    if (flags != 0) return SOAPY_SDR_NOT_SUPPORTED;
   
    return 0;
}

int SoapySpectranV6::readStream(
        SoapySDR::Stream *stream,
        void * const *buffs,
        const size_t numElems,
        int &flags,
        long long &timeNs,
        const long timeoutUs)
{    
    //result buffer
    unsigned char *buff0 = reinterpret_cast<unsigned char*>(buffs[0]);

    dev->GetSamples(numElems, buff0);
    
   
    //std::memcpy(buff0, _currentBuff, returnedElems * bytesPerSample);
    return numElems;
}

void SoapySpectranV6::updateDemod(){
    if(m_streaming_started){
    dev->UpdateDemodulator(m_demodblock, m_spectranbloc ,m_fc, m_fc_offset, m_sample_rate);
    }
}
