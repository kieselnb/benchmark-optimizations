#! /usr/bin/perl

open(OUT, ">single_core.csv");
print OUT "Num_of_Iters,Baseline(GFLOPS/s),Baseline(Val of PI),Optimized(GFLOPS/s),Optimized(Val of PI),Theoretical_Peak(GFLOPS/s)\n";

$theoretical_peak = 2.4 * 3 * 8 / 2.5;
$rounded_TP = sprintf("%0.2f", $theoretical_peak);
for ($i=1920; $i<=19200000; $i=$i*2) {
	$baseline_perf = `./baseline.x $i 1000 | grep GFLOPS | cut -d':' -f2`;
	$optimized_perf = `./optimized.x $i 1000 | grep GFLOPS | cut -d':' -f2`;
	$baseline_pi = `./baseline.x $i 1000 | grep Pi | cut -d':' -f2`;
	$optimized_pi = `./optimized.x $i 1000 | grep Pi | cut -d':' -f2`;
	
	chomp($baseline_perf);
	chomp($baseline_pi);
	chomp($optimized_perf);
	chomp($optimized_pi);

	$rounded_bperf = sprintf("%0.2f", $baseline_perf);
	$rounded_bpi = sprintf("%0.3f", $baseline_pi);
	$rounded_operf = sprintf("%0.2f", $optimized_perf);
	$rounded_opi = sprintf("%0.3f", $optimized_pi);
	print OUT "$i,$rounded_bperf,$rounded_bpi,$rounded_operf,$rounded_opi,$theoretical_peak\n";
}
close OUT;
