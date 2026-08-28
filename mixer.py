from direct.showbase import DirectObject
from direct.showbase.ShowBase import ShowBase
from panda3d.core import (
    load_prc_file_data, PandaNode
)
import numpy as np
import bleeps
import soundfile_maker as sfile
import sounddevice as sd
import time
import copy

SAMPLE_RATE = 48000

P3D_CONFIG: str = """
window-type none
"""

# write an audio sample to the buffer (use `amp` to dampen inputs to prevent clipping)
class AudioWriter(DirectObject.DirectObject):
    def __init__(self, sequencer, sample: np.array, offset: int = 0, amp: float = .8):
        self.sequencer = sequencer                                  # save ref to sequencer
        self.sample = sample * amp                                  # scale sample by amplitude
        self.offset = offset                                        # offset from BEAT

        def _write(beats) -> np.array:
            print("writing samp")
            for beat_offset in beats[0]:
                self.sequencer.write(self.sample, int(beat_offset+self.offset))
        
        self.accept("buffer_ready", _write)                         # call _write when messenger says buffer is ready


class Sequencer:
    @staticmethod
    def list_sd_devices():                                          # call for list of available devices
        print(sd.query_devices())

    def __init__(self, bpm, channels = 1):
        self.bpm: int      = bpm                                    # set sequencer rate (beats per minute)

        # discretise time into samples
        self.bps: int    = self.bpm // 60.                          # beats per second
        self.spb: int    = int(SAMPLE_RATE // self.bps)             # beat length in samples (beat stride)

        self.buffer_len = self.spb*4                                     # size of pre-buffer (4 beats)
        self.buffer = np.zeros((self.buffer_len, 1))                     # larger pre-buffer for loading samples into
        self.buffer_step = 0                                        # track step through pre-buffer

        assert (channels > 0) and (channels < 3), f"!!! only mono or stereo pls !!!"
        device = sd.default.device

        # callback function to load pre-buffer into audio stream
        def _callback(output_data: np.ndarray,                      # one channel per column: shape (frames, channels)
                     frames: int,                                   # length of stream output buffer
                     time,                                          # DAC output time of first sample in buffer, with time callback invoked
                     status: sd.CallbackFlags) -> None:             # warnings for buffer adding or dropping
            # if self.prev_callback_time is None:                     # initialise callback time
            #     self.prev_callback_time = self.stream.time
            #     elapsed = 0.
            # else:                                                   # elapsed is time since last callback
            #     elapsed = self.stream.time - self.prev_callback_time
            #     self.prev_callback_time += elapsed

            # calculate current timestep for each frame in window
            #t = (self.start_idx + np.arange(frames)) / self.samplerate

            output_data[:] = self.buffer[self.buffer_step:self.buffer_step+frames]
            self.buffer_step += frames                              # move writehead on by a frame of samplerate
            if self.buffer_step >= self.buffer_len:                 # when the step takes us outside the range of the buffer:
                self._flush_buffer()                                #   flush the buffer when last frame of buffer is reached
            elif self.buffer_step+SAMPLE_RATE >= self.buffer_len:   # if we're near the end of the buffer; copy, flush, and paste
                self._flush_buffer(1)

        print(f"== Initialising stream ====" 
              f"\n\t| sample rate : {SAMPLE_RATE}"
              f"\n\t| channels : {"mono" if channels == 1 else "stereo"}"
              f"\n\t| default device : {device}")
        self.stream = sd.OutputStream(samplerate=SAMPLE_RATE, device=device, channels=channels, callback=_callback)
        self.stream.start()
        print("== Sequencer audio stream running. ")

    def __del__(self):
        self.stream.stop()
        self.stream.close()

    def _flush_buffer(self, overflow=0):
        if overflow:                                                # copy if overflowing
            data_copy = copy.deepcopy(self.buffer[self.buffer_step:])
        self.buffer = np.zeros((self.buffer_len, 1))                # wipe the buffer
        if overflow:
            self.buffer[:len(data_copy)] = data_copy                # paste if overflowing
            buffer_len = self.buffer_len - len(data_copy)
        else:
            buffer_len = self.buffer_len
        self.buffer_step = 0                                        # reset writehead to start of buffer
        num_beats = buffer_len//self.spb                            # calculate beats in new buffer
        beats = []
        for i in range(num_beats):                                  # write sample index of beat to list
            beats.append((i*self.buffer) // buffer_len)
        messenger.send("buffer_ready", beats)

    def write(self, sample, offset:int=0):
        self.buffer[offset:offset+len(sample)] += sample[:].reshape(len(sample),1)

    def beats_to_offset(self, offset_beat_len: float):
        return int(offset_beat_len * self.spb)


if __name__ == '__main__':
    print("="*48 + "\n" + "="*20 + " Mixer " + 21*"=" + "\n==")

    print("== Initialising ShowBase...")
    load_prc_file_data('',P3D_CONFIG)
    ShowBase()
    
    print("== Creating sequencer...")
    sequencer = Sequencer(180)                                 # 180bpm = 3 beats per second

    print("== Adding some sounds...")
    writers = []
    bleep = bleeps.ping("beep")
    writers.append(AudioWriter(sequencer, bleep))
    #writers.append(AudioWriter(sequencer, bleep, 30000))
    writers.append(AudioWriter(sequencer, bleeps.ping("kick", 800)))
    writers.append(AudioWriter(sequencer, bleeps.ping("hat"), sequencer.beats_to_offset(.5)))
    print("\n======= Running: =======")

    #base.accept("esc", base.user_exit)

    base.run()

    print("== Goodbye!\n" + "="*48)

# =========================== IDEA 2 ==========================================================================

class AudioRenderer:
    def __init__(self, channels: int = 1):
        assert channels == 1, f"currently only mono supported"

        self.buffer_len = SAMPLE_RATE*4                             # size of pre-buffer
        
        self.buffer = np.zeros(self.buffer_len)                     # larger pre-buffer for loading samples into
        self.buffer_step = 0                                        # track step through pre-buffer

        print("= Loading stream...")
        device = sd.default.device
        print(f"= Default device set: {device}")
        print(f"= Initialising stream with sample rate of {SAMPLE_RATE} and {channels} channel(s)")
        self.stream = sd.OutputStream(samplerate=SAMPLE_RATE, device=device, channels=channels)
        stream.start()
        print("= Stream running. ")

        # TODO bring in the panda
        base.taskMgr.add("load_stream", load_stream_from_buffer)

    def _flush_buffer(self):
        self.buffer = np.zeros(self.buffer_len)
        messenger.send("buffer_ready")

    def add_beat_trigger(self, sample, bps, offset: int = 0, trigger_per_beat: float = 1., amp: float = 1.):
        beat_sample_len = bps*SAMPLE_RATE
        AudioWriterMgr(sample, offset, amp)

    def load_stream_from_buffer(self, task):
        # TODO step through frames of larger buffer
        self.stream.write(self.buffer[self.buffer_step:self.buffer_step+SAMPLE_RATE])
        self.buffer_step += SAMPLE_RATE                             # move writehead on by a frame of samplerate
        if self.buffer_step >= self.buffer_len:                     # when the step takes us outside the range of the buffer:
            self._flush_buffer()                                    #   flush the buffer when last frame of buffer is reached
            self.buffer_step = 0                                    #   reset writehead to start of buffer

        return task.cont


# each buffer writer should have its own lifetime,
#   so the task writes as much to the buffer as it ought,
#   then awaits the next buffer flush to add its data

# the buffer can then be any reasonable length, because
#   the writers manage the overflow

# the sequencer should therefor manage an Awaitable state
#   such that all tasks can quietly wait for the refresh

# there should also probably be some degree of gap between
#   the stream-write start and the beat start, something like:
#   audio buffer:           ||:beat-1 ----- | -2---------- | -3---------- | -4----------:||
#   stream writer: | -------||:------- | ------- | ------- | ------- | ------- | -------:||
# which is to say, the processing of the next buffer should 
#   begin before the stream-writer needs the new buffer data


# =========================== IDEA 1 ==========================================================================
# doesn't work to use stream.write - it adds data sequentially, not additively
# i'm gonna need to write some kind of buffer

# write an audio sample to the buffer (use `amp` to dampen inputs to prevent clipping)
#[buffer: np.array, sample: np.array, offset: int, amp: float]
class AudioWriterMgr(DirectObject.DirectObject):
    def __init__(self, sample, amp):
        self.sample = sample * amp
        #self.offset = offset

        def _write() -> np.array:
            # TODO write whole buffer of samples from start to end
            #base.AudioRenderer.buffer[self.offset:self.offset+len(self.sample)] += self.sample
            print("writing samp")
            base.stream.write(self.sample)
        
        self.accept("beat", _write)


class TestSequencer:
    def __init__(self, bpm):
        # set bpm and length
        self.bpm: int      = bpm

        # discretise time into samples
        self.bps: int    = self.bpm // 60.                          # beats per second
        self.spb: int    = int(SAMPLE_RATE // self.bps)             # beat length in samples (beat stride)
        self.dt: float   = 0.                                       # time since last tick 
        self.prev_time   = time.time()                              # time of previous tick

        def _tick(task):
            self.dt += (self.prev_time + time.time())
            if self.dt >= (1./self.bps):                                # bps is inverse of length of a beat
                messenger.send("beat")                                  #   trigger awaiting tasks
                self.dt = 0.                                            #   reset dt
            self.prev_time = time.time()

            return task.cont

        base.taskMgr.add(_tick, "sequencer_tick")

    def add_beat_trigger(self, sample, amp: float = .8):
        return AudioWriterMgr(sample, amp)
        #base.render.attach_new_node(PandaNode(new_awriter))


def test_idea_1():
    print("="*48 + "\n" + "="*20 + " Mixer " + 21*"=" + "\n==")

    print("== Initialising ShowBase...")
    P3D_CONFIG: str = """
    window-type none
    """
    load_prc_file_data('',P3D_CONFIG)
    ShowBase()

    device = sd.default.device
    channels = 1
    assert (channels > 0) and (channels < 3), f"!!! only mono or stereo pls !!!"
    print(f"== Initialising stream:" 
          f"\n\t| sample rate : {SAMPLE_RATE}"
          f"\n\t| channels : {"mono" if channels == 1 else "stereo"}"
          f"\n\t| default device : {device}")
    base.stream = sd.OutputStream(samplerate=SAMPLE_RATE, device=device, channels=channels)
    base.stream.start()
    
    print("== Creating sequencer...")
    base.sequencer = TestSequencer(180)                                 # 180bpm = 3 beats per second

    triggers = []

    print("== Sequencer initialised; adding sounds...")
    triggers.append(base.sequencer.add_beat_trigger(bleeps.ping("kick", 4000, atk=100)))
    triggers.append(base.sequencer.add_beat_trigger(bleeps.ping("beep"), .6))

    #base.accept("esc", base.user_exit)

    print("== Sounds added!\n\n======= Running: =======")
    base.run()

    base.stream.stop()
    base.stream.close()
    print("== Goodbye!\n" + "="*48)


# =========================== IDEA 0 ==========================================================================

# write an audio sample to the buffer (use `amp` to dampen inputs to prevent clipping)
async def _write_audio_buff(buffer: np.array, sample: np.array, offset: int, amp: float):
    await messenger("buffer_ready")
    buffer[offset:offset+len(sample)] += sample * amp
    return task.cont

def simple_sequence():
    # set bpm and length
    bpm: int = 180                                                  # 180bpm = 3 beats per second
    length_beats: int = 32                                          # total length of clip in beats

    # discretise time into samples
    bps: int = bpm // 60.                                           # beats per second
    spb: int = int(SAMPLE_RATE // bps)                              # beat length in samples (beat stride)
    length_time = length_beats * bps                                # length of audio in seconds
    n_samples = int(SAMPLE_RATE * length_time)                      # total number of samples in audio
    signal = np.zeros(n_samples, dtype=np.float32)                  # initialise a buffer for the audio


    # add components layer by layer
    for beat in range(length_beats):
        # four to the floor
        idx_beat = beat*spb                                         # calculate sample index of beat
        kick = bleeps.ping("kick", 8000, atk=6)                     # generate a kick sound
        signal = _write_audio_buff(signal, kick, idx_beat, .8)      # write the kick to the buffer on the beat

        # hat on offbeat
        hat = bleeps.ping("hat", 6000, atk=14)                      # generate a hihat sound
        offbeat_smp_start = idx_beat + spb//2                       # add offset of spb/2 to move to offbeat
        signal = _write_audio_buff(signal, hat, offbeat_smp_start, .8) # write hat to buffer on offbeat

        # chords - play I - V every 8 beats
        chord_len = spb - 200                                       # set chord length to half a beat
        chord = bleeps.ping("chord", chord_len, freq=360*(((beat//8)%2)+1)) # generate a chord
        signal = _write_audio_buff(signal, chord, idx_beat, .7)     # write chord to buffer on the beat

    # hard limit to reduce clipping
    for sample in signal:
        sample = max(min(sample,.99), -.99)

    # play audio
    sfile.play_audio(SAMPLE_RATE, signal)
    # save to wav file
    #sfile.write_audio_to_wav("test_file.wav", signal)
    print("== Goodbye!\n" + "="*48)

    # FIXME this isn't needed - salvage the trigger control code to set up tasks
    # def buff_trigger(self):
    #     # TODO some sort of overflow buffer?
    #     # make a 1-beat-long buffer
    #     self.buffer = np.zeros(self.beat_sample_len)
    #     # iterate through how many times it triggers per beat and add it to the buffer
    #     for trigger in range(self.trigger_per_beat):
    #         offset = self.offset*(trigger+1)/self.trigger_per_beat
    #         self.buffer[offset:offset+len(self.sample)] += self.sample * self.amp
    #     beat_pos = # index where beat written to buffer starts
    #     # write the beat to the renderer buffer
    #     base.AudioRenderer.buffer[beat_pos:beat_pos+self.beat_sample_len] += self.buffer
    #
    # offset = int(task.time * sample)%SAMPLE_RATE
