---
type: note
category: formula
tags: [normal-distribution, erf, calculus]
related: [black-scholes-formula]
status: complete
---

# Normal CDF via erf (error function)

## Derivation

$$N(x) = \frac{1}{\sqrt{2\pi}} \int_{-\infty}^{x} e^{-t^2/2} \, dt$$

Split at 0 (using symmetry of the standard normal, the left half integrates to $\frac{1}{2}$):

$$N(x) = \frac{1}{2} + \frac{1}{\sqrt{2\pi}} \int_{0}^{x} e^{-t^2/2} \, dt$$

Substitute $u = t/\sqrt{2}$, so $t = u\sqrt{2}$ and $dt = \sqrt{2}\, du$.

**Limits:** when $t = 0$, $u = 0$; when $t = x$, $u = x/\sqrt{2}$. This is where the upper limit changes.

**Integrand:** $e^{-t^2/2} = e^{-(u\sqrt{2})^2/2} = e^{-u^2}$.

**Measure:** $dt = \sqrt{2}\, du$, and $\sqrt{2}$ cancels with the $\sqrt{2}$ inside $\sqrt{2\pi}$:

$$\frac{\sqrt{2}}{\sqrt{2\pi}} = \frac{1}{\sqrt{\pi}}$$

Putting it together:

$$N(x) = \frac{1}{2} + \frac{1}{\sqrt{\pi}} \int_{0}^{x/\sqrt{2}} e^{-u^2}\, du$$

By definition $\text{erf}(z) = \frac{2}{\sqrt{\pi}} \int_{0}^{z} e^{-u^2}\, du$, so $\int_{0}^{z} e^{-u^2}\, du = \frac{\sqrt{\pi}}{2}\,\text{erf}(z)$. Therefore:

$$N(x) = \frac{1}{2} + \frac{1}{\sqrt{\pi}} \cdot \frac{\sqrt{\pi}}{2}\,\text{erf}\!\left(\frac{x}{\sqrt{2}}\right) = \frac{1}{2}\left(1 + \text{erf}\!\left(\frac{x}{\sqrt{2}}\right)\right)$$

## Result

$$\boxed{N(x) = \frac{1}{2}\left(1 + \text{erf}\!\left(\frac{x}{\sqrt{2}}\right)\right)}$$

## C++ implementation

```cpp
#include <cmath>

double normalCdf(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}
```

### Numerical stability note

For large negative $x$, `1 + erf(x)` suffers cancellation (both terms near zero). The `erfc` version avoids this:

$$N(x) = \frac{1}{2}\,\text{erfc}\!\left(-\frac{x}{\sqrt{2}}\right)$$

```cpp
double normalCdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}
```

---

#normal-distribution #erf
