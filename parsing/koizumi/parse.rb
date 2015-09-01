class Parse
  def initialize rules_file
    @rules_1 = []
    @rules_2 = []
    @lexicon = []
    include_rules(rules_file)
  end

  def cky_parse sentence, pflag: false
    input = sentence.split(" ")#(/\s+|\.|,|/)
    length = input.size
    @trace = []
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

  def earley_parse sentence, pflag:false
    input = sentence.split(" ")#(/\s+|\.|,|/)
    length = input.size
    @trace_e = []
    @POS = @lexicon.map{|sym| sym[0]}.uniq
    @chart=Array.new(length+1).map{Array.new}
    @chart[0].push({left: "gamma", right: ["S"], dot: 0, start: 0, end: 0, func: "Init"})
    0.upto(length) do |i|
      @chart[i].each do |state|
        if incomplete?(state)
          is_POS?(next_cat(state)) ? scanner(state, input): predictor(state) 
        else
          completer(state, pflag)
        end
      end
    end
    chart_count = count_and_display_chart length
    display_earley length, chart_count
  end

  def predictor state
    (@rules_1+@rules_2).each do |rule|
      dot = state[:dot]
      if rule[0] == state[:right][dot]
        new_state = {left: rule[0], right: rule[1..-2], dot: 0, start: state[:end], end: state[:end], func: "Pred"}
        unless  @chart[state[:end]].include?(new_state)
          @chart[state[:end]].push(new_state)
        end
      end
    end
  end

  def scanner state, input
    if @lexicon.select{|entry| entry[0] == state[:right][state[:dot]]}.find {|word| word[1][1..-1] == input[state[:end]]}
      new_state = {left: state[:right][state[:dot]], right: [input[state[:end]]], dot: 1, start: state[:end], end: state[:end]+1, func: "Scan"}
      unless state[:end] == input.length || @chart[state[:end]+1].include?(new_state) 
        @chart[state[:end]+1].push(new_state)
      end
    end
  end

  def completer state, pflag
    @chart[state[:start]].each do |entry|
      if entry[:right][entry[:dot]] == state[:left] && entry[:left] != "gamma"
        new_state = {left: entry[:left], right: entry[:right], dot: entry[:dot]+1, start: entry[:start], end: state[:end], func: "Comp", bp: [{incomp: entry, comp: state}]}
        unless same = @chart[state[:end]].find{|s| same_state?(s, new_state)}
          @chart[state[:end]].push(new_state)
        else
          same[:bp].push(new_state[:bp])
        end
      end
    end
  end

  def same_state? state1, state2
    puts "1", state1, "2", state2
    (state1[:left] == state2[:left] && state1[:right] == state2[:right] && state1[:dot] == state2[:dot] && state1[:start] == state2[:start] && state1[:end] == state2[:end] && state1[:func] == state2[:func])? true:false
  end

  def incomplete? state
    state[:right].count != state[:dot]
  end

  def count_and_display_chart length
    state_count = 0
    0.upto(length) do |i|
      puts "[chart"+i.to_s+"]"
      @chart[i].each do |state|
        puts "S"+state_count.to_s+" "+state[:left]+"->"+state[:right].dup.insert(state[:dot], "・").join(" ")+" ["+state[:start].to_s+", "+state[:end].to_s+"] "+state[:func]
        state_count += 1
      end
    end
    return state_count
  end

  def next_cat state
    state[:right][state[:dot]]
  end

  def is_POS? symbol
    @POS.include?(symbol)
  end

  def display_earley length, count
    if root = @chart[length].find{|state| state[:left] == "S" && !incomplete?(state) && state[:start] == 0 && state[:end] == length}
      @tree_count = 1
      puts "success"
      puts "length: "+length.to_s
      puts "state: "+count.to_s
      count_tree_e(root)
      puts "tree: "+@tree_count.to_s
      display_tree_e root, 0
    end
  end

  def count_tree_e state, skip:false
    if skip
      count_tree_e state[:bp]
    elsif state.is_a?(Hash)
      if state.has_key?(:incomp)
        count_tree_e state[:incomp],skip:true
        count_tree_e state[:comp]
      elsif state.has_key?(:bp)
        count_tree_e state[:bp]
      end
    elsif state.is_a?(Array)
      state.each_with_index do |bp, index|
        @tree_count += 1 if index > 0
        count_tree_e bp
      end
    end
  end

  def display_tree_e state, space, skip:false
    if skip
      display_tree_e state[:bp], space
    elsif state.is_a?(Hash)
      if state.has_key?(:incomp)
        display_tree_e state[:incomp], space, skip:true
        display_tree_e state[:comp], space+1
      elsif state.has_key?(:bp)
        puts node=" "*space+state[:left]
        @trace_e.push(node)
        display_tree_e state[:bp], space+1
      else
        puts node=" "*space+state[:left]
        puts leaf=" "*(space+1)+"_"+state[:right].first
        @trace_e.push(node, leaf)
      end
    elsif state.is_a?(Array)
      size = @trace_e.size
      state.each_with_index do |bp, index|
        puts @trace_e if index > 0
        display_tree_e bp, space
        @trace_e.slice!(size..-1)
      end
    end
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


