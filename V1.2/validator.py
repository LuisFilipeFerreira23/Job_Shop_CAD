import sys

def validate_jss(jss_file, output_file):
    try:
        # 1. Parse Input File (.jss)
        with open(jss_file, 'r') as f:
            lines = f.read().split()

        if not lines:
            print("Error: Empty JSS file.")
            return

        num_jobs = int(lines[0])
        num_machines = int(lines[1])

        jobs_data = [] # Structure: [[(machine, duration), ...], ...]
        idx = 2
        for i in range(num_jobs):
            job = []
            for j in range(num_machines):
                job.append((int(lines[idx]), int(lines[idx+1])))
                idx += 2
            jobs_data.append(job)

        # 2. Parse Output File (.txt)
        with open(output_file, 'r') as f:
            out_lines = [line for line in f.readlines() if line.strip()]

        if not out_lines:
            print("Error: Empty output file.")
            return

        reported_makespan = int(out_lines[0].strip())
        start_times = []
        for i in range(num_jobs):
            times = list(map(int, out_lines[i+1].split()))
            start_times.append(times)

        # 3. Validation Logic
        errors = []
        machine_intervals = {m: [] for m in range(num_machines)}
        actual_max_completion = 0

        for j in range(num_jobs):
            for op_idx in range(num_machines):
                machine, duration = jobs_data[j][op_idx]
                start = start_times[j][op_idx]
                end = start + duration

                actual_max_completion = max(actual_max_completion, end)

                # Rule A: Job Precedence Constraint
                if op_idx > 0:
                    prev_end = start_times[j][op_idx-1] + jobs_data[j][op_idx-1][1]
                    if start < prev_end:
                        errors.append(f"Precedence Error (Job {j}): Op {op_idx} starts at {start}, but Op {op_idx-1} finishes at {prev_end}")

                # Store interval for Rule B validation
                machine_intervals[machine].append((start, end, j, op_idx))

        # Rule B: Machine Capacity Constraint (No overlaps)
        for m in range(num_machines):
            intervals = sorted(machine_intervals[m]) # Sort by start time
            for i in range(len(intervals) - 1):
                s1, e1, j1, op1 = intervals[i]
                s2, e2, j2, op2 = intervals[i+1]
                if e1 > s2:
                    errors.append(f"Overlap Error (Machine {m}): Job {j1} Op {op1} [{s1}-{e1}] overlaps with Job {j2} Op {op2} [{s2}-{e2}]")

        # Rule C: Makespan Accuracy
        if reported_makespan != actual_max_completion:
            errors.append(f"Makespan Error: Reported {reported_makespan}, Actual {actual_max_completion}")

        # 4. Result Output
        if errors:
            print(f"❌ Validation FAILED ({len(errors)} errors detected):")
            for e in errors[:15]: # Print first 15 errors to avoid terminal flooding
                print("  -", e)
            if len(errors) > 15:
                print(f"  ... and {len(errors) - 15} more.")
        else:
            print("✅ Validation PASSED: Schedule is mathematically valid.")

    except Exception as e:
        print(f"Execution Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python validator.py <input.jss> <output.txt>")
    else:
        validate_jss(sys.argv[1], sys.argv[2])