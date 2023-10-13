def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  m = f(n)
  ary = []
  (2..n).each{|i|
    j = i ** n
    ary << m % j
    break if m <= j
  }
  ary
end

def A340810(n)
  (2..n).map{|i| A(i)}.flatten
end

n = 200
ary = A340810(n)
(2..ary.size + 1).each{|i|
  j = ary[i - 2]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}