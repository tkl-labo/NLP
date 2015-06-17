#!/usr/bin/env ruby
$target = ARGV[0]
$source = ARGV[1]

INSERT_COST = 1
DELETE_COST = 1
SUBSTITUTE_COST = 2


def get_insert_cost(a)
    return INSERT_COST
end

def get_delete_cost(a)
    return DELETE_COST
end

def get_substitute_cost(a, b)
    return SUBSTITUTE_COST * ((a != b) ? 1 : 0)
end

if $target.nil? or $source.nil? then
    puts "Usage: " + __FILE__ + " target_string source_string"
    exit
end

puts "target: " + $target
puts "source: " + $source

N = $target.length
M = $source.length

# create a distance matrix
$distance = Array.new(N+1){ Array.new(M+1) }

# a table keeping back pointers to follow minimum edit distance processes
$back_pointers = Array.new(N+1){ Array.new(M+1) }

# Initialize the zeroth row and column to be the distance from the empty string
$distance[0][0] = 0

for i in 1..N do
    $distance[i][0] = $distance[i-1][0] + INSERT_COST
    $back_pointers[i][0] = [true, false, false]
end

for i in 1..M do
    $distance[0][i] = $distance[0][i-1] + DELETE_COST
    $back_pointers[0][i] = [false, false, true]
end

for i in 1..N do
    for j in 1..M do
        # try 3 ways
        three_ways =  [$distance[i-1][j] + get_insert_cost($target[i-1]),
                        $distance[i-1][j-1] + get_substitute_cost($source[j-1], $target[i-1]),
                        $distance[i][j-1] +  get_delete_cost($source[j-1])]
        # search minimum cost
        $distance[i][j] = three_ways.min
        
        # create a vector like [min, min, min]
        vmin = Array.new(3) { $distance[i][j] }
        # check from which columns give minimum costs
        # e.g. three_ways = [1, 2, 1] -> min = 1 and back_pointers[i][j] = [true, false, true]
        $back_pointers[i][j] = vmin.zip(three_ways).map{|m, t| m == t}
    end
end

puts "Distance: " + $distance[N][M].to_s

puts "\n"

# traverse tree from distance[N][M]
# preorder may work properly
$num_of_solution = 0
def traverse(i, j, process, tar, src)
    if i <= 0 && j <= 0 then
        $num_of_solution = $num_of_solution + 1
        puts "--- solution " + $num_of_solution.to_s + " ---"
        puts "action: " + process
        puts "target: " + tar
        puts "source: " + src
        return
    end
    if $back_pointers[i][j][0] then
        traverse(i-1, j, "+" + process, tar, src.dup.insert(j, "\s"))
    end
    if $back_pointers[i][j][1] then
        # if others are not minimum, this character is equivalent to target's
        unless $back_pointers[i][j][0] then # using $back_pointers[i][j][2] is also ok
            traverse(i-1, j-1, "=" + process, tar, src)
        else
            traverse(i-1, j-1, "*" + process, tar, src)
        end
    end
    if $back_pointers[i][j][2] then
        traverse(i, j-1, "-" + process, tar.dup.insert(i, "\s"), src)
    end
end

traverse(N, M, "", $target.dup, $source.dup)
