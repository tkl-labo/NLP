package EditDistance;

import java.util.Scanner;

public class Min_EditDistance {
	
	private static Scanner sc;

	//Calculate the minimum Edit Distance from sword to dword using Dynamic Programming
	public static float DP_EditDistance(String sword, String dword, float scost, float icost, float dcost) {
		
		//Gain the length of source_word and destination_word
		int slen = sword.length();
		int dlen = dword.length();
	 
		//Minimum Edit Distance finally return to ed[slen][dlen]
		float[][] ed = new float[slen + 1][dlen + 1];
	 
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
				
				//Check last two chars equal or not, If equal, substitutions cost 0
				float cost;
				if (c1 == c2)
					cost = 0;
				else
					cost = scost;
				
				float substitute = ed[i][j] + cost;
				float insert = ed[i][j + 1] + icost;
				float delete = ed[i + 1][j] + dcost;
 
				float min = substitute > insert ? insert : substitute;
				
				min = delete > min ? min : delete;
				ed[i + 1][j + 1] = min;
				
				
			}
		}
	
		/*for(int i=0 ; i<slen+1 ; i++){
			for(int j=0 ; j<dlen+1 ; j++){
				System.out.print(ed[i][j]+"\t");
				
			}
			System.out.println();
		}*/
		return ed[slen][dlen];
	}
	
	public static void main(String[] args){
		
		sc = new Scanner(System.in); 
	    System.out.print("Please input two words");
		
		String sword = sc.next(); //source_wrod	
		String dword = sc.next();//destination_word
		
		System.out.println("Please input the cost value of substitution: ");
		float scost = sc.nextFloat(); 
		System.out.println("Please input the cost value of insertion: ");
		float icost = sc.nextFloat();
		System.out.println("Please input the cost value of deletion: ");
		float dcost = sc.nextFloat();
		
		float ed = DP_EditDistance(sword,dword,scost,icost,dcost);//Calculate the minimum edit distance using DP
		System.out.println("S_Word: " + sword);
		System.out.println("D_Word: " + dword);
		System.out.println("Min_ED: " + ed);
		
	}


}
