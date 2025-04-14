import subprocess
import time
import matplotlib.pyplot as plt


def run(prog_name, nplanets, timesteps):
    """
    Run the program with the given parameters and return execution time and final planet position.
    
    Args:
        prog_name: Name of the executable
        nplanets: Number of planets to simulate
        timesteps: Number of simulation steps
        
    Returns:
        tuple: (execution time, (final_x, final_y))
    """
    command = f"./{prog_name} {nplanets} {timesteps}"
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    output = result.stdout.strip()
    
    # Parse the execution time and final location from the output
    if "Total time to run simulation" in output:
        parts = output.split()
        time = float(parts[5])
        final_x = float(parts[-2])
        final_y = float(parts[-1])
        return time, (final_x, final_y)
    return None, (None, None)
    
    
def main():
    # List of executables to test
    prog_names = ["0_original.exe", "1_opt.exe", "2_mem.exe", "3_openmp.exe", "4_final.exe"]
    names = ["Original", "Native March", "Preallocation", "OpenMP", "Final (Barnes-Hut)"]
    nplanets = 10000
    timesteps = [1, 2, 5, 10, 20, 50, 100, 500]
    
    # Pre-compute original version results for all timesteps
    print(f"Running original version ({prog_names[0]}) for all timesteps...")
    orig_results = {}  # Dictionary to store original results: {timestep: (time, pos)}
    for ts in timesteps:
        orig_results[ts] = run(prog_names[0], nplanets, ts)
    
    # For each optimized version
    for prog_name in prog_names[1:]:
        print(f"\nComparing {prog_name} vs {prog_names[0]}")
        print(f"Testing with {nplanets} planets")
        print("-------------------------------")
        print(f"{'Timesteps':<10} {'Original (s)':<15} {prog_name + ' (s)':<15} {'Speedup':<10} {'Position Diff':<15}")
        print("-------------------------------")
        
        # Initialize data containers for this comparison
        ori_times = []
        opt_times = []
        pos_diffs = []
        
        # Run experiments for each timestep
        for ts in timesteps:
            # Get cached original version results
            ori_time, ori_pos = orig_results[ts]
            
            # Run optimized version
            opt_time, opt_pos = run(prog_name, nplanets, ts)
            
            # Store results
            ori_times.append(ori_time)
            opt_times.append(opt_time)
            
            # Calculate speedup and position difference
            if ori_time and opt_time and ori_pos[0] is not None and opt_pos[0] is not None:
                speedup = ori_time / opt_time if opt_time > 0 else 0
                pos_diff = ((ori_pos[0] - opt_pos[0])**2 + (ori_pos[1] - opt_pos[1])**2)**0.5
                pos_diffs.append(pos_diff)
                print(f"{ts:<10} {ori_time:<15.6f} {opt_time:<15.6f} {speedup:<10.2f} {pos_diff:<15.6e}")
            else:
                pos_diffs.append(None)
                print(f"{ts:<10} {'N/A':<15} {'N/A':<15} {'N/A':<10} {'N/A':<15}")
        
        # Create two subplots
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
        
        # First subplot: Performance comparison (execution time)
        ax1.plot(timesteps, ori_times, 'o-', label='Original')
        ax1.plot(timesteps, opt_times, 's-', label=prog_name.split('.')[0])
        ax1.set_xlabel('Timesteps')
        ax1.set_ylabel('Time (seconds)')
        ax1.set_title('Performance Comparison')
        ax1.legend()
        ax1.grid(True)
        
        # Second subplot: Speedup and position difference
        # Create primary Y axis for speedup
        ax2_speedup = ax2
        speedups = [o/n if n > 0 and o is not None else 0 for o, n in zip(ori_times, opt_times)]
        line1 = ax2_speedup.plot(timesteps, speedups, 'd-', color='blue', label='Speedup')
        ax2_speedup.set_xlabel('Timesteps')
        ax2_speedup.set_ylabel('Speedup', color='blue')
        ax2_speedup.tick_params(axis='y', labelcolor='blue')
        
        # Create secondary Y axis for position difference
        ax2_diff = ax2.twinx()
        valid_indices = [i for i, diff in enumerate(pos_diffs) if diff is not None]
        if valid_indices:  # Only plot if we have valid data
            valid_ts = [timesteps[i] for i in valid_indices]
            valid_diffs = [pos_diffs[i] for i in valid_indices]
            line2 = ax2_diff.plot(valid_ts, valid_diffs, 's--', color='red', label='Position Diff')
            ax2_diff.set_ylabel('Position Difference', color='red')
            ax2_diff.tick_params(axis='y', labelcolor='red')
            ax2_diff.set_yscale('log')  # Use log scale for better visualization
            
        # Add combined legend
        lines = line1
        labels = ['Speedup']
        if valid_indices:
            lines += line2
            labels.append('Position Diff')
        ax2.legend(lines, labels)
        ax2.set_title('Speedup and Numerical Difference')
        ax2.grid(True)
        
        # Save the figure
        plt.tight_layout()
        plt.savefig(f'performance_comparison_{prog_name.split(".")[0]}.pdf')
    
    # Uncomment to display plots interactively
    # plt.show()


if __name__ == "__main__":
    main()