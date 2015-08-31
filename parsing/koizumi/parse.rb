class Parse
  def initialize rules_file
    @rules_1 = []
    @rules_2 = []
    @lexicon = []
    include_rules(rules_file)
  end

  def cky_parse sentence
    input = sentence.split(" ")#(/\s+|\.|,|/)
    length = input.size
    @memo = Array.new(length).map{Array.new(length+1).map{Array.new}}
    #POSタグを終端記号として入れる
    input.each_with_index do |word, ind|
      raise word + " doesn't exist in lexicon" if (rules = @lexicon.select {|item| item[1][1..-1] == word}).empty?
      rules.each {|rule| @memo[ind][ind+1].push(BackPointer.new rule[0], right: rule[1])}
      #p @memo[ind][ind+1]
    end
    debug length
    1.upto(length) do |i|
      (i-2).downto(0) do |j|
        (i+1).upto(j-1) do |k|
          @memo[i][k].each do |sym1|
            @memo[k][j].each do |sym2|
              (left_hand_of(sym1, sym2)).merge(@memo[i][j])
            end
          end
          @memo[i][j].each {|sym| (left_hand_of(sym)).merge(@memo[i][j])}
        end
      end
    end
    debug length
  end
  def cky_prob_parse
  end
  def early_parse
  end
  def early_prob_parse
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
        symbols.push(BackPointer.new rule[0], right: [symptr1]) if rule[1] == symptr1.left
      end
    else
      @rules_2.each do |rule|
        symbols.push(BackPointer.new rule[0], right: [symptr1, symptr1]) if rule[1] == symptr1.left && rule[2] == symptr2.left
      end
    end
    symbols
  end
  def debug length
    1.upto(length) do |i|
      (i-1).downto(0) do |j|
        p "["+j.to_s+", "+i.to_s+"]"
        p @memo[j][i]#.each {|k| p k.left}
      end
    end
  end
end
class BackPointer
  attr_accessor :left, :right
  def initialize left, right: []
    @left = left
    @right = [right]
  end
  def add_right right: []
    @right.push(right)
  end
  def merge bp_array
    if same_left = bp_array.find{|bp| bp.left == self.left}
      self.add_right(same_left.add_right(self.right))
    else
      bp_array.push(self)
    end
  end
end


