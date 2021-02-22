//-----------------------------------------------------------------------------
// CS-600-WS Project
// Juan Carlos Dibene Simental
// 05/08/2019
// ----------------------------------------------------------------------------
// *

package project;

// class used from processing command line arguments
// options
// -search:"multiple search terms"
// -all
public class options {
	private String search; 
	private boolean all;
	
	// initialization
	public options(String[] args) {
		search = "";
		all    = false;
		
		// parse arguments
		for (String cmd : args) {
			String lcmd = cmd.toLowerCase();

			     if (lcmd.startsWith("-search:")) { search = get_search_string(lcmd); }
			else if (lcmd.equals("-all"))         { all    = true; }
		}
	}
	
	// extract search string from argument
	private String get_search_string(String lcmd) {
		try { return lcmd.substring(8); } catch (Exception e) { return ""; }
	}

	// returns true if arguments contain option -all 
	public boolean all() {
		return all;
	}
	
	// returns search string, the text after -search:
	// quotes are removed automatically by the environment
	public String search() {
		return search;
	}
}
