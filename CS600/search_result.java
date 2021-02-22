//-----------------------------------------------------------------------------
// CS-600-WS Project
// Juan Carlos Dibene Simental
// 05/08/2019
// ----------------------------------------------------------------------------
// *

package project;

// class used to store search results
// a search result represents a webpage
public class search_result {
	// number of times the search term (or sum if multiple terms) appears in
	// this webpage
	private int count;
	// address of this webpage
	private String address;
	
	// initialization
	// count and address are "constants"
	public search_result(int count, String address) {
		this.count   = count;
		this.address = address;
	}
	
	// get count
	public int count() {
		return count;
	}
	
	// get address
	public String address() {
		return address;
	}
	
	// comparator used to sort search results
	// a search result with higher count must appear before a search result
	// with lower count
	public static int comparator(search_result o1, search_result o2) {
		return Integer.compare(o2.count(), o1.count());
	}
}
