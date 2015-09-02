#!/usr/bin/env ruby
# -*- coding: utf-8 -*-
require "./colored"
require "./nlp_util"

# ===== Initialize =====
# check input argument
source = ARGV[0]
target_string = ARGV[1]
if source.nil? or target_string.nil? then
    puts "Usage: " + __FILE__ + " cfg_rule_file \"target_string\""
    exit
end
puts "target: " + target_string
# ===== /Initialize =====

# ===== Read Rules =====
rules = Array.new
dict = Hash.new
begin
    # read and split each line
    File.foreach(source) do |line|
        rule = line.split(" ")
        lhs, rhs, prob = rule[0], rule[1..-2], rule[-1].to_f
        rules.push([lhs, rhs, prob])
        
        if rhs.length == 1 and rhs[0].terminal? then
            word = rhs[0].slice(1..-1)
            if dict.has_key?(word) then
                dict[word].push([lhs, rule])
            else
                dict[word] = [[lhs, rule]]
            end
        end
    end
rescue SystemCallError => e
    puts %Q(class=[#{e.class}] message=[#{e.message}])
end
# ===== /Read Rules =====

# p rules
# p dict

# ===== CKY =====
target = target_string.split(" ")
N = target.length

# ---- Initialize -----
cky_table = Array.new(N) { Array.new(N) }
for i in 0...N do
    cky_table[i][i] = dict[target[i]]
    if dict[target[i]].nil? then
        puts "This CNF can't distinguish this sentence"
        exit
    end
end
# ---- /Initialize -----

def draw(rules, table, i, k, j)
    l, r = table[i][k], table[k+1][j]
    parents = Array.new
    for rule in rules do
        lhs, rhs, prob = rule
        if rhs.length == 2 then
            is_find = false
            p = prob
            for lhs_, rule_, i_, k_, j_ in l do
                if lhs_ == rhs[0] then
                    p = p * rule_[2].to_f
                    is_find = true
                end
            end
            
            next unless is_find
            
            for lhs_, rule_, i_, k_, j_ in r do
                if lhs_ == rhs[1] then
                    p = p * rule_[2].to_f
                    parents.push([lhs, rule, [i, k, j], p]) unless does_include
                end
            end
        end
    end
    return parents
end

for d in 0...N do
    for i in 0...(N-d) do
        j = i + d
        for k in i...j do
            if cky_table[i][j].nil? then
                cky_table[i][j] = draw(rules, cky_table, i, k, j)
            else
                cky_table[i][j] = cky_table[i][j] + draw(rules, cky_table, i, k, j)
            end
        end
    end
end
# ===== /CKY =====

puts "-----"

# def show_table(table, n)
#     for i in 0...n do
#         print table[i]
#         puts ""
#     end
# end
# show_table(cky_table, N)

# ===== Show result =====
is_correct = false
start_symbols = Array.new
for lhs_, rule_, (i_, k_, j_) in cky_table[0][N-1] do
    if lhs_ == "S" then
        is_correct = true
        start_symbols.push([rule_, i_, k_, j_])
    end
end
if is_correct then
    puts "Correct Grammar!".green
else
    puts "Not Correct Grammar...".red
    exit
end
# ===== /Show result =====


puts "-----"


# ===== Show grammar tree =====
def search_tree(table, rule, i, k, j)
    if i.nil? or i == -1 then
        print "( " + rule[1] + " ) "
        return rule[2].to_f
    end
    lhs, rhs, prob = rule
    l, r = table[i][k], table[k+1][j]
    
    print "( " + rhs[0] + " " 
    for lhs_, rule_, (i_, k_, j_) in l do
        if lhs_ == rhs[0] then
            prob = prob * search_tree(table, rule_, i_, k_, j_)
        end
    end
    
    print rhs[1] + " "
    for lhs_, rule_, (i_, k_, j_) in r do
        if lhs_ == rhs[1] then
            prob = prob * search_tree(table, rule_, i_, k_, j_)
        end
    end
    print ") "
    return prob
end

for symbol in start_symbols do
    rule, i, k, j = symbol
    print "( " + rule[0] + " "
    prob = search_tree(cky_table, rule, i, k, j)
    puts ")"
    puts "prob: " + prob.to_s
    puts "---"
end

# ===== /Show grammar tree =====

