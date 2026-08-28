import sounddevice as sd
import numpy as np
import scipy.io.wavfile as sp
import wave
import bleeps

SAMPLE_RATE = 48000

def write_audio_to_wav(filename: str, audio: np.array):
    print("== Writing to .wav file... ")
    sp.write(filename, SAMPLE_RATE, audio)
    print(f"== Completed! File written to {filename}")

def play_audio(samplerate: int, audio: np.array):
    print("== Loading stream...")
    device = sd.default.device
    print(f"== ")
    print(f"== Initialising stream with sample rate of {samplerate} and default device ({device}) set")
    stream = sd.OutputStream(samplerate=samplerate, device=device, channels=1)
    stream.start()
    print("== Stream running ")
    padding = len(audio) % samplerate
    if padding > 0:
        print("== Padding audio to whole buffer size")
        audio = np.append(audio, np.zeros(padding, dtype=np.float32))
    print("== Playing audio: ")
    stream.write(audio)
    print("== Stopping stream")
    stream.stop()
    print("== Stream stopped ")
    stream.close()


if __name__ == "__main__":
    print("="*48 + "\n" + "="*15 + " Soundfile Maker " + 16*"=")

    print("= Creating test tone... ")
    tone = bleeps.ping("chord", 92000)

    print("= Writing tone to wav file: ")
    filename = "test_tone_2.wav"
    write_audio_to_wav(filename,tone)

    print("= Reading tone from file and playing: ")
    play_audio(sp.read(filename))

    print("= Goodbye!\n" + "="*49)
