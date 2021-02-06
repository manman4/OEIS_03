def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end
def A318636(n)
  ary = Array.new(n + 1, 0)
  (1..n).each{|i|
    i.step([n, i * i].min, i){|j|
      ary[j] += ncr(i, j / i)
    }
  }
  ary
end


n = 10000
ary = A318636(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
