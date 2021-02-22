//-----------------------------------------------------------------------------
// CS-600-WS Project
// Juan Carlos Dibene Simental
// 05/08/2019
// ----------------------------------------------------------------------------
// *

package project;

import java.util.*;

// implementation of compressed trie structure described in section 23.5.2
public class trie {
	// root of the trie
	private internal_node root;
	// number of words in the trie
	private int count;
	
	// helper interface that allows us to store internal and external nodes in
	// the same hash map
	// necessitates static casts though
	private interface node {
	}

	// internal node of trie
	private class internal_node implements node {
		// characters stored in the node (substring)
		public ArrayList<Character> characters;
		// references to children (map where a key is the next character and
	    // its value the node for the substring that begins with that
		// character)
		public HashMap<Character, node> children;

		// initialization
		public internal_node() {
			this.characters = new ArrayList<Character>();
			this.children   = new HashMap<Character, node>();
		}
	}
	
	// external node of trie
	private class external_node implements node {
		// index of word in trie
		public int id;
		
		// initialization
		public external_node(int id) {
			this.id = id;
		}
	}
	
	// initialization
	public trie() {
		root  = new internal_node();
		count = 0;
	}

	// helper function for the add method
	// create a node storing the remaining substring of word in a single node
	// instead of creating a node for each remaining character
	// details in add method
	private internal_node fill(internal_node current, Character c, String word, int k, int src_length) {
		// create new internal node
		internal_node next = new internal_node();
		
		// make it a child of current node
		current.children.put(c, next);
		
		// copy remaining substring of word into new node
		// character at index k (c) is already stored in children hash map so
		// we don't copy it
		for (int z = k + 1; z < src_length; ++z) { next.characters.add(word.charAt(z)); }
		
		// return a reference to the new node so it can be used in subsequent
		// operations
		return next;
	}
	
	// helper function for the add method
	// split a node storing a substring so we can new words into the trie that
	// only contain a fragment of the substring
	// details in add method
	private void split(internal_node current, Character d, int p, int dst_length) {
		// create new internal node
		internal_node bottom = new internal_node();
		
		// break substring at index p
		// copy substring from index p + 1 to new node, we don't copy character
		// at index p (d) since we will store it into children hash map
		for (int z = p + 1; z < dst_length; ++z) { bottom.characters.add(current.characters.get(z)); }
		
		// move children of current node to the new node
		bottom.children = current.children;
		
		// create a new children hash map for current node
		current.children = new HashMap<Character, node>();
		
		// make new node a child of current node with character d
		current.children.put(d, bottom);
		
		// remove the substring that was copied to new node from current node
		// also remove character at index p since it is already in children
		// hash map
		for (int z = dst_length - 1; z >= p; --z) { current.characters.remove(z); }
	}
	
	// helper function for the add method
	// creates a new external node with a new id for a new word added into the
	// trie, if word is already in trie just return its id
	// details in add method
	private int close(internal_node current) {
		// get external node for this word 
		external_node last = (external_node)current.children.get(null);
		
		// if external node does not exist, create a new one with a new id
		// this happens if the word if not in the trie
		if (last == null) {
			last = new external_node(count++);
			current.children.put(null, last);
		}
		
		// return id of word
		return last.id;
	}

