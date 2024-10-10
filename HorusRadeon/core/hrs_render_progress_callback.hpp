#pragma once
#include <atomic>

class RenderProgressCallback {
public:
    struct Update {
        std::atomic<int> HasUpdate{ 0 };
        std::atomic<int> Done{ 0 };
        std::atomic<int> Aborted{ 0 };
        bool RenderReady = false;
        int CamUpdated = 0;
        float Progress = 0.0f;

        void Clear();
    };

    static void NotifyUpdate(float progress, void* userData);
};
