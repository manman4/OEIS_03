# a(0) = 1
# A382319(n) = Sum_{d|n} a(d)
# a(n) = Sum_{i,j,k >= 1 and i + j + k = n} A382319(i) * A382319(j) * A382319(k) for n>1.
def a(n)
  ary =   [0, 1]
  s_ary = [0, 1]
  (2..n).each{|i|
    s = 0
    (1..i - 1).each{|j|
      (1..i - 1 - j).each{|k|
        s += s_ary[j] * s_ary[k] * s_ary[i - j - k]
      }
    }
    ary << s
    sum_div = 0
    (1..i).each{|j|
      sum_div += ary[j] if i % j == 0
    }
    s_ary << sum_div
    # p [i, s_ary]
  }
  ary
end

n = 1000
ary = a(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
