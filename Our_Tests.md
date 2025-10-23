# Our_Tests.md

Space Launch Timeline Manager — Test Plan (TTY, plain text)

## How to run tests (Ubuntu/WSL)

```bash
# from repo root
make                   # builds ./sltm
mkdir -p tests/actual  # where we store raw outputs
mkdir -p tests/expect  # where we store expected outputs (optional, if you want files)
```

Normalization helper:

```bash
# normalize: remove leading prompts "sltm> "
normalize() { sed -E 's/^sltm> //'; }
```

Use it as: `normalize < tests/actual/t1.out > tests/actual/t1.out.norm`

---

## Dataset used

* `data/sample_launches.csv` (12 valid rows)
* `data/test_invalid.csv` (contains invalid rows to be skipped)

---

## Test 1 — load + range (basic happy path)

**Goal:** Verify CSV load count and date-range filtering (inclusive).

**Script:**

```bash
./sltm <<'EOF' | tee tests/actual/t1.out
load data/sample_launches.csv
range 2025-03-01 2025-04-15
exit
EOF
normalize < tests/actual/t1.out > tests/actual/t1.out.norm
```

**Expected (normalized):**

```
Loaded 12; ignored 0
SLV005,2025-03-01,Falcon Heavy,Lunar Probe Pathfinder,KSC LC-39A,Scheduled
SLV006,2025-03-10,Electron,WeatherSat-3,Mahia LC-1B,Success
SLV007,2025-03-25,Starship,Deep Space Cargo Test,Boca Chica Orbital Pad,Delayed
SLV008,2025-04-02,Vega C,Earth Imaging Cluster,Vega Launch Zone,Success
SLV009,2025-04-12,PSLV,NavIC Expansion,Satish Dhawan FLP,Success
```

**Pass criteria:** exact match after normalization.

---

## Test 2 — invalid rows are skipped

**Goal:** Ensure malformed/missing fields are ignored and counted.

**Script:**

```bash
./sltm <<'EOF' | tee tests/actual/t2.out
load data/test_invalid.csv
range 2025-06-01 2025-06-30
exit
EOF
normalize < tests/actual/t2.out > tests/actual/t2.out.norm
```

**Expected (normalized):**

```
Loaded 1; ignored 4
X001,2025-06-01,Falcon 9,Starlink Batch X,CCSFS SLC-40,Success
```

**Pass criteria:** exact match after normalization.

---

## Test 3 — add + find (case-insensitive substring)

**Goal:** Insert a new record at the proper date and find it via mission substring.

**Script:**

```bash
./sltm <<'EOF' | tee tests/actual/t3.out
load data/sample_launches.csv
add SLV999,2025-06-02,Falcon 9,CubeSat Deploy,CCSFS SLC-40,Scheduled
find cube
exit
EOF
normalize < tests/actual/t3.out > tests/actual/t3.out.norm
```

**Expected (normalized):**

```
Loaded 12; ignored 0
OK added.
SLV999,2025-06-02,Falcon 9,CubeSat Deploy,CCSFS SLC-40,Scheduled
```

**Pass criteria:** exact match after normalization.

---

## Test 4 — update + range

**Goal:** Update fields by ID and confirm the changes via a focused range query.

**Script:**

```bash
./sltm <<'EOF' | tee tests/actual/t4.out
load data/sample_launches.csv
add SLV999,2025-06-02,Falcon 9,CubeSat Deploy,CCSFS SLC-40,Scheduled
update SLV999 status=Success date=2025-06-03
range 2025-06-01 2025-06-10
exit
EOF
normalize < tests/actual/t4.out > tests/actual/t4.out.norm
```

**Expected (normalized):**

```
Loaded 12; ignored 0
OK added.
OK updated.
SLV999,2025-06-03,Falcon 9,CubeSat Deploy,CCSFS SLC-40,Success
```

**Pass criteria:** exact match after normalization.

---

## Test 5 — delete (exists + not found)

**Goal:** Delete an existing record, then try deleting it again.

**Script:**

```bash
./sltm <<'EOF' | tee tests/actual/t5.out
load data/sample_launches.csv
delete SLV010
delete SLV010
exit
EOF
normalize < tests/actual/t5.out > tests/actual/t5.out.norm
```

**Expected (normalized):**

```
Loaded 12; ignored 0
OK deleted.
Not found.
```

**Pass criteria:** exact match after normalization.

---

## Test 6 — export (file content)

**Goal:** Export current list to CSV and verify the header and first rows.

**Script:**

```bash
./sltm <<'EOF' | tee tests/actual/t6.out
load data/sample_launches.csv
export tests/out_export.csv
exit
EOF
normalize < tests/actual/t6.out > tests/actual/t6.out.norm

# Inspect first 3 lines of the exported file
head -n 3 tests/out_export.csv | tee tests/actual/t6.head3.out
```

**Expected (normalized stdout):**

```
Loaded 12; ignored 0
Exported.
```

**Expected file head (`tests/out_export.csv` first 3 lines):**

```
id,date,vehicle,mission,site,status
SLV001,2025-01-15,Falcon 9,Starlink Group 9-1,CCSFS SLC-40,Success
SLV002,2025-01-20,Ariane 6,Artemis Supply Test,Kourou ELA-4,Delayed
```

**Pass criteria:** both the normalized stdout and the exported-file head match exactly.

---

## Optional: quick diff checks

```bash
# Example for Test 1:
cat > tests/expect/t1.out.norm <<'EOF'
Loaded 12; ignored 0
SLV005,2025-03-01,Falcon Heavy,Lunar Probe Pathfinder,KSC LC-39A,Scheduled
SLV006,2025-03-10,Electron,WeatherSat-3,Mahia LC-1B,Success
SLV007,2025-03-25,Starship,Deep Space Cargo Test,Boca Chica Orbital Pad,Delayed
SLV008,2025-04-02,Vega C,Earth Imaging Cluster,Vega Launch Zone,Success
SLV009,2025-04-12,PSLV,NavIC Expansion,Satish Dhawan FLP,Success
EOF

diff -u tests/expect/t1.out.norm tests/actual/t1.out.norm && echo "T1 PASS"
```

Repeat similarly for the other tests.

---

## Notes & assumptions

* Tests target the provided simple CSV format (6 fields, unquoted).
* Outputs are UTF-8 plain text; run in a text console/TTY.
* Use `Ctrl-C` to abort interactive sessions; scripts end with `exit`.
* For memory safety, run `valgrind --leak-check=yes ./sltm` manually if desired.

---
