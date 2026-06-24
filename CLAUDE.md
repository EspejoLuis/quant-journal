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

Learning mode is **on by default**. Only switch it off when I explicitly say "learning mode off".

Tutor behaviour:

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
Follow the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) throughout.

### Naming (NL.8, NL.9)

Follows QuantLib convention, which does not conflict with any C++ Core Guidelines hard rule.
NL.8 requires consistency; NL.9 requires `ALL_CAPS` for macros only. Both are met.

| Thing | Convention | Example |
| ----- | ---------- | ------- |
| Files | `lowerCamelCase`, `.hpp` / `.cpp` | `mcEngine.hpp`, `barrier.cpp` |
| Types (class, struct, enum) | `UpperCamelCase` | `class GbmPath` |
| Functions | `lowerCamelCase` | `simulateGbmPath()` |
| Variables | `lowerCamelCase` (or short math names) | `spotPrice`, `S0`, `sigma` |
| Class data members | `lowerCamelCase_` (trailing underscore) | `numSteps_` |
| Constants (`constexpr`) | `lowerCamelCase` | `defaultNumPaths` |
| Namespaces | `lowerCamelCase` | `namespace mcEngine` |
| Macros | `ALL_CAPS` — avoid; use `constexpr` instead | |

### Formatting (NL.4, NL.17)

- **Indentation:** 4 spaces. No tabs. (NL.4 — maintain a consistent indentation style)
- **Line length:** 80 characters max.
- **Braces:** K&R style — opening brace on the same line. (NL.17)

  ```cpp
  if (condition) {
      do_something();
  }
  ```

- **Spaces:** around binary operators, after commas, after keywords (`if`, `for`, `while`).
- One statement per line. No trailing whitespace.

### Header Files (SF.1, SF.8)

- Use `#pragma once` at the top of every `.h` file.
- Include order (one blank line between groups):
  1. Related `.h` for this `.cpp`
  2. C system headers (`<cmath>`, `<cstdlib>`)
  3. C++ standard library (`<vector>`, `<thread>`)
  4. Third-party headers (QuantLib)
  5. Project headers (`"mc_engine.h"`)
- Use angle brackets for system/third-party, quotes for project headers.

### Core Guidelines rules in force

#### Philosophy (P)

- **P.8** — Don't leak resources. Use RAII: `std::vector` for paths and grids, `std::unique_ptr` for owned heap objects.
- **P.9** — Don't waste time or space. Avoid unnecessary copies and allocations inside simulation loops.
- **P.10** — Prefer immutable data. Inputs to path generators (`s0`, `r`, `sigma`) should always be `const`.

#### Functions (F)

- **F.2** — One function = one logical operation. `simulateGbmPath` simulates a path; it does not also compute the payoff.
- **F.3** — Keep functions short and simple. If a function needs a comment to explain its own structure, split it.
- **F.6** — Mark `noexcept` on path generators and payoff functions. They must not throw inside a simulation loop.
- **F.8** — Prefer pure functions. Path generators should be pure functions of their inputs — same seed → same path, no hidden state.
- **F.16** — Pass `double`, `int` by value; pass `std::vector` and grid objects by `const&`.
- **F.20** — Prefer return values over output parameters. Return `std::vector<double>` rather than filling a pointer.
- **F.21** — Return multiple values as a `struct`. Example: a Heston step returning `{s_new, v_new}`.

#### Interfaces (I)

- **I.2** — No non-`const` global variables. All shared state must be passed explicitly or owned by a class.
- **I.5 / I.6** — State preconditions with `Expects()` or `assert`. Example: `assert(sigma > 0.0)` at the top of every path generator.
- **I.11** — Never transfer ownership via a raw pointer. Return by value or use `std::unique_ptr`.
- **I.13** — Don't pass arrays as `double* arr, int n`. Use `std::span<double>` or `std::vector<double>&`.
- **I.23** — Keep the number of function arguments low. Bundle related model parameters into a config struct:

  ```cpp
  struct HestonParams { double v0, kappa, theta, xi, rho; };
  ```

#### Constants and immutability (Con)

- **Con.1** — `const` on every variable that is not updated after initialisation.
- **Con.4** — `constexpr` for all compile-time constants. Never `#define` for numbers.

#### Resource management (R)

