# coding: utf-8
class Conv_to_CNF
  def initialize
    @new_rules = []
    @conved_det = []
  end

  def conversion(input, output)
    @ind=0
    File.open(input).each do |rule|
      next if (symbols = rule.split(" ")).empty?
      p symbols
      if symbols.count < 3
        raise "invalid rule"
      elsif symbols.count > 3
        symbols.each {|symbol| introduce_dummy_NT(symbol) if symbol.match(/^_.*/)} #右辺に終端記号がある場合
        introduce_new_rule(symbols)
        @new_rules.push(symbols)
      else
        @new_rules.push(symbols)
      end
      p @new_rules
    end
    File.write(output, @new_rules.map{|rule| rule.join(" ")}.join("\n"))
  end

  def introduce_dummy_NT (symbol)
    symbol_T = symbol
    symbol.slice!(0)
    symbol.upcase!
    p symbol_T, symbol
    unless @conved_det.include?(symbol) #新しいルールにあったらそれを適用
      @conved_det.push(symbol)
      @new_rules.push([symbol, symbol_T, 1.0])
    end
  end

  def introduce_new_rule (symbols)
    #右辺が２以上の長さのときの変換
    while symbols.count > 4 do
      if new_sym = already_exist?(symbols[1], symbols[2])
        symbols[1] = new_sym[0]
      else
        @new_rules.push(["X"+@ind.to_s, symbols[1], symbols[2], 1.0])
        symbols[1] = "X"+@ind.to_s
        @ind += 1
      end
      symbols.delete_at(2)
    end
  end
  def already_exist?(symbol1, symbol2)
    @new_rules.find{|item| item[1] == symbol1 && item[2] == symbol2 && item[0].match(/X\d+/)}
  end
end
