# Delay fx

Several simple effects can be produced using **delay**.

Crucially, there is a psychoacoustic effect where the ear struggles to distinguish sounds less than __40ms__ apart.

This makes <40ms delay times a powerful way to transform a sound.

Short enough, and the waves will have phase interactions, creating *comb filtering*. This is the sound of a **phaser**.

Automating this delay gives the distinctive sound of a **flanger**.

Phasers and flangers are very similar, and the difference pretty much comes down to parameterisation.

**Chorus** effects have longer delays to minimise the phase interactions (comb filtering), and utilise *pitch modulation* to emulate the effect of multiple simultaneous performers or sound sources.

```
            time      number  other
         |          |       |
chorus:  | 10-40 ms | 2+    | pitch shift
         |          |       |
phaser:  | 0-2 ms   | 2     | filtering
         |          |       |
flanger: | 1-5 ms   | 2     | movement
```

parameters:

- **chorus**: rate, depth, delay, feedback, dry/wet
- **phaser**: depth, dry/wet
- **flanger**: rate, depth, dry/wet

## Delay vs Phase
A delay unit creates a phase kickback - i.e. it reproduces the original wave out-of-phase with the original. 
This phase difference may be a multiple of tau, i.e. in-phase with a regular signal (like a sine wave). 
Phasers and flangers ensure that the phase difference is small enough such that there will be comb filtering 
on irregular signals (e.g. human voice, transients like drums and sound effects, music). 

The biggest challenge here is the quantised nature of the signal. Phase differences in continuous signals 
emerge naturally from many electronic signal paths, but with a discrete-time signal, we face the problem 
that samples cannot be played out-of-time.

For example:
```
a: |   |   |   |
b:  |   |   |   |
```
The sound card of the computer is locked to a sample rate (usually 44100 or 48000 hz). It cannot produce an 
output with extra samples in-between. So if stream `a` is at our sampling rate, and we introduce a phase 
kickback of less than a sample yielding stream `b`, we have no way to output both streams concurrently at 
the timings given. Instead, `b` will have to be interpolated to give the *effect of* phase knockback, 
without actually producing a secondary signal. The point is to interfere with `a` such that it *appears to 
be* the sum of `a` and `b`.