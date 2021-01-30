# mŸˆÈ‰º‚ğæ‚èo‚·
def mul(f_ary, b_ary, m)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary[0..m]
end

def f(n)
  ps = Array.new(n + 1, 0)
  ps[0] = 1
  (1..n).each{|num|
    (num..n).each{|i|
      ps[i] += ps[i - num]
    }
  }
  ps
end

def A(n)
  f_ary = f(70)
  ary = [1]
  (1..70).each{|i|
    j = f_ary[i]
    break if j > n
    b_ary = Array.new(j + 1, 0)
    b_ary[0], b_ary[-1] = 1, -1
    ary = mul(ary, b_ary, n)
  }
  ary
end

n = 10150
ary = A(n)
(0..10000).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
