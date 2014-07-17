package EditDistance;

public class Min_EditDistance {
	
	//Calculate the minimum Edit Distance from sword to dword using Dynamic Programming
	public static int DP_EditDistance(String sword, String dword) {
		
		//Gain the length of source_word and destination_word
		int slen = sword.length();
		int dlen = dword.length();
	 
		//Minimum Edit Distance finally return to ed[slen][dlen]
		int[][] ed = new int[slen + 1][dlen + 1];
	 
		//Initialize the Edit Distance
		for (int i = 0; i <= slen; i++) {
			ed[i][0] = i;
		}
	 
		for (int j = 0; j <= dlen; j++) {
			ed[0][j] = j;
		}
	 
		//Iteration process
		for (int i = 0; i < slen; i++) {
			char c1 = sword.charAt(i);
			for (int j = 0; j < dlen; j++) {
				char c2 = dword.charAt(j);
				
				//Check last two chars equal or not
				if (c1 == c2)
					ed[i + 1][j + 1] = ed[i][j];
				
				//Calculate the minimum editing operations
				else {
					int substitute = ed[i][j] + 1;//In Levenshtein, substitutions cost 2
					int insert = ed[i][j + 1] + 1;
					int delete = ed[i + 1][j] + 1;
	 
					int min = substitute > insert ? insert : substitute;
					min = delete > min ? min : delete;
					ed[i + 1][j + 1] = min;
				}
			}
		}
	 
		return ed[slen][dlen];
	}
	
	public static void main(String[] args){
		
		String sword = "intention"; //source_wrod	
		String dword = "excution";//destination_word
		int ed = DP_EditDistance(sword,dword);//Calculate the minimum edit distance using DP
		System.out.println("S_Word: " + sword);
		System.out.println("D_Word: " + dword);
		System.out.println("Min_ED: " + ed);
		
	}
	

}
