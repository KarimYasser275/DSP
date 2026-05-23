<div align="center">

# ⚡ DSP — Digital Signal Processing Toolkit

### A lightweight, pure-C DSP library built from the ground up

![C](https://img.shields.io/badge/language-C11-blue?style=for-the-badge&logo=c&logoColor=white)
![License](https://img.shields.io/badge/license-MIT-green?style=for-the-badge)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Embedded-orange?style=for-the-badge)
![Build](https://img.shields.io/badge/build-Make-red?style=for-the-badge&logo=gnu&logoColor=white)

*From Fourier transforms to FIR filters — everything you need for signal processing in portable C.*

---

</div>

## 🎯 Overview

**DSP** is a modular, zero-dependency digital signal processing library written in C11. It's designed for clarity, portability, and educational use — every function is documented with the underlying math and ready to deploy on anything from a Linux workstation to a bare-metal MCU.

```
📡 Signal In ──▶ 🔬 Transform ──▶ 🎛️ Filter ──▶ 📊 Signal Out
```

---

## 🧩 Modules

| Module | Header | Description |
|--------|--------|-------------|
| 🔊 **Fourier** | `dsp_ft.h` | DFT, IDFT, and radix-2 Cooley-Tukey FFT |
| 🎛️ **Filter** | `dsp_filter.h` | Windowed-sinc FIR (LPF / HPF / BPF) + 1st-order IIR (float & Q15 fixed-point) |
| 📈 **Averaging** | `dsp_avg.h` | Moving average (basic, enhanced power-of-2, recursive EMA) |
| 🧮 **Math** | `dsp_math.h` | Cartesian ↔ Polar coordinate conversion |
| 📦 **Signal Types** | `dsp_signal_types.h` | Shared data structures (`dsp_FTsignal_t`, `dsp_cartPolar_t`) |

---

## 📂 Project Structure

```
DSP/
└── core/
    ├── signals/
    │   └── dsp_signal_types.h      # Shared structs & constants
    ├── Fourier/
    │   ├── dsp_ft.h                # DFT / IDFT / FFT prototypes
    │   └── dsp_ft.c
    ├── Filter/
    │   ├── dsp_filter.h            # FIR & IIR filter prototypes
    │   └── dsp_filter.c
    ├── avg/
    │   ├── dsp_avg.h               # Moving average prototypes
    │   └── dsp_avg.c
    └── dsp_math/
        ├── dsp_math.h              # Coordinate conversion prototypes
        └── dsp_math.c
```

---

## 🚀 Getting Started

### Integration

Copy the `core/` directory into your project and compile the `.c` files alongside your application:

```bash
gcc -std=c11 -Icore/ your_app.c core/**/*.c -lm -o your_app
```

### Requirements

- Any **C11-compatible compiler** (GCC, Clang, MSVC, ARM GCC, …)
- **libm** (`-lm`) for `sin`, `cos`, `exp`, `atan2`

---

## 🎛️ Filter Module — At a Glance

The filter module provides **three tiers** of filtering:

### Windowed-Sinc FIR Filters

Generate a Blackman-windowed sinc kernel and convolve it with the input signal. ~74 dB stop-band attenuation.

```c
dsp_filter_t cfg = {
    .input      = signal,
    .output     = out_buf,
    .kernel     = kernel_buf,
    .input_len  = 1024,
    .kernel_len = 101,       // odd number of taps
    .fc         = 0.1,       // normalised cutoff (0.0–0.5)
    .fc_high    = 0.3,       // upper cutoff (BPF only)
};

dsp_LPF(&cfg);   // Low-pass
dsp_HPF(&cfg);   // High-pass  (spectral inversion)
dsp_BPF(&cfg);   // Band-pass  (LPF_high − LPF_low)
```

### 1st-Order IIR Filter (Floating-Point)

Single-pole recursive filter — processes one sample at a time.

```c
dsp_1stOrderFilter_t filt = {
    .fc     = 1000.0,    // cutoff in Hz
    .fs     = 44100.0,   // sample rate in Hz
    .is_hpf = false,     // false = LPF, true = HPF
};

dsp_1stOrderFilterInit(&filt);

for (int i = 0; i < num_samples; i++) {
    output[i] = dsp_1stOrderFilterCalc(&filt, input[i]);
}
```

### 1st-Order IIR Filter (Q15 Fixed-Point)

Identical algorithm using `int16_t` arithmetic — ideal for MCUs without an FPU.

```c
dsp_1stOrderFilterFP_t filt_fp = {
    .fc     = 1000.0,
    .fs     = 44100.0,
    .is_hpf = false,
};

dsp_1stOrderFilterFPInit(&filt_fp);

for (int i = 0; i < num_samples; i++) {
    output[i] = dsp_1stOrderFilterFPCalc(&filt_fp, input[i]);
}
```

---

## 🔊 Fourier Module

```c
dsp_FTsignal_t sig = {
    .src_signal_re = time_domain_re,
    .src_signal_im = time_domain_im,
    .re            = freq_re,
    .im            = freq_im,
    .size          = N,
};

dsp_dft_calc(&sig);    // O(N²) DFT
dsp_fft_calc(&sig);    // O(N log N) radix-2 FFT  (N must be power of 2)
dsp_idft_calc(&sig);   // Inverse DFT
```

---

## 📈 Averaging Module

| Function | Method | Buffer? | Best For |
|----------|--------|---------|----------|
| `dsp_movingAvgCalc` | Simple circular buffer | ✅ | General-purpose smoothing |
| `dsp_movingAvgEnhancedCalc` | Power-of-2 bitmask, warm-up aware | ✅ | Efficient embedded use |
| `dsp_movingAvgRecursiveCalc` | Exponential (IIR), Q16 fixed-point | ❌ | Minimal RAM, no buffer needed |

```c
// Recursive EMA — just 3 bytes of state, no buffer
mvAvgRecursive_t ema = { .shift = 4 };  // alpha = 1/16

for (int i = 0; i < num_samples; i++) {
    dsp_movingAvgRecursiveCalc(&ema, input[i]);
    // ema.average holds the smoothed value (Q16)
}
```

---

## 🧮 Math Utilities

```c
dsp_cartPolar_t coords = {
    .re    = re_array,
    .im    = im_array,
    .mag   = mag_array,
    .phase = phase_array,
    .size  = N,
};

dsp_cart2Polar(&coords);   // (re, im) → (mag, phase)
dsp_polar2Cart(&coords);   // (mag, phase) → (re, im)
```

---

## 🏗️ Design Principles

| Principle | Detail |
|-----------|--------|
| **Pure C11** | No C++ or external dependencies beyond `libm` |
| **Modular** | Each module is self-contained with its own `.h` / `.c` pair |
| **Documented** | Doxygen-style comments with `@pre` contracts on every function |
| **Portable** | Runs on Linux, macOS, Windows (MinGW), and bare-metal ARM |
| **Fixed-point ready** | Q15 variants available for integer-only hardware |
| **No dynamic allocation** | All buffers are caller-allocated — safe for real-time & embedded |

---

## 📜 License

This project is licensed under the **MIT License**. See `LICENSE` for details.

---

<div align="center">

*Built with ☕ and convolution kernels by **Karim Yasser***

</div>
