# Stochastic Exponential

## How to get there?

**1.** We are starting from a Brownian motion $W_t$

**2.** Define a function $e^{W_t}$

**3.** Itô's lemma tells us that:

$$
df(x) = f'(x) \, dx + \frac{1}{2} f''(x)(dx)^2
$$

In our case $f'(x) = e^{x}$ and $f''(x) = e^{x}$, so:

$$
de^{W_t} = e^{W_t} \, dW_t + \frac{1}{2} e^{W_t} \, dt
$$

**4.** $e^{W_t}$ is **not** a martingale because of the positive $dt$ drift term.

**5.** To remove the drift, define $X_t = W_t - \frac{1}{2}t$, so $dX_t = dW_t - \frac{1}{2} \, dt$.

**6.** Apply Itô's lemma to $e^{X_t}$ with $f'(x) = f''(x) = e^x$:

$$
de^{X_t} = e^{X_t} \, dX_t + \frac{1}{2} e^{X_t} \, dt
         = e^{X_t} \left(dW_t - \frac{1}{2} \, dt\right) + \frac{1}{2} e^{X_t} \, dt
         = e^{X_t} \, dW_t
$$

The $dt$ terms cancel exactly.

**7.** $e^{W_t - \frac{1}{2}t}$ is a martingale because there is no drift term.

**8.** Let $Z_t = e^{X_t} = e^{W_t - \frac{1}{2}t}$. Then $dZ_t = Z_t \, dW_t$ and $Z_0 = e^{W_0 - 0} = 1$.

## Doléans-Dade exponential

**$\mathcal{E}(M)_t$** is the unique solution to:

$$
dZ_t = Z_t \, dM_t, \quad Z_0 = 1
$$

where $M$ is the driving semimartingale. Since our SDE is driven by $W$:

$$
\mathcal{E}(W)_t = e^{W_t - \frac{1}{2}t}
$$

## General formula

For any semimartingale $X$:

$$
\mathcal{E}(X)_t = \exp\!\left(X_t - X_0 - \frac{1}{2}\langle X \rangle_t\right)
$$

For standard Brownian motion $W_t$ (where $X_0 = 0$ and $\langle W \rangle_t = t$):

$$
\mathcal{E}(W)_t = \exp\!\left(W_t - \frac{1}{2}t\right)
$$

The $-\frac{1}{2}t$ is the Itô correction that cancels the drift and makes it a martingale.

#itoLemma #martingale #doleansDade
