# AI-TDD Transparency Report

**File:** `test/enemy/giant_test.cc`
**Contributor:** Eric Qian
**AI Tool:** DeepSeek Harness (CLI agent) - deepseek-v4.1-flash / glm-5.3

## Input

My prompts were written in Chinese; they are reproduced below in English.

I listed the skills and gave the chained rule: the super charge always forces the super charge hit on the following turn, the same way the goblin dash forces a dash attack.

## Output

`test/enemy/giant_test.cc` - 6 GoogleTest cases: each level only draws from its own skill list, a charge is always followed by the charge hit, the hit never opens the fight, the hit never appears without a charge before it, every charge is paid back exactly once, and a dead giant returns `Already_dead`.

## Human Contribution

`Giant.h` and `Giant.cc` are mine, including the wind-up flag, the probability split and the hp values. I fixed the level-3 branch that was returning a level-2 skill after the tests pointed at it.

## References

- TDD tests: `5dac988` "Test for giant"
- Implementation: `6f6ced6` "Passed TDD test for giant"
