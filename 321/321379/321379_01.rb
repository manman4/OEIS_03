def S(n)
  cnt = 0
  (2..n).each{|i|
    (i..n / i).each{|j|
      (j..n / (i * j)).each{|k|
        if n % (i * j * k) == 0
          cnt += 1 if n / (i * j * k) >= k
        end
      }
    }
  }
  cnt
end

p (1..100).map{|i| S(i)}
