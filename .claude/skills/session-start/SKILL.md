---
name: session-start
description: Run at the start of a mentoring session. On weekends Opus reviews the week's journal and code, grades last week's homework, assigns new homework, and updates the learning plan. On weekdays Sonnet reviews recent code and sets the day's objectives.
---

## Instructions

All paths are relative to `.claude/`.

**Always do this first, regardless of day:** read `./HANDOFF.md`. It is the single source of
current state — the focus, the ordered next actions, open questions, and anything blocked.
Restate the current focus and the top next action to the user before doing anything else.

### Weekend days — Opus

- `./journal/` — summaries of previous work
    - Read the week's work (journal file named for the Monday of the week, `DD-MM-YYYY.md`).
    - Review the code referenced in the journal as needed.
    - If Sonnet has been making recurring mistakes, propose new skills to prevent them.
- `./homework/` — homework questions and answers
    - Read the previous week's answers and grade them.
    - Create a directory named for the current date in `DD-MM-YYYY` format.
    - Inside it, create `QUESTIONS.md` with 3–5 homework questions for the week, each
      noting how it connects to the project work.
- `./LEARNING_PLAN.md` — the current learning plan
    - Make any adjustments and update progress/milestone status.
- Check in: summarize feedback for the last week, then discuss future work and design.
- Reconcile `./HANDOFF.md` with the decisions made this session (focus, next actions, blockers).

### Weekdays — Sonnet

- Lead with `./HANDOFF.md`: confirm the current focus and the top next action, then get the
  user working on it immediately under your guidance. Don't re-derive state from scratch.
- Review the previous code and put feedback in the `.claude/feedback/` directory in a markdown file with today's date.
- Assign today's objectives, anchored to the next actions in the handoff.
