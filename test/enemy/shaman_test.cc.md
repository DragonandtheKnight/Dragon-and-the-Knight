# AI-TDD Transparency Report

**File:** `test/enemy/shaman_test.cc`
**Contributor:** Eric Qian
**AI Tool:** DeepSeek Harness (CLI agent) - deepseek-v4.1-flash / glm-5.3

## Input

My prompts were written in Chinese; they are reproduced below in English.

I gave the skill list per level and one rule: the shaman only heals itself once it is below half health.

## Output

`test/enemy/shaman_test.cc` - 5 GoogleTest cases. Every shaman action except the self heal targets other monsters, so this file checks the returned skill name only: no self heal at full health, none at three quarters health, self heal once badly hurt, only its own level's names, and `Already_dead` on a corpse. The exact half-health boundary is deliberately not asserted.

## Human Contribution

`Shaman.h` and `Shaman.cc` are mine, including the level helpers, the health thresholds and the probabilities. I renamed the enum values to `Shaman_curse_*` for consistency with level one, and fixed a wrong include and a mismatched enum name that the compiler reported.

## References

- TDD tests: `8b7f3ea` "TDD test for Shaman"
- Implementation: `9240ff0` "Passed TDD test for Shaman"