- **R.11** — No raw `new`/`delete`. Use `std::make_unique` or `std::vector`.
- **R.20** — `std::unique_ptr` for exclusive ownership; `std::shared_ptr` only when shared ownership is genuinely needed.

#### Concurrency (CP) — active from Heston week onward

- **CP.1** — Design path generators to be thread-safe by being stateless: inputs in, path out, nothing shared.
- **CP.8** — Use `std::atomic<double>` for simple shared accumulators. Use `std::mutex` with `std::lock_guard` for compound updates. Primitive hierarchy (cheapest → heaviest): atomic (single variable, single op) → spinlock (short critical section, low contention) → mutex+CV (longer section or sleep-until-condition).
- **CP.20** — Never share an RNG across threads. Each `std::thread` gets its own seeded `std::mt19937`.
- **Lambda captures in parallel code** — never capture local variables by reference (`[&]`) in a lambda passed to a thread if the variable may go out of scope before the thread finishes. Capture by value (`[=]`) for safety; capture by reference only for objects with guaranteed longer lifetime (e.g. the engine itself).

#### Error handling

- The Core Guidelines recommend exceptions (I.10), but this codebase avoids them for predictability in numerical loops. Use `assert` / `Expects()` for preconditions. Document this at the top of each engine file.

### Testing

- **Framework:** Catch2 (header-only, no build setup required).
- **Rule:** every function gets a test file before or immediately after implementation. No exceptions.
- **Unit tests:** verify the function in isolation — known inputs, expected outputs.
- **Integration tests:** run the full pricer and compare against a closed-form or QuantLib benchmark (tolerance < 0.5%).
- Test files live in `code/cpp/tests/`, named after the source file: `monteCarloEngine.cpp`, `barrier.cpp`, etc.

### Comments (NL.1, NL.2)

- Use `//` for all inline and block comments.
- File header comment (top of every file):

  ```cpp
  // mc_engine.h — Monte Carlo path generators.
  // Grows one function per study block; see CLAUDE.md for the plan.
  ```

- Function comment above the declaration (what it does, not how):

  ```cpp
  // Returns daily prices for a single GBM path of length n_steps.
  std::vector<double> simulateGbmPath(...);
  ```

- No redundant comments — if the name explains it, skip the comment.

### Engines and file structure

```text
code/cpp/src/
├── common/
│   ├── modelParameters.hpp      ← ModelParameters struct (shared by engine + product)
│   ├── optionParameters.hpp     ← OptionType, OptionDirection enums (shared by Vanilla + Digital)
│   ├── mathFunctions.hpp        ← mean, sampleVariance, sampleStdDev, normalCdf, interpolationLinear (header-only inline)
│   ├── simulationParameters.hpp ← SimulationParameters struct + VarianceReduction enum (header-only; shared by all MC engines)
│   ├── pathGeneration.hpp       ← simulateGbmPath(), createRng() as free inline functions (header-only; stateless, reusable by any engine)
│   └── pdeParameters.hpp        ← PdeScheme enum (Explicit/Implicit/CrankNicolson), PdeGrid enum (Uniform/Log), PdeParameters struct, PdeCoefficients struct {a,b,c}
├── engine/
│   ├── engine.hpp               ← abstract base: virtual double price(const Instrument&) = 0
│   ├── monteCarloEngine.hpp     ← McEngine class (derives Engine); stores ModelParameters + SimulationParameters
│   ├── monteCarloEngine.cpp     ← McEngine::price(), validateGbmInputs(); calls simulateGbmPath() from pathGeneration.hpp
│   ├── blackScholesCloseForm.hpp/cpp ← BsCloseForm class; does NOT derive Engine; price() overloads for VanillaEuropeanOption + DigitalEuropeanOption; normalCdf from mathFunctions.hpp
│   ├── chooserEngine.hpp/cpp    ← ChooserEngine; does NOT derive Engine; price(const ChooserEuropeanOption&); simulates to T_c via simulateGbmPath(), BS sub-prices per path, discounts by e^{-rT_c}
│   └── pdeEngine.hpp/cpp        ← PdeEngine derives Engine; stores ModelParameters + PdeParameters; 1D Explicit/Implicit/CN schemes; S_max from GBM 3-sigma; 2D ADI (planned)
└── product/
    ├── instrument.hpp              ← non-pure-virtual base: payoff(path) default throws; products that can express payoff as f(path) override it; others (Chooser, Cliquet) derive but do not override
    ├── vanillaEuropeanOption.hpp/cpp ← derives Instrument; payoff() ternary call/put; parameters() getter returns const OptionParameters&
    ├── digitalEuropeanOption.hpp/cpp ← derives Instrument; payoff() uses indicator + payoutAmount ternaries; BS priced via BsCloseForm::price() overload
    └── chooserEuropeanOption.hpp/cpp ← derives Instrument (does NOT override payoff); ChooserOptionParameters {strike, maturity (=T), direction}; ModelParameters.timeHorizonInYears = T_c; priced via ChooserEngine + BsCloseForm::price() overload
```

