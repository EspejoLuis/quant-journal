---
type: note
category: engine
tags: [pde, finite-difference, boundary-conditions]
related: [pde-engine, pde-fd-coefficients]
status: in-progress
---

# PDE Boundary Conditions

## Types

| Type | Formula | Time-dependent? | Implementation |
| ---- | ------- | --------------- | -------------- |
| Dirichlet (constant) | $V = c$ | No | `V[0] = c` or `V[N] = c` |
| Dirichlet (time-dep.) | $V = f(t)$ | Yes | `V[0] = f(t)` each step. Examples: vanilla put lower $Ke^{-r(T-t)}$, CoN Dirichlet $Pe^{-r(T-t)}$ |
| Neumann 1st (constant) | $\partial V/\partial S = c$ | No | `V[0] = V[1] - c*dS`, `V[N] = V[N-1] + c*dS`. $c=0$: flat |
| Neumann 1st (time-dep.) | $\partial V/\partial S = g(t)$ | Yes | `V[N] = V[N-1] + g(t)*dS` each step. Example: AoN $e^{-q(T-t)}$ |
| Neumann 2nd | $\partial^2 V/\partial S^2 = 0$ | No | `V[N] = 2*V[N-1] - V[N-2]` (linear extrapolation). QuantLib default |

Time-dependent BCs require `std::function<double(double t)>` in `BoundaryCondition`, evaluated at each step. `ModelParameters` ($r$, $q$, $T$) must be captured at construction to build the lambda.

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
