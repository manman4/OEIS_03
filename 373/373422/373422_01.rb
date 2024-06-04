def diff(ary, n)
  a = ary.clone
  (n - 1).times{|i|
    (0..n - 2 - i).each{|j|
      a[j] = a[j + 1] - a[j]
    }
    a.pop
  }
  a[0] == 0
end

def A(n)
  ary = [0] * (n + 1)
  (1..n).to_a.permutation{|i|
    ary[i[0]] += 1 if diff(i, n)
    p [i[0], i] if diff(i, n)
  }
  ary[1..-1]
end

b = []
(1..5).each{|i| 
  j = A(i)
  p [j.inject(:+), j]
  b<<j
}
p b.flatten