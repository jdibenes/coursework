//-----------------------------------------------------------------------------
// CS-600-WS Project
// Juan Carlos Dibene Simental
// 05/08/2019
// ----------------------------------------------------------------------------
// *

package project;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

// class used to filter stopwords
public class word_filter {
	// trie containing filtered words
	private trie blacklist;
	
	// create trie from text file
	// each line in the text file must contain exactly one word
	// path is the filename of the text file
	public word_filter(String path) {
		// create trie
		blacklist = new trie();
	try {
		// open file
		BufferedReader reader = new BufferedReader(new FileReader(path));
		// read words from file into trie
		for (String line = reader.readLine(); line != null; line = reader.readLine()) { blacklist.add(line); }
		// close file
		reader.close();
	} catch (IOException e) {
		// do nothing
	}
	}
	
	// check if trie contains word
	public boolean contains(String word) {
		// trie::find returns
        //     nonnegative index of word in trie
		//     -1 if word is not in trie
		return blacklist.find(word) >= 0;
	}
}
