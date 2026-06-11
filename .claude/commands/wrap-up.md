# End-of-session wrap-up. Do the following steps in order

1. **Daily log** — run `bash scripts/new-daily-log.sh` to create today's log file (it handles the frontmatter, title, and date). Then populate the sections using the snippet structures from `.vscode/quant-journal.code-snippets`. Available sections and their headers:
   - `sec-anki` → `## 📚 Anki`
   - `sec-coding` → `## 👨🏾‍💻 Coding`
   - `sec-exploring` → `## 🧭 Exploring`
   - `sec-next` → `## ❓ Next`
   - `sec-key` → `## 🔑 Key Points`

   Add only the sections relevant to today's session. If today's work involved a type of activity that doesn't match any existing snippet, ask the user if they want a new snippet created for it.

2. **Anki screenshot** — find the most recent screenshot in the `log` folder. The screenshot will be newer than yesterday's log. Rename it to `Anki_<D><Mon>_<YYYY>.<ext>`, move it to `log/<YYYY>/<MM-Mon>/images/`. Update the image link in today's log. Comment like the previous days unless told otherwise.

3. **Coding section** — summarise what was actually done in the session: files created or modified, design decisions made, concepts learned. Link to relevant files using relative markdown links (depth from log file: `../../../` to reach repo root). Follow the bullet style of previous log entries (check last 5 days).

4. **Memory files** — update `project_progress.md` to reflect what was completed and what is still pending. Update the relevant week memory file if files were created or moved. Update any other memory file that has become stale.

5. **CLAUDE.md** — update the file structure section or weekly schedule if the session introduced architectural changes or if we drifted from the planned schedule.

6. **README.md** — check if any new files, notes, or products were added that should be reflected in the tables (Models, Numerical Methods, Products, Notes). Also check if all links the notes in `notes` folder are present in the README.md file.

Use the style of previous log entries. Do not ask for confirmation on each step unless explicitly stated in the step. Complete all steps and report a summary at the end for each point.
