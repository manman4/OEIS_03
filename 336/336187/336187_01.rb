def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  (0..n).inject(0){|s, i| s + ncr(n, i) ** k * k ** i}.to_i
end

def B(n)
  ary = []
  (0..n).each{|i|
    (0..i).each{|j|
      ary << A(i - j, j)
    }
  }
  ary
end

n = 10
p ary = B(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

# (0..5).each{|i| p [i, (0..10).map{|j| A(i, j)}]}
# (0..5).each{|i| p (0..5).map{|j| A(j, i)}}
# p (0..10).map{|i| A(i, i)}