**Design principles:**

- `Engine` defines *how* to price (MC, PDE) — pure virtual `price(const Instrument&)`
- `Instrument` defines *what* to price — `payoff(const vector<double>& path) const` is **non-pure virtual** (default throws); most products override it; products needing model params at intermediate dates (Chooser, Cliquet) derive from `Instrument` but do not override `payoff()`
- `ModelParameters` lives in `common/` — shared by engine and instrument, no circular dependency; `timeHorizonInYears` = full option maturity T for all products
- `SimulationParameters` lives in `common/simulationParameters.hpp` — shared by all MC engines; BS/PDE engines don't need it
- `McEngine` stores `ModelParameters` and `SimulationParameters` as private value members, validated in the constructor
- `McEngine::price()` computes discount factor per path inside the loop — ready for stochastic interest rates
- Generic `McEngine` prices products where payoff is a pure function of the path (vanilla, digital, Asian, barrier, lookback, variance swap, rainbow, worst-of, accumulator, autocallable)
- Products requiring mid-path BS sub-pricing (Chooser, Cliquet) get dedicated typed engines (e.g. `ChooserEngine`) — same pattern as `BsCloseForm` not deriving `Engine`
- `ChooserEngine`: `ModelParameters.timeHorizonInYears` = T_c (simulation horizon); `ChooserOptionParameters.maturity` = T (full expiry); simulates to T_c, constructs `ModelParameters` copy per path with `underlyingPrice = S_{T_c}` and `timeHorizonInYears = T - T_c`, discounts by e^{-r*T_c}
- `BsCloseForm` does NOT derive `Engine` — analytic engines need instrument-specific parameters (strike, type) that are not on the `Instrument` interface; forcing them through would require `dynamic_cast`
- `simulateGbmPath` and `createRng` are free inline functions in `common/pathGeneration.hpp` — stateless (CP.1), reusable by any engine, tested through `McEngine::price()` and `ChooserEngine::price()`

**`monteCarloEngine` grows one function per block:**

- `simulateGbmPath()` — GBM (flat vol, Black-Scholes world)
- `simulateLocalVolPath()` — σ(S,t) surface, bilinear interpolation at each Euler step
- `simulateHestonPath()` — Euler-Milstein on (S,v) pair with variance reflection
- `simulateSlvPath()` — Heston variance multiplied by leverage function L(S,t)
- `simulateCorrelatedGbmPath()` — 2-asset GBM via Cholesky decomposition
- `priceLsm()` — Longstaff-Schwartz American MC

**RNG options to consider:** Mersenne Twister (`std::mt19937`) for pseudo-random; Sobol sequences for quasi-Monte Carlo (lower discrepancy, faster convergence).

**Variance reduction techniques to integrate:** antithetic variates, control variates, importance sampling.

**PDE solvers** (in `pdeEngine`):

- 1D Explicit scheme — no system to solve, conditionally stable (CFL condition Δt ≤ ΔS²/(σ²S_max²)); implement first to see stability failure
- 1D Implicit scheme — unconditionally stable, first-order in time; Thomas algorithm tridiagonal solve
- 1D Crank-Nicolson — implicit, second-order in time and space, standard choice; Thomas algorithm tridiagonal solve
- 2D Craig-Sneyd ADI for the Heston/SLV PDE in (S,v)

---

## Active Study Plan

The plan is incremental: each block extends the engine by one capability, then immediately uses it for one or more products. Nothing is built speculatively — every engine function earns its place by pricing something.

### Product checklist

