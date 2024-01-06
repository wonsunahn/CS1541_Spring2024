
/** Code by @author Wonsun Ahn
 * 
 * <p>Adapted from https://www.baeldung.com/java-branch-prediction
 * 
 * <p>BranchExperiment: The program calculates the count of numbers in a
 * sequence of numbers (1 ~ top) that fit a certain criterion.  The
 * criterion is simple: if the number is less than top/2.  So the count is
 * obviously going to be top/2.  The program intentionally does a very
 * inefficient implementation of this task, by iterating over the entire
 * sequence, for the purposes of measuring the impact of the branch
 * predictor on performance.
 *
 * <p>Depending on the "sort" or "shuffle" argument, the iteration will
 * happen either in the sorted state, or after shuffling the ordering of
 * those numbers.  In theory, the ordering should not have an impact on the
 * performance as you are executing the exact same set of instructions in
 * the loop, regardless of ordering.  In practice, there is a big difference
 * in performance (up to 5X) because in the sorted state, branch prediction
 * is exceedingly simple (all taken during the first half and all not taken
 * in the second half), and in the shuffled state, branch prediction is
 * essentially impossible since the branch direction is random.
*/

import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.LongStream;

public class BranchExperiment {
	private static long top = 0;
	private static boolean shuffle = false;

	/**
	 * Prints usage information.
	 */
	public static void showUsage() {
		System.out.println("Usage: java BranchExperiment list_length <sort | shuffle>");
	}

	public static void main(String[] args) {

		if (args.length != 2) {
			showUsage();
			return;
		}

		try {
			top = Integer.parseInt(args[0]);
		} catch (NumberFormatException ne) {
			showUsage();
			return;
		}

		if (args[1].equals("shuffle")) {
			shuffle = true;
		}

        /* Generates a sequence of numbers from 0 to top. */
		List<Long> numbers = LongStream.range(0, top).boxed().collect(Collectors.toList());

		if (shuffle) {
			Collections.shuffle(numbers);
		}

		long cutoff = top / 2;
		long count = 0;

		long start = System.currentTimeMillis();
		for (Long number : numbers) {
			if (number < cutoff) { /* <- branch hard to predict if shuffled. */
				++count;
			}
		}
		long end = System.currentTimeMillis();

		System.out.println(
				"Counted " + count + " " + (shuffle ? "shuffled" : "sorted") + " numbers in " + (end - start) + "ms");
	}
}
