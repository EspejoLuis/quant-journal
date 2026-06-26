---
type: note
category: engine
tags: [pde, finite-difference, black-scholes, explicit, implicit, crank-nicolson]
related: [black-scholes-formula, vanilla-european-option, pde-fd-coefficients, pde-boundary-conditions]
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

$(LV)_i = a_i V_{i-1} + b_i V_i + c_i V_{i+1}$

Coefficients $a_i$, $b_i$, $c_i$ differ between uniform ($S$-space) and log ($x$-space) grids — see [[pde-fd-coefficients]].

### Schemes (backward in time: know $V^{n+1}$, want $V^n$)

| Scheme | Formula | System to solve | Stable? |
| ------ | ------- | -------------- | ------- |
| Explicit | $V^n = (I + \Delta t\, L)\, V^{n+1}$ | None — direct update | Conditionally (see CFL section) |
| Implicit | $(I - \Delta t\, L)\, V^n = V^{n+1}$ | Tridiagonal (Thomas) | Unconditionally |
| Crank-Nicolson | $(I - \frac{\Delta t}{2} L)\, V^n = (I + \frac{\Delta t}{2} L)\, V^{n+1}$ | Tridiagonal (Thomas) | Unconditionally, 2nd order |

### CFL Condition (Explicit Scheme)

**Uniform grid derivation:**

The explicit update at interior node $i$:

$$V_i^n = V_i^{n+1} + \Delta t \left( a_i V_{i-1}^{n+1} + b_i V_i^{n+1} + c_i V_{i+1}^{n+1} \right)$$

For stability, the coefficient of $V_i^{n+1}$ must be non-negative (otherwise the scheme amplifies errors). Dropping the small $-r$ term from $b_i$:

$$1 + b_i \Delta t \geq 0 \implies 1 - \frac{\sigma^2 S_i^2}{(\Delta S)^2}\,\Delta t \geq 0$$

The diffusion coefficient $D_i = \tfrac{1}{2}\sigma^2 S_i^2$ is largest at $i = N$ (i.e. $S_i = S_{\max}$), so the tightest constraint comes from that node:

$$\boxed{\Delta t \leq \frac{(\Delta S)^2}{\sigma^2 S_{\max}^2}}$$

**Why $S_{\max}^2$ appears:** the BS diffusion coefficient is $\tfrac{1}{2}\sigma^2 S^2$, not a constant. The S-space step $\Delta S$ is the same at every node, but the "speed" at which information spreads grows as $S^2$. The node at $S_{\max}$ runs the fastest — it sets the global constraint.

**Log grid:**

Under $x = \log S$, the BS PDE transforms to:

$$\frac{\partial V}{\partial t} + \frac{\sigma^2}{2}\frac{\partial^2 V}{\partial x^2} + \left(r - q - \frac{\sigma^2}{2}\right)\frac{\partial V}{\partial x} - rV = 0$$

The $S^2$ factor is absorbed into the change of variables. The diffusion coefficient is now the constant $D = \sigma^2/2$. With uniform log-space step $\Delta x = (\log S_{\max} - \log S_{\min})/N$:

$$\Delta t \leq \frac{(\Delta x)^2}{\sigma^2}$$

No $S_{\max}^2$ — the constraint is the same at every node. For large $S_{\max}$, $\Delta x \sim \log S_{\max}/N$ grows only logarithmically, so the log-grid CFL is far less restrictive than the uniform-grid one.

**Implementation note:** the CFL check must use the grid-appropriate formula. For a log grid the coefficients $a_i, b_i, c_i$ must also be rederived in $x$-space (constant-coefficient PDE); using the $S$-space coefficients with non-uniform $\Delta S_i$ is incorrect.

### Boundary conditions

See [[pde-boundary-conditions]] for the full BC table (Dirichlet/Neumann per product) and implementation notes.

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
