def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  s = 0
  (0..n).each{|i|
    (0..n).each{|j|
      (0..n).each{|k|
        (0..n).each{|l|
        s += (-1) ** (i + j + k + l) * f(i + j + k + l) / (f(i) * f(j) * f(k) * f(l))
        }
      }
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
