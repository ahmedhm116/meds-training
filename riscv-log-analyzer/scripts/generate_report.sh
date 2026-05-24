#!/bin/bash
# generate_report.sh — Generates an HTML report from a RISC-V simulation log file.

set -euo pipefail

show_help() {
    echo "Usage: $0 <log_file> [--output <path>]"
    echo "       $0 --all"
    echo "  <log_file>        Path to the .log file to generate a report for"
    echo "  --output <path>   Where to write the HTML (default: output/<logname>_report.html)"
    echo "  --all             Generate a report for every .log file in test_data/"
    echo "  --help            Show this help message"
}

extract_stats() {
    local input_file="$1"

    TOTAL=$(grep -c "TEST START:" "$input_file" || true)
    PASS_COUNT=$(grep -c "TEST PASS:"  "$input_file" || true)
    FAIL_COUNT=$(grep -c "TEST FAIL:"  "$input_file" || true)
    SKIP_COUNT=$(grep -c "TEST SKIP:"  "$input_file" || true)

    EXEC_TIMES=$(grep -E "TEST (PASS|FAIL):" "$input_file" \
        | sed -n 's/.*(\([0-9.]*\)s).*/\1/p' || true)

    if [[ -n "$EXEC_TIMES" ]]; then
        AVG_TIME=$(echo "$EXEC_TIMES" | awk '{ sum += $1; n++ } END { if (n > 0) printf "%.2f", sum/n; else print "0" }')
        MAX_TIME=$(echo "$EXEC_TIMES" | sort -n | tail -1)
        MIN_TIME=$(echo "$EXEC_TIMES" | sort -n | head -1)
    else
        AVG_TIME="0"; MAX_TIME="0"; MIN_TIME="0"
    fi

    if [ "$TOTAL" -gt 0 ]; then
        PASS_RATE=$(( (PASS_COUNT * 100) / TOTAL ))
    else
        PASS_RATE=0
    fi

    if [ "$FAIL_COUNT" -gt 0 ]; then
        VERDICT="FAIL"
    else
        VERDICT="PASS"
    fi
}

build_test_rows() {
    local input_file="$1"
    local rows=""

    while IFS= read -r line; do
        if echo "$line" | grep -q "TEST PASS:"; then
            local name time
            name=$(echo "$line" | awk '{print $5}')
            time=$(echo "$line" | sed -n 's/.*(\([0-9.]*\)s).*/\1/p')
            rows+="<tr><td>$name</td><td class=\"pass\">PASS</td><td>${time:-—}s</td></tr>"

        elif echo "$line" | grep -q "TEST FAIL:"; then
            local name time
            name=$(echo "$line" | awk '{print $5}')
            time=$(echo "$line" | sed -n 's/.*(\([0-9.]*\)s).*/\1/p')
            rows+="<tr><td>$name</td><td class=\"fail\">FAIL</td><td>${time:-—}s</td></tr>"

        elif echo "$line" | grep -q "TEST SKIP:"; then
            local name
            name=$(echo "$line" | awk '{print $5}')
            rows+="<tr><td>$name</td><td class=\"skip\">SKIP</td><td>—</td></tr>"
        fi
    done < "$input_file"

    echo "$rows"
}

