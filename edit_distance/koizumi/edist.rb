# coding: utf-8
class Edist
  INS_COST = 1
  DEL_COST = 1
  SUB_COST = 2
  def initialize
  end

  def calc_med(source, target)
    @source = source
    @target = target
    len_s = source.length
    len_t = target.length
    @matrix = Array.new(len_s+1).map{Array.new(len_t+1, 0)}
    @btrace = Array.new(len_s+1).map{Array.new(len_t+1).map{Array.new}}
    @matrix.map.with_index{|r, index| r[0] = index*DEL_COST}
    @matrix[0].map!.with_index{|c, index| c = index*INS_COST}
    @btrace.map.with_index{|r, index| r[0]=[:del] unless index==0}
    @btrace[0].map!.with_index{|c, index| c=[:ins] unless index==0}
    @matrix.each_with_index do |row, index_s|
      row.each_with_index do |cell, index_t|
        @matrix[index_s][index_t] = calc_cell(index_s, index_t) unless index_s == 0 || index_t == 0 
      end
    end
    @align = Array.new #trace of action
    @result= String.new #trace of letter
    back_trace(len_s, len_t)
    return @matrix[len_s][len_t]
  end

  private

  def calc_cell(s, t)
    sub_cost, del_cost, ins_cost = 100, 100, 100
    if @source[s-1] == @target[t-1]
      minimum = @matrix[s-1][t-1]
      @btrace[s][t].push(:eq)
    else
      sub_cost = get_sub_cost(s,t)
      del_cost = get_del_cost(s,t)
      ins_cost = get_ins_cost(s,t)
      minimum = [@matrix[s-1][t-1]+sub_cost, @matrix[s-1][t]+del_cost, @matrix[s][t-1]+ins_cost].min
    end
    @btrace[s][t].push(:sub) if minimum == @matrix[s-1][t-1]+sub_cost
    @btrace[s][t].push(:del) if minimum == @matrix[s-1][t]+del_cost
    @btrace[s][t].push(:ins) if minimum == @matrix[s][t-1]+ins_cost
    return minimum
  end

  #重み付けがあれば以下の関数に追加
  def get_sub_cost(s,t)
      sub_cost = 0.1 if (@source[s-1] =="ン" && @target[t-1] == "ソ") || (@source[s-1] =="ソ" && @target[t-1] == "ン")
      sub_cost = [sub_cost ||= 100000, SUB_COST].min
  end

  def get_del_cost(s,t)
      del_cost = 0.1 if @source[s-1] == "'"
      del_cost = [del_cost ||= 100000, DEL_COST].min
  end

  def get_ins_cost(s,t)
      ins_cost = [ins_cost ||= 100000, INS_COST].min
  end

  def back_trace(s, t)
    if s == 0 && t == 0
      #p @align.reverse
      print @source+"\n"+@result.reverse+"\n"+@target+"\n\n"
      return
    end
    if @btrace[s][t].include? :del
      @align.push(:del)
      @result << "-"
      back_trace(s-1, t)
      @align.pop
      @result.chop!
    end
    if @btrace[s][t].include? :sub
      @align.push(:sub)
      @result << @target[t-1]
      back_trace(s-1, t-1)
      @align.pop
      @result.chop!
    end
    if @btrace[s][t].include? :ins
      @align.push(:ins)
      @result << "*"
      back_trace(s, t-1)
      @align.pop
      @result.chop!
    end
    if @btrace[s][t].include? :eq
      @align.push(:eq)
      @result << @source[s-1]
      back_trace(s-1, t-1)
      @align.pop
      @result.chop!
    end
  end
end
