//-----------------------------------------------------------------------------
// CS-600-WS Project
// Juan Carlos Dibene Simental
// 05/08/2019
// ----------------------------------------------------------------------------
// *

package project;

import java.util.ArrayList;

// implementation of the simplified search engine described in section 23.5.4
// assumes the web pages are stored locally and organized in the following way:
// web/
//     website_1/
//         index.html
//         (additional html pages)
//     website_2/
//         index.html
//         (additional html pages)
//     ...
//     website_n/
//         index.html
//         (additional html pages)
//
// where the web folder is in the current working directory
// details in web_crawler.java
public class search_engine {
	// inverted file
	private inverted_file occurrence_file;
	// used to filter stopwords from webpage text and search input
	private word_filter stopword_filter;
	
	// callback for performing stopword filtering
	private interface filter_callback {
		void action(String s);
	}
	
	// inititalization
	public search_engine() {
		// load stopwords from text file
		stopword_filter = new word_filter("./web/stopwords.txt");
		
		// create inverted file
		occurrence_file = new inverted_file();
	    
		// crawl stored websites
		web_crawler.crawl("./web/", (path, content) -> process(path, content));
	}
	
	// perform filter action on words that are not in stopword_filter
	private void filter(word_extractor list, filter_callback f) {
		for (String s = list.get_next_word(); s != null; s = list.get_next_word()) { if (!stopword_filter.contains(s)) { f.action(s); } }
	}
	
	// filter stopwords from webpage text and add to inverted file
	private void process(String path, String content) {
		filter(new word_extractor(content), (s) -> occurrence_file.add(s, path));
	}
	
	// extract search terms from input string
	// ignores stopwords and characters that are nor letters or digits
	// search terms returned in an (extendable) array
	public ArrayList<String> extract(String string) {
		ArrayList<String> search_terms = new ArrayList<String>();
		filter(new word_extractor(string), (s) -> search_terms.add(s));
		return search_terms;
	}
	
	// search function, case insensitive
	// if all is true search for webpages containing all of the search terms
	public ArrayList<search_result> search(ArrayList<String> search_terms, boolean all) {
		return all ? occurrence_file.find_all(search_terms) : occurrence_file.find_any(search_terms);
	}
}
