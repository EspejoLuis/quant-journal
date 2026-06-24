---
type: note
category: engine
tags: [pde, finite-difference, black-scholes, explicit, implicit, crank-nicolson]
related: [black-scholes-formula, vanilla-european-option]
status: in-progress
---

# PDE Engine

## Intuition

Instead of simulating thousands of paths forward in time (Monte Carlo), the PDE approach works backward: starting from the known payoff at maturity, it steps back through time slice by slice until it reaches today. At each step, neighbouring stock price nodes "communicate" through the diffusion term $\sigma^2 S^2 \partial^2 V / \partial S^2$, spreading value across the grid the way heat spreads through a rod. The choice of scheme (explicit vs implicit) controls how that communication is handled numerically.

## Math

### The Black-Scholes PDE

$$\frac{\partial V}{\partial t} + \frac{1}{2}\sigma^2 S^2 \frac{\partial^2 V}{\partial S^2} + (r - q) S \frac{\partial V}{\partial S} - r V = 0$$

Solved backward in time from $t = T$ to $t = 0$.

### Spatial grid

**Uniform:** $S_i = i \cdot \Delta S$, $\Delta S = S_{\max} / N$, for $i = 0, \ldots, N$.

**Log:** $x_i = x_{\min} + i \cdot \Delta x$, $S_i = e^{x_i}$, where $x_{\min} = \log(S_{\min})$, $x_{\max} = \log(S_{\max})$, $\Delta x = (x_{\max} - x_{\min}) / N$. $S_{\min} = 10^{-4}$ to avoid $\log(0)$.

**$S_{\max}$ from GBM 3-sigma rule:**

$$S_{\max} = S_0 \exp\!\left((r - q - \tfrac{1}{2}\sigma^2)T + 3\sigma\sqrt{T}\right)$$

The $-\tfrac{1}{2}\sigma^2$ is the Itô correction converting the log-process back to $S$.

### Discretisation operator $L$

Second-order central differences on the uniform grid:

$$a_i = \frac{\sigma^2 S_i^2}{2(\Delta S)^2} - \frac{(r-q) S_i}{2 \Delta S}$$

$$b_i = -\frac{\sigma^2 S_i^2}{(\Delta S)^2} - r$$

$$c_i = \frac{\sigma^2 S_i^2}{2(\Delta S)^2} + \frac{(r-q) S_i}{2 \Delta S}$$

$(LV)_i = a_i V_{i-1} + b_i V_i + c_i V_{i+1}$

### Schemes (backward in time: know $V^{n+1}$, want $V^n$)

| Scheme | Formula | System to solve | Stable? |
|--------|---------|----------------|---------|
| Explicit | $V^n = (I + \Delta t\, L)\, V^{n+1}$ | None — direct update | Conditionally: $\Delta t \leq (\Delta S)^2 / 2$ |
| Implicit | $(I - \Delta t\, L)\, V^n = V^{n+1}$ | Tridiagonal (Thomas) | Unconditionally |
| Crank-Nicolson | $(I - \frac{\Delta t}{2} L)\, V^n = (I + \frac{\Delta t}{2} L)\, V^{n+1}$ | Tridiagonal (Thomas) | Unconditionally, 2nd order |

### Boundary conditions

- **$S = 0$ (Dirichlet):** $V_0^n = 0$ for a call (option worthless).
- **$S = S_{\max}$ (Neumann — zero second derivative):** $V_N^n = 2 V_{N-1}^n - V_{N-2}^n$. Correct asymptotically because $\Delta \to 1$, $\Gamma \to 0$ as $S \to \infty$. No knowledge of $K$ required — works for any product.

### Terminal condition

$$V_i^{N_T} = \texttt{instrument.payoff}(\{S_i\})$$

Uses `Instrument::payoff(path)` with a single-element vector. Valid because all products use `path.back()` — same as $S_T$.

## C++ Notes

- `PdeEngine` derives `Engine`; `price(const Instrument&)` override.
- `defineSpaceGrid() const` — private, returns `std::vector<double>` of $S_i$ values.
- Two value slices: `valueCurrent` (initialized to terminal condition) and `valueNext` (workspace). Swap at end of each time step. After the loop, `valueCurrent` holds $V(S, 0)$.
- Coefficients $a_i$, $b_i$, $c_i$ computed once as vectors before the scheme switch — shared by all three schemes.
- Return value: interpolate `valueCurrent` at the node closest to $S_0$.

## QuantLib Benchmark

- `FdBlackScholesVanillaEngine` — compare price vs BS closed-form, tolerance < 0.5%.

## Greeks / Sensitivities

- **Delta:** $\partial V / \partial S \approx (V_{i+1} - V_{i-1}) / (2\Delta S)$ — central difference on `valueCurrent` at $t = 0$.
- **Gamma:** $\partial^2 V / \partial S^2 \approx (V_{i+1} - 2V_i + V_{i-1}) / (\Delta S)^2$.
- Both available from the final slice at no extra cost.

#pde #finite-difference #engine
