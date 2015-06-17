#!/usr/bin/env ruby
target = ARGV[0]
source = ARGV[1]

INSERT_COST = 1
DELETE_COST = 1
SUBSTITUTE_COST = 2

if target.nil? or source.nil? then
    puts "Usage: " + __FILE__ + " target_string source_string"
    exit
end

puts "target: " + target
puts "source: " + source

N = target.length
M = source.length

# create a distance matrix
distance = Array.new(N+1){ Array.new(M+1) }

# Initialize the zeroth row and column to be the distance from the empty string
distance[0][0] = 0

for i in 1..N do
    distance[i][0] = distance[i-1][0] + INSERT_COST
end

for i in 1..M do
    distance[0][i] = distance[0][i-1] + DELETE_COST
end

for i in 1..N do
    for j in 1..M do
        distance[i][j] = [distance[i-1][j] + INSERT_COST,
                            distance[i-1][j-1] + SUBSTITUTE_COST * ((source[j-1] != target[i-1]) ? 1 : 0),
                            distance[i][j-1] +  DELETE_COST].min
    end
end

puts "Distance: " + distance[N][M].to_s






