def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n)
  ary = [1]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      break if j * j > i
      s += ary[i - j * j] * ncr(i - 1, j * j - 1) 
    }
    ary << s
  }
  ary
end

n = 700
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}