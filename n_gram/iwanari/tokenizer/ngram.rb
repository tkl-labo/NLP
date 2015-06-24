#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

class NGRAM
    def initialize(input_file_name)
        @filename = input_file_name
    end
    
    def calc
        # open and read a file
        string_data = read_data(@filename)
        splited_string = split_string(string_data)
        
        splited_string.each do |str|
            puts str
        end
    end
    
    # -----
    private
    
    def read_data(filename)
        File.read(filename)
    end
    
    def split_string(string)
        # I assume the language is either Japanese or English
        # Japanese check. see: http://qiita.com/okappy/items/65f9612ee29c0b0c8cef
        if string =~ /(?:\p{Hiragana}|\p{Katakana}|[一-龠々])/ then
            # use `natto` to use MeCab for Morphological analysis
            # see: http://kitak.hatenablog.jp/entry/2014/10/24/084847
            require 'natto' 
            
            natto = Natto::MeCab.new 
            splited_string = Array.new
            natto.parse(string) do |n|
                splited_string.push(n.surface)
            end
            return splited_string
        else
            # use `tokenizer` to tokenize string
            # see: https://github.com/arbox/tokenizer
            require 'tokenizer'
            
            tokenizer = Tokenizer::Tokenizer.new
            tokenizer.tokenize(string)
        end
    end
end

if ARGV[0].nil? then
    puts "Usage: " + __FILE__ + " input_file_name"
    exit
end

test = NGRAM.new(ARGV[0])
test.calc


