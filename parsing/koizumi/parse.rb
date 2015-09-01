class Parse
  def initialize rules_file
    @rules_1 = []
    @rules_2 = []
    @lexicon = []
    @trace = []
    include_rules(rules_file)
  end

  def cky_parse sentence, pflag: false
    input = sentence.split(" ")#(/\s+|\.|,|/)
    length = input.size
    @pflag=pflag
    @memo = Array.new(length).map{Array.new(length+1).map{Array.new}}
    #POSタグを終端記号として入れる
    input.each_with_index do |word, ind|
      raise word + " doesn't exist in lexicon" if (rules = @lexicon.select {|item| item[1][1..-1] == word}).empty?
      rules.each do |rule| 
        @memo[ind][ind+1].push({left: rule[0], right: [rule[1]], prob:rule[2].to_f, count: 1})
      end
      @memo[ind][ind+1].each {|sym| merge_bp_array(left_hand_of(sym), ind, ind+1)} #loopがあっても大丈夫
    end
    1.upto(length) do |i|
      (i-2).downto(0) do |j|
        (j+1).upto(i-1) do |k|
          @memo[j][k].each do |sym1|
            @memo[k][i].each do |sym2|
              merge_bp_array(left_hand_of(sym1, symptr2: sym2), j, i)
            end
          end
        end
        @memo[j][i].each {|sym| merge_bp_array(left_hand_of(sym), j,i)}
      end
    end
    #debug length
    display length
  end
  def cky_prob_parse sentence
    cky_parse sentence, pflag: true
  end
  def early_parse
  end
  def include_rules filename
    File.open(filename).each do |rule|
      rule_ary=rule.split(" ")
      if rule_ary.size == 3
        if rule_ary[1].match(/^_.*/)
          @lexicon.push(rule_ary)
        else
          @rules_1.push(rule_ary)
        end
      else
        @rules_2.push(rule_ary)
      end
    end
  end
  def left_hand_of symptr1, symptr2: nil
    symbols = []
    unless symptr2
      @rules_1.each do |rule|
        if rule[1] == symptr1[:left]
          symbols.push({left: rule[0], right: [symptr1], prob: rule[2].to_f*symptr1[:prob], count: symptr1[:count]})
        end
      end
    else
      @rules_2.each do |rule|
        symbols.push({left: rule[0], right: [symptr1, symptr2], prob: rule[3].to_f*symptr1[:prob]*symptr2[:prob], count: symptr1[:count]*symptr2[:count]}) if rule[1] == symptr1[:left] && rule[2] == symptr2[:left]
      end
    end
    symbols
  end
  def merge_bp_array new_ary, i, j 
    new_ary.each do |new|
      if (same_left_bp = @memo[i][j].find {|upd| new[:left] == upd[:left]})
        if @pflag
          if same_left_bp[:prob] < new[:prob]
            same_left_bp[:right] = new[:right]
            same_left_bp[:prob] = new[:prob]
          end
        else
          same_left_bp[:right] = [same_left_bp[:right], new[:right]]
        end
        same_left_bp[:count] += new[:count]
        new_ary.delete(new)
      end
    end
    @memo[i][j].concat(new_ary)
  end
  def display length
    unless @memo[0][length].empty?
      puts "success"
      puts "edges: "
      puts "trees: "+ @memo[0][length].first[:count].to_s
      @memo[0][length].select! {|tree| tree[:left] == "S"}
      puts @memo[0][length].first[:prob] if @pflag
      display_tree  @memo[0][length], 0
    else
      puts "failed"
    end
  end
  def display_tree rule, count
    if rule.is_a?(Array)
      if rule.count == 1 && rule[0].is_a?(String)
        puts " "*count + rule[0]
        @trace.push(" "*count+rule[0])
      else
        size = @trace.size
        rule.each_with_index do |subtree, index|
          puts @trace if index > 0 && subtree.is_a?(Array)
          display_tree subtree, count
          @trace.slice!(size..-1)
        end
      end
    elsif rule.is_a?(Hash)
      indent = " "*count
      puts indent+rule[:left]
      @trace.push(indent+rule[:left])
      display_tree rule[:right], count+1
    end
  end
  def debug length
    1.upto(length) do |i|
      (i-1).downto(0) do |j|
        p "["+j.to_s+", "+i.to_s+"]"
        p @memo[j][i]
      end
    end
  end
end


