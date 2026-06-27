---
type: note
category: engine
tags: [architecture, quantlib, engine, instrument, template, pricing]
related: [pde-engine, mc-engine, black-scholes-formula]
status: done
---

# Engine / Instrument Architecture

## Intuition

The pricing flow is driven by the **instrument**, not the engine — exactly as QuantLib does it.
The instrument says "I need a price"; it hands its parameters to whatever engine has been
registered; the engine runs its calculation; the instrument reads the result. The engine never
needs to know which product it is pricing in advance — the product fills the engine's argument
slot before `calculate()` is called.

Engines are **class templates** parameterised by the instrument type `T`. Each engine stores
`typename T::Arguments args_` as a typed value member, so `getArguments()` returns a pointer to
the exact derived type. `setArguments` can therefore cast to `T::Arguments*` with a single
`dynamic_cast` that always succeeds.

## Design

### `Engine` (abstract base)

```cpp
class Engine {
public:
    struct Arguments { virtual ~Arguments() = default; };
    virtual Engine::Arguments* getArguments() = 0;
    virtual void calculate() = 0;
    virtual double getResult() const = 0;
    virtual ~Engine() = default;
};
```

### `Instrument` (concrete base)

NVI (Non-Virtual Interface) — `price()` is public non-virtual; `setArguments()` is private virtual.
Derived classes override `setArguments`; callers outside the class cannot invoke it directly.

```cpp
class Instrument {
public:
    double price();                  // drives the flow — not virtual
    void setPricingEngine(Engine*);
    virtual ~Instrument() = default;
private:
    virtual void setArguments(Engine::Arguments*) const;  // default throws
    Engine* engine_ = nullptr;
};
```

`price()` flow:

```cpp
Engine::Arguments* args = engine_->getArguments();
setArguments(args);
engine_->calculate();
return engine_->getResult();
```

### `T::Arguments` (instrument level)

Each instrument defines its own nested Arguments type inheriting from `Engine::Arguments`.
The key method is a **scalar payoff** `double payoff(double S) const` — mirrors
QuantLib's `Payoff::operator()(double)`:

```cpp
struct VanillaEuropeanOption::Arguments : Engine::Arguments {
    OptionParameters engineParams;
    double payoff(double S) const;
};
```

How each engine uses `payoff(S)`:

| Engine | Call |
|--------|------|
| `McEngine<T>` | `args_.payoff(path.back())` |
| `PdeEngine<T>` | `args_.payoff(spaceGrid[i])` for terminal condition |
| `BsCloseForm<T>` | reads `args_.engineParams` directly — no `payoff(S)` call |

`ChooserEuropeanOption::Arguments` omits `payoff(double S)` — the chooser payoff requires
a mid-path BS sub-price at T_c, not a scalar spot value.

For **path-dependent** products (Asian, Barrier, Lookback) `T::Arguments` will add:

```cpp
virtual double pathPayoff(const std::vector<double>& path) const;
```

`McEngine<T>` will call `pathPayoff(path)` for those instruments.

### `McEngine<T>` (template, header-only)

```cpp
template <typename T>
class McEngine : public Engine {
    typename T::Arguments args_;  // typed — instrument level
    ModelParameters modelParams_;
    SimulationParameters simParams_;
    double result_ = 0.0;
public:
    Engine::Arguments* getArguments() override { return &args_; }
    void calculate() override;    // simulate paths → args_.payoff(path.back())
    double getResult() const override { return result_; }
};
```

### `PdeEngine<T>` (template, header-only)

```cpp
template <typename T>
class PdeEngine : public Engine {
    typename T::Arguments args_;
    ModelParameters modelParams_;
    PdeParameters pdeParams_;
    PdeGridParameters grid_;
    PdeCoefficients pdeCoeffs_;
    double result_ = 0.0;
public:
    Engine::Arguments* getArguments() override { return &args_; }
    void calculate() override;    // terminal condition via args_.payoff(S_i); backward sweep
    double getResult() const override { return result_; }
};
```

### `BsCloseForm<T>` (template)

Not a subclass of `McEngine<T>` — same Engine pattern, no `SimulationParameters`, no path loop.
Full specializations defined in `.cpp` (not header) — full template specializations are regular
functions; defining in a header included by multiple TUs causes duplicate-symbol linker errors.

```cpp
template <typename T>
class BsCloseForm : public Engine {
    typename T::Arguments args_;
    ModelParameters modelParams_;
    double result_ = 0.0;
public:
    Engine::Arguments* getArguments() override { return &args_; }
    void calculate() override;    // specialised per instrument — reads args_.engineParams
    double getResult() const override { return result_; }
};

// Declarations only in .hpp; bodies in blackScholesCloseForm.cpp:
template <> void BsCloseForm<VanillaEuropeanOption>::calculate();
template <> void BsCloseForm<DigitalEuropeanOption>::calculate();
template <> void BsCloseForm<ChooserEuropeanOption>::calculate();
```

### `setArguments` — ONE cast to instrument's own type

```cpp
void VanillaEuropeanOption::setArguments(Engine::Arguments* args) const {
    auto* a = dynamic_cast<VanillaEuropeanOption::Arguments*>(args);
    if (!a) throw std::logic_error("wrong engine for VanillaEuropeanOption");
    a->engineParams = params_;
}
```

The cast always succeeds because `McEngine<VanillaEuropeanOption>`,
`PdeEngine<VanillaEuropeanOption>`, and `BsCloseForm<VanillaEuropeanOption>` all store
`VanillaEuropeanOption::Arguments args_` as a typed member.

## Notes

- `ChooserEuropeanOption` keeps its dedicated `ChooserEngine` — mid-path BS sub-pricing does
  not fit the generic `args_.payoff(S)` pattern.
- Template general bodies go in `.hpp`; full specializations go in `.cpp`.
- `ChooserEuropeanOption::Arguments` has no `payoff(double S)` — the chooser payoff
  is not a scalar function of spot; it requires a call+put sub-price at T_c.

#engine #architecture #quantlib
