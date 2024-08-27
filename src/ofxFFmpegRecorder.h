#pragma once
// openFrameworks
#include "ofTypes.h"
#include "ofVideoBaseTypes.h"
#include "ofRectangle.h"
#include "ofPixels.h"

#include <thread>
#include <list>

using HighResClock = std::chrono::time_point<std::chrono::high_resolution_clock>;

template <typename T>
struct LockFreeQueue {
    LockFreeQueue() {
        m_List.push_back(T());
        m_HeadIt = m_List.begin();
        m_TailIt = m_List.end();
    }

    void produce(const T &t) {
        m_List.push_back(t);
        m_TailIt = m_List.end();
        m_List.erase(m_List.begin(), m_HeadIt);
    }

    bool consume(T &t) {
        typename TList::iterator nextIt = m_HeadIt;
        ++nextIt;
        if (nextIt != m_TailIt) {
            m_HeadIt = nextIt;
            t = *m_HeadIt;
            return true;
        }

        return false;
    }

    int size() const {
        return std::distance(m_HeadIt, m_TailIt) - 1;
    }

    typename std::list<T>::iterator getHead() const {
        return m_HeadIt;
    }

    typename std::list<T>::iterator getTail() const {
        return m_TailIt;
    }

private:
    using TList = std::list<T>;
    TList m_List;
    typename TList::iterator m_HeadIt, m_TailIt;
};

class ofxFFmpegRecorder {
public:
    ofxFFmpegRecorder();
    ~ofxFFmpegRecorder();

    void setup(bool recordVideo, glm::vec2 videoSize = glm::vec2(0,0), float fps = 30.f, unsigned int bitrate = 2000, const std::string &ffmpegPath = "");

    bool isRecordVideo() const;
    void setRecordVideo(bool record);

    std::string getFFmpegPath() const;
    void setFFmpegPath(const std::string &path);
    void setFFmpegPathToAddonsPath();

    float getCaptureDuration() const;
    void setCaptureDuration(float duration);

    ofVideoDevice getDefaultVideoDevice() const;
    void setDefaultVideoDevice(const ofVideoDevice &device);

    std::string getOutputPath() const;
    void setOutputPath(const std::string &path);

    float getFps() const;
    void setFps(float fps);

    unsigned int getBitRate() const;
    void setBitRate(unsigned int rate);

    std::string getVideoCodec() const;
    void setVideoCodec(const std::string &codec);

    float getWidth();
    void setWidth(float aw);
    float getHeight();
    void setHeight(float ah);

    bool isPaused() const;
    void setPaused(bool paused);

    void setInputPixelFormat(ofImageType aType);
    void setOutputPixelFormat(ofImageType aType);

    float getRecordedDuration() const;

    bool record(float duration = 0);
    bool startCustomRecord();
    size_t addFrame(const ofPixels &pixels);

    void stop();
    void cancel();

    bool isOverWrite() const;
    void setOverWrite(bool overwrite);

    const std::vector<std::string> &getAdditionalInputArguments() const;
    void setAdditionalInputArguments(const std::vector<std::string> &args);
    void addAdditionalInputArgument(const std::string &arg);
    void clearAdditionalInputArguments();

    const std::vector<std::string> &getAdditionalOutputArguments() const;
    void setAdditionalOutputArguments(const std::vector<std::string> &args);
    void addAdditionalOutputArgument(const std::string &arg);
    void clearAdditionalOutputArguments();

    void clearAdditionalArguments();

    bool isRecording() const;
    bool isRecordingCustom() const;
    bool isRecordingDefault() const;

    void saveThumbnail(const unsigned int &hour, const unsigned int &minute, const float &second, const std::string &output, glm::vec2 size = glm::vec2(0, 0),
                       ofRectangle crop = ofRectangle(0, 0, 0, 0), std::string videoFilePath = "");

private:
    std::string m_FFmpegPath, m_OutputPath;
    bool m_IsRecordVideo;

    bool m_IsOverWrite;
    bool m_IsPaused;

    glm::vec2 m_VideoSize;
    unsigned int m_BitRate, m_AddedVideoFrames;

    float m_Fps, m_CaptureDuration, m_TotalPauseDuration;

    ofVideoDevice m_DefaultVideoDevice;

    std::string m_VideCodec;
    FILE *m_CustomRecordingFile, *m_DefaultRecordingFile;

    HighResClock m_RecordStartTime;
    HighResClock m_PauseStartTime, m_PauseEndTime;

    std::vector<std::string> m_AdditionalInputArguments, m_AdditionalOutputArguments;

    std::thread m_Thread;
    LockFreeQueue<ofPixels *> m_Frames;

    std::string mInputPixFmt = "rgb24";
    std::string mOutputPixFmt = "rgb24";

    bool mBStopRequested = false;

private:
    void determineDefaultDevices();
    void processFrame();
    void joinThread();
};