| # | Product | Block | Needs | MC pricing | Status |
| - | ------- | ----- | ----- | ---------- | ------ |
| — | Engine seed (GBM + 1D CN) | A | — | — | [~] |
| 1 | Digital Option (European) | A | GBM | European only | [x] |
| 1b | Digital Option (American) | A+LSM | GBM + LSM | LSM primary | [ ] |
| 2 | Chooser Option | A | GBM | European only | [x] |
| 3 | Asian Option | A | GBM + path avg | European + LSM (American Asian) | [ ] |
| 4 | Barrier Option | A | GBM + barrier check | European + LSM (American barrier) | [ ] |
| 5 | Lookback Option | A | GBM + running max/min | European + LSM (American lookback) | [ ] |
| 6 | Variance Swap | A | GBM + log-return accum | European only | [ ] |
| 7 | Volatility Swap | A | GBM + log-return accum | European only | [ ] |
| — | American MC (LSM) | A | GBM | — | [ ] |
| — | Local Vol engine | B | — | — | [ ] |
| 8 | Range Accrual | B | Local Vol MC | European only | [ ] |
| 9 | Cliquet Option | B | Local Vol MC | European + LSM (American cliquet) | [ ] |
| — | Correlated GBM engine | C | — | — | [ ] |
| 10 | Rainbow Option | C | Cholesky 2-asset | European + LSM (American rainbow) | [ ] |
| 11 | Worst-of Option | C | Cholesky + LSM | European + LSM | [ ] |
| — | Heston engine | D | — | — | [ ] |
| 12 | Accumulator | D | Heston + LSM | LSM primary | [ ] |
| — | SLV engine | E | — | — | [ ] |
| 13 | Autocallable | E | SLV + LSM + AAD | LSM primary | [ ] |

**MC method rationale:**

- *European only* — payoff is determined at T with no meaningful early exercise (realized variance, digital binary)
- *European + LSM* — standard version is European, but the American variant (early exercise) is a natural extension once LSM exists; implement both
- *LSM primary* — the product is inherently Bermudan/path-stopping; European MC gives the wrong price

---

## Weekly Schedule

### Block A — GBM engine + BS products (~14 weeks, Jun 5 – Sep 5)

#### Week 1 — Jun 5–10: Engine Seed

Build the minimum viable engine — one path generator, one PDE solver, one product.

- `mc_engine.h`: `simulateGbmPath(S0, r, sigma, T, N)` returning daily prices. Price a vanilla European call by averaging `max(S_T - K, 0)` over many paths.
- `pde_engine.h`: 1D PDE solver with three schemes — Explicit (conditionally stable, observe CFL failure), Implicit (first-order, unconditionally stable), Crank-Nicolson (second-order, unconditionally stable). Solve the BS PDE backward in time from the call payoff at maturity.
- Verify both against BS closed-form. If both agree to < 0.5%, the engines are correct.

**RNG this week:** `std::mt19937` seeded with `std::random_device`. Understand why the seed matters for reproducibility.

**Variance reduction this week:** antithetic variates — for each path drawn with Z, also price with −Z. Check that it reduces standard error for the same number of paths.

**From the book:** design `mc_engine.h` with parallelism in mind from the start. Each path is independent — think about what that means for thread safety before adding `std::thread` later.

---

#### Week 2 — Jun 10–28: Digital Option + Chooser Option

No new engine code — these are pure payoff changes on top of the GBM engine. PDE engine (1D CN) deferred to Weeks 3–4.

**Digital Option**
Note: `notes/products/digital-option.md`
Code: `code/cpp/products/digital.cpp`

**Theory:** cash-or-nothing payoff P·𝟏_{S_T > K}; risk-neutral pricing as discounted probability; delta spike at K; why you can't hedge a digital with a single vanilla.
**C++:** MC counts paths where S_T > K. BS closed-form: P·e^{-rT}·N(d₂). PDE: step function terminal condition on the 1D CN grid.
**QuantLib:** `CashOrNothingPayoff`, `AnalyticEuropeanEngine`.

**Chooser Option**
Note: `notes/products/chooser-option.md`
Code: `code/cpp/products/chooser.cpp`

**Theory:** at choice date T_c the holder picks max(call, put) with same strike K and expiry T; decomposition via put-call parity into a call expiring at T plus a put expiring at T_c; closed-form under BS.
**C++:** MC prices both call and put at T_c and takes the max. Closed-form as double-check.
**QuantLib:** `SimpleChooserOption`, `AnalyticSimpleChooserEngine`.

---

#### Weeks 3–4 — Jun 29 – Jul 13: PDE Engine + Asian Option

