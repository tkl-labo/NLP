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
            # dict[rhs[0]] = lhs
            word = rhs[0].slice(1..-1)
            if dict.has_key?(word) then
                dict[word].push(lhs)
            else
                dict[word] = [lhs]
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

def draw(rules, r, l)
    parents = Array.new
    for rule in rules do
        lhs, rhs, prob = rule
        if rhs.length == 2 and
            r.include?(rhs[0]) and l.include?(rhs[1]) then
            parents.push(lhs)
        end
    end
    return parents
end

for d in 0...N do
    for i in 0...(N-d) do
        j = i + d
        for k in i...j do
            if cky_table[i][j].nil? then
                cky_table[i][j] = draw(rules, cky_table[i][k], cky_table[k+1][j])
            else
                cky_table[i][j] = cky_table[i][j] + draw(rules, cky_table[i][k], cky_table[k+1][j])
            end
        end
    end
end
# ===== /CKY =====

puts "-----"

# p cky_table


if cky_table[0][N-1].include?("S") then
    puts "Good!".green
else
    puts "Bad...".red
end
