def f(ary, n)
  (1..n - 1).each{|i|
    (1..n - i).each{|j|
      k, m = ary[j - 1], ary[j]
      if k < m
        ary[j - 1] +=  m
      else
        ary[j - 1] -= m
      end
    }
  }
  ary[0] == 0
end

def a(n)
  cnt = 0
  (1..n).to_a.each_permutation{|i|
    cnt += 1 if f(i, n)
  }
  cnt
end

(1..15).each{|i| p a(i)}