First engine extension: add path-average accumulation. Also complete PDE engine (1D Forward Euler, Backward Euler, Crank-Nicolson) deferred from Week 2.

Note: `notes/products/asian-option.md`
Code: `code/cpp/products/asian.cpp`

**Engine extension:** `simulateGbmPath` already returns all daily prices — accumulate the arithmetic mean inside the payoff loop. No change to the header yet; just a new product file.

**Theory:** arithmetic average payoff max(S_avg − K, 0); why arithmetic average has no closed-form (sum of log-normals is not log-normal); geometric average does — Kemna-Vorst formula.
**C++:** MC tracks path average. Control variate: price geometric average analytically, use as baseline to reduce arithmetic MC variance.
**American Asian (after LSM week):** exercise at any time τ receiving max(A_τ − K, 0). State for LSM regression is 2D: `{1, S, A, S², A², SA}` where A is the running average to date. The path already tracks A_t — adding early exercise is almost free once LSM exists.
**QuantLib:** `DiscreteAveragingAsianOption`, `AnalyticDiscreteGeometricAveragePriceAsianEngine`.

---

#### Weeks 5–6 — Jul 14–29: Barrier Option

Engine extension: barrier condition checking in MC + absorbing BC in PDE.

Note: `notes/products/barrier-option.md`
Code: `code/cpp/products/barrier.cpp`

**Engine extension:** add per-step barrier check inside the GBM path loop. Extend `pde_engine.h` with a Dirichlet absorbing boundary condition at the barrier level.

**Theory:** knock-in / knock-out variants; reflection principle; Rubinstein-Reiner closed-form; in-out parity (knock-in + knock-out = vanilla).
**C++:** MC checks barrier at each step; if hit, path is killed / activated. PDE: absorbing BC at barrier. Compare MC, PDE, Rubinstein-Reiner.
**QuantLib:** `BarrierOption`, `AnalyticBarrierEngine`, `FdBlackScholesBarrierEngine`.

---

#### Weeks 7–8 — Jul 30 – Aug 13: Lookback Option

Engine extension: running max/min tracking + 2D PDE.

Note: `notes/products/lookback-option.md`
Code: `code/cpp/products/lookback.cpp`

**Engine extension:** MC already iterates over daily prices — track running max/min inside the loop. PDE needs to become 2D: grid in (S, M) where M is the running extremum. Extend `pde_engine.h`.

**Theory:** floating and fixed strike variants; Goldman-Sosin-Gatto closed form; why lookbacks are expensive.
**C++:** MC tracks running max/min. 2D PDE with payoff living at the boundary M = S_T.
**QuantLib:** `LookbackOption`, `AnalyticContinuousFixedLookbackEngine`.

---

#### Weeks 9–10 — Aug 14–28: Variance Swap + Volatility Swap

Engine extension: log-return accumulation.

**Variance Swap**
Note: `notes/products/variance-swap.md`
Code: `code/cpp/products/variance_swap.cpp`

**Engine extension:** accumulate squared log-returns along each path — one extra variable per path, no structural change to the engine.

**Theory:** realized variance definition; log-contract replication (Demeterfi-Derman-Kamal-Zou 1999).
**C++:** MC averages squared log-returns; PDE prices the log-contract with 1D CN.
**QuantLib:** `VarSwap` or manual replication check.

**Volatility Swap**
Note: `notes/products/volatility-swap.md`
Code: `code/cpp/products/vol_swap.cpp`

**Theory:** payoff = √(realized var) − K_vol; Jensen's gap; Brockhaus-Long approximation.
**C++:** two-line change from variance swap (take sqrt). No clean PDE — use the approximation.
**QuantLib:** validate MC against the Brockhaus-Long formula.

---

#### Week 11 — Aug 29 – Sep 5: American MC (Longstaff-Schwartz)

Engine extension: add `priceLsm()` to `mc_engine.h`. This is the core technique needed for Worst-of, Accumulator, and Autocallable.

**Algorithm:**

1. Simulate N paths forward (use existing `simulateGbmPath`)
2. At each exercise date, regress estimated continuation value on basis functions of S (e.g. Laguerre polynomials or simple {1, S, S²})
3. Compare immediate exercise value to continuation estimate — exercise if intrinsic > continuation
4. Work backward through exercise dates to time 0

