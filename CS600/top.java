//-----------------------------------------------------------------------------
// CS-600-WS Project
// Juan Carlos Dibene Simental
// 05/08/2019
// ----------------------------------------------------------------------------
// *

package project;

import java.util.ArrayList;

public class top {
	// entry point
	public static void main(String[] args) {
		// process arguments
		options cmd = new options(args);
		
		// initialize search engine
		search_engine engine = new search_engine();
		
		String input = cmd.search();
		boolean all  = cmd.all();
		
		// extract search terms
		ArrayList<String> search_terms = engine.extract(input);
		
		// print inputs
		System.out.printf("Input string: %s", input);
		System.out.println();
		
		System.out.printf("Search terms:");
		for (String word : search_terms) { System.out.printf(" %s", word); }
		System.out.println();
		
		System.out.printf("Must contain all: %s", all);
		System.out.println();

		// perform search
		ArrayList<search_result> results = engine.search(search_terms, all);
		if (results == null) {
			System.out.println("No results");
			return;
		}
		
		// print results
		System.out.println("Results:");
		for (search_result result : results) { System.out.println("<" + result.count() + "> " + result.address()); }
		System.out.println();
	}
}
