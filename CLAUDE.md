# Quant Journal — Claude Context

## What this repo is

A personal quant self-study journal. It combines:

- Markdown notes in `notes/` (rendered and linked in VS Code with Foam).
- Daily logs in `log/`
- C++ implementations in `code/cpp/` — written by me, not by Claude

**Claude's role here:** help me understand theory, review my C++ implementations, check my math, suggest test cases, explain QuantLib APIs, and discuss trade-offs. Do not write full implementations for me unless I explicitly ask.

- Since I'm also reviewing [[Modern-Computational-Finance-AAD-and-Parallel-Simulations]], integrate points from there into the study/planning session. For example, use concepts of threading, mutex, atomic and all the concepts for C++ you find there.

---

## Learning Mode

When I say "learning mode on", switch to tutor behaviour:

- Ask me what I think before explaining anything.
- Let me attempt the implementation first, then correct or validate.
- If I'm wrong, ask a question that leads me to the right answer rather than giving it directly.
- Break problems into small steps and check my understanding at each one.
- Challenge my reasoning — if I give an answer, ask me why.
- Only write code if I'm genuinely stuck after multiple attempts, and even then write the minimum needed.
- After I solve something, ask me if I can generalise it or think of edge cases.
- Suggest ideas and directions, don't just answer.

When I say "learning mode off", go back to normal behaviour.

The goal is that I understand what I built, not just that the code works.

---

## C++ Conventions

All code lives under `code/cpp/`.

Two shared header-only engines are built incrementally:

**`code/cpp/mc_engine.h`** — Monte Carlo path generators.
Added one function per model as study progresses:

- GBM path (Black-Scholes, flat vol) — engine week
- Local vol path — σ(S,t) looked up via bilinear interpolation at each Euler step
- Heston path — Euler-Milstein on (S,v) pair with variance reflection
- SLV path — Heston variance multiplied by leverage function L(S,t)

**RNG options to consider:** Mersenne Twister (`std::mt19937`) for pseudo-random; Sobol sequences for quasi-Monte Carlo (lower discrepancy, faster convergence).

**Variance reduction techniques to integrate:** antithetic variates, control variates, importance sampling.

**`code/cpp/pde_engine.h`** — Finite-difference solvers:

- 1D Crank-Nicolson with Thomas algorithm (tridiagonal solve)
- 2D Craig-Sneyd ADI for the Heston/SLV PDE in (S,v)

**Model and product files** are standalone `.cpp` files that `#include` the two headers above and nothing else. Each compiles and runs with:

```bash
g++ -std=c++17 <file>.cpp -o out && ./out
```

---

## Active Study Plan

### Track 1 — Volatility Models (~10 weeks, starting Thu Jun 5 2026)

| # | Model | Note | Code | Status |
|---|-------|------|------|--------|
| 1 | Local Volatility (Dupire) | `notes/models/local-volatility.md` | `code/cpp/models/local_vol.cpp` | [ ] |
| 2 | Heston Model | `notes/models/heston-model.md` | `code/cpp/models/heston.cpp` | [ ] |
| 3 | Stochastic Local Vol (SLV) | `notes/models/stochastic-local-vol.md` | `code/cpp/models/slv.cpp` | [ ] |

### Track 2 — Exotic Products (~19 weeks, starting ~Aug 2026)

| # | Product | Note | Code | Needs | Status |
|---|---------|------|------|-------|--------|
| 1 | Variance Swap | `notes/products/variance-swap.md` | `code/cpp/products/variance_swap.cpp` | BS | [ ] |
| 2 | Lookback Option | `notes/products/lookback-option.md` | `code/cpp/products/lookback.cpp` | BS | [ ] |
| 3 | Volatility Swap | `notes/products/volatility-swap.md` | `code/cpp/products/vol_swap.cpp` | BS | [ ] |
| 4 | Range Accrual | `notes/products/range-accrual.md` | `code/cpp/products/range_accrual.cpp` | Local Vol | [ ] |
| 5 | Cliquet Option | `notes/products/cliquet-option.md` | `code/cpp/products/cliquet.cpp` | Local Vol | [ ] |
| 6 | Accumulator | `notes/products/accumulator.md` | `code/cpp/products/accumulator.cpp` | Heston | [ ] |
| 7 | Autocallable | `notes/products/autocallable.md` | `code/cpp/products/autocallable.cpp` | SLV | [ ] |

---

## Weekly Schedule

### Track 1 — Volatility Models

#### Week 1 — Jun 5–7: Engine Foundations

Build the two shared headers from scratch.

- `mc_engine.h`: one function `simulate_gbm_path(S0, r, sigma, T, N)` returning daily prices. Price a vanilla European call by averaging `max(S_T - K, 0)` over many paths.
- `pde_engine.h`: 1D Crank-Nicolson solver. Solve the BS PDE backward in time from the call payoff at maturity.
- Verify both against BS closed-form. If both agree to < 0.5%, the engines are correct.

