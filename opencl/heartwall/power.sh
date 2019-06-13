 #!/bin/bash         
nvidia-smi --query-gpu=power.draw --format=csv --loop-ms=300 > results/GTX_power_results

