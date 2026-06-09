---
type: note
category: formula
tags: [black-scholes, options, measure-change, ito]
related: [gbm-expectation, stochastic-exponential]
status: in-progress
---

# Black-Scholes Formula

## Intuition

The call price decomposes into two terms: the expected asset received if the option is exercised, minus the expected cost paid. Each term uses a different probability measure — one from the perspective of the stock, one from the perspective of cash — which is why there are two distinct probabilities $N(d_1)$ and $N(d_2)$ rather than one.

## Math

### Formula (with continuous dividend yield $q$)

$$C = S_0 e^{-qT} N(d_1) - K e^{-rT} N(d_2)$$

$$P = K e^{-rT} N(-d_2) - S_0 e^{-qT} N(-d_1)$$

where

$$d_1 = \frac{\ln(S_0/K) + \left(r - q + \frac{1}{2}\sigma^2\right)T}{\sigma\sqrt{T}}$$

$$d_2 = d_1 - \sigma\sqrt{T} = \frac{\ln(S_0/K) + \left(r - q - \frac{1}{2}\sigma^2\right)T}{\sigma\sqrt{T}}$$

### What $N(d_1)$ and $N(d_2)$ represent

- $N(d_2)$ — risk-neutral probability $\mathbb{Q}(S_T > K)$: the option finishes in the money.
- $N(d_1)$ — **Delta**: probability of finishing ITM under the **stock (share) measure** $\mathbb{S}$, i.e. using $S_t$ as numeraire.

### Why the signs of $\frac{1}{2}\sigma^2$ differ

Under $\mathbb{Q}$, the log-price follows:

$$\log S_T = \log S_0 + \left(r - q - \frac{1}{2}\sigma^2\right)T + \sigma\sqrt{T}\, Z, \quad Z \sim N(0,1)$$

The $-\frac{1}{2}\sigma^2$ is the Itô correction that makes $e^{-rt}S_t$ a martingale. $d_2$ directly uses this drift. It is the standardised distance to the strike under $\mathbb{Q}$.

To compute $E^{\mathbb{Q}}[S_T \mathbf{1}_{S_T > K}]$ (the asset term), we change numeraire from the bond to the stock. The Radon-Nikodym derivative shifts the drift by $+\sigma^2$:

$$\left(r - q - \frac{1}{2}\sigma^2\right) + \sigma^2 = r - q + \frac{1}{2}\sigma^2$$

That is the drift of $\log S_T$ under $\mathbb{S}$, which gives $d_1$. So the $+\frac{1}{2}\sigma^2$ in $d_1$ is not a new correction. it is what remains after the $-\frac{1}{2}\sigma^2$ is cancelled and a $+\sigma^2$ is added by the measure change. Hence $d_1 = d_2 + \sigma\sqrt{T}$.

## C++ notes

Implemented in [vanillaEuropeanOption.cpp](../../code/cpp/src/product/vanillaEuropeanOption.cpp) as `bsPriceEuropeanVanillaOption`. Uses `std::erfc` (or `std::erf`) for $N(\cdot)$ — no external dependency needed.

## QuantLib benchmark

`AnalyticEuropeanEngine` with `BlackScholesMertonProcess`. Used to cross-check MC vs closed-form within 0.5% tolerance.

## Greeks / sensitivities

| Greek | Call | Put |
| ----- | ---- | --- |
| Delta $\Delta$ | $e^{-qT} N(d_1)$ | $-e^{-qT} N(-d_1)$ |
| Vega $\mathcal{V}$ | $S_0 e^{-qT} \phi(d_1) \sqrt{T}$ | same |
| Theta $\Theta$ | negative (time decay) | can be positive deep ITM |

---

#black-scholes #options #measure-change
