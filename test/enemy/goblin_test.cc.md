# AI-TDD Transparency Report

**File:** `test/enemy/goblin_test.cc`
**Contributor:** Eric Qian
**AI Tool:** DeepSeek Harness (CLI agent) - deepseek-v4.1-flash / glm-5.3

## Input

My prompts were written in Chinese; they are reproduced below in English.

Same task message as `enemy_test.cc`. The rule specific to this file was the sentinel skill: a new `Already_dead` value acts as a fallback so a call on a corpse deals no damage, so `calMove()` must check for death first and return it.

Everything else these tests check was designed and implemented by me before the session: two skills at level 2, four at level 3, and a dash that is always followed by exactly one dash attack.

## Output

`test/enemy/goblin_test.cc` - 18 GoogleTest cases: the stats of each level, an out-of-range level falling back to level one, the same seed replaying the same fight, each level's skill set, the 70/30 and 30/40/30 splits with a tolerance six standard deviations wide, the dash always being paid back by a dash attack, and a dead goblin answering `Already_dead` forever.

## Human Contribution

`Goblin` is entirely my code, including the dash / dash-attack state machine and the level-to-stats helpers, which I renamed so the same shape copies to the other monsters. I chose the probabilities and tuned them while the tests were already in place.

## References

- TDD tests: `fc075a2` "Enemy and Goblin TDD unit tests added"
- Implementation: `ec033c1` "Implement deal_dmg, shield and death handling, TDD tests passed"