**RNG this week:** use `std::mt19937` seeded with `std::random_device`. Understand why the seed matters for reproducibility.

**Variance reduction this week:** implement antithetic variates in the GBM pricer — for each path drawn with Z, also price with −Z. Check that it reduces standard error for the same number of paths.

**From the book:** structure `mc_engine.h` with parallelism in mind from the start. Each path is independent — think about what that means for thread safety before adding `std::thread` later.

Note about PDE will be needed!

---

#### Weeks 2–3 — Jun 10–20: Local Volatility (Dupire)

Note: `notes/models/local-volatility.md`
Code: `code/cpp/models/local_vol.cpp`

**Theory:**

- Dupire's formula — extracting σ(K,T) from a market call surface C(K,T)
- Breeden-Litzenberger — implied risk-neutral density from call prices
- Why local vol is a complete model (unique diffusion matching all European prices)
- The flat-forward problem — local vol smiles flatten at long maturities, intuition why

**C++:**

- Extend `mc_engine.h` with `simulate_local_vol_path` — bilinear interpolation on a discrete σ(S,t) grid at each Euler step
- In `local_vol.cpp`: build a toy skewed vol surface, run MC, run 1D PDE, compare to QuantLib

**Variance reduction this week:** add control variates — use the BS flat-vol price as the control. The local vol price should be close to it; the control variate removes the common variance.

**QuantLib:** `BlackVarianceSurface`, `LocalVolSurface`, `GeneralizedBlackScholesProcess`

---

#### Weeks 4–6 — Jun 23 – Jul 11: Heston Model

Note: `notes/models/heston-model.md`
Code: `code/cpp/models/heston.cpp`

**Theory:**

- Two SDEs: stock price + CIR variance process
- Parameters: v₀, κ (mean reversion), θ (long-run variance), ξ (vol of vol), ρ (correlation)
- Feller condition 2κθ > ξ² — keeps variance positive; what breaks if violated
- Characteristic function of log(S_T) — closed-form
- Semi-analytic vanilla pricer via inverse Fourier transform (Heston 1993)

**C++:**

- Extend `mc_engine.h` with `simulate_heston_path` using Euler and Milstein. If v_t goes negative, reflect: v_t ← |v_t|
- Extend `pde_engine.h` with 2D Craig-Sneyd ADI on a (S,v) grid (~100×50 nodes). Splits the 2D step into two 1D tridiagonal solves per time step
- In `heston.cpp`: implement the characteristic function pricer as baseline, compare MC and ADI PDE against QuantLib

**RNG upgrade this week:** swap `std::mt19937` for Sobol sequences and compare convergence. Quasi-MC converges as O(1/N) vs O(1/√N) for standard MC — you should see it clearly in a convergence plot.

**From the book:** parallelize the Heston MC using `std::thread`. Each thread gets its own RNG state (never share an RNG across threads). Use `std::atomic<double>` or a mutex to accumulate results safely. Compare wall-clock time with 1, 2, 4 threads.

**QuantLib:** `HestonModel`, `HestonProcess`, `AnalyticHestonEngine`, `FdHestonVanillaEngine`

---

#### Weeks 7–10 — Jul 14 – Aug 8: Stochastic Local Vol (SLV)

Note: `notes/models/stochastic-local-vol.md`
Code: `code/cpp/models/slv.cpp`

**Theory:**

- Motivation: local vol fits surface exactly but distorts forward smiles; Heston has better dynamics but doesn't fit surface. SLV combines both
- SLV SDE: dS = r S dt + L(S,t)·√v·S·dW^S with v following Heston
- Gyöngy's lemma — basis for the leverage function: any Itô process has the same marginals as a 1D diffusion
- Calibration: find L(S,t) such that SLV reproduces the full implied vol surface exactly
- Particle method — simulate a large ensemble, at each step estimate E[v|S=x] by binning, set L²(x,t) = σ_LV²(x,t) / E[v|S=x]

**C++:**

- Extend `mc_engine.h` with `simulate_slv_path` — pre-computed L(S,t) grid, looked up at each step
- In `slv.cpp`: demo the calibration loop. Start with L=1 (pure Heston), run paths, estimate E[v|S], update L, iterate to convergence

**Variance reduction this week:** importance sampling — shift the drift to concentrate paths near the region of interest (e.g. near the barrier for barrier products). Requires a Radon-Nikodym weight per path.

**From the book:** the particle method calibration loop is embarrassingly parallel up to the binning step. Use a thread pool pattern to simulate the ensemble, then synchronise at the binning step with a mutex.

**QuantLib:** `HestonSLVProcess`, `HestonSLVFokkerPlanckFdmEngine`

---

### Track 2 — Exotic Products

#### Weeks 11–12 — Aug 11–22: Variance Swap

Note: `notes/products/variance-swap.md`
Code: `code/cpp/products/variance_swap.cpp` | Needs: BS

