# Quantitative Finance Journal

A comprehensive, version-controlled repository documenting research, analysis, and practical applications in quantitative finance. This repository serves as a structured knowledge base featuring daily study notes, code implementations (in C++), research findings, and technical documentation related to quantitative finance methodologies and strategies.

## Topics

### Models

<div align="center">

| Model | Feature | Tests | Status |
| ----- | ------- | ----- | ------ |
| **GBM (Black-Scholes)** | [Path simulation (`simulateGbmPath`)](code/cpp/src/engine/monteCarloEngine.cpp) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp) | In progress |
| | Antithetic variates | — | Planned |
| | [Closed-form (`BsCloseForm`)](code/cpp/src/engine/blackScholesEngine.cpp) | — | In progress |
| **Local Volatility (Dupire)** | Bilinear interpolation on $\sigma(S,t)$ grid | — | Planned |
| | Forward PDE calibration | — | Planned |
| **Heston** | Euler-Milstein with variance reflection | — | Planned |
| | Characteristic function pricer | — | Planned |
| | 2D ADI solver | — | Planned |
| **SLV** | Leverage function calibration via particle method | — | Planned |

</div>

### Numerical Methods

<div align="center">

| Method | Feature | Tests | Status |
| ------ | ------- | ----- | ------ |
| **Monte Carlo** | [GBM engine (`McEngine`)](code/cpp/src/engine/monteCarloEngine.cpp) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp) | In progress |
| | Antithetic variates | — | Planned |
| | Quasi-MC with Sobol sequences | — | Planned |
| **Longstaff-Schwartz (LSM)** | Continuation value regression | — | Planned |
| **Finite Differences** | 1D Crank-Nicolson (BS PDE) | — | Planned |
| | 2D Craig-Sneyd ADI (Heston/SLV) | — | Planned |

</div>

### Products

<div align="center">

| Product | Method | Tests | Status |
| ------- | ------ | ----- | ------ |
| [Vanilla European Option](code/cpp/src/product/vanillaEuropeanOption.cpp) | MC + BS closed-form | [tests](code/cpp/tests/unit/product/vanillaEuropeanOption.cpp) | In progress |
| Digital Option | MC + 1D CN | — | Planned |
| Chooser Option | MC + closed-form | — | Planned |
| Asian Option | MC + control variates + LSM | — | Planned |
| Barrier Option | MC + 1D CN + Rubinstein-Reiner | — | Planned |
| Lookback Option | MC + 2D CN | — | Planned |
| Variance Swap | MC + log-contract replication | — | Planned |
| Volatility Swap | MC + Brockhaus-Long | — | Planned |
| Range Accrual | Local Vol MC + 1D CN | — | Planned |
| Cliquet Option | Local Vol MC | — | Planned |
| Rainbow Option | Correlated GBM + Margrabe | — | Planned |
| Worst-of Option | Correlated GBM + LSM | — | Planned |
| Accumulator | Heston MC + LSM | — | Planned |
| Autocallable | SLV + LSM + AAD Greeks | — | Planned |

</div>

### Books

- *Modelling, Pricing, and Hedging Counterparty Credit Exposure* — Giovanni Cesari, John Aquilina, Niels Charpillon, Zlatko Filipovic, Gordon Lee, Ion Manda
- *Modern Computational Finance: AAD and Parallel Simulations* — Antoine Savine

### Notes

- [GBM Expectation Under Risk-Neutral Measure](notes/random-notes/gbm-expectation.md)
- [Stochastic Exponential](notes/random-notes/stochastic-exponential.md)
- [Normal CDF via erf](notes/random-notes/normal-cdf-erf.md)
- [Black-Scholes Formula](notes/random-notes/black-scholes-formula.md)
