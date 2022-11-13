def A030717(n)
  ary = [1]
  (1..n).each{|i|
    ary += ary.uniq.sort.map{|i| ary.count(i)}
  }
  ary
end

n = 300
ary = A030717(n).uniq
(1..1000).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}