**Theory:** realized variance definition; log-contract replication (Demeterfi-Derman-Kamal-Zou 1999); why variance swaps can be replicated with a strip of vanillas.
**C++:** MC averages squared log-returns; PDE prices the log-contract with 1D CN.
**QuantLib:** `VarSwap` or manual replication check.

---

#### Weeks 13–15 — Aug 25 – Sep 12: Lookback Option

Note: `notes/products/lookback-option.md`
Code: `code/cpp/products/lookback.cpp` | Needs: BS

**Theory:** floating and fixed strike variants; Goldman-Sosin-Gatto closed form; why lookbacks are expensive.
**C++:** MC tracks running max/min per path. PDE is 2D in (S, M) where M is the running maximum.
**QuantLib:** `LookbackOption`, `AnalyticContinuousFixedLookbackEngine`.

---

#### Week 16 — Sep 15–20: Volatility Swap

Note: `notes/products/volatility-swap.md`
Code: `code/cpp/products/vol_swap.cpp` | Needs: BS

**Theory:** payoff = √(realized var) − K_vol; Jensen's gap; Brockhaus-Long approximation K_vol ≈ √(K_var) − Var/(8 K_var^(3/2)).
**C++:** MC is a two-line change from variance swap (take sqrt). No clean PDE — use the approximation instead.
**QuantLib:** validate MC against the Brockhaus-Long formula.

---

#### Weeks 17–18 — Sep 22 – Oct 3: Range Accrual

Note: `notes/products/range-accrual.md`
Code: `code/cpp/products/range_accrual.cpp` | Needs: Local Vol

**Theory:** coupon accrues on days where L ≤ S ≤ U; decomposition into a digital call spread; why vol skew at the barriers dominates the price.
**C++:** MC simulates daily and counts barrier hits. PDE adds an inhomogeneous source term c·𝟏_{L≤S≤U} to the 1D CN solver at each time step.
**QuantLib:** decompose into `CashOrNothingPayoff` digital calls as a check.

---

#### Weeks 19–21 — Oct 6–24: Cliquet Option

Note: `notes/products/cliquet-option.md`
Code: `code/cpp/products/cliquet.cpp` | Needs: Local Vol

**Theory:** series of forward-starting options reset at T₁, T₂, …; forward vol; why BS misprices cliquets (Gatheral ch.7).
**C++:** MC with local vol, strike resets at each observation date. PDE is a chain of 1D CN solves stitched at reset dates.
**QuantLib:** `ForwardVanillaOption`, `ForwardPerformanceVanillaOption`.

---

#### Weeks 22–23 — Oct 27 – Nov 7: Accumulator

Note: `notes/products/accumulator.md`
Code: `code/cpp/products/accumulator.cpp` | Needs: Heston

**Theory:** daily share delivery at a fixed forward price while stock stays above knock-out barrier; leveraged accumulation below forward; risk profile and why these caused losses in 2008.
**C++:** Heston MC with per-step knock-out check. PDE uses the 2D Heston grid with Dirichlet absorbing BC at knock-out level.
**QuantLib:** `BarrierOption` with `FdHestonBarrierEngine` as partial benchmark.

---

#### Weeks 24–29 — Nov 10 – Dec 19: Autocallable

Note: `notes/products/autocallable.md`
Code: `code/cpp/products/autocallable.cpp` | Needs: SLV

**Theory:** periodic autocall schedule (knock-out coupon if S > autocall level); barrier protection at maturity; memory coupon feature. Needs SLV because it is sensitive to both the marginal distribution (local vol) and the forward dynamics (stochastic vol).
**C++:** SLV MC with per-observation-date autocall and barrier checks. PDE is 2D ADI with Bermudan backward induction — at each observation date replace continuation value with autocall payoff where triggered.
**From the book:** compute Greeks (delta, vega, theta, barrier digital risk) using AAD — bump-and-reprice is too slow for a full Greek surface on a path-dependent product. AAD gives all Greeks in roughly the same time as one price.
**QuantLib:** `FdHestonBarrierEngine` + custom payoff as rough benchmark.

---

## Per-Note Deliverables

Each note in `notes/models/` or `notes/products/` must contain:

1. **Intuition** — one paragraph, no formulas
2. **Math** — SDE / PDE / key formulas with derivation steps
3. **C++ notes** — discretisation scheme, variance reduction used, grid dimensions
4. **QuantLib benchmark** — class and engine used, how output was compared
5. **Greeks / sensitivities**

## Notes Style

- YAML frontmatter: `type`, `category`, `tags`, `related`, `status`
- Wikilinks `[[note-name]]` for Foam graph
- `#tag` at the bottom for Foam filtering


[Modern-Computational-Finance-AAD-and-Parallel-Simulations]: books/Modern-Computational-Finance-AAD-and-Parallel-Simulations.md "AAD and // Simulation"