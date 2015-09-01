class Parse
  def initialize rules_file
    @rules_1 = []
    @rules_2 = []
    @lexicon = []
    @trace = []
    include_rules(rules_file)
  end

  def cky_parse sentence
    input = sentence.split(" ")#(/\s+|\.|,|/)
    length = input.size
    @memo = Array.new(length).map{Array.new(length+1).map{Array.new}}
    @count= Array.new(length).map{Array.new(length+1).map{Array.new}}
    #POSタグを終端記号として入れる
    input.each_with_index do |word, ind|
      raise word + " doesn't exist in lexicon" if (rules = @lexicon.select {|item| item[1][1..-1] == word}).empty?
      rules.each {|rule| @memo[ind][ind+1].push({left: rule[0], right: [rule[1]]})}
      @memo[ind][ind+1].each {|sym| merge_bp_array(left_hand_of(sym), @memo[ind][ind+1])} #loopがあっても大丈夫
    end
    1.upto(length) do |i|
      (i-2).downto(0) do |j|
        (j+1).upto(i-1) do |k|
          @memo[j][k].each do |sym1|
            @memo[k][i].each do |sym2|
              merge_bp_array(left_hand_of(sym1, symptr2: sym2), @memo[j][i])
            end
          end
          @memo[j][i].each {|sym| merge_bp_array(left_hand_of(sym), @memo[j][i])}
        end
      end
    end
    #debug length
    display length
  end
  def cky_prob_parse
  end
  def early_parse
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
        symbols.push({left: rule[0], right: [symptr1]}) if rule[1] == symptr1[:left]
      end
    else
      @rules_2.each do |rule|
        symbols.push({left: rule[0], right: [symptr1, symptr2]}) if rule[1] == symptr1[:left] && rule[2] == symptr2[:left]
      end
    end
    symbols
  end
  def merge_bp_array new_ary, updated
    new_ary.each do |new|
      if (same_left_bp = updated.find {|upd| new[:left] == upd[:left]})
        same_left_bp[:right] = [same_left_bp[:right], new[:right]]
        new_ary.delete(new)
      end
    end
    updated.concat(new_ary)
  end
  def display length
    unless @memo[0][length].empty?
      puts "success"
      puts "edges: "
      puts "trees: "+ @memo[0][length].count.to_s
      @memo[0][length].delete_if {|tree| tree[:left].match(/^X\d+/)}
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


