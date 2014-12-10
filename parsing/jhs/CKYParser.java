import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Scanner;

public class CKYParser {

private static String vTable[][];
private static String sTable[][];
private static String fTable[][][];
private static String[] inputs;
private static HashMap<Integer, Rule> grammar;
private static HashMap<String, Rule> lexicon;

public CKYParser(){
        grammar = new HashMap<Integer, Rule>();
        lexicon = new HashMap<String, Rule>();
}
public void loadGrammar(String file) throws IOException
{
        BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
        String line = null;
        int size = 0;

        while((line = br.readLine())!= null){
        //System.out.println("Loading the rules...");
        String[] lines = line.split(" ");
        String word = "";
                for(int i = 1; i < lines.length-1; i++){
                        word = word +" "+lines[i];
                        word = word.trim();
                }
                if(word.contains("_")){
                        word = (String) word.substring(1);
                        if(lexicon.get(word)!= null){
                                String words = lexicon.get(word).getRule();
                                String score = lexicon.get(word).getScore();
                                lines[0] = words + "|" + lines[0];
                                lines[lines.length-1] = score + "|" + lines[lines.length-1];
                        }
                        Rule rule = new Rule(lines[0]+","+lines[lines.length-1]);
                        lexicon.put(word,rule);
                }
                else{
                        size++;
                        Rule rule = new Rule(lines[0]+"->"+word+","+lines[lines.length-1]);
                        grammar.put(size,rule);
                }
        }
        br.close();
        System.out.println("Finish loading the rules...");
}

public  String[] mergeGrammer(String r,String rule,String s,String score){
        //System.out.println(r+"\t"+s);
        //System.out.println(rule+"\t"+score);
        String[] result = new String[2];

        result[0]="";
        result[1]="";
        if(rule.equals("")&&score.equals("")){
                rule = r;
                result[0]=r;
                result[1]=s;
        }
        else{
                Boolean flag = false;
                String[] rs = rule.split("\\|");
                String[] ss = score.split("\\|");
                for(int i = 0 ; i < rs.length ; i++){
                    if(r.equals(rs[i])){
                        if(Float.parseFloat(ss[i])<Float.parseFloat(s))
                                ss[i] = s;
                        flag = true;
                    }
                }
                if(flag == true){
                        result[1] = ss[0];
                        for(int i = 1 ; i < ss.length ; i++){
                              result[1] = result[1] + "|" +ss[i];
                        }
                        result[0] = rule;
                }
                else{
                        result[0] = r +"|"+ rule;
                        result[1] = s +"|"+ score;
                }
            }
        return result;
}

public void CKY(String sentence)
{
        inputs = sentence.split(" ");
        vTable = new String[inputs.length][inputs.length+1];
        sTable = new String[inputs.length][inputs.length+1];
        fTable = new String[inputs.length][inputs.length+1][grammar.size()];
        System.out.println("input: "+sentence);
        System.out.println("length: " + inputs.length);
        for (int i = 0; i < inputs.length; i++){
                for (int j = 1; j <= inputs.length; j++){
                        vTable[i][j]= "";
                        sTable[i][j]="";
                }
        }
        for (int i = 0; i < inputs.length; i++){
                for (int j = 1; j <= inputs.length; j++){
                        for(int k = 0; k < grammar.size(); k++)
                                fTable[i][j][k]= ",0";
                }
        }
        for (int j = 1; j <= inputs.length; j++){
                if(lexicon.get(inputs[j-1]) != null){
                        Rule r = lexicon.get(inputs[j-1]);
                        vTable[j-1][j] = r.getRule();
                        sTable[j-1][j] = r.getScore();
                }
                for(int i = j-2; i >= 0; i--){
                        for(int k = i+1 ; k <= j-1 ; k++){
                                String[] r1 = vTable[i][k].split("\\|");
                                String[] s1 = sTable[i][k].split("\\|");
                                String[] r2 = vTable[k][j].split("\\|");
                                String[] s2 = sTable[k][j].split("\\|");
                                for(int i1 = 0 ; i1< r1.length ; i1++){
                                        for (int i2 = 0 ; i2 <r2.length ; i2++){
                                                String rr = r1[i1]+ " " + r2[i2];
                                                for(int g = 0; g< grammar.size() ; g++){
                                                        String rule = grammar.get(g+1).getRule();
                                                        String score = grammar.get(g+1).getScore();
                                                        String[] rs = rule.split("->",2);
                                                        if(rs[1].trim().equals(rr)){
                                                                float curs = Float.parseFloat(score)*(Float.parseFloat(s1[i1]))*(Float.parseFloat(s2[i2]));
                                                                float pre = Float.parseFloat(fTable[i][j][g].substring(fTable[i][j][g].indexOf(",")+1));
                                                                if(pre <= curs){
                                                                        score = String.valueOf(curs);
                                                                        String[] result= mergeGrammer(rs[0],vTable[i][j],score,sTable[i][j]);
                                                                        vTable[i][j]= result[0];
                                                                        sTable[i][j]= result[1];
                                                                        fTable[i][j][g] = String.valueOf(k) + ":" + rule + "," + String.valueOf(curs);
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
                //printMatrix(inputs.length);
                //printTree(0,inputs.length,"X3");
        }
}

private void printTree(int row, int col, String rule, String sign)
{
        if(col == row+1){
                System.out.println(sign+"["+row+" "+col+"] "+rule+"->"+inputs[row]+","+lexicon.get(inputs[row]).getScore(rule));
                if(col == inputs.length)
                        System.out.println();
        }
        else{
                for(int k = 0; k<grammar.size();k++){
                        String key = fTable[row][col][k];
                        if(!key.equals(",0")){
                                int i = Integer.parseInt(key.substring(0, key.indexOf(":")));
                                String s = key.substring(key.indexOf(":")+1,key.indexOf("-"));
                                String rs = key.substring(key.indexOf(">")+1,key.indexOf(" "));
                                String cs = key.substring(key.indexOf(" ")+1,key.indexOf(","));
                                if(s.equals(rule)){
                                        if(row == 0 && col == inputs.length)
                                                sign = "";
                                        System.out.println(sign+"["+row+" "+col+"]~"+key);
                                        //System.out.println(key.indexOf(":"));
                                        sign = sign + "\t";
                                        printTree(row,i,rs,sign);
                                        printTree(i,col,cs,sign);
                                }
                        }
                }
        }
}
private static void printMatrix(int size)
{
        System.out.println("=======================  V Matrix  =========================");

        for (int i = 0; i <size; i++){
                for (int k = 1; k <= size; k++){
                        if(vTable[i][k].equals(""))
                                System.out.print( "*\t");
                        System.out.print(vTable[i][k] + "\t");
                }
                System.out.println();
                for (int k = 1; k <= size; k++){
                        if(sTable[i][k].equals(""))
                                System.out.print( "*\t");
                        System.out.print(sTable[i][k] + "\t");
                }
                System.out.println();
        }
        System.out.println("==============================================================");
}

public static void main (String[] args) throws IOException{
        CKYParser c = new CKYParser();
        c.loadGrammar("rules_cnf.txt");
        System.out.println("Input the sentence");
        Scanner sc = new Scanner(System.in);
        String input = null;
        while((input =  sc.nextLine())!=null && !input.equals("n")){
                c.CKY(input);
                c.printMatrix(inputs.length);
                System.out.println();
                //System.out.println("print the grammar tree");
                String[] rull = vTable[0][inputs.length].split("\\|");
                //System.out.println(vTable[0][inputs.length]);
                for(int i = 0; i<rull.length ; i++){
                        System.out.println("-------------------print the grammar tree of "+rull[i]+"--------------------");
                        c.printTree(0,inputs.length,rull[i],"");
                        System.out.println();
                }
        }
}
}