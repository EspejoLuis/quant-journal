# Quantitative Finance Journal

A comprehensive, version-controlled repository documenting research, analysis, and practical applications in quantitative finance. This repository serves as a structured knowledge base featuring daily study notes, code implementations (in C++), research findings, and technical documentation related to quantitative finance methodologies and strategies.

## Topics

### Models

| Model | Feature | `.h` | `.cpp` | Tests | Status |
| ----- | ------- | ---- | ------ | ----- | ------ |
| **GBM** | Path simulation (`simulateGbmPath`) | [.h](code/cpp/src/engine/monteCarloEngine.h#L35) | [.cpp](code/cpp/src/engine/monteCarloEngine.cpp#L29) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp#L89) | In progress |
| | Antithetic variates (`VarianceReduction::Antithetic`) | [.h](code/cpp/src/engine/monteCarloEngine.h#L9) | [.cpp](code/cpp/src/engine/monteCarloEngine.cpp#L52) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp#L158) | In progress |
| | Closed-form (`BsCloseForm`) | [.h](code/cpp/src/engine/blackScholesCloseForm.h#L7) | [.cpp](code/cpp/src/engine/blackScholesCloseForm.cpp#L8) | [tests](code/cpp/tests/unit/engine/blackScholesCloseForm.cpp#L5) | In progress |
| **Local Volatility (Dupire)** | Bilinear interpolation on $\sigma(S,t)$ grid | — | — | — | Planned |
| | Forward PDE calibration | — | — | — | Planned |
| **Heston** | Euler-Milstein with variance reflection | — | — | — | Planned |
| | Characteristic function pricer | — | — | — | Planned |
| | 2D ADI solver | — | — | — | Planned |
| **Correlated GBM** | 2-asset path (`simulateCorrelatedGbmPath`) via Cholesky | — | — | — | Planned |
| **SLV** | Path simulation (`simulateSlvPath`) via pre-computed L(S,t) grid | — | — | — | Planned |
| | Leverage function calibration via particle method | — | — | — | Planned |

### Numerical Methods

| Method | Feature | `.h` | `.cpp` | Tests | Status |
| ------ | ------- | ---- | ------ | ----- | ------ |
| **Monte Carlo** | GBM engine (`McEngine`) | [.h](code/cpp/src/engine/monteCarloEngine.h#L22) | [.cpp](code/cpp/src/engine/monteCarloEngine.cpp#L12) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp#L8) | In progress |
| | Antithetic variates | [.h](code/cpp/src/engine/monteCarloEngine.h#L9) | [.cpp](code/cpp/src/engine/monteCarloEngine.cpp#L52) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp#L158) | In progress |
| | Control variates | — | — | — | Planned |
| | Importance sampling | — | — | — | Planned |
| | Quasi-MC with Sobol sequences | — | — | — | Planned |
| **Longstaff-Schwartz (LSM)** | Continuation value regression | — | — | — | Planned |
| **Finite Differences** | 1D Crank-Nicolson (BS PDE) | — | — | — | Planned |
| | 2D Crank-Nicolson (Lookback, S×M grid) | — | — | — | Planned |
| | 2D Craig-Sneyd ADI (Heston/SLV, S×v grid) | — | — | — | Planned |
| **AAD** | Greeks via reverse-mode automatic differentiation | — | — | — | Planned |

### Products

| Product | Method | `.h` | `.cpp` | Tests | Status |
| ------- | ------ | ---- | ------ | ----- | ------ |
| Vanilla European Option | MC + BS closed-form | [.h](code/cpp/src/product/vanillaEuropeanOption.h#L13) | [.cpp](code/cpp/src/product/vanillaEuropeanOption.cpp#L6) | [tests](code/cpp/tests/unit/product/vanillaEuropeanOption.cpp#L7) | In progress |
| Digital Option (European) | MC + BS closed-form | — | — | — | Planned |
| Digital Option (American) | MC + LSM | — | — | — | Planned |
| Chooser Option | MC + closed-form | — | — | — | Planned |
| Asian Option | MC + control variates + LSM | — | — | — | Planned |
| Barrier Option | MC + 1D CN + Rubinstein-Reiner | — | — | — | Planned |
| Lookback Option | MC + 2D CN | — | — | — | Planned |
| Variance Swap | MC + log-contract replication | — | — | — | Planned |
| Volatility Swap | MC + Brockhaus-Long | — | — | — | Planned |
| Range Accrual | Local Vol MC + 1D CN | — | — | — | Planned |
| Cliquet Option | Local Vol MC | — | — | — | Planned |
| Rainbow Option | Correlated GBM + Margrabe | — | — | — | Planned |
| Worst-of Option | Correlated GBM + LSM | — | — | — | Planned |
| Accumulator | Heston MC + LSM | — | — | — | Planned |
| Autocallable | SLV + LSM + AAD Greeks | — | — | — | Planned |

### Books

- *Modelling, Pricing, and Hedging Counterparty Credit Exposure* — Giovanni Cesari, John Aquilina, Niels Charpillon, Zlatko Filipovic, Gordon Lee, Ion Manda
- *Modern Computational Finance: AAD and Parallel Simulations* — Antoine Savine

### Notes

- [GBM Expectation Under Risk-Neutral Measure](notes/random-notes/gbm-expectation.md)
- [Stochastic Exponential](notes/random-notes/stochastic-exponential.md)
- [Normal CDF via erf](notes/random-notes/normal-cdf-erf.md)
- [Black-Scholes Formula](notes/random-notes/black-scholes-formula.md)
