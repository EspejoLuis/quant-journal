# End-of-session wrap-up. Do the following steps in order

1. **Daily log** — run `bash scripts/new-daily-log.sh` to create today's log file (it handles the frontmatter, title, and date). Then populate the sections using the snippet structures from `.vscode/quant-journal.code-snippets`. Available sections and their headers:
   - `sec-anki` → `## 📚 Anki`
   - `sec-coding` → `## 👨🏾‍💻 Coding`
   - `sec-exploring` → `## 🧭 Exploring`
   - `sec-next` → `## ❓ Next`
   - `sec-key` → `## 🔑 Key Points`

   Add only the sections relevant to today's session. If today's work involved a type of activity that doesn't match any existing snippet, ask the user if they want a new snippet created for it.

2. **Anki screenshot** — find the most recent screenshot in the `log` folder. The screenshot will be newer than yesterday's log. If no screenshot is found, skip this step. If found, rename it to `Anki_<D><Mon>_<YYYY>.<ext>`, move it to `log/<YYYY>/<MM-Mon>/images/`. Update the image link in today's log. Write the image caption/alt text in the same style as previous entries. Also check section from previous day and similar comments unless told otherwise. For example, if previous day anki section has "reviewed this link focusing on" then use same comment updating the image only.

3. **Coding section** — summarise what was actually done in the session: files created or modified, design decisions made, concepts learned. Link to relevant files using relative markdown links (depth from log file: `../../../` to reach repo root). Follow the bullet style of previous log entries (check last 5 days).

4. **Memory files** — update `project_progress.md` to reflect what was completed and what is still pending. Update the relevant week memory file if files were created or moved. Update any other memory file that has become stale.

5. **CLAUDE.md** — update the file structure section or weekly schedule if the session introduced architectural changes or if we drifted from the planned schedule. Estimate if the dates of the plan are still fine or if they need to be extended or shorten.

6. This file [[Modern-Computational-Finance-AAD-and-Parallel-Simulations]] is going to be updated frequently but not every day with new updates. so compare the plan defined in CLAUDE.md with the information in this file. For example if the file says how performace can be improved using threads and parallel computing then the plan in CLAUDE.md needs to change accordingly. Skip the CLAUDE.md update if the information from the book has already been incorporated.

7. **README.md** — check if any new files, notes, or products were added that should be reflected in the tables (Models, Numerical Methods, Products, Notes). Also check if all links the notes in `notes` folder are present in the README.md file.

8. **Anki Cards** - check sections `key points` and `coding` defined in point 1 and compare them with my existing Anki cards. If there are existing Anki cards that are linked to what has been reviewed during the session, add the tag `reviewFromQuantJournal` to those cards. If there no existing cards linked to what has been reviewed during the session, create Anki Cards using existing tags. Add new tags only if no tags representing what has been reviewed can be found. Note about tags: if there is a tag you want to use like A::B::C, don't put A::B and A on top of A::B::C,because they will put automatically if you use A::B::C.

Use the style of previous log entries. Do not ask for confirmation on steps 1/2/3/4/5/6/7 unless explicitly stated in the step. For point 8 you have to tell me which cards are you planning to add with a summary, I will confirm or not. Complete all steps and report a summary at the end for each point.

[Modern-Computational-Finance-AAD-and-Parallel-Simulations]: ../../books/Modern-Computational-Finance-AAD-and-Parallel-Simulations.md "AAD and // Simulation"
