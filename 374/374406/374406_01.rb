# m次以下を取り出す
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

def poly_num(n, k)
  n * ((k - 2) * n - k + 4) / 2
end

def A(n, k, l, m)
  ary0 = Array.new(n + 1, 0)
  (0..n).each{|i|
    j = poly_num(i, k)
    if j > n
      break
    end
    ary0[j] = 1
  }
  ary1 = Array.new(n + 1, 0)
  (0..n).each{|i|
    j = poly_num(i, l)
    if j > n
      break
    end
    ary1[j] = 1
  }
  ary2 = Array.new(n + 1, 0)
  (0..n).each{|i|
    j = poly_num(i, m)
    if j > n
      break
    end
    ary2[j] = 1
  }
  mul(mul(ary0, ary1, n), ary2, n)
end

n = 10100
m = 10000
ary = A(n, 3, 4, 6)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
