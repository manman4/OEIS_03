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

def A(n)
  ary0 = Array.new(n + 1, 0)
  (0..n).each{|i|
    j = i * i
    if j > n
      break
    end
    ary0[j] = 1
  }
  ary1 = Array.new(n + 1, 0)
  (0..n).each{|i|
    j = (2 * i + 1) * (2 * i + 1)
    if j > n
      break
    end
    ary1[j] = 1
  }
  ary2 = Array.new(n + 1, 0)
  (0..n).each{|i|
    j = i * (i + 1) / 2
    if j > n
      break
    end
    ary2[j] = 1
  }
  mul(mul(ary0, ary1, n), ary2, n)
end

n = 10100
m = 10000
ary = A(n)
(0..m).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
