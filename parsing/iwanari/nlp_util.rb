# -*- coding: utf-8 -*-

class String
    def terminal?
        self[0] == "_"
    end

    def nonterminal?
        !(self[0] == "_")
    end
end

def normalized?(rule)
    lhs, rhs, prob = rule
    # 3 normalized pattern
    if (rhs.length == 2 and rhs[0].nonterminal? and rhs[1].nonterminal?) or 
        (rhs.length == 1 and rhs[0].terminal?) or 
        (rhs.length == 0 and lhs == "S") then
        true
    else
        false
    end
end

def show_and_normalized_check(rules)
    is_normalized = true
    for rule in rules do
        if normalized?(rule)
            print rule
        else
            print rule.to_s.pink
            is_normalized = false
        end
        puts ""
    end
    
    # show check result
    if is_normalized then
        puts "Normalized!".green
    else
        puts "Not Normalized...".red
    end
end

