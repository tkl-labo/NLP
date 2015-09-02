#!/usr/bin/env ruby
# -*- coding: utf-8 -*-
require "./colored"
require "./nlp_util"

# ===== Initialize =====
# check input argument
source = ARGV[0]
if source.nil? then
    puts "Usage: " + __FILE__ + " cfg_rule_file [debug: 1]"
    exit
end
is_debug = ARGV[1].to_i == 1
# ===== /Initialize =====

# ===== Read Rules =====
cfg_rules = Array.new
begin
    # read and split each line
    File.foreach(source) do |line|
        rule = line.split(" ")
        lhs, rhs, prob = rule[0], rule[1..-2], rule[-1].to_f
        cfg_rules.push([lhs, rhs, prob])
    end
rescue SystemCallError => e
    puts %Q(class=[#{e.class}] message=[#{e.message}])
end
# ===== /Read Rules =====


if is_debug then
    show_and_normalized_check(cfg_rules)
    puts "------------"
end

# ===== Conversion =====
# ----- 1st step (Invalid symbol reduction) -----
# I assume there is no invalid symbol in the rules
# so, pass this step

# ----- 2nd step (Epsilon symbol reduction) -----
# I assume there is no epsilon symbol in the rules
# so, pass this step
passed_2nd = cfg_rules

# ----- 3rd step (Unit production reduction) -----
passed_3rd = Array.new

def reachable_unit(rules, reachable, lhs, prob)
    new_reachable = Array.new
    # add new reachable symbols
    for rule in rules do
        if normalized?(rule) then
            next
        end
        
        # search unit production
        lhs_, rhs_, prob_ = rule
        if unit_production?(rule) and lhs == lhs_ and !reachable.include?(rhs_) then
            reachable.push([rhs_[0], prob * prob_])
            new_reachable.push([rhs_[0], prob_])
        end
    end
    
    # search more reachable symbols recursively
    for lhs_, prob_ in new_reachable do
        reachable_unit(rules, reachable, lhs_, prob * prob_)
    end
end

# calc reachable symbols from each symbol
reachable_symbols = Hash.new
for rule in passed_2nd do
    # check all non-terminal symbols
    lhs, rhs, prob = rule
    if reachable_symbols.has_key?(lhs) then
        next
    end
    
    # search unit production
    reachable = [[lhs, 1.0]]
    reachable_unit(passed_2nd, reachable, lhs, 1.0)
    reachable_symbols[lhs] = Array.new(reachable)
end

# remove unit production
for rule in passed_2nd do
    lhs, rhs, prob = rule
    # ignore unit production
    if rhs.length == 1 and rhs[0].nonterminal? then
        next
    end
    
    # add more rules
    reachable_symbols.each {|key, value|
        for symbol, prob_ in value do
            if symbol == lhs then
                passed_3rd.push([key, rhs, (prob_ * prob).round(6)])
            end
        end
    }
end

# ----- final step (Convert CFG to CNF) -----
$added_rules = Hash.new

def get_new_symbol(rhs)
    if $added_rules.has_key?(rhs) then
        return $added_rules[rhs]
    end
    
    new_symbol = "X" + $added_rules.length.to_s
    $added_rules[rhs] = new_symbol
    return new_symbol
end

def normalize_rule(rule)
    normalized_rules = Array.new
    
    lhs, rhs, prob = rule
    rhs_ = Array.new
    
    # split
    head, tail = rhs[0], rhs[1..-1]
    
    # A -> a B => A -> X1 B, X1 -> a
    if head.terminal? then
        sub_lhs = get_new_symbol([head])
        normalized_rules.push([sub_lhs, head, 1.0])
        rhs_.push(sub_lhs)
    else
        rhs_.push(head)
    end
    
    # deal with tail
    if tail.length >= 2 then
        sub_lhs = get_new_symbol(tail)
        normalized_rules = normalized_rules + normalize_rule([sub_lhs, tail, 1.0])
        rhs_.push(sub_lhs)
    elsif tail[0].terminal? # and tail.length == 1
        sub_lhs = get_new_symbol(tail)
        normalized_rules.push([sub_lhs, tail, 1.0])
        rhs_.push(sub_lhs)
    else
        rhs_.push(tail[0])
    end
    
    normalized_rules.push([lhs, rhs_, prob])
end

normalized_rules = Array.new
for rule in passed_3rd do
    if normalized?(rule) then
        normalized_rules.push(rule)
    else
        normalized_rules = normalized_rules + normalize_rule(rule)
    end
end
# remove duplication
normalized_rules.uniq!
# ===== /Conversion =====

if is_debug then
    show_and_normalized_check(normalized_rules)
    puts "# of Added rules: " + $added_rules.length.to_s
    puts "------------"
end

for rule in normalized_rules do
    lhs, rhs, prob = rule
    puts lhs + " " + rhs.join(" ") + " " + prob.to_s
end