	// add word into trie, case insensitive
	// returns id of word in trie
	public int add(String string) {
		String word = string.toLowerCase();
	
		int src_length = word.length();
		if (src_length <= 0) { return -1; }
	
		// start from root of trie
		internal_node current = root;
		
		// root contains no characters so dst_length is 0
		int dst_length = 0;
		
		// starting index of substring in current node is 0
		int p = 0;

		// for each character in word
		for (int k = 0; k < src_length; ++k) {
			// get character at index k
			Character c = word.charAt(k);
			
			// if we have read all characters in current node
			if (p >= dst_length) {
				// select next node (child with character c)
				internal_node next = (internal_node)current.children.get(c);
				
				// if next is null we have reached a dead end
				// create a new internal node and store all remaining
				// characters of word in this node, set this new node as child
				// of current node with character c (fill function)
				// then add an external node with a new id for this word as
				// child of the new internal node with character null (close
				// function) and return new id
				if (next == null) { return close(fill(current, c, word, k, src_length)); }
				
				// if next is not null, move to next node and update dst_length
				// with length of substring stored in next node
				// reset index p to start comparing from beginning of substring
				// (see below)
				current = next;
				dst_length = current.characters.size();
				p = 0;
			}
			else {
				// compare character at index k of word (c) with character at
				//index p of substring in current node (d)
				Character d = current.characters.get(p);
				
				// if the characters are different, we have reached a dead end
				// create a new internal node (bottom), copy the substring of
				// current node starting from index p + 1 into bottom, copy
				// children hash map to bottom, replace current node children
				// with a new hash map, make bottom a child of current node
				// with character d (put (d, bottom) into hash map of current)
				// remove from current node substring starting from index p
				// (split function)
				// then create a new internal node and store all remaining
				// characters of word in this node, set this new node as child
				// of current node with character c (fill function)
				// then add an external node with a new id for this word as
				// child of the new internal node with character null (close
				// function) and return new id
				if (!d.equals(c)) {
					split(current, d, p, dst_length);
					return close(fill(current, c, word, k, src_length));
				}
				// if the characters are equal, move to the next character
				// of current substring
				else {
					p++;
				}
			}
		}
		
		// at this point we have processed all characters in word, if there are
		// characters remaining in current substring, perform a split operation
        // so we can add the external node with the new id for this word (close
		// function) and return new id 
		if (p < dst_length) { split(current, current.characters.get(p), p, dst_length); }
		return close(current);
	}

	// search for word in trie, case insensitive
	// if word is in trie, returns its id
	// else returns -1
	public int find(String string) {
		// The code for this function is the same as for the add method except
		// that when a dead end is reached we return -1 instead of inserting
		// new nodes or splitting exisiting nodes
		String word = string.toLowerCase();
		
		int src_length = word.length();
		if (src_length <= 0) { return -1; }

		internal_node current = root;
		int dst_length = 0;
		int p = 0;

		for (int k = 0; k < src_length; ++k) {
			Character c = word.charAt(k);
			
			if (p >= dst_length) {
				internal_node next = (internal_node)current.children.get(c);
				if (next == null) { return -1; }
				current = next;
				dst_length = current.characters.size();
				p = 0;
			}
			else {
				if (!current.characters.get(p).equals(c)) { return -1; }
			    p++;
			}
		}
		
		if (p < dst_length) { return -1; }
		
		external_node last = (external_node)current.children.get(null);
		return (last != null) ? last.id : -1;
	}
	
//-----------------------------------------------------------------------------
// print utility
// ----------------------------------------------------------------------------
	// auxiliary stack for printing the words in the trie
	private Stack<Character> print_auxiliar = new Stack<Character>();

	// helper function for print method
	private void print_helper(internal_node n) {
		// push characters in current node
		for (Character c : n.characters) { print_auxiliar.push(c); }
		
		// for all children
		for (Character c : n.children.keySet()) {
			// null denotes the end of a word
			if (c == null) {
				// print word in stack and id
				System.out.println("word: " + print_auxiliar.toString() + " id: " + ((external_node)n.children.get(null)).id);
			} else {
				// push child character
				print_auxiliar.push(c);
				// visit child node
				print_helper((internal_node)n.children.get(c));
				// pop child character
				print_auxiliar.pop();
			}
		}
		
		// pop characters in current node
		for (int k = 0; k < n.characters.size(); ++k) { print_auxiliar.pop(); }
	}

	// recursively prints all the words in the trie
	public void print() {
		print_helper(root);
	}
}
