public class Rule {
        private String rule;
        private String score;

        public Rule(){

        }

        public Rule(String line){
                if(line.contains(",")){
                String[] words = line.split(",");
                rule = words[0];
                score = words[1];
                }
        }
        public String getRule(){
                return rule;
        }
        public String getTRule(){
                String[] rules = rule.split("->") ;
                return rules[0];
        }
        public String getScore(){
                return score;
        }
        public String getRule(int i){
                String[] rules = rule.split("\\|") ;
                return rules[i];
        }
        public float getScore(int i){
                String[] rules = score.split("\\|") ;
                if(i <= rules.length)
                        return Float.parseFloat(rules[i]);
                else
                        return 0;
        }
        public String getScore(String rs){
                String[] rules = rule.split("\\|") ;
                String[] scores = score.split("\\|") ;
                String r = "";
                Boolean flag = false;
                for(int i=0; i<rules.length && flag == false ;i++){
                        if(rules[i].equals(rs)){
                                r = scores[i];
                                flag = true;
                        }
                }
                return r;
        }
        public String toString(){
                return rule+","+score;
        }
}
