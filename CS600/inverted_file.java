//-----------------------------------------------------------------------------
// CS-600-WS Project
// Juan Carlos Dibene Simental
// 05/08/2019
// ----------------------------------------------------------------------------
// *

package project;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

// implementation of the inverted file structure described in section 23.5.4
public class inverted_file {
	// occurrence list
	// we use a hash map to count how many times any given word appears in each
	// webpage
	private ArrayList<HashMap<String, Integer>> occurrence_list;
	// trie storing the indices of the index terms in the occurrence list
	private trie index_terms;

	// initialization
	public inverted_file() {
		occurrence_list = new ArrayList<HashMap<String, Integer>>();
		index_terms     = new trie();
	}
	
	// add word and address of webpage into inverted file
	public void add(String word, String address) {
		// add word into trie and get index in occurrence list
		// if word is already in trie just get the stored index
		int word_index = index_terms.add(word);
		
		// if word was not in trie create an entry for this word in the
		// occurence list
		if (word_index >= occurrence_list.size()) { occurrence_list.add(new HashMap<String, Integer>()); }

		// update the count for the number of times this word appears in this
		// webpage
		HashMap<String, Integer> dictionary = occurrence_list.get(word_index);
		Integer total = dictionary.get(address);
		dictionary.put(address, (total != null) ? total + 1 : 1);
	}

	// retrieve the corresponding entry in occurrence list for word
	// returns null if word is not in the inverted file 
	private HashMap<String, Integer> find(String word) {
		// get word index stored in trie
		int word_index = index_terms.find(word);
		
		// return entry from occurrence list or null if does not exist
		return (word_index >= 0) ? occurrence_list.get(word_index) : null;
	}
	
	// convert the search results into search_result(s) and sort by number of
	// times the search terms appear in the webpages
	private ArrayList<search_result> pack_results(HashMap<String, Integer> occurrences) {
		// convert to search_result an store in (extendable) array
		ArrayList<search_result> results = new ArrayList<search_result>();
		for (String address : occurrences.keySet()) { results.add(new search_result(occurrences.get(address), address)); }
		
		// sort by count
		Collections.sort(results, search_result::comparator);
		
		// return sorted array
		return results;
	}
	
	// union occurrence entries  (combine web pages that contain any of the
	// search terms into a single hash map, corresponding values are summed)
	private static void union_occurrences(HashMap<String, Integer> self, HashMap<String, Integer> other) {
		// for each address in other
		for (String address : other.keySet()) {
			Integer count = self.get(address);
			// sum counts and add to self
			self.put(address, other.get(address) + ((count != null) ? count : 0));
		}
	}
	
	// intersect occurence entries (combine web pages that contain all of the
	// search terms into a single hash map, corresponding values are summed)
	private static void intersect_occurrences(HashMap<String, Integer> self, HashMap<String, Integer> other) {
		// auxiliar array to keep track of addresses that are not part of the 
		// intersection
		ArrayList<String> remove = new ArrayList<String>(); 
		
		// for each address in self
		for (String address : self.keySet()) {
			// if address also in other sum count and add to self
			// else mark address for removal (not part of the intersection)
			Integer count = other.get(address);
			if (count != null) { self.put(address, self.get(address) + count); } else { remove.add(address); }
		}
		
		// remove marked addresses from self
		for (String word : remove) { self.remove(word); }
	}
	
	// find webpages containing any of the search terms
	public ArrayList<search_result> find_any(ArrayList<String> terms) {
		if (terms.size() < 1) { return null; }
		// create hash map where keys are the addresses of the webpages that
		// contain any of the search terms and the values are the number of
		// times the search terms appears in the webpage (sum)
		HashMap<String, Integer> occurrences = new HashMap<String, Integer>();
		
		// for each search term
		for (String word : terms) {
			// get entry in occurrence list
			HashMap<String, Integer> occurrence = find(word);
			// perform union operation
			if (occurrence != null) { union_occurrences(occurrences, occurrence); }
		}

		// return search results or null if none
		return (occurrences.size() > 0) ? pack_results(occurrences) : null;
	}
	
	// find webpages containing all of the search terms
	public ArrayList<search_result> find_all(ArrayList<String> terms) {
		if (terms.size() < 1) { return null; }
		// create hash map where keys are the addresses of the webpages that
		// contain all of the search terms and the values are the number of
		// times the search terms appears in the webpage (sum)
		// initialize with entry for first search term 
		HashMap<String, Integer> occurrences = new HashMap<String, Integer>(find(terms.get(0)));
		
		// for each remaining search term
		for (String word : terms.subList(1, terms.size())) {
			// if initial map is empty intersection is also empty and we are
			// done
			if (occurrences.size() <= 0) { return null; }
			// get entry in occurrence list
			HashMap<String, Integer> occurrence = find(word);
			// if not in occurrence list intersection is empty and we are done
			if (occurrence == null) { return null; }
			// perform intersection operation
			intersect_occurrences(occurrences, occurrence);
		}
		
		// return search results or null if none
		return (occurrences.size() > 0) ? pack_results(occurrences) : null;
	}
}
