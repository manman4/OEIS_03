def sigma(n)
  cnt = 0
  (1..n).each{|i|
    cnt += i if n % i == 0
  }
  cnt
end

def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A340904(n)
  s_ary = (0..n).map{|i| sigma(i)}
  ary = [1]
  (1..n).each{|i|
    ary << (1..i).inject(0){|s, j| s + ncr(i, j) * s_ary[j] * ary[-j]}
  }
  ary
end

n = 500
ary = A340904(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
