/*****************************************************************//**
 * \file   CPRElevenLabs.cpp
 * \brief  Implementation of Eleven Labs API using C++ Requests and PortAudio
 * 
 * \author 16478
 * \date   November 2023
 *********************************************************************/

#include "CPRElevenLabs.h"

PaStream* stream;

void startStream() {
    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        // Handle error
    }

    // Open an audio I/O stream
    err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 256, nullptr, nullptr);
    if (err != paNoError) {
        // Handle error
    }

    // Start the audio stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        // Handle error
    }
}

void playAudioChunk(const std::string & chunk) {
    // Assuming chunk is raw audio data
    Pa_WriteStream(stream, chunk.data(), chunk.size());
}

// Don't forget to close the stream and terminate PortAudio when done


// Pseudo-code for the audio playback function
void audioPlayback(std::queue<std::string>& buffer, std::mutex& mtx, std::condition_variable& cv, bool& done) {
    while (!done || !buffer.empty()) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return !buffer.empty() || done; });

        if (!buffer.empty()) {
            std::string chunk = buffer.front();
            buffer.pop();

            // Play the chunk using your audio library
            playAudioChunk(chunk);
        }
    }
}

int main() {
    std::string api_key = "YOUR_API_KEY";
    std::string voice_id = "VOICE_ID";
    std::string text = "Your text to convert to speech";

    std::queue<std::string> buffer;
    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;

    // Start the audio playback thread
    std::thread playbackThread(audioPlayback, std::ref(buffer), std::ref(mtx), std::ref(cv), std::ref(done));

    // Lambda function to handle streaming response
    auto responseHandler = [&](cpr::Response r) {
        std::lock_guard<std::mutex> lock(mtx);
        buffer.push(r.text); // Assuming each response chunk is a string
        cv.notify_one();
        };

    cpr::Response r = cpr::Post(
        cpr::Url{ "https://api.elevenlabs.io/v1/text-to-speech/" + voice_id + "/stream" },
        cpr::Header{ {"xi-api-key", api_key} },
        cpr::Header{ {"Content-Type", "application/json"} },
        cpr::Header{ {"Accept", "audio/mpeg"} },
        cpr::Body{ text },
        cpr::Parameter{ "output_format", "mp3_44100_128" },
    );

    // Signal the playback thread that streaming is done
    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
        cv.notify_one();
    }

    // Wait for the playback thread to finish
    playbackThread.join();

    return 0;
}
