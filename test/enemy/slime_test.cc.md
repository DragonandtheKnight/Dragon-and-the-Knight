# AI-TDD Transparency Report

**File:** `test/enemy/slime_test.cc`
**Contributor:** Eric Qian
**AI Tool:** DeepSeek Harness (CLI agent) - deepseek-v4.1-flash / glm-5.3

## Input

My prompts were written in Chinese; they are reproduced below in English.

I listed the six skills myself and stated the one rule that governs them: a slime only heals while it is not at full health, which makes it a deliberately annoying small monster.

## Output

`test/enemy/slime_test.cc` - 6 GoogleTest cases: a slime at full health only attacks, a wounded slime uses the heal skill of its own level, it still attacks while wounded, it only draws from its own level's skill names, and a dead slime returns `Already_dead` on every call. No damage number or percentage is pinned, because those are balance.

## Human Contribution

`src/enemy/Slime.h` and `Slime.cc` are mine: the level-to-type / hp / difficulty helpers and the `calMove()` branch that compares `get_hp()` with `get_max_hp()`. I fixed the fallback branch after review, since it originally healed at full health.

## References

- TDD tests: `1e2226d` "TDD test for slime" (red: `Slime` not declared)
- Implementation: `52b3d35` "Slime added and passed TDD test, get_max_hp added and tested, small change in goblin"