**First test:** American put — compare `priceLsm()` output to the PDE backward induction result (PDE handles American naturally via early exercise condition). If they agree within ~1%, LSM is working.

**Revisit earlier products with American MC once LSM is green:**

- American barrier option — barrier knock-out + early exercise; compare to European barrier price
- American lookback — early exercise on running max/min; significantly more expensive than European

**From the book:** LSM forward simulation is embarrassingly parallel per path. The backward regression pass is NOT simply sequential — in a multi-threaded context it requires a synchronization barrier: all threads pause their path work, one thread runs the regression across all paths, then threads resume. The thread pool + condition variable machinery from Ch. 3 is exactly what implements this barrier correctly.

---

### Block B — Local Vol engine + products (~7 weeks, Sep 1 – Oct 17)

#### Weeks 12–14 — Sep 1–19: Local Volatility (Dupire)

Engine extension: `simulateLocalVolPath()` in `mc_engine.h`.

Note: `notes/models/local-volatility.md`
Code: `code/cpp/models/local_vol.cpp`

**Theory:**

- Dupire's formula — extracting σ(K,T) from a market call surface C(K,T)
- Breeden-Litzenberger — implied risk-neutral density from call prices
- Why local vol is a complete model (unique diffusion matching all European prices)
- The flat-forward problem — local vol smiles flatten at long maturities, intuition why

**Engine extension:** add `simulateLocalVolPath` to `mc_engine.h` — bilinear interpolation on a discrete σ(S,t) grid at each Euler step. Add the Dupire forward PDE to `pde_engine.h` for calibration.

**Variance reduction this week:** control variates — use the BS flat-vol price as the control.
**QuantLib:** `BlackVarianceSurface`, `LocalVolSurface`, `GeneralizedBlackScholesProcess`

---

#### Weeks 15–16 — Sep 20 – Oct 3: Range Accrual

Note: `notes/products/range-accrual.md`
Code: `code/cpp/products/range_accrual.cpp`

**Theory:** coupon accrues on days where L ≤ S ≤ U; decomposition into a digital call spread; why vol skew at the barriers dominates the price.
**C++:** Local vol MC simulates daily and counts barrier hits. PDE adds source term c·𝟏_{L≤S≤U} to the 1D CN solver at each time step.
**QuantLib:** decompose into `CashOrNothingPayoff` digital calls as a check.

---

#### Weeks 17–18 — Oct 4–17: Cliquet Option

Note: `notes/products/cliquet-option.md`
Code: `code/cpp/products/cliquet.cpp`

**Theory:** series of forward-starting options reset at T₁, T₂, …; forward vol; why BS misprices cliquets (Gatheral ch.7).
**C++:** Local vol MC with strike resets at each observation date. PDE is a chain of 1D CN solves stitched at reset dates.
**QuantLib:** `ForwardVanillaOption`, `ForwardPerformanceVanillaOption`.

---

### Block C — Correlated GBM + multi-asset products (~4 weeks, Oct 18 – Nov 14)

#### Weeks 19–20 — Oct 18–31: Rainbow Option

Engine extension: `simulate_correlated_gbm_path()` using Cholesky.

Note: `notes/products/rainbow-option.md`
Code: `code/cpp/products/rainbow.cpp`

**Engine extension:** add a 2-asset correlated GBM function to `mc_engine.h`. Input: two vols σ₁, σ₂ and correlation ρ. Internally: Cholesky-decompose the 2×2 covariance matrix, generate two independent normals, transform them.

**Theory:** payoff max(S₁ − K₁, S₂ − K₂); Margrabe's exchange option formula (K=0 case); correlation risk — how ρ affects the price.
**C++:** MC prices the max payoff. Compare to Margrabe's formula as sanity check.
**QuantLib:** `TwoAssetCorrelationOption` or manual Margrabe.

---

#### Weeks 21–22 — Nov 1–14: Worst-of Option

First use of LSM in a multi-asset setting.

Note: `notes/products/worst-of-option.md`
Code: `code/cpp/products/worst_of.cpp`

**Theory:** payoff based on the worst performer: min(S₁, S₂) − K; why worst-of is cheaper than a single-asset option; correlation sensitivity — worst-of gets cheaper as ρ → 1.
**C++:** reuse the 2-asset correlated GBM. European MC for the plain version. Apply `priceLsm()` for the American-style worst-of — regress on {1, S₁, S₂, S₁², S₂², S₁S₂}.
**QuantLib:** `BasketOption` with `MCAmericanBasketEngine`.

