---
type: note
category: product
tags: [chooser, options, put-call-parity, exotic]
related: [black-scholes-formula, digital-option]
status: in-progress
---

# Chooser Option

## Intuition

A chooser option gives the holder the right, at a future date $T_c$, to decide whether they want to hold a call or a put — both with the same strike $K$ and expiry $T > T_c$. The value today is the price of that flexibility. Because put-call parity links call and put prices at $T_c$, this flexibility decomposes cleanly into a vanilla call plus a put with a modified strike and shorter maturity — no simulation of the full period to $T$ is needed just to price the choice.

## Math

### Payoff at choice date $T_c$

$$V(T_c) = \max\!\left(C_{BS}(S_{T_c}, K, T - T_c),\; P_{BS}(S_{T_c}, K, T - T_c)\right)$$

### Decomposition via put-call parity

At $T_c$, put-call parity (with continuous dividend yield $q$) gives:

$$P - C = K e^{-r(T-T_c)} - S_{T_c} e^{-q(T-T_c)}$$

Substituting into $\max(C, P) = C + \max(0, P - C)$:

$$V(T_c) = C + \max\!\left(0,\; K e^{-r(T-T_c)} - S_{T_c} e^{-q(T-T_c)}\right)$$

The second term is a put payoff on $S_{T_c}$ with adjusted strike $K^* = K e^{-(r-q)(T-T_c)}$ at $T_c$, scaled by $e^{-q(T-T_c)}$.

### Closed-form price at $t = 0$

$$V_0 = C_{BS}(S_0, K, T, r, q, \sigma) + e^{-qT_c}\, P_{BS}(S_0,\; K e^{-(r-q)(T-T_c)},\; T_c,\; r, q, \sigma)$$

- First term: vanilla call, full maturity $T$, strike $K$.
- Second term: put expiring at $T_c$, strike reduced to $K^* = K e^{-(r-q)(T-T_c)}$ (future value of $K$ from $T$ discounted back to $T_c$), scaled by $e^{-qT_c}$.

For $q = 0$ this simplifies to:

$$V_0 = C_{BS}(S_0, K, T) + P_{BS}(S_0,\; K e^{-r(T-T_c)},\; T_c)$$

### Why the put strike is adjusted

At $T_c$, the holder compares the call and put on equal footing. For the put to be worth more than zero, $S_{T_c}$ must be below the *forward value* of $K$ discounted to $T_c$ — hence the strike $K^*$ is $K$ deflated by the drift from $T_c$ to $T$.

## C++ notes

**MC implementation:** simulate a GBM path to $T_c$ using the existing `simulateGbmPath`. At $T_c$, compute BS call and put prices analytically using `BsCloseForm`. Take the max. Discount back to 0. No simulation past $T_c$ needed.

**Alternatively (full MC to $T$):** simulate to $T$, but record $S_{T_c}$ along the path. At $T_c$, apply the decomposition payoff: call payoff at $T$ plus the put payoff at $T_c$ on the discounted strike. Equivalent, but requires tracking an intermediate path value.

**Closed-form check:** compute using the formula above directly — call `BsCloseForm::price` twice with the modified parameters and sum.

**No new engine extension required** — the GBM engine and BS close form already exist. This is purely a payoff file.

## QuantLib benchmark

- `SimpleChooserOption` with `AnalyticSimpleChooserEngine`.
- Cross-check MC and closed-form against QuantLib at < 0.5% tolerance.

## Greeks / sensitivities

| Greek | Behaviour |
| ----- | --------- |
| Delta $\Delta$ | Between call delta and put delta; sign depends on which the holder is likely to choose |
| Vega $\mathcal{V}$ | Higher than either vanilla alone — uncertainty about which to choose increases with vol |
| Theta $\Theta$ | Accelerates as $T_c$ approaches; after $T_c$ behaves like whichever was chosen |

---

#chooser #options #exotic #put-call-parity
