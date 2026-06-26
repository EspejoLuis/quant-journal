---
type: note
category: engine
tags: [pde, finite-difference, boundary-conditions]
related: [pde-engine, pde-fd-coefficients]
status: in-progress
---

# PDE Boundary Conditions

## Types

- **Dirichlet:** fix the value — $V = c$. Implementation: `V[0] = c` or `V[N] = c`.
- **Neumann (first derivative):** fix the slope — $\partial V / \partial S = c$. Implementation: `V[0] = V[1] - c*dS` (lower), `V[N] = V[N-1] + c*dS` (upper). Special case $c = 0$: `V[N] = V[N-1]` (flat).
- **Neumann (second derivative = 0):** zero curvature — $\partial^2 V / \partial S^2 = 0$. Implementation: `V[N] = 2*V[N-1] - V[N-2]` (linear extrapolation). QuantLib's default for vanilla outer boundaries.

Neumann (first derivative) avoids time-dependent BCs. The vanilla put Dirichlet lower BC is $Ke^{-r(T-t)}$ (changes every step); Neumann $\partial V/\partial S = -1$ is constant and equally accurate far from the strike.

## BC Table

QL = QuantLib default. For each boundary all three options are listed; the chosen approach and QL default are marked.

### Vanilla Call — terminal: $\max(S-K,\,0)$

| Boundary | Dirichlet | Neumann 1st | Neumann 2nd | QL |
| -------- | --------- | ----------- | ----------- | -- |
| Lower $S=0$ | $V=0$ ✓ | $\partial V/\partial S = 0$ | $\partial^2 V/\partial S^2=0$ | Dirichlet |
| Upper $S=S_{\max}$ | $V=S_{\max}-Ke^{-r(T-t)}$ (time-dep.) | $\partial V/\partial S=1$ | $\partial^2 V/\partial S^2=0$ ✓ | Neumann 2nd |

### Vanilla Put — terminal: $\max(K-S,\,0)$

| Boundary | Dirichlet | Neumann 1st | Neumann 2nd | QL |
| -------- | --------- | ----------- | ----------- | -- |
| Lower $S=0$ | $V=Ke^{-r(T-t)}$ (time-dep.) | $\partial V/\partial S=-1$ | $\partial^2 V/\partial S^2=0$ ✓ | Neumann 2nd |
| Upper $S=S_{\max}$ | $V=0$ ✓ | $\partial V/\partial S=0$ | $\partial^2 V/\partial S^2=0$ | Dirichlet |

### Cash-or-Nothing Call — terminal: $P\cdot\mathbf{1}_{S>K}$

| Boundary | Dirichlet | Neumann 1st | Neumann 2nd | QL |
| -------- | --------- | ----------- | ----------- | -- |
| Lower $S=0$ | $V=0$ ✓ | $\partial V/\partial S=0$ | $\partial^2 V/\partial S^2=0$ | Dirichlet |
| Upper $S=S_{\max}$ | $V=Pe^{-r(T-t)}$ (time-dep.) | $\partial V/\partial S=0$ | $\partial^2 V/\partial S^2=0$ ✓ | Neumann 2nd |

### Cash-or-Nothing Put — terminal: $P\cdot\mathbf{1}_{S<K}$

| Boundary | Dirichlet | Neumann 1st | Neumann 2nd | QL |
| -------- | --------- | ----------- | ----------- | -- |
| Lower $S=0$ | $V=Pe^{-r(T-t)}$ (time-dep.) | $\partial V/\partial S=0$ | $\partial^2 V/\partial S^2=0$ ✓ | Neumann 2nd |
| Upper $S=S_{\max}$ | $V=0$ ✓ | $\partial V/\partial S=0$ | $\partial^2 V/\partial S^2=0$ | Dirichlet |

### Asset-or-Nothing Call — terminal: $S\cdot\mathbf{1}_{S>K}$

| Boundary | Dirichlet | Neumann 1st | Neumann 2nd | QL |
| -------- | --------- | ----------- | ----------- | -- |
| Lower $S=0$ | $V=0$ ✓ | $\partial V/\partial S=e^{-q(T-t)}$ (time-dep.) | $\partial^2 V/\partial S^2=0$ | Dirichlet |
| Upper $S=S_{\max}$ | $V=S_{\max}e^{-q(T-t)}$ (time-dep.) | $\partial V/\partial S=e^{-q(T-t)}$ (time-dep.) | $\partial^2 V/\partial S^2=0$ ✓ | Neumann 2nd |

### Asset-or-Nothing Put — terminal: $S\cdot\mathbf{1}_{S<K}$

| Boundary | Dirichlet | Neumann 1st | Neumann 2nd | QL |
| -------- | --------- | ----------- | ----------- | -- |
| Lower $S=0$ | $V=0$ | $\partial V/\partial S=e^{-q(T-t)}$ (time-dep.) | $\partial^2 V/\partial S^2=0$ ✓ | Neumann 2nd |
| Upper $S=S_{\max}$ | $V=0$ ✓ | $\partial V/\partial S=0$ | $\partial^2 V/\partial S^2=0$ | Dirichlet |

**QuantLib pattern:** Dirichlet where the value is exactly known ($V=0$); Neumann 2nd (linear extrapolation) everywhere else. Simple and universal. Our ✓ choices use first-derivative Neumann where the delta is analytically known — more precise but requires knowing the delta at the boundary.

## Implementation

`BoundaryCondition` class in `pdeParameters.hpp` — `type` (Dirichlet / Neumann1st / Neumann2nd) + `value` (constant double or `std::function<double(double t)>` for time-dependent cases). Constructor overloads for `OptionParameters` and `DigitalOptionParameters` auto-set lower and upper BCs. `explicitMethod()` evaluates `bc.value(t)` at each time step.

#pde #finite-difference #boundary-conditions