---

### Block D — Heston engine + products (~6 weeks, Nov 15 – Dec 26)

#### Weeks 23–25 — Nov 15 – Dec 5: Heston Model

Engine extension: `simulateHestonPath()` + 2D ADI in `pde_engine.h`.

Note: `notes/models/heston-model.md`
Code: `code/cpp/models/heston.cpp`

**Theory:**

- Two SDEs: stock price + CIR variance process
- Parameters: v₀, κ, θ, ξ, ρ
- Feller condition 2κθ > ξ² — keeps variance positive
- Characteristic function of log(S_T) — closed-form semi-analytic pricer (Heston 1993)

**Engine extension:** `simulateHestonPath` — Euler-Milstein with variance reflection (v_t ← |v_t|). Extend `pde_engine.h` with 2D Craig-Sneyd ADI on a (S,v) grid (~100×50 nodes).

**RNG upgrade:** swap `std::mt19937` for Sobol sequences. Compare convergence: O(1/N) quasi-MC vs O(1/√N) standard MC.

**From the book:** parallelize Heston MC with `std::thread`. Each thread gets its own RNG state. Use `std::atomic<double>` or a mutex to accumulate. Compare 1, 2, 4 threads. Watch for **false sharing** — if per-thread accumulators are packed on the same 64-byte cache line, writing one invalidates the other thread's cache. Fix with `alignas(64)` on accumulator variables.

**QuantLib:** `HestonModel`, `AnalyticHestonEngine`, `FdHestonVanillaEngine`

---

#### Weeks 26–28 — Dec 6–26: Accumulator

LSM + Heston: knock-out is a path-dependent stopping condition, natural fit for LSM.

Note: `notes/products/accumulator.md`
Code: `code/cpp/products/accumulator.cpp`

**Theory:** daily share delivery at a fixed forward price while stock stays above knock-out barrier; leveraged accumulation below forward; risk profile and why these caused losses in 2008.
**C++:** Heston MC with per-step knock-out check. Apply `priceLsm()` — the holder can be thought of as deciding whether to continue accumulating. PDE: 2D Heston grid with Dirichlet absorbing BC at knock-out.
**QuantLib:** `BarrierOption` with `FdHestonBarrierEngine` as partial benchmark.

---

### Block E — SLV engine + Autocallable (~11 weeks, Jan 5 – Mar 20 2027)

#### Weeks 29–32 — Jan 5 – Feb 1: Stochastic Local Vol (SLV)

Engine extension: `simulate_slv_path()` + leverage function calibration.

Note: `notes/models/stochastic-local-vol.md`
Code: `code/cpp/models/slv.cpp`

**Theory:**

- SLV SDE: dS = r S dt + L(S,t)·√v·S·dW^S with v following Heston
- Gyöngy's lemma — basis for the leverage function
- Particle method: simulate ensemble, estimate E[v|S=x] by binning, set L²(x,t) = σ_LV²(x,t) / E[v|S=x]

**Engine extension:** `simulate_slv_path` — pre-computed L(S,t) grid looked up at each step. In `slv.cpp`: calibration loop starting from L=1, iterating to convergence.

**Variance reduction:** importance sampling — shift the drift to concentrate paths near the barrier region.

**From the book:** particle method calibration is embarrassingly parallel up to the binning step. Use a thread pool, synchronise at binning with a mutex.

**QuantLib:** `HestonSLVProcess`, `HestonSLVFokkerPlanckFdmEngine`

---

#### Weeks 33–38 — Feb 2 – Mar 20: Autocallable

LSM + SLV + AAD: the capstone product.

Note: `notes/products/autocallable.md`
Code: `code/cpp/products/autocallable.cpp`

**Theory:** periodic autocall schedule; barrier protection at maturity; memory coupon feature. Needs SLV because it is sensitive to both the marginal distribution (local vol) and the forward dynamics (stochastic vol).
**C++:** SLV MC with per-observation-date autocall and barrier checks. Apply `priceLsm()` for the Bermudan backward induction — at each observation date, regress continuation value and compare to autocall payoff. PDE: 2D ADI with explicit Bermudan induction step.
**From the book:** compute Greeks (delta, vega, theta, barrier digital risk) using AAD — bump-and-reprice is too slow for a full Greek surface on a path-dependent product.
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
