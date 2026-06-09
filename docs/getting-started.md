
# Getting Started

This document describes how to set up the development environment and follow the recommended workflow
for the **ATSAMV71 firmware platform**.

The project uses:

- Python (tooling and automation)
- MkDocs (documentation)
- Robot Framework (automated CLI tests)
- Renode (simulation)
- ARM GCC toolchain (firmware build)
- Git (version control)
- MPLAB X or VS Code (development environment)

This document is intended to help a new developer clone the repository and become productive quickly.

---

# 1. Install Python

Check if Python is installed:

```bash
python --version
```

Recommended:

```
Python 3.9+
```

If Python is missing:

https://www.python.org/downloads/

---

# 2. Create Python Virtual Environment

Always use a virtual environment for this project.

```bash
python -m venv .venv
```

Activate it.

### Windows

```bash
.venv\Scripts\activate
```

### Linux / macOS

```bash
source .venv/bin/activate
```

Your shell prompt should show:

```
(.venv)
```

---

# 3. Install Python Dependencies

```bash
pip install mkdocs
pip install mkdocs-material
pip install pymdown-extensions
pip install robotframework
pip install pyserial
```

Verify:

```bash
mkdocs --version
robot --version
```

---

# 4. Install MPlabX

Firmware is compiled using the MplabX IDE from Microchip.


# 5. Install Renode

Renode is used for firmware simulation.

Download:

https://renode.io/

Verify:

```bash
renode --version
```

Typical use:

```bash
cd renode
renode run.resc
```

---

# 6. Build Documentation

The documentation is built using **MkDocs**.

Run locally:

```bash
mkdocs serve --livereload   
```

Open browser:

```
http://127.0.0.1:8000
```

Build static site:

```bash
mkdocs build
```

Output folder:

```
site/
```

The `site/` folder must **not be committed to Git**.

---

# 7. Running Robot Framework Tests

Robot tests are located in:

```
tests/robot/
```

Run tests:

```bash
robot smoke.robot
```
If COM port need to be specìied

```bash
robot --variable SERIAL_PORT:COM3 test/tests/
```
Robot generates reports:

```
output.xml
log.html
report.html
```

These files should not be committed.

---

# 8. Typical Daily Workflow

Typical developer workflow:

1. Update repository
2. Create feature branch
3. Implement firmware changes
4. Build firmware
5. Run simulation
6. Run automated tests
7. Update documentation
8. Commit changes
9. Push branch
10. Create Pull Request

---

# 8.1 Update Repository

Before starting work:

```bash
git checkout main
git pull origin main
```

---

# 8.2 Create Feature Branch

Never work directly on `main`.

```bash
git checkout -b feature/<short-description>
```

Example:

```bash
git checkout -b feature/spi-driver
```

---

# 8.3 Implement Changes

Typical work may include:

- modifying firmware
- adding CLI commands
- updating documentation
- updating Renode platform
- adding Robot tests

---

# 8.4 Build Firmware

```bash
cd firmware
make
```

Expected artifact:

```
build/firmware.elf
```

---

# 8.5 Run Simulation

```bash
cd renode
renode run.resc
```

Verify:

- firmware boots
- CLI prompt appears

---

# 8.6 Run Automated Tests

```bash
cd tests/robot
robot smoke.robot
```

All tests should pass before committing.

---

# 8.7 Update Documentation

If you change:

- CLI commands
- firmware architecture
- Renode configuration
- testing workflow

Update documentation accordingly.

Preview docs:

```bash
mkdocs serve --livereload
```

---

# 8.8 Commit Changes

Stage files:

```bash
git add .
```

Check:

```bash
git status
```

Commit:

```bash
git commit -m "feat: add SPI initialization"
```

Recommended format:

```
feat: new feature
fix: bug fix
docs: documentation update
test: testing update
refactor: internal change
```

---

# 8.9 Push Branch

```bash
git push origin feature/spi-driver
```

---

# 8.10 Create Pull Request

Create a PR including:

- description
- related issue
- testing summary

Example:

```
Add SPI driver initialization

Tested with:
- Renode simulation
- Robot smoke tests
```

---

# 9. MPLAB X Project Handling

MPLAB X generates temporary files when opening a project.

Examples:

```
nbproject/private/
build/
dist/
*.elf
*.map
*.hex
```

These files may cause Git checkout failures.

---

## Do Not Open MPLAB on main

Correct:

```
git checkout feature/my-feature
(open MPLAB)
```

Incorrect:

```
git checkout main
(open MPLAB)
```

Opening MPLAB on `main` may modify files automatically.

---

## Stash Before Switching Branch

Check status:

```bash
git status
```

If temporary changes exist:

```bash
git stash
git checkout main
```

Restore later:

```bash
git stash pop
```

---

# 10. When Remote Main Changes

Sometimes `main` changes while you work on a branch.

Update your branch.

Step 1:

```bash
git checkout main
git pull origin main
```

Step 2:

```bash
git checkout feature/my-feature
```

Step 3:

```bash
git merge main
```

Resolve conflicts if required.

---

# 11. Handling Checkout Errors

Example error:

```
error: Your local changes would be overwritten by checkout
```

Fix:

```bash
git stash
git checkout main
```

Or discard changes:

```bash
git restore .
```

Use restore carefully.

---

# 12. Recommended .gitignore

```
.venv/
venv/

site/

build/
dist/

nbproject/private/

output.xml
log.html
report.html
```

---

# 13. CI Pipeline Overview

Typical CI pipeline:

1. Checkout repository
2. Install Python dependencies
3. Build firmware
4. Run simulation tests
5. Run Robot tests
6. Build documentation
7. Archive artifacts

Artifacts include:

```
firmware.elf
test reports
documentation
```

---

# 14. Code Review Guidelines

Pull requests should follow these rules:

- firmware builds successfully
- Robot smoke tests pass
- documentation updated if required
- commit history is clear

Reviewers should check:

- code readability
- CLI command stability
- simulation compatibility
- documentation completeness

---

# 15. Workflow Summary

Typical development cycle:

1. Pull latest `main`
2. Create feature branch
3. Implement changes
4. Build firmware
5. Run simulation
6. Run tests
7. Update documentation
8. Commit
9. Push
10. Create Pull Request

