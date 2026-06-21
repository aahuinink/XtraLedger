---
name: session-end
description: Run at the end of a mentoring session. Sonnet reviews the day's code work and records feedback, progress, and mistakes in this week's journal file.
---

## Instructions

All paths are relative to `.claude/`.

If a `./journal/` markdown file for this week does not exist yet, create one named for the
Monday of the current week in `DD-MM-YYYY` format.

### Sonnet

- Review today's code work.
- Summarize feedback, progress, and mistakes, and append them to this week's journal file.
- **Update `./HANDOFF.md`** so the next session can start immediately: mark what was
  completed, reorder/replace the "Next actions" list, and note anything now blocked or any
  new open question. This is the most important step — `/session-start` reads it first.
