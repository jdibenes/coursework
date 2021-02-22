//-----------------------------------------------------------------------------
// CS-600-WS Project
// Juan Carlos Dibene Simental
// 05/08/2019
// ----------------------------------------------------------------------------
// *

package project;

import java.io.*;
import java.util.ArrayList;
import java.util.HashSet;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

// class used to crawl a set of web pages
// assumes the web pages are stored locally and organized in the following way:
// path/
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
// the crawler lists all the folders in the path directory (websites) and
// begins crawling each website from index.html, so the crawler cannot
// discover webpages that are not linked from index.html or any of the pages
// linked from index.html or any of the pages linked from these pages (and so
// on)
//
// all links in a webpage should be relative to the directory containing the
// webpage
// links to webpages not on disk are ignored
//
// webpages are read using jsoup: https://jsoup.org/
public class web_crawler {
	// callback for processing the plain text of a webpage
	public interface document_visitor {
		void visit(String path, String content);
	}
	
	// obtains the webpages referenced in a webpage, returns the addresses of
	// these webpages in an (extendable) array of strings (may contain
	// duplicates)
	// considers only links ending in .html pointing to html files
	// returns null if empty
	// source: https://jsoup.org/cookbook/extracting-data/example-list-links
	private static ArrayList<String> list_referenced_pages(Document document) {
		// select hyperlinks in webpage
		Elements links = document.select("a[href]");

		// count links
		int count = links.size();
		// if no links we are done
		if (count <= 0) { return null; }
	
		// create (extendable) array
		ArrayList<String> pages = new ArrayList<String>(count);
	
		// put links into array, consider only html files (ignore anchors,
		// htm, and the like)
		for (Element link : links) {
			String path = link.attr("href");
			if (path.endsWith(".html")) { pages.add(path); }
		}

		// return array or null if array is empty
		return (pages.size() > 0) ? pages : null;
	}
	
	// returns directory/filename
	private static String make_path(String directory, String filename) {
		return directory + File.separator + filename;
	}
	
	// relativizes directory/filename with respect to root such that for
	// root/directory/filename
	// root/directory/../directory/filename
	// root/directory/./filename
	// etc.
	// this function returns
	// root/directory/filename
	// this allows us to get a unique path for any given webpage
	private static String relativize(File root, String directory, String filename) {
		try { return root.toURI().relativize(new File(make_path(directory, filename)).getCanonicalFile().toURI()).getPath(); } catch (IOException e) { return null; }
	}
	
	// recursively crawls webpage at root/directory/filename
	// visits the webpage and the webpages that it references
	// the visit action is defined by the visitor callback
	// we use a hashset to keep track of the webpages that we already visited
	private static void recursive_crawl(File root, String directory, String filename, document_visitor visitor, HashSet<String> visited) {
		// get the unique path of this webpage
		String page = relativize(root, directory, filename);
		if (page == null) { return; }
		
		// do nothing if we already visited this page
		// we only check the address of the webpage, we do not check for
		// duplicate content
		if (visited.contains(page)) { return; }
		
		// add to the set of visited pages
		// this why we need a unique path for each webpage
		visited.add(page);
		
		// read webpage using jsoup 
		File file = new File(make_path(root.getPath(), page));
		
		Document document;
		try { document = Jsoup.parse(file, null); } catch (IOException e) { return; }
		
		// process the plain text of the webpage
		visitor.visit(file.getPath(), document.text());
		
		// create a list of the webpages referenced by this webpage
		ArrayList<String> children = list_referenced_pages(document);
		if (children == null) { return; }
		
		// crawl referenced webpages
		String path = file.getParent();
		for (String child : children) { recursive_crawl(root, path, child, visitor, visited); }
	}
	
	// list websites stored in root directory
	private static File[] list_local_sites(File root) {
		return root.listFiles(File::isDirectory);	
	}
	
	// crawler entry point
	// path is the root directory storing the websites
	// visitor defines the action to perform on the text of each webpage
	public static void crawl(String path, document_visitor visitor) {
		File root = new File(path);
		
		// create new hashset to keep track of the webpages we already visited
		HashSet<String> visited = new HashSet<String>();
		
		// crawl every website starting from its index.html
		for (File site : list_local_sites(root)) { recursive_crawl(root, site.getPath(), "index.html", visitor, visited); }
	}
}
