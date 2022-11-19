def A030717(n)
  ary = [1]
  (n - 1).times{
    ary += ary.uniq.sort.map{|i| ary.count(i)}
  }
  ary
end
p A030717(50).uniq
# n = 300
# ary = A030717(n).uniq
# (1..1000).each{|i|
#   print i
#   print ' '
#   puts ary[i - 1]
# }