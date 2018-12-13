#! /usr/bin/perl

open(OUT, ">parallel_cores.csv");
print OUT "Num_of_Iters,Num_of_threads,Parallel(GFLOPS/s),Parallel(Val of PI),Single_Core_Theoretical_Peak(GFLOPS/s)\n";

$theoretical_peak = 2.4 * 3 * 8 / 2.5;
$rounded_TP = sprintf("%0.1f", $theoretical_peak);
@arr = (6, 12, 24);
for ($t = 0; $t <=$#arr; $t++) {
	for ($i=1920; $i<=19200000; $i=$i*2) {
		$parallel_perf = `./parallel.x $i 1000 $arr[$t] | grep GFLOPS | cut -d':' -f2`;
		$parallel_pi = `./parallel.x $i 1000 $arr[$t] | grep Pi | cut -d':' -f2`;
		chomp($parallel_perf);
		chomp($parallel_pi);
		$rounded_pperf = sprintf("%0.2f", $parallel_perf);
		$rounded_ppi = sprintf("%0.3f", $parallel_pi);
		print OUT "$i,$arr[$t],$rounded_pperf,$rounded_ppi,$theoretical_peak\n";
	}
}
close OUT;
