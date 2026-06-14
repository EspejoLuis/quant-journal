---
type: note
category: product
tags: [digital, binary, options, risk-neutral, measure-change]
related: [black-scholes-formula, chooser-option]
status: in-progress
---

# Digital Option (European)

## Intuition

A digital option pays a fixed amount if the underlying ends up on the right side of the strike at expiry, and nothing otherwise. Unlike a vanilla, the payoff does not grow with how far in the money the option is — it is either all or nothing. This makes the price a pure probability (discounted), and the delta a spike at the strike rather than a smooth curve.

## Math

### Payoff

Two independent dimensions:

**Call vs put** — which side of the strike triggers payment:

$$\mathbf{1}_{S_T \geq K} \quad \text{(call)}, \qquad \mathbf{1}_{S_T < K} \quad \text{(put)}$$

**Asset-or-nothing vs cash-or-nothing** — what is paid:

$$\text{asset-or-nothing: } S_T, \qquad \text{cash-or-nothing: } K$$

Full payoff (call, long, for each type):

$$\text{Asset-or-nothing call: } S_T \cdot \mathbf{1}_{S_T \geq K}$$

$$\text{Cash-or-nothing call: } K \cdot \mathbf{1}_{S_T \geq K}$$

### Black-Scholes closed-form prices

Using the same $d_1$, $d_2$ as the vanilla BS formula:

| Type | Call (Long) | Put (Long) |
| ---- | ----------- | ---------- |
| Asset-or-nothing | $S_0 e^{-qT} N(d_1)$ | $S_0 e^{-qT} N(-d_1)$ |
| Cash-or-nothing | $K e^{-rT} N(d_2)$ | $K e^{-rT} N(-d_2)$ |

Short positions flip the sign.

### Why $d_1$ for asset, $d_2$ for cash

- **Cash-or-nothing:** price $= e^{-rT} \mathbb{E}^{\mathbb{Q}}[K \cdot \mathbf{1}_{S_T \geq K}] = K e^{-rT} \mathbb{Q}(S_T \geq K) = K e^{-rT} N(d_2)$.  
  $N(d_2)$ is the risk-neutral probability of finishing in the money.

- **Asset-or-nothing:** price $= e^{-rT} \mathbb{E}^{\mathbb{Q}}[S_T \cdot \mathbf{1}_{S_T \geq K}]$.  
  $S_T$ inside the expectation prevents factoring out $\mathbb{Q}(S_T \geq K)$ directly. Changing numeraire to the stock measure $\mathbb{S}$ (Radon-Nikodym: $S_T e^{-rT} / S_0$) shifts the drift by $+\sigma^2$, giving $\mathbb{S}(S_T \geq K) = N(d_1)$.  
  Price $= S_0 e^{-qT} N(d_1)$.

### Put-call parity for digitals

Indicators partition the real line: $\mathbf{1}_{S_T \geq K} + \mathbf{1}_{S_T < K} = 1$ for all $S_T$.

$$C + P = S_0 e^{-qT} \quad \text{(asset-or-nothing)}$$

$$C + P = K e^{-rT} \quad \text{(cash-or-nothing)}$$

This is a **sum** identity, not a difference — unlike the vanilla $C - P = S_0 e^{-qT} - K e^{-rT}$.

### Vanilla as difference of digitals

A vanilla call decomposes into one asset-or-nothing call minus $K$ cash-or-nothing calls:

$$C_{\text{vanilla}} = C_{\text{AoN}} - K \cdot C_{\text{CoN}}$$

$$\max(S_T - K, 0) = S_T \cdot \mathbf{1}_{S_T \geq K} - K \cdot \mathbf{1}_{S_T \geq K}$$

This links the two digital types back to the vanilla and provides a consistency check.

### Boundary convention

At $S_T = K$ exactly (probability zero under GBM): the call uses $\geq$ (claims the boundary), the put uses $<$. Ensures $\mathbf{1}_{\text{call}} + \mathbf{1}_{\text{put}} = 1$ everywhere.

## C++ notes

**Payoff:** two independent ternaries — `indicator` (call: `S_T >= K`, put: `S_T < K`) and `payoutAmount` (asset: `S_T`, cash: `K`). Return `sign * indicator * payoutAmount`. No nested switch needed.

**MC:** `McEngine::price()` counts paths where the indicator fires; `payoff()` returns the payout amount per path. No structural change to the engine.

**Files:** [digitalEuropeanOption.h](../../code/cpp/src/product/digitalEuropeanOption.h), [digitalEuropeanOption.cpp](../../code/cpp/src/product/digitalEuropeanOption.cpp).

**BS close form:** `BsCloseForm::price(const DigitalEuropeanOption&)` overload — computes `payoutFactor` ($S_0 e^{-qT}$ vs $K e^{-rT}$) and `dFactor` ($\pm d_1$ vs $\pm d_2$) independently.

## QuantLib benchmark

- `CashOrNothingPayoff` / `AssetOrNothingPayoff` with `AnalyticEuropeanEngine`.
- Cross-check MC and BS closed-form at < 1% tolerance (MC) and < 0.01% (BS vs QuantLib).

## Greeks / sensitivities

| Greek | Behaviour |
| ----- | --------- |
| Delta $\Delta$ | Spike at $K$ — very large near the strike, zero elsewhere. Impossible to hedge with a single vanilla. |
| Gamma $\Gamma$ | Dirac delta at $K$ in the limit — extreme, sign-changing at the strike. |
| Vega $\mathcal{V}$ | Negative near ATM for a call — higher vol spreads the distribution, reducing the probability of finishing just above $K$. |

---

#digital #binary #options #exotic #risk-neutral
