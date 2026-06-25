---
type: note
category: engine
tags: [pde, finite-difference, coefficients, log-grid, uniform-grid]
related: [pde-engine]
status: in-progress
---

# PDE Finite-Difference Coefficients

The BS PDE is discretised in space using second-order central differences. The resulting coefficients $a_i$, $b_i$, $c_i$ depend on which space variable is used — $S$ (uniform grid) or $x = \log S$ (log grid). The two forms have different structure and different stability properties.

## Uniform grid — $S$-space

Starting from the BS PDE:

$$\frac{\partial V}{\partial t} + \frac{1}{2}\sigma^2 S^2 \frac{\partial^2 V}{\partial S^2} + (r-q) S \frac{\partial V}{\partial S} - rV = 0$$

Central differences on uniform step $\Delta S$:

$$\frac{\partial V}{\partial S} \approx \frac{V_{i+1} - V_{i-1}}{2\Delta S}, \qquad \frac{\partial^2 V}{\partial S^2} \approx \frac{V_{i+1} - 2V_i + V_{i-1}}{(\Delta S)^2}$$

Collecting terms by $V_{i-1}$, $V_i$, $V_{i+1}$:

$$a_i = \frac{\sigma^2 S_i^2}{2(\Delta S)^2} - \frac{(r-q) S_i}{2\Delta S}$$

$$b_i = -\frac{\sigma^2 S_i^2}{(\Delta S)^2} - r$$

$$c_i = \frac{\sigma^2 S_i^2}{2(\Delta S)^2} + \frac{(r-q) S_i}{2\Delta S}$$

**Key property:** $a_i$, $b_i$, $c_i$ vary node by node through $S_i^2$ and $S_i$. The local diffusion $D_i = \frac{1}{2}\sigma^2 S_i^2$ is largest at $S_{\max}$, driving the CFL constraint.

## Log grid — $x$-space ($x = \log S$)

### Change of variables

With $x = \log S$ (so $S = e^x$), apply the chain rule to the S-derivatives:

$$\frac{dV}{dS} = \frac{dV}{dx} \cdot \frac{dx}{dS} = \frac{1}{S} \frac{\partial V}{\partial x}$$

$$\frac{d^2V}{dS^2} = \frac{d}{dS}\!\left[\frac{1}{S}\frac{\partial V}{\partial x}\right] = -\frac{1}{S^2}\frac{\partial V}{\partial x} + \frac{1}{S^2}\frac{\partial^2 V}{\partial x^2}$$

Substituting into the BS PDE — the $S^2$ and $S$ factors cancel:

$$S^2 \frac{\partial^2 V}{\partial S^2} = \frac{\partial^2 V}{\partial x^2} - \frac{\partial V}{\partial x}, \qquad S \frac{\partial V}{\partial S} = \frac{\partial V}{\partial x}$$

The PDE in $x$-space:

$$\frac{\partial V}{\partial t} + \frac{\sigma^2}{2}\frac{\partial^2 V}{\partial x^2} + \left(r - q - \frac{\sigma^2}{2}\right)\frac{\partial V}{\partial x} - rV = 0$$

### Coefficients

With uniform log-space step $\Delta x = (\log S_{\max} - \log S_{\min})/N$:

$$a_i = \frac{\sigma^2}{2(\Delta x)^2} - \frac{r - q - \frac{\sigma^2}{2}}{2\Delta x}$$

$$b_i = -\frac{\sigma^2}{(\Delta x)^2} - r$$

$$c_i = \frac{\sigma^2}{2(\Delta x)^2} + \frac{r - q - \frac{\sigma^2}{2}}{2\Delta x}$$

**Key property:** $a_i$, $b_i$, $c_i$ are **constant** — no $S_i$ dependence. Same coefficient at every node.

## Comparison

| | Uniform ($S$-space) | Log ($x$-space) |
|-|---------------------|-----------------|
| Coefficients | Vary with $S_i^2$, $S_i$ | Constant across all nodes |
| `diffusionTerm` (FD) | $\dfrac{\sigma^2 S_i^2}{2(\Delta S)^2}$ | $\dfrac{\sigma^2}{2(\Delta x)^2}$ |
| `driftTerm` (FD) | $\dfrac{(r-q)\,S_i}{2\,\Delta S}$ | $\dfrac{r - q - \frac{\sigma^2}{2}}{2\,\Delta x}$ |
| CFL constraint | $\Delta t \leq (\Delta S)^2 / (\sigma^2 S_{\max}^2)$ | $\Delta t \leq (\Delta x)^2 / \sigma^2$ |
| Accuracy near $S=0$ | Poor (coarse in log-space) | Good (uniform in log-space) |

## C++ Notes

`computeCoefficients()` must branch on `pdeParams_.grid`:

- **Uniform:** use S-space formulas with `grid_.spaceDelta` = $\Delta S$ and `grid_.spaceGrid[i]` = $S_i$.
- **Log:** use x-space formulas with `grid_.spaceDelta` = $\Delta x$; coefficients are constant, no loop over $S_i$ needed (or loop with same value).

#pde #finite-difference #coefficients
