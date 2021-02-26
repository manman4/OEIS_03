def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

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

def A(n)
  a = [0] + (1..n).map{|i| i ** 4 / f(i).to_r}
  b = a
  ary = a.clone
  (2..n).each{|i|
    b = mul(a, b, n)
    i1 = (-1) ** (i % 2) / i.to_r
    (1..n).each{|j|
      ary[j] -= i1 * b[j]
    }
  }
  [0] + (1..n).map{|i| (ary[i] * f(i)).to_i}
end

n = 400
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
