# AI-TDD Transparency Report

**File:** `test/enemy/enemy_test.cc`
**Contributor:** Eric Qian
**AI Tool:** DeepSeek Harness (CLI agent) - deepseek-v4.1-flash / glm-5.3

## Input

My prompts were written in Chinese; they are reproduced below in English.

The task first: go back to the CS 100 project - the Enemy and Goblin classes are already written, and the TDD tests still need to be added.

Then the behaviour, rule by rule:

- A hit empties the shield first, whatever is left goes to hp, hp stops at zero, and reaching zero makes the enemy dead.
- Replace the shield setter with add and clear; the shield is wiped at the end of every round.
- Healing caps at max hp, and a dead monster cannot be healed.
- Add `get_max_hp()`, needed as the ceiling to clamp healing against.

## Output

`test/enemy/enemy_test.cc` - 24 GoogleTest cases for the Enemy base class, driven through a small stub: starting hp, shield and alive state; damage clamped at zero; the shield absorbing before hp; shields stacking and clearing; a defeated enemy ignoring further damage; healing capped and refused on a corpse; two enemies not sharing state; and `calMove()` dispatched through `Enemy&`.

## Human Contribution

All of `src/enemy/` is my code, including `deal_dmg`, `heal`, `add_shield`, `clear_shield`, `is_alive` and `get_max_hp`. The rules above are my own design decisions; the AI turned them into assertions and did not choose them. I ran cmake and the test binary myself after every change, fixed the compile errors, and adjusted the tests when I changed a difficulty value.

## References

- TDD tests: `fc075a2` "Enemy and Goblin TDD unit tests added" (red: members not yet declared)
- Implementation: `ec033c1` "Implement deal_dmg, shield and death handling, TDD tests passed"
- Heal tests: `4203757` "TDD test for heal function"
- Heal implementation: `78d1101` "Passed TDD test for heal and Enemy_type change"
