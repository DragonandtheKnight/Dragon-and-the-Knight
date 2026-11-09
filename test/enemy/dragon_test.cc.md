# AI-TDD Transparency Report

**File:** `test/enemy/dragon_test.cc`
**Contributor:** Eric Qian
**AI Tool:** DeepSeek Harness (CLI agent) - deepseek-v4.1-flash / glm-5.3

## Input

My prompts were written in Chinese; they are reproduced below in English.

I designed the final boss and wrote the skill list with its rules:

- Fly may only be used above half health; flying means 999 defence.
- The drop slam may and must be used only after flying.
- Roar may be used once per fight: probabilistic below half health, guaranteed below a quarter.

## Output

`test/enemy/dragon_test.cc` - 9 GoogleTest cases: only the dragon's own skill names at any health, no roar while healthy or only slightly hurt, no flight below half health, flight always followed by the drop slam, the slam never coming from anywhere else, the roar appearing at most once in a long run, the roar appearing at all when nearly dead, and `Already_dead` on a corpse.

## Human Contribution

`Dragon.h` and `Dragon.cc` are mine: both state flags (mid-flight and already roared), the three health bands, the level helpers and every probability. I wrote it after the other four monsters and fixed the copy-paste errors the compiler reported.

## References

- TDD tests: `f584cc9` "TDD tests for dragon"
- Implementation: `2fd576b` "Passed TDD tests for dragon"
