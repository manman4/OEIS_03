def A(n)
  d = 0
  a = (1..n - 1).map{|i| [i]}
  cnt = n - 1
  while d < n - 2
    d += 1
    b = []
    a.each{|i|
      (1..n - 1).each{|j|
        m = i.clone + [j]
        if (0..d).inject(0){|s, k| s + m[k] * n ** (d - k)} % (d + 1) == 0
          b << m
          cnt += 1
        end
      }
    }
    a = b
  end
  cnt
end
def A324020(n)
  (2..n).map{|i| A(i)}
end
p A324020(10)
