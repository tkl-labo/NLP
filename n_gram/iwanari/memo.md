勘違いして POS の調べ物をしたのでちょっとメモ．

# TreeTagger
For OSX users, TreeTagger can be installed according 
to the instruction of here: 

# Wrapper
http://d.hatena.ne.jp/rkmt/20140114/1389687306
a TreeTagger ruby wrapper for POS 
see: https://github.com/arbox/treetagger-ruby
```
require "treetagger"

tagger = TreeTagger::Tagger.new
tagger.process(string) do |n|
    puts n
end
#tagger.flush
#puts tagger.get_output
```
