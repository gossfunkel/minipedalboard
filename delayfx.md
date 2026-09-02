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