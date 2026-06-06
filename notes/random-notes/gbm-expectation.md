---
type: note
category: math
---

# GBM Expectation Under Risk-Neutral Measure

## The GBM step

The exact discretisation of GBM under risk-neutral measure is:

$$S_{t+\Delta t} = S_t \cdot \exp\!\left[(r - q)\Delta t - \tfrac{1}{2}\sigma^2 \Delta t + \sigma\sqrt{\Delta t}\, Z\right]$$

where $Z \sim \mathcal{N}(0,1)$.

## Taking the expectation

We want $E[S_T]$. By iterating over all steps and using independence:

$$E[S_T] = S_0 \cdot E\!\left[\exp\!\left((r-q)T - \tfrac{1}{2}\sigma^2 T + \sigma\sqrt{T}\, Z\right)\right]$$

This is the expectation of a lognormal. Use the key identity:

$$E[\exp(a + bZ)] = \exp\!\left(a + \tfrac{b^2}{2}\right), \quad Z \sim \mathcal{N}(0,1)$$

**Why?** Because:
$$E[e^{bZ}] = \int_{-\infty}^{\infty} e^{bz} \frac{e^{-z^2/2}}{\sqrt{2\pi}}\, dz = e^{b^2/2}$$

(complete the square in the exponent).

## Applying to GBM

Set $a = (r-q)T - \tfrac{1}{2}\sigma^2 T$ and $b = \sigma\sqrt{T}$:

$$a + \frac{b^2}{2} = (r-q)T - \tfrac{1}{2}\sigma^2 T + \frac{\sigma^2 T}{2}$$

The $\sigma^2$ terms cancel:

$$= (r-q)T$$

Therefore:

$$\boxed{E[S_T] = S_0 \cdot e^{(r-q)T}}$$

This holds for **any** $\sigma$ — the Itô correction $-\tfrac{1}{2}\sigma^2 T$ is precisely designed to cancel $\tfrac{b^2}{2}$, keeping the expectation equal to the forward price.

## Special case: $\sigma = 0$

When $\sigma = 0$, the path is deterministic:

$$S_T = S_0 \cdot e^{(r-q)T}$$

Every path is identical. This is the cleanest unit test for the simulator.

## Implication for testing

- **Unit test:** set $\sigma = 0$, check every path equals $S_0 \cdot e^{(r-q)T}$ exactly.
- **Integration test:** run $N = 100{,}000$ paths with a fixed seed, check the mean of $S_T$ across paths is within 0.5% of $S_0 \cdot e^{(r-q)T}$.

The CLT guarantees the error is $\mathcal{O}(\sigma / \sqrt{N})$ - with $N = 100{,}000$ this is ~0.3% for typical parameters.

#gbm #expectation #lognormal #itoLemma
