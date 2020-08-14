#ifndef AUDIOPARAMS_H
#define AUDIOPARAMS_H


class AudioParams
{
public:
    AudioParams();

    void SetSampleRate(int rate);
    int GetSampleRate() const;

    void SetFrameSampleRate(int rate);
    int GetFrameSampleRate() const;

    int GetChannelsCount() const;
    int GetBps() const;
    int GetWavFileDataOffset() const;
    int GetDataChunkSize() const;

private:
    int _sampleRate = 48000;
    int _frameSampleRate = 960;
};

#endif // AUDIOPARAMS_H