generate_html() {
    local log_file="$1"
    local out_file="$2"

    if [[ ! -f "$log_file" ]]; then
        echo "Error: log file '$log_file' not found."
        exit 1
    fi

    echo "Generating report for: $log_file"

    # Extract all stats into global variables
    extract_stats "$log_file"

    # Build per-test rows
    local test_rows
    test_rows=$(build_test_rows "$log_file")

    # Verdict badge color
    local verdict_color="#16a34a"   # green for PASS
    [ "$VERDICT" = "FAIL" ] && verdict_color="#dc2626"

    # Make sure the output directory exists
    mkdir -p "$(dirname "$out_file")"

    # Write the HTML file
    cat > "$out_file" <<HTML
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8"/>
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>RISC-V Report — $(basename "$log_file")</title>
  <style>
    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

    body {
      font-family: ui-monospace, 'Cascadia Code', monospace;
      background: #f9fafb;
      color: #111827;
      padding: 2rem;
      max-width: 900px;
      margin: 0 auto;
    }

    header { margin-bottom: 2rem; }
    h1 { font-size: 1.4rem; font-weight: 700; }
    .meta { color: #6b7280; font-size: 0.82rem; margin-top: 0.3rem; }

    /* ── Stat cards ── */
    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(140px, 1fr));
      gap: 1rem;
      margin-bottom: 2rem;
    }
    .card {
      background: #fff;
      border: 1px solid #e5e7eb;
      border-radius: 8px;
      padding: 1rem 1.25rem;
    }
    .card .label {
      font-size: 0.7rem;
      color: #9ca3af;
      text-transform: uppercase;
      letter-spacing: 0.06em;
    }
    .card .value {
      font-size: 1.8rem;
      font-weight: 700;
      margin-top: 0.2rem;
    }

    /* ── Colors ── */
    .pass { color: #16a34a; }
    .fail { color: #dc2626; }
    .skip { color: #d97706; }
    .blue { color: #1d4ed8; }

    /* ── Tables ── */
    section { margin-bottom: 2rem; }
    h2 { font-size: 0.95rem; font-weight: 600; color: #374151; margin-bottom: 0.75rem; }
    table {
      width: 100%;
      border-collapse: collapse;
      background: #fff;
      border: 1px solid #e5e7eb;
      border-radius: 8px;
      overflow: hidden;
    }
    th {
      background: #f3f4f6;
      text-align: left;
      padding: 0.55rem 1rem;
      font-size: 0.75rem;
      text-transform: uppercase;
      color: #6b7280;
      letter-spacing: 0.04em;
    }
    td {
      padding: 0.55rem 1rem;
      border-top: 1px solid #f3f4f6;
      font-size: 0.88rem;
    }
    tr:hover td { background: #fafafa; }

    /* ── Verdict ── */
    .verdict {
      display: inline-block;
      padding: 0.4rem 1.4rem;
      border-radius: 999px;
      color: #fff;
      font-weight: 700;
      font-size: 1rem;
      background: ${verdict_color};
    }
  </style>
</head>
<body>

  <header>
    <h1>RISC-V Simulation Log Report</h1>
    <p class="meta">
      Log file: <strong>$log_file</strong>
      &nbsp;|&nbsp;
      Generated: $(date '+%Y-%m-%d %H:%M:%S')
    </p>
  </header>

  <!-- ── Summary cards ── -->
  <div class="grid">
    <div class="card"><div class="label">Total Tests</div><div class="value blue">$TOTAL</div></div>
    <div class="card"><div class="label">Passed</div><div class="value pass">$PASS_COUNT</div></div>
    <div class="card"><div class="label">Failed</div><div class="value fail">$FAIL_COUNT</div></div>
    <div class="card"><div class="label">Skipped</div><div class="value skip">$SKIP_COUNT</div></div>
    <div class="card"><div class="label">Pass Rate</div><div class="value">${PASS_RATE}%</div></div>
  </div>

  <!-- ── Timing table ── -->
  <section>
    <h2>Timing Statistics</h2>
    <table>
      <thead>
        <tr><th>Metric</th><th>Time</th></tr>
      </thead>
      <tbody>
        <tr><td>Minimum</td><td>${MIN_TIME}s</td></tr>
        <tr><td>Maximum</td><td>${MAX_TIME}s</td></tr>
        <tr><td>Average</td><td>${AVG_TIME}s</td></tr>
      </tbody>
    </table>
  </section>

  <!-- ── Per-test results table ── -->
  <section>
    <h2>Test Results</h2>
    <table>
      <thead>
        <tr><th>Test Name</th><th>Result</th><th>Time</th></tr>
      </thead>
      <tbody>
        $test_rows
      </tbody>
    </table>
  </section>

  <!-- ── Verdict ── -->
  <section>
    <h2>Verdict</h2>
    <span class="verdict">$VERDICT</span>
  </section>

</body>
</html>
HTML

    echo "  -> Written to: $out_file"
}

if [[ $# -lt 1 ]]; then
    echo "Error: missing argument."
    show_help; exit 1
fi

# --help
if [[ "$1" == "--help" ]]; then
    show_help; exit 0
fi

# --all: generate a report for every .log in test_data/
if [[ "$1" == "--all" ]]; then
    if [ ! -d "test_data/" ] || [ -z "$(ls -A test_data/*.log 2>/dev/null)" ]; then
        echo "Error: no .log files found in test_data/"
        exit 1
    fi
    for log in test_data/*.log; do
        base=$(basename "$log" .log)
        generate_html "$log" "output/${base}_report.html"
    done
    echo "Done. All reports written to output/"
    exit 0
fi

# Single log file
LOG_FILE="$1"
shift

# Default output path
BASE=$(basename "$LOG_FILE" .log)
OUT_FILE="output/${BASE}_report.html"

# Optional --output override
while [[ $# -gt 0 ]]; do
    case "$1" in
        --output) OUT_FILE="$2"; shift 2 ;;
        --help)   show_help; exit 0 ;;
        *) echo "Unknown option: $1"; show_help; exit 1 ;;
    esac
done

generate_html "$LOG_FILE" "$OUT_FILE"