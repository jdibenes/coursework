//-----------------------------------------------------------------------------
// CS-600-WS Project
// Juan Carlos Dibene Simental
// 05/08/2019
// ----------------------------------------------------------------------------
// *

package project;

// class used to extract words from a string
// by words we mean sequences of letters or digits, anything that is not a
// letter or digit is treated as a space separating words
public class word_extractor {
	// full text
	private String text;
	// current position in the text
	private int index;
	// length of text
	private int length;
	
	// document is the string from where to extract words
	public word_extractor(String document) {
		text   = document;
		index  = 0;
		length = document.length();
	}
	
	// get next word from text
	// returns null if there are no more words to extract (end of text)
	public String get_next_word() {
		// find first letter or digit from current position, update index
		for (; (index < length) && !Character.isLetterOrDigit(text.charAt(index)); ++index);
		// if found end of string before finding a letter or digit return null
		if (index >= length) { return null; }
		
		// current position is beginning of word
		int start = index;
		
		// find first character that is not a letter or digit from current
		// position (end of word), update index
		// end of string is also end of word
		for (; (index < length) &&  Character.isLetterOrDigit(text.charAt(index)); ++index);
		
		// return word, which is the substring from the index of first letter 
		// or digit to the index of the first not letter or digit (or end of 
		// string) minus 1
		return text.substring(start, index);
	}
}
