---
type: note
category: engine
tags: [pde, finite-difference, boundary-conditions]
related: [pde-boundary-conditions]
status: scratch
---

# Neumann 2nd — Derivation of V[N] = 2V[N-1] - V[N-2]

Backward second difference at node $N$:

$$\frac{\partial^2 V}{\partial S^2} \approx \frac{V[N] - 2V[N-1] + V[N-2]}{(\Delta S)^2} = 0$$

Multiply both sides by $(\Delta S)^2$:

$$V[N] - 2V[N-1] + V[N-2] = 0$$

Rearrange:

$$V[N] = 2V[N-1] - V[N-2]$$

That is the linear extrapolation formula in `explicitMethod()`.

#pde #finite-difference
