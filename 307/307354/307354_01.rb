def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  s = 0
  (0..n).each{|i|
    (i..n).each{|j|
        s += (-1) ** (i + j) * f(i + j) / (f(i) * f(j))
    }
  }
  s
end

n = 20
b=[]
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
b<<j
}
p b
