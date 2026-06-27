# Quantitative Finance Journal

A comprehensive, version-controlled repository documenting research, analysis, and practical applications in quantitative finance. This repository serves as a structured knowledge base featuring daily study notes, code implementations (in C++), research findings, and technical documentation related to quantitative finance methodologies and strategies.

## Topics

### Models

| Model | Feature | `.hpp` | `.cpp` | Tests | Status |
| ----- | ------- | ------ | ------ | ----- | ------ |
| **GBM** | Path simulation (`simulateGbmPath`) | [.hpp](code/cpp/src/common/pathGeneration.hpp) | — (header-only) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp#L89) | Done |
| | Antithetic variates (`VarianceReduction::Antithetic`) | [.hpp](code/cpp/src/common/simulationParameters.hpp) | [.cpp](code/cpp/src/engine/monteCarloEngine.cpp#L52) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp#L158) | Done |
| | Closed-form (`BsCloseForm`) | [.hpp](code/cpp/src/engine/blackScholesCloseForm.hpp) | [.cpp](code/cpp/src/engine/blackScholesCloseForm.cpp) | [tests](code/cpp/tests/unit/engine/blackScholesCloseForm.cpp) | Done |
| **Local Volatility (Dupire)** | Bilinear interpolation on $\sigma(S,t)$ grid | — | — | — | Planned |
| | Forward PDE calibration | — | — | — | Planned |
| **Heston** | Euler-Milstein with variance reflection | — | — | — | Planned |
| | Characteristic function pricer | — | — | — | Planned |
| | 2D ADI solver | — | — | — | Planned |
| **Correlated GBM** | 2-asset path (`simulateCorrelatedGbmPath`) via Cholesky | — | — | — | Planned |
| **SLV** | Path simulation (`simulateSlvPath`) via pre-computed L(S,t) grid | — | — | — | Planned |
| | Leverage function calibration via particle method | — | — | — | Planned |

### Numerical Methods

| Method | Feature | `.hpp` | `.cpp` | Tests | Status |
| ------ | ------- | ------ | ------ | ----- | ------ |
| **Monte Carlo** | GBM engine (`McEngine`) | [.hpp](code/cpp/src/engine/monteCarloEngine.hpp) | [.cpp](code/cpp/src/engine/monteCarloEngine.cpp) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp) | Done |
| | Antithetic variates | [.hpp](code/cpp/src/common/simulationParameters.hpp) | [.cpp](code/cpp/src/engine/monteCarloEngine.cpp) | [tests](code/cpp/tests/unit/engine/monteCarloEngine.cpp) | Done |
| | Chooser engine (`ChooserEngine`) | [.hpp](code/cpp/src/engine/chooserEngine.hpp) | [.cpp](code/cpp/src/engine/chooserEngine.cpp) | [tests](code/cpp/tests/integration/engine/chooserEngine.cpp) | Done |
| | Control variates | — | — | — | Planned |
| | Importance sampling | — | — | — | Planned |
| | Quasi-MC with Sobol sequences | — | — | — | Planned |
| **Longstaff-Schwartz (LSM)** | Continuation value regression | — | — | — | Planned |
| **Finite Differences** | 1D Explicit scheme (BS PDE) | [.hpp](code/cpp/src/engine/pdeEngine.hpp) | [.cpp](code/cpp/src/engine/pdeEngine.cpp) | — | In progress |
| | 1D Implicit scheme (BS PDE) | [.hpp](code/cpp/src/engine/pdeEngine.hpp) | [.cpp](code/cpp/src/engine/pdeEngine.cpp) | — | In progress |
| | 1D Crank-Nicolson (BS PDE) | [.hpp](code/cpp/src/engine/pdeEngine.hpp) | [.cpp](code/cpp/src/engine/pdeEngine.cpp) | — | In progress |
| | 2D Crank-Nicolson (Lookback, S×M grid) | — | — | — | Planned |
| | 2D Craig-Sneyd ADI (Heston/SLV, S×v grid) | — | — | — | Planned |
| **AAD** | Greeks via reverse-mode automatic differentiation | — | — | — | Planned |

### Products

| Product | Method | `.hpp` | `.cpp` | Tests | Status |
| ------- | ------ | ------ | ------ | ----- | ------ |
| Vanilla European Option | MC + BS closed-form | [.hpp](code/cpp/src/product/vanillaEuropeanOption.hpp) | [.cpp](code/cpp/src/product/vanillaEuropeanOption.cpp) | [tests](code/cpp/tests/unit/product/vanillaEuropeanOption.cpp) | Done |
| Digital Option (European) | MC + BS closed-form + 1D PDE | [.hpp](code/cpp/src/product/digitalEuropeanOption.hpp) | [.cpp](code/cpp/src/product/digitalEuropeanOption.cpp) | [tests](code/cpp/tests/unit/product/digitalEuropeanOption.cpp) | In progress |
| Digital Option (American) | MC + LSM | — | — | — | Planned |
| Chooser Option | MC + closed-form | [.hpp](code/cpp/src/product/chooserEuropeanOption.hpp) | [.cpp](code/cpp/src/product/chooserEuropeanOption.cpp) | [tests](code/cpp/tests/unit/product/chooserEuropeanOption.cpp) | Done |
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
- [Digital Option (European)](notes/products/digital-option.md)
- [Chooser Option](notes/products/chooser-option.md)
- [PDE Engine](notes/engines/pde/pde-engine.md)
- [PDE Finite-Difference Coefficients](notes/engines/pde/pde-fd-coefficients.md)
- [PDE Boundary Conditions](notes/engines/pde/pde-boundary-conditions.md)
