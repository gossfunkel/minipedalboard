import numpy as np
#import scipy as sci
from scipy.signal import chirp

TAU = 2 * np.pi

SAMPLE_RATE = 48000

# generate a brief impulse-like ping in the given timbre
def ping(wave:str, length: int = 24000, freq: float = 400, atk=600):
    freq *= length/SAMPLE_RATE                  # scale frequency for sample length
    t: np.array = np.linspace(0,1,length)       # create an array of timepoints
    env: np.array = ad_env(length, atk)              # generate a simple AD envelope
    match wave:
        case "beep":
            signal = gen_beep(t, length, freq)
        case "chord":
            signal = gen_chord(t, length, freq)
        case "diss":
            signal = gen_diss(t, length, freq)
        case "chirp":
            signal = gen_chirp(t, length)
        case "fm":
            signal = gen_fm(t, length//2, freq)
        case "kick":
            signal = gen_chirp(t, length, 300, 40, 'log')
        case "hat":
            signal = gen_noise(length)
        case _:
            print("== Unrecognised wave!")      # throw valueerror?
    return signal * env                         # apply envelope and return
    


def ad_env(length: int, atk: int = 600) -> np.array:
    assert length > 0, f"Envelope must have a positive length!"
    atk: np.array = np.linspace(0,1,atk, dtype=np.float32)                # ascending attack
    dec: np.array = np.linspace(1,0,length - len(atk), dtype=np.float32)  # descending decay
    return np.append(atk, dec*dec)

def gen_beep(t: np.array, length: int, freq: float) -> np.array:
    return np.sin(TAU * freq * t, dtype=np.float32)

def gen_chord(t: np.array, length: int, freq: float) -> np.array:
    wave = np.sin(TAU * freq * t, dtype=np.float32) * .5
    return wave + np.sin(TAU * freq*1.5 * t, dtype=np.float32) * .5

def gen_diss(t: np.array, length: int, freq: float) -> np.array:
    wave = np.sin(TAU * freq * t, dtype=np.float32) * .5
    return wave + np.sin(TAU * (freq+12), dtype=np.float32) * .5

def gen_chirp(t: np.array, length: int, start_freq: int = 40, end_freq: int = 4000, mode='lin') -> np.array:
    duration: float = length / SAMPLE_RATE      # translate length into seconds
    return np.array(chirp(t,                    # return value from scipy.signal.chirp with our parameters
                          f0=start_freq,
                          f1=end_freq, 
                          t1=duration, 
                          method=mode), dtype=np.float32)

def gen_fm(t: np.array, length: int, freq: float, mod_freq: float = 200) -> np.array:
    wave = np.sin(TAU * mod_freq * t, dtype=np.float32) * .5
    return np.sin(TAU * wave * freq * t, dtype=np.float32)

def gen_noise(length: int):
    return np.array(np.random.uniform(-1., 1., length), dtype=np.float32)

def play_continuous_tone():
    task_frame = 0
    while input() != "\n":
        # generate 1s of sine and add to the buffer
        t: np.array = np.linspace(0,1,SAMPLE_RATE)
        sample = gen_beep(t, SAMPLE_RATE, 440)
        stream.write(sample)
        task_frame += 1

def play_looping_kick():
    while input() != "\n":
        stream.write(ping("kick", SAMPLE_RATE))




if __name__ == "__main__":
    print("="*48 + "\n" + "="*20 + " Bleeps " + 20*"=")

    import sounddevice as sd

    device = sd.default.device
    stream = sd.OutputStream(samplerate=SAMPLE_RATE, device=device, channels=1)
    stream.start()

    #play_continuous_tone()
    #play_looping_kick()

    print("== beeping: ")
    stream.write(ping("beep"))

    print("== chord: ")
    stream.write(ping("chord"))

    print("== dissonant: ")
    stream.write(ping("diss", 12000))

    print("== chirping: ")
    stream.write(ping("chirp"))

    print("== blorping: ")
    stream.write(ping("fm"))

    print("= kick: ")
    stream.write(ping("kick", 6000))
    stream.write(np.zeros(6000, dtype=np.float32))
    print("= hat: ")
    stream.write(ping("hat", 6000))
    stream.write(np.zeros(6000, dtype=np.float32))
    print("= kick: ")
    stream.write(ping("kick", 6000))
    stream.write(np.zeros(6000, dtype=np.float32))
    print("= hat: ")
    stream.write(ping("hat", 6000))
    stream.write(np.zeros(6000, dtype=np.float32))
    print("= kick: ")
    stream.write(ping("kick", 6000))
    stream.write(np.zeros(6000, dtype=np.float32))
    print("= hat: ")
    stream.write(ping("hat", 6000))
    stream.write(np.zeros(6000, dtype=np.float32))
    print("= kick: ")
    stream.write(ping("kick", 6000))
    stream.write(np.zeros(6000, dtype=np.float32))
    print("= hat: ")
    stream.write(ping("hat", 6000))
    stream.write(np.zeros(6000, dtype=np.float32))

    stream.stop()
    print("== Stream stopped ")
    stream.close()
    print("== Goodbye!\n" + "="*48